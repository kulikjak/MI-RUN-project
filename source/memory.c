#include <limits.h>
#include <string.h>

#include "lang.h"

#define MARKED(obj) (obj->tag & 0x80)
#define MARK(obj) (obj->tag |= (0x80))
#define UNMARK(obj) (obj->tag &= (0x7f))

#define RESTRICT_PARENT(x) (x = (void*)((int64_t)x | 1))
#define IS_RESTRICTED(x) ((int64_t)x & 1)
#define CLEAN_RESTRICTION(x) (void*)((int64_t)x & ~(0x1))

OBJ heap[HEAP_SIZE];
int32_t heapAllocatedRecords;

objEnvironment* localEnvironment;

appAggregator aggregatorMemory[10];

void heapMarkAndSweep() {
  int32_t i;

  if (heapAllocatedRecords <= MAS_LIMIT) return;

  // Mark phase: Go through each root and it's children and mark them
  objEnvironment* lenv = localEnvironment;
  if (lenv != NULL) {
    while (1) {
      MARK(lenv);
      for (i = 0; i < lenv->count; i++) {
        MARK(lenv->objects[i].key);
        MARK(lenv->objects[i].value);
      }
      if (lenv->parent == NULL) break;
      lenv = CLEAN_RESTRICTION(lenv->parent);
    }
  }

  objEnvironment* genv = gAppCtx.global;
  if (genv != NULL) {
    MARK(genv);
    for (i = 0; i < genv->count; i++) {
      MARK(genv->objects[i].key);
      MARK(genv->objects[i].value);
    }
  }

  // Sweep phase: Free non marked nodes and set mark to false for others
  for (int i = 0; i < HEAP_SIZE; i++) {
    if (heap[i]) {
      if (!MARKED(heap[i])) {
        if (heap[i]->tag == T_ENVIRONMENT)
          free(((objEnvironment*)heap[i])->objects);
        free(heap[i]);

        heapAllocatedRecords--;
        heap[i] = NULL;
        continue;
      }
      UNMARK(heap[i]);
    }
  }
}

int _heapGetFreeSlot() {
  int32_t i;
  for (i = 0; i < HEAP_SIZE; i++)
    if (heap[i] == NULL) {
      heapAllocatedRecords++;
      return i;
    }

  fatal("Runtime is out of memory.");
  return 0;
}

void heapFreeRest() {
  int32_t i;
  for (i = 0; i < HEAP_SIZE; i++)
    if (heap[i]) {
      if (heap[i]->tag == T_ENVIRONMENT)
        free(((objEnvironment*)heap[i])->objects);
      free(heap[i]);
    }
}

void heapInit() {
  heapAllocatedRecords = 0;
  memset(heap, 0, sizeof(heap));
}

void _reallocEnvironment(objEnvironment* env) {
  env->size *= 2;
  env->objects = (OBJEntry*)realloc(env->objects, env->size * sizeof(OBJEntry));
}

void initializeApplicationMemory() {
  heapInit();

  gAppCtx.global = (objEnvironment*)newEnvironment(NULL, FALSE);
  gAppCtx.local = NULL;
  gAppCtx.main = NULL;

  localEnvironment = NULL;
}

void initializeAggregatorMemory() {
  int32_t i;
  for (i = 0; i < 10; i++) {
    aggregatorMemory[i].count = 0;
    aggregatorMemory[i].size = 0;
    aggregatorMemory[i].array = NULL;
  }
}

void freeAggregatorMemory() {
  int32_t i;
  for (i = 0; i < 10; i++) {
    if (aggregatorMemory[i].count) free(aggregatorMemory[i].array);
  }
}

bool _compareIdentifiers(const char* key, OBJ object) {
  if (object->tag == T_UNINITIALIZED) return FALSE;
  if (object->tag != T_STRING)
    fatal("Runtime error - variables must have string identifiers.");

  objString* ident = (objString*)object;
  if (!strcmp(key, ident->string)) return TRUE;
  return FALSE;
}

OBJEntry* memoryGetObjectEntry(const char* key) {
  int32_t i;

  // check in local memory
  objEnvironment* lenv = localEnvironment;
  while (1) {
    for (i = 0; i < lenv->count; i++) {
      if (_compareIdentifiers(key, lenv->objects[i].key))
        return &(lenv->objects[i]);
    }
    if (IS_RESTRICTED(lenv->parent) || lenv->parent == NULL) break;

    lenv = CLEAN_RESTRICTION(lenv->parent);
  }

  // check in global memory
  objEnvironment* genv = gAppCtx.global;

  for (i = 0; i < genv->count; i++)
    if (_compareIdentifiers(key, genv->objects[i].key))
      return &(genv->objects[i]);

  OBJ created = newUninitialized();
  return _newLocalVariable(key, created);
}

OBJ memoryGetObject(const char* key) {
  OBJEntry* entry = memoryGetObjectEntry(key);
  return entry->value;
}

void pushLocalContext(bool restricted) {
  objEnvironment* lenv =
      (objEnvironment*)newEnvironment(localEnvironment, restricted);
  localEnvironment = lenv;
}

void popLocalContext() {
  if (localEnvironment == NULL) fatal("Cannot exit nonexisting environment.");
  localEnvironment = CLEAN_RESTRICTION(localEnvironment->parent);
}

void memoryAggregatorAdd(int8_t number, OBJ res) {
  int64_t pos;

  if (number < 0 || number > 10) fatal("Invalid aggregator number.");

  if (aggregatorMemory[number].count >= aggregatorMemory[number].size) {
    aggregatorMemory[number].size =
        (aggregatorMemory[number].size) ? aggregatorMemory[number].size * 2 : 2;
    aggregatorMemory[number].array =
        (OBJ*)realloc(aggregatorMemory[number].array,
                      aggregatorMemory[number].size * sizeof(OBJ));
  }
  pos = aggregatorMemory[number].count++;
  aggregatorMemory[number].array[pos] = res;
}

appAggregator* memoryGetAggregator(int8_t number) {
  if (number < 0 || number > 10) fatal("Invalid aggregator number.");

  return &(aggregatorMemory[number]);
}

void _check(objEnvironment* env, const char* name) {
  int32_t i;

  if (!env) return;

  for (i = 0; i < env->count; i++) {
    if (env->objects[i].key->tag == T_UNINITIALIZED) continue;
    if (env->objects[i].key->tag != T_STRING)
      fatal("Runtime error - variables must have string identifiers.");

    objString* ident = (objString*)env->objects[i].key;
    if (!strcmp(name, ident->string))
      fatal("Redeclaration of variable / function.");
  }
}

OBJEntry* _newLocalVariable(const char* name, OBJ variable) {
  if (localEnvironment->count >= localEnvironment->size)
    _reallocEnvironment(localEnvironment);

  localEnvironment->objects[localEnvironment->count].key = newString(name);
  localEnvironment->objects[localEnvironment->count].value = variable;
  localEnvironment->count++;

  return &(localEnvironment->objects[localEnvironment->count - 1]);
}

void newGlobalVariable(const char* name) {
  objEnvironment* global = gAppCtx.global;
  _check(global, name);

  if (global->count >= global->size) _reallocEnvironment(global);

  global->objects[global->count].key = newString(name);
  ;
  global->objects[global->count].value = newUninitialized();
  global->count++;
}

OBJ newUninitialized() {
  OBJ newObj;

  newObj = (OBJ)malloc(sizeof(struct object));
  newObj->tag = T_UNINITIALIZED;

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return newObj;
}

OBJ newBoolean(bool val) {
  struct objBoolean* newObj;

  newObj = (struct objBoolean*)malloc(sizeof(struct objBoolean));
  newObj->tag = T_BOOLEAN;
  newObj->val = val;

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return (OBJ)newObj;
}

OBJ newInteger(int32_t val) {
  struct objInteger* newObj;

  newObj = (struct objInteger*)malloc(sizeof(struct objInteger));
  newObj->tag = T_INTEGER;
  newObj->val = val;

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return (OBJ)newObj;
}

OBJ newBigInteger(int64_t val) {
  struct objBigInteger* newObj;

  newObj = (struct objBigInteger*)malloc(sizeof(struct objBigInteger));
  newObj->tag = T_BIGINTEGER;
  newObj->val = val;

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return (OBJ)newObj;
}

OBJ newAutoInteger(int64_t val) {
  if (val > INT_MAX || val < INT_MIN) return newBigInteger(val);
  return newInteger(val);
}

OBJ newString(const char* string) {
  int32_t len;
  struct objString* newObj;

  len = (int32_t)(strlen(string));

  newObj = (struct objString*)malloc(sizeof(struct objString) + len);
  newObj->tag = T_STRING;
  newObj->size = len;

  strcpy(newObj->string, string);

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return (OBJ)newObj;
}

OBJ newEnvironment(struct objEnvironment* parent, bool restricted) {
  struct objEnvironment* newObj;

  newObj = (struct objEnvironment*)malloc(sizeof(struct objEnvironment));
  newObj->tag = T_ENVIRONMENT;
  newObj->size = 3;
  newObj->count = 0;
  newObj->parent = parent;
  newObj->objects = (OBJEntry*)malloc(newObj->size * sizeof(OBJEntry));
  if (restricted && parent) RESTRICT_PARENT(newObj->parent);

  heap[_heapGetFreeSlot()] = (OBJ)newObj;
  return (OBJ)newObj;
}

OBJ cloneObject(const OBJ obj) {
  switch (obj->tag) {
    case T_UNINITIALIZED:
      return newUninitialized();
    case T_BOOLEAN:
      return newBoolean(((objBoolean*)obj)->val);
    case T_INTEGER:
      return newInteger(((objInteger*)obj)->val);
    case T_BIGINTEGER:
      return newBigInteger(((objBigInteger*)obj)->val);
    case T_STRING:
      return newString(((objString*)obj)->string);
    case T_ENVIRONMENT:
      fatal("Environment object cannot be cloned.");
    default:
      printf("%d\n", obj->tag);
      fatal("Unexpected object type.");
  }
  return NULL;
}

void printMemoryStatus() {
  int32_t i;

  objEnvironment* lenv = localEnvironment;

  if (lenv != NULL) {
    if (lenv->count) {
      printf("Local variables:\n");
      while (lenv) {
        for (i = 0; i < lenv->count; i++) {
          printObject(lenv->objects[i].key);
          printf(": ");
          printObjectVerbose(lenv->objects[i].value);
        }
        lenv = CLEAN_RESTRICTION(lenv->parent);
        if (lenv) printf("<<<\n");
      }
    }
  }

  objEnvironment* genv = gAppCtx.global;
  if (genv != NULL) {
    if (genv->count) {
      printf("Global variables:\n");
      for (i = 0; i < genv->count; i++) {
        printObject(genv->objects[i].key);
        printf(": ");
        printObjectVerbose(genv->objects[i].value);
      }
    }
  }
}

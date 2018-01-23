#include "memory.h"

/*void newEntry(Context* ctx, char* key, OBJ value) {
  int32_t i;

  // check if symbol is already inside this context
  for (i = 0; i < ctx->count; i++) {
    if (!strcmp(key, ctx->objects[i].key)) fatal("Redeclaration of variable.");
  }

  // realloc entries if structure is full
  if (ctx->count >= ctx->size) {
    ctx->size += 2 * ctx->size;
    ctx->objects = (OBJEntry*)realloc(ctx->objects, sizeof(OBJEntry) * size);
  }

  // add object into the context
  strcpy(ctx->objects[ctx->count].key, key);
  ctx->objects[ctx->count].value = value;

  ctx->count++;
}*/

LocalContext* newLocalContext(int32_t size) {
  LocalContext* ctx;

  ctx = (LocalContext*)malloc(sizeof(LocalContext));
  ctx->size = size;
  ctx->count = 0;
  ctx->restricted = true;
  ctx->parent = NULL;
  ctx->objects = (OBJEntry*)malloc(sizeof(OBJEntry) * size);
  memset(ctx->objects, 0, (sizeof(OBJEntry) * size));

  return ctx;
}

GlobalContext* newGlobalContext(int32_t size) {
  GlobalContext* ctx;

  ctx = (GlobalContext*)malloc(sizeof(GlobalContext));
  ctx->size = size;
  ctx->count = 0;
  ctx->objects = (OBJEntry*)malloc(sizeof(OBJEntry) * size);
  memset(ctx->objects, 0, (sizeof(OBJEntry) * size));

  return ctx; 
}

FuncContext* newFuncContext(int32_t size) {
  FuncContext* ctx;

  ctx = (FuncContext*)malloc(sizeof(FuncContext));
  ctx->size = size;
  ctx->count = 0;
  ctx->objects = (FuncEntry*)malloc(sizeof(FuncEntry) * size);
  memset(ctx->objects, 0, (sizeof(FuncEntry) * size));

  return ctx; 
}

appContext* initAppContext() {
  appContext* ctx;

  ctx = (appContext*)malloc(sizeof(appContext));
  ctx->local = newLocalContext(5);
  ctx->global = newGlobalContext(5);
  ctx->fuction = newFuncContext(5);
  ctx->main = NULL;

  return ctx;
}





OBJ newUninitialized() {
  OBJ newObj;

  newObj = (OBJ)malloc(sizeof(struct object));
  newObj->tag = T_UNINITIALIZED;

  return newObj;
}

OBJ newBoolean(bool val) {
  OBJ newObj;

  newObj = (OBJ)malloc(sizeof(struct objBoolean));
  newObj->tag = T_BOOLEAN;
  newObj->val = val;

  return newObj;
}

OBJ newInteger(int32_t val) {
  OBJ newObj;

  newObj = (OBJ)malloc(sizeof(struct objInteger));
  newObj->tag = T_INTEGER;
  newObj->val = val;

  return newObj;
}

OBJ newBigInteger(int64_t val) {
  OBJ newObj;

  newObj = (OBJ)malloc(sizeof(struct objBigInteger));
  newObj->tag = T_BIGINTEGER;
  newObj->val = val;

  return newObj;
}

OBJ newString(const char* string) {
  int32_t len;
  OBJ newObj;

  len = (int32_t)(strlen(string));

  newObj = (OBJ)malloc(sizeof(struct objString) + size);
  newObj->tag = T_STRING;
  newObj->size = len;

  strcpy(newObj->string, string);
  return newObj;
}

#ifndef __MEMORY__
#define __MEMORY__

#include "lang.h"

typedef struct OBJEntry {
  char key[20];
  OBJ value;
} OBJEntry;

typedef struct FuncEntry {
  char key[20];
  astBlockNode* value;
} FuncEntry;

typedef struct LocalContext {
  int32_t size;
  int32_t count;
  OBJEntry* objects;
  struct Context* parent;
  bool restricted;
} LocalContext;

typedef struct FuncContext {
  int32_t size;
  int32_t count;
  FuncEntry* objects;
} FuncContext;

typedef struct GlobalContext {
  int32_t size;
  int32_t count;
  OBJEntry* objects;
} GlobalContext;

typedef struct appContext {
  GlobalContext* global;
  FuncContext* fuction;
  LocalContext* local;
  FunctionNode* main;
} appContext;

LocalContext* newLocalContext(int32_t size);
GlobalContext* newGlobalContext(int32_t size);
FuncContext* newFuncContext(int32_t size);

appContext* initAppContext();

/*void newEntry(Context* ctx, char* key, OBJ value);*/
/*Context* newContext(int32_t size);*/
/*appContext* initAppContext();*/
/*void printContext(Context* ctx);*/

#endif /* __MEMORY__ */

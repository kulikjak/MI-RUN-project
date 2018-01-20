#ifndef __MEMORY__
#define __MEMORY__

typedef struct OBJEntry {
  char key[20];
  OBJ value;
} OBJEntry;

typedef struct Context {
  int32_t size;
  int32_t count;
  bool restricted;
  struct Context* parent;
  OBJEntry* objects;
} Context;

typedef struct appContext {
  Context* local;
  Context* global;
  Context* fuction;
} appContext;

#endif /* __MEMORY__ */

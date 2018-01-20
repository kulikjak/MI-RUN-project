#include "memory.h"


void newEntry(Context* ctx, char* key, OBJ value) {
  int32_t i;

  // check if symbol is already inside this context
  for (i = 0; i < ctx->count; i++) {
    if (!strcmp(key, ctx->objects[i].key))
      fatal("Redeclaration of variable.");
  }

  // realloc entries if structure is full
  if (ctx->count >= ctx->size) {
    ctx->size += 2 * ctx->size;
    ctx->objects = (OBJEntry*) realloc (ctx->objects, sizeof(OBJEntry) * size);
  }

  // add object into the context
  strcpy(ctx->objects[ctx->count].key, key);
  ctx->objects[ctx->count].value = value;

  ctx->count++;
}

Context* newContext(int32_t size) {
  Context* ctx;

  ctx = (Context*) malloc (sizeof(appContext));
  ctx->size = size;
  ctx->count = 0;
  ctx->restricted = true;
  ctx->parent = NULL;
  ctx->objects = (OBJEntry*) malloc (sizeof(OBJEntry) * size);
  memset(ctx->objects, 0, (sizeof(OBJEntry) * size));

  return ctx;
}

appContext* initAppContext() {
  appContext* ctx;

  ctx = (appContext*) malloc (sizeof(appContext));
  ctx->local = newContext(5);
  ctx->global = newContext(5);
  ctx->fuction = NULL;

  return ctx;
}

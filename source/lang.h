#ifndef __LANG__
#define __LANG__

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

#define HEAP_SIZE 1024
#define MAS_LIMIT 512

#define DEBUG__ FALSE
#define _UNREACHABLE assert(0)

#include "frontend.h"

typedef enum objTag {
  T_UNINITIALIZED = 0,
  T_BOOLEAN = 1,
  T_INTEGER = 2,
  T_BIGINTEGER = 3,
  T_STRING = 4,
  T_ENVIRONMENT = 5
} objTag;

typedef struct object {
  enum objTag tag;
} object;

typedef object* OBJ;

typedef struct OBJEntry {
  OBJ key;
  OBJ value;
} OBJEntry;

typedef struct objBoolean {
  enum objTag tag;
  bool val;
} objBoolean;

typedef struct objInteger {
  enum objTag tag;
  int32_t val;
} objInteger;

typedef struct objBigInteger {
  enum objTag tag;
  int64_t val;
} objBigInteger;

typedef struct objString {
  enum objTag tag;
  int32_t size;
  char string[1];
} objString;

typedef struct objEnvironment {
  enum objTag tag;
  int32_t size;
  int32_t count;
  struct objEnvironment* parent;
  OBJEntry* objects;
} objEnvironment;

typedef struct {
  objEnvironment* global;
  objEnvironment* local;
  astStatementNode* main;
} appContext;

typedef struct {
  int32_t size;
  int32_t count;
  OBJ* array;
} appAggregator;

// Global applocation context variable
appContext gAppCtx;

#include "prototypes.h"

#define SKIP_ENDL                                 \
  {                                               \
    while (Symb.type == ENDL) Symb = readLexem(); \
  }

#define GET_INT_VALUE(obj)                                             \
  ((obj->tag == T_INTEGER) ? (int64_t)(((struct objInteger*)obj)->val) \
                           : ((struct objBigInteger*)obj)->val)

#define INT_ONLY_CHECK(obj, operation)                              \
  {                                                                 \
    if (obj->tag == T_UNINITIALIZED)                                \
      fatal("Cannot work with uninitialized objects.");             \
    if (obj->tag == T_BOOLEAN)                                      \
      fatalExt(operation, " cannot be used with Boolean objects."); \
    if (obj->tag == T_STRING)                                       \
      fatalExt(operation, " cannot be used with String objects.");  \
    if (obj->tag != T_INTEGER && obj->tag == T_BIGINTEGER)          \
      fatalExt(operation, " cannot be used with Unknown objects."); \
  }

#endif /* __LANG__ */

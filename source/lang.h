#ifndef __LANG__
#define __LANG__

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

#define DEBUG__ FALSE
#define _UNREACHABLE assert(0)

#include "frontend.h"

typedef enum objTag {
  T_UNINITIALIZED = 0,
  T_BOOLEAN,
  T_INTEGER,
  T_BIGINTEGER,
  T_STRING
} objTag;

typedef struct object {
	enum objTag tag;
} object;

typedef object* OBJ;

struct objBoolean {
  enum objTag tag;
  bool val;
};

struct objInteger {
  enum objTag tag;
  int32_t val;
};

struct objBigInteger {
  enum objTag tag;
  int64_t val;
};

struct objString {
  enum objTag tag;
  int size;
  char string[1];
};

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
  astBlockNode* main;
} appContext;

#include "prototypes.h"

#define SKIP_ENDL                                 \
  {                                               \
    while (Symb.type == ENDL) Symb = readLexem(); \
  }

#endif /* __LANG__ */

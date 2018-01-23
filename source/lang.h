#ifndef __LANG__
#define __LANG__

#include <stdbool.h>

typedef enum objTag {
  T_UNINITIALIZED = 0,
  T_BOOLEAN,
  T_INTEGER,
  T_BIGINTEGER,
  T_STRING
} astTag;

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

#endif /* __LANG__ */

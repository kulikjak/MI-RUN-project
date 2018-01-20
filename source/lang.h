
typedef enum objTag {
  T_UNUSED = 0,
  T_SMALLINTEGER,
  T_INTEGER,
  T_BIGINTEGER,
  T_FLOAT,
  T_STRING
} astTag;

typedef struct object {
	enum objTag tag;
} object;

typedef object* OBJ;

struct objSmallInteger {
  enum objTag tag;
  int16_t val;
};

struct objInteger {
  enum objTag tag;
  int32_t val;
};

struct objBigInteger {
  enum objTag tag;
  int64_t val;
};

struct objFloat {
  enum objTag tag;
  double val;
};

struct objString {
  enum objTag tag;
  int size;
  char string[1];
};

#include <string.h>
#include <ctype.h>

#include "lang.h"

OBJ getObjectFromBuffer(char* buffer) {
  int64_t val;
  int32_t i, len = strlen(buffer);

  if (buffer[len-1] == '\n')
    buffer[--len] = 0;
  if (buffer[len-1] == '\r')
    buffer[--len] = 0;

  val = 0;
  for (i = (buffer[0] == '-') ? 1 : 0; i < len; i++) {
    if (!isdigit(buffer[i]))
      return newString(buffer);
    else
      val = val * 10 + (buffer[i] - '0');
  }
  val *= (buffer[0] == '-') ? -1 : 1;
  return newAutoInteger(val);
}

void printObject(OBJ obj) {
  switch (obj->tag) {
    case T_UNINITIALIZED:
      fatal("Cannot print uninitialized variable.");
    case T_BOOLEAN:
      printf("%s", (((struct objBoolean*)obj)->val) ? "TRUE" : "FALSE");
      break;
    case T_INTEGER:
      printf("%d", ((struct objInteger*)obj)->val);
      break;
    case T_BIGINTEGER:
      printf("%ld", ((struct objBigInteger*)obj)->val);
      break;
    case T_STRING:
      printf("%s", ((struct objString*)obj)->string);
      break;
    default:
      fatal("Unexpected variable type: cannot be printed.");
  }
}

void printObjectVerbose(OBJ obj) {
  switch (obj->tag) {
    case T_UNINITIALIZED:
      printf("[UNINITIALIZED]\n");
      break;
    case T_BOOLEAN:
      printf("[BOOLEAN] %s\n", (((objBoolean*)obj)->val) ? "TRUE" : "FALSE");
      break;
    case T_INTEGER:
      printf("[INTEGER] %d\n", ((objInteger*)obj)->val);
      break;
    case T_BIGINTEGER:
      printf("[BIGINTEGER] %ld\n", ((objBigInteger*)obj)->val);
      break;
    case T_STRING:
      printf("[STRING] %s\n", ((objString*)obj)->string);
      break;
    case T_ENVIRONMENT:
      printf("[ENVIRONMENT]\n");
      break;
    default:
      fatal("Unexpected object type.");
  }
}

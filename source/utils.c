
#include "lang.h"

void fatal(const char* message) {
  fprintf(stderr, "fatal: %s\n", message);
  exit(EXIT_FAILURE);
}

void fatalExt(const char* first, const char* second) {
  fprintf(stderr, "fatal: %s%s\n", first, second);
  exit(EXIT_FAILURE);
}

void debug(const char* message) {
  if (DEBUG__) fprintf(stderr, "[Debug]: %s\n", message);
}

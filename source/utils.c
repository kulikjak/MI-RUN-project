#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void fatal(const char* message) {
  fprintf(stderr, "fatal: %s\n", message);
  exit(EXIT_FAILURE);
}

void debug(const char* message) {
  if (DEBUG__) fprintf(stderr, "[Debug]: %s\n", message);
}

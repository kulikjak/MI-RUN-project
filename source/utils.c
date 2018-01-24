
#include "lang.h"

void fatal(const char* message) {
  fprintf(stderr, "fatal: %s\n", message);
  exit(EXIT_FAILURE);
}

void error(char* message, OBJ object) {
  fprintf(stderr, "error: %s", message);
  if (object != NULL) {
    fprintf(stderr, " ");
    // TODO
  }
  fprintf(stderr, "\n");
}

void debug(const char* message) {
  if (DEBUG__) fprintf(stderr, "[Debug]: %s\n", message);
}

void argumentCountError(char* functionName, int expected, int given) {
  char msg[256];

  snprintf(msg, sizeof(msg), "[%s] expects %d arguments (%d given)",
           functionName, expected, given);
  fatal(msg);
}

#include <stdio.h>

#include "utils.h"

#define BUFFER_SIZE 2048

char line[BUFFER_SIZE];
char *linePointer = line;
FILE *inputFile;

void initInput(const char *filename) {
  if (!filename) fatal("No input file.");

  inputFile = fopen(filename, "rt");
  if (!inputFile) fatal("Cannot open fiven input file.");
}

char getChar() {
  if (!*linePointer) {
    if (!fgets(line, BUFFER_SIZE, inputFile)) {
      fclose(inputFile);
      return EOF;
    }
    linePointer = line;
  }
  return *linePointer++;
}

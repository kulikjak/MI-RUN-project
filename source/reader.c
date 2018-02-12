#include <stdio.h>

#include "lang.h"

char line[READ_BUFFER_SIZE];
char *linePointer = line;
FILE *inputFile;

void initInput(const char *filename) {
  if (!filename) fatal("No input file.");

  inputFile = fopen(filename, "rt");
  if (!inputFile) fatal("Cannot open given input file.");
}

char getChar() {
  if (!*linePointer) {
    if (!fgets(line, READ_BUFFER_SIZE, inputFile)) {
      fclose(inputFile);
      return EOF;
    }
    linePointer = line;
  }
  return *linePointer++;
}

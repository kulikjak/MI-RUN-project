#include <stdio.h>
#include <stdlib.h>

#include "../source/lang.h"

int main(int argc, char* argv[]) {
  int i;

  if (argc < 2) fatal("Set your argument.\n");

  for (i = 1; i < argc; i++) {
    printf("=== Parsing file: \x1b[32m%s\x1b[0m ===\n", argv[1]);

    initLexan(argv[1]);

    LexicalSymbol Symb = readLexem();
    while (Symb.type != EOI) {
      printf("%s %s %d\n", symbTable[Symb.type], Symb.ident, Symb.number);
      Symb = readLexem();
    }

    printf("=== File \x1b[32m%s\x1b[0m parsed succesfully ===\n", argv[1]);
  }

  return EXIT_SUCCESS;
}

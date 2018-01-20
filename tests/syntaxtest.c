#include <stdio.h>
#include <stdlib.h>

#include "../source/ast.h"
#include "../source/parser.h"
#include "../source/utils.h"

int main(int argc, char* argv[]) {
  int i;

  if (argc < 2) fatal("Set your argument.\n");

  for (i = 1; i < argc; i++) {
    printf("=== Parsing file: \x1b[32m%s\x1b[0m ===\n", argv[1]);

    initParser(argv[1]);
    astBlockNode* node = Block();

    printAstTree((astNode*)node);
    burnAstTree((astNode*)node);

    printf("=== File \x1b[32m%s\x1b[0m parsed succesfully ===\n", argv[1]);
  }

  return EXIT_SUCCESS;
}

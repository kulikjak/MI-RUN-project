#include "lang.h"

int main(int argc, char* argv[]) {
  if (argc < 2)
    fatal("No input file.\nExecution terminated.");

  initializeApplicationMemory();
  initializeAggregatorMemory();
  initParser(argv[1]);

  ParseProgram();
  runProgram();

  burnAstTree((astNode*)gAppCtx.main);
  freeAggregatorMemory();
  heapFreeRest();

  return EXIT_SUCCESS;
}

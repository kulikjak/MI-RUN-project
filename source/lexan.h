#ifndef __LEXAN__
#define __LEXAN__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IDENT_LEN 64

typedef enum {
  IDENT, NUMB, STRING, PLUS, MINUS, MUL, DIV, MOD, GT, GTE, LT, LTE,
  EQ, NEQ, LPAR, RPAR, COMMA, ENDL, ASSIGN, kwGLOBAL, kwVARIABLE,
  kwFUNCTION, kwARGS, kwMAIN, kwBEGIN, kwEND, kwWHILE, kwIF, kwDO,
  kwTIMES, kwINTO, kwWRITE, kwREAD, kwINCREMENT, kwDECREMENT, kwBY,
  kwOR, kwAND, kwTRUE, kwFALSE, kwNOT, EOI, ERR, AGGREGATOR
} LexSymbolType;

extern const char *symbTable[44];

typedef struct LexicalSymbol {
  LexSymbolType type;
  char ident[MAX_IDENT_LEN];
  int number;
} LexicalSymbol;

LexicalSymbol readLexem();
void initLexan(const char *);

#endif /* __LEXAN__ */

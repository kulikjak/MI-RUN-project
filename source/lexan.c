#include <string.h>

#include "lang.h"

typedef enum {
  LETTER,
  NUMBER,
  WHITE_SPACE,
  NEWLINE,
  END,
  NO_TYPE
} InputCharType;

const char* symbTable[47] = {
    "IDENT",       "NUMB",       "STRING",     "PLUS",     "MINUS",
    "MUL",         "DIV",        "MOD",        "GT",       "GTE",
    "LT",          "LTE",        "EQ",         "NEQ",      "LPAR",
    "RPAR",        "COMMA",      "ENDL",       "ASSIGN",   "ARROW",
    "kwGLOBAL",    "kwVARIABLE", "kwFUNCTION", "kwARGS",   "kwMAIN",
    "kwBEGIN",     "kwEND",      "kwWHILE",    "kwIF",     "kwDO",
    "kwTIMES",     "kwINTO",     "kwWRITE",    "kwREAD",   "kwINCREMENT",
    "kwDECREMENT", "kwBY",       "kwOR",       "kwAND",    "kwTRUE",
    "kwFALSE",     "kwNOT",      "kwELSE",     "kwASSERT", "EOI",
    "ERR",         "AGGREGATOR"};

static int character;        // vstupni znak
static InputCharType input;  // vstupni symbol

void getInputType() {
  if ((character >= 'A' && character <= 'Z') ||
      (character >= 'a' && character <= 'z') || character == '_')
    input = LETTER;
  else if (character >= '0' && character <= '9')
    input = NUMBER;
  else if (character == EOF)
    input = END;
  else if (character == ' ' || character == '\t')
    input = WHITE_SPACE;
  else if (character == '\n')
    input = NEWLINE;
  else
    input = NO_TYPE;
}

void readInput() {
  character = getChar();
  getInputType();
}

const struct {
  char* slovo;
  LexSymbolType symb;
} keyWordTable[] = {{"global", kwGLOBAL},
                    {"variable", kwVARIABLE},
                    {"function", kwFUNCTION},
                    {"args", kwARGS},
                    {"main", kwMAIN},
                    {"begin", kwBEGIN},
                    {"end", kwEND},
                    {"while", kwWHILE},
                    {"if", kwIF},
                    {"do", kwDO},
                    {"times", kwTIMES},
                    {"into", kwINTO},
                    {"write", kwWRITE},
                    {"read", kwREAD},
                    {"increment", kwINCREMENT},
                    {"decrement", kwDECREMENT},
                    {"by", kwBY},
                    {"or", kwOR},
                    {"and", kwAND},
                    {"true", kwTRUE},
                    {"false", kwFALSE},
                    {"not", kwNOT},
                    {"else", kwELSE},
                    {"assert", kwASSERT},
                    {NULL, (LexSymbolType)0}};

LexSymbolType keyWord(char* id) {
  int i = 0;
  while (keyWordTable[i].slovo)
    if (strcmp(id, keyWordTable[i].slovo) == 0)
      return keyWordTable[i].symb;
    else
      i++;
  return IDENT;
}

LexicalSymbol readLexem(void) {
  LexicalSymbol data;
  int delkaId;
q0:
  switch (character) {
    case '#':
      readInput();
      goto q1;
    case '+':
      data.type = PLUS;
      readInput();
      return data;
    case '-':
      readInput();
      goto q10;
    case '*':
      data.type = MUL;
      readInput();
      return data;
    case '/':
      data.type = DIV;
      readInput();
      return data;
    case '%':
      data.type = MOD;
      readInput();
      return data;
    case '(':
      data.type = LPAR;
      readInput();
      return data;
    case ')':
      data.type = RPAR;
      readInput();
      return data;
    case '"':
      readInput();
      goto q4;
    case ':':
      readInput();
      goto q5;
    case '&':
      readInput();
      goto q6;
    case '<':
      readInput();
      goto q7;
    case '>':
      readInput();
      goto q8;
    case '=':
      data.type = EQ;
      readInput();
      return data;
    case '!':
      readInput();
      goto q9;
    case ',':
      data.type = COMMA;
      readInput();
      return data;
    default: {}
  }

  switch (input) {
    case WHITE_SPACE:
      readInput();
      goto q0;
    case NEWLINE:
      data.type = ENDL;
      readInput();
      return data;
    case END:
      data.type = EOI;
      return data;
    case LETTER:
      delkaId = 1;
      data.ident[0] = character;
      readInput();
      goto q2;
    case NUMBER:
      data.number = character - '0';
      data.type = NUMB;
      readInput();
      goto q3;
    default:
      data.type = ERR;
      fatal("Nedovoleny znak.");
      return data;
  }

q1:  // comments parsing state
  switch (input) {
    case NEWLINE:
      data.type = ENDL;
      readInput();
      return data;
    default:
      readInput();
      goto q1;
  }

q2:  // letters parsing state
  switch (input) {
    case LETTER:
    case NUMBER:
      data.ident[delkaId] = character;
      delkaId++;
      readInput();
      goto q2;
    default:
      if (character == '_') {
        data.ident[delkaId] = character;
        delkaId++;
        readInput();
        goto q2;
      }
      data.ident[delkaId] = 0;
      data.type = keyWord(data.ident);
      return data;
  }

q3:  // number parsing state
  switch (input) {
    case NUMBER:
      data.number = 10 * data.number + (character - '0');
      readInput();
      goto q3;
    default:
      return data;
  }

q4:  // string parsing state
  data.type = STRING;
  int position = 0;

  while (character != '"') {
    data.ident[position] = character;
    position++;
    readInput();
  }
  data.ident[position] = '\0';
  readInput();
  return data;

q5:  // symbols after :
  switch (character) {
    case '=':
      readInput();
      data.type = ASSIGN;
      return data;
    default:
      data.type = ERR;
      fatal("Nedovoleny znak.");
      return data;
  }

q6:  // symbols after &
  switch (input) {
    case NUMBER:
      data.number = (character - '0');
      data.type = AGGREGATOR;

      readInput();
      return data;
    default:
      data.type = ERR;
      fatal("Nedovoleny znak.");
      return data;
  }

q7:  // symbols after <
  switch (character) {
    case '=':
      data.type = LTE;
      readInput();
      return data;
    default:
      data.type = LT;
      return data;
  }

q8:  // symbols after >
  switch (character) {
    case '=':
      data.type = GTE;
      readInput();
      return data;
    default:
      data.type = GT;
      return data;
  }

q9:  // symbols after !
  switch (character) {
    case '=':
      readInput();
      data.type = NEQ;
      return data;
    default:
      data.type = ERR;
      fatal("Nedovoleny znak.");
      return data;
  }

q10:  // symbols after -
  switch (character) {
    case '>':
      data.type = ARROW;
      readInput();
      return data;
    default:
      data.type = MINUS;
      return data;
  }
}

void initLexan(const char* fileName) {
  initInput(fileName);
  readInput();
}

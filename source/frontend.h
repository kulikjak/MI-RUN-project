#include <stdlib.h>

// Max size of any program identifier
#define MAX_IDENT_LEN 64

// Size of program reading buffer
#define READ_BUFFER_SIZE 2048

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

typedef enum astTag {
  N_BLOCK,
  N_STATEMENT,
  N_ASIGN,
  N_IF,
  N_WHILE,
  N_READ,
  N_WRITE,
  N_INCREMENT,
  N_INT,
  N_STRING,
  N_VAR,
  N_UNARY,
  N_BINARY
} astTag;

typedef enum exprOperator {
  OP_ASSIGN,
  OP_PLUS,
  OP_MINUS,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_EQ,
  OP_NEQ,
  OP_LT,
  OP_GT,
  OP_LTE,
  OP_GTE,
  OP_AND,
  OP_OR
} exprOperator;

typedef struct astNode astNode;

// struct only for common paramener passing
typedef struct astNode { enum astTag tag; } astNode;

typedef struct astBlockNode {
  enum astTag tag;
  astNode* statement;
} astBlockNode;

typedef struct astStatementNode {
  enum astTag tag;
  int8_t aggregator;
  astNode* statement;
  astNode* next;
} astStatementNode;

typedef struct astAsignNode {
  enum astTag tag;
  astNode* left;
  astNode* right;
} astAsignNode;

typedef struct astIfNode {
  enum astTag tag;
  astNode* cond;
  astNode* thenBlk;
  astNode* elseBlk;
} astIfNode;

typedef struct astWhileNode {
  enum astTag tag;
  astNode* cond;
  astNode* block;
} astWhileNode;

typedef struct astReadNode {
  enum astTag tag;
  astNode* var;
} astReadNode;

typedef struct astWriteNode {
  enum astTag tag;
  astNode* expr;
} astWriteNode;

typedef struct astIncrementNode {
  enum astTag tag;
  int amount;
  astNode* var;
} astIncrementNode;

typedef struct astFuncCallNode {
  enum astTag tag;
  astNode* args;
  astStatementNode *body;
} astFuncCallNode;

typedef struct astExprIntegerNode {
  enum astTag tag;
  int value;
} astExprIntegerNode;

typedef struct astExprStringNode {
  enum astTag tag;
  char string[1];
} astExprStringNode;

typedef struct astExprVariableNode {
  enum astTag tag;
  char name[1];
} astExprVariableNode;

typedef struct astExprUnaryNode {
  enum astTag tag;
  enum exprOperator op;
  astNode* expr;
} astExprUnaryNode;
 
typedef struct astExprBinaryNode {
  enum astTag tag;
  enum exprOperator op;
  astNode* left;
  astNode* right;
} astExprBinaryNode;

void printAstTree(astNode*);
void burnAstTree(astNode*);

astBlockNode* newAstBlockNode(astStatementNode*);
astStatementNode* newAstStatementNode(astNode*, astStatementNode*, int8_t);
astAsignNode* newAstAsignNode(astExprVariableNode*, astNode*);
astReadNode* newAstReadNode(astExprVariableNode* _v);
astWriteNode* newAstWriteNode(astNode*);
astIncrementNode* newAstIncrementNode(astExprVariableNode*, int);
astExprVariableNode* newAstExprVariableNode(char*);
astExprIntegerNode* newAstExprIntegerNode(int);
astExprUnaryNode* newAstExprUnaryNode(exprOperator, astNode*);
astExprBinaryNode* newAstExprBinaryNode(exprOperator, astNode*, astNode*);

void initLexan(const char * fileName);
void initInput(const char* fileName);
void initParser(const char* fileName);

char getChar();
LexicalSymbol readLexem();

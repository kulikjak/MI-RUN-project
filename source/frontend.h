#include <stdlib.h>

// Max size of any program identifier
#define MAX_IDENT_LEN 64

// Size of program reading buffer
#define READ_BUFFER_SIZE 2048

typedef enum {
  IDENT, NUMB, STRING, PLUS, MINUS, MUL, DIV, MOD, GT, GTE, LT, LTE,
  EQ, NEQ, LPAR, RPAR, COMMA, ENDL, ASSIGN, ARROW, kwGLOBAL, kwVARIABLE,
  kwFUNCTION, kwARGS, kwMAIN, kwBEGIN, kwEND, kwWHILE, kwIF, kwDO,
  kwTIMES, kwINTO, kwWRITE, kwREAD, kwINCREMENT, kwDECREMENT, kwBY,
  kwOR, kwAND, kwTRUE, kwFALSE, kwNOT, kwELSE, EOI, ERR, AGGREGATOR
} LexSymbolType;

extern const char *symbTable[46];

typedef struct LexicalSymbol {
  LexSymbolType type;
  char ident[MAX_IDENT_LEN];
  int64_t number;
} LexicalSymbol;

typedef enum astTag {
  N_STATEMENT,
  N_IF,
  N_WHILE,
  N_DO,
  N_READ,
  N_WRITE,
  N_BOOL,
  N_INT,
  N_STRING,
  N_VAR,
  N_UNARY,
  N_BINARY,
  N_CALL
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

typedef struct astStatementNode {
  enum astTag tag;
  int8_t aggregator;
  astNode* statement;
  astNode* next;
} astStatementNode;

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

typedef struct astDoNode {
  enum astTag tag;
  astNode* amount;
  astNode* block;
} astDoNode;

typedef struct astReadNode {
  enum astTag tag;
  astNode* var;
} astReadNode;

typedef struct astWriteNode {
  enum astTag tag;
  astNode* expr;
} astWriteNode;

typedef struct astExprIntegerNode {
  enum astTag tag;
  int64_t value;
} astExprIntegerNode;

typedef struct astExprBooleanNode {
  enum astTag tag;
  bool value;
} astExprBooleanNode;

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

typedef struct astExprCallNode {
  enum astTag tag;
  int8_t aggr;
  astNode* var;
  char name[1];
} astExprCallNode;


void printAstTree(astNode*);
void burnAstTree(astNode*);

astStatementNode* newAstStatementNode(astNode*, astStatementNode*, int8_t);
astIfNode* newAstIfNode(astNode* condition, astNode* thenBlk, astNode* elseBlk);
astWhileNode* newAstWhileNode(astNode* condition, astNode* block);
astDoNode* newAstDoNode(astNode* amount, astNode* block);
astReadNode* newAstReadNode(astExprVariableNode* _v);
astWriteNode* newAstWriteNode(astNode*);
astExprVariableNode* newAstExprVariableNode(char*);
astExprIntegerNode* newAstExprIntegerNode(int64_t);
astExprBooleanNode* newAstExprBooleanNode(bool __v);
astExprStringNode* newAstExprStringNode(char* __b);
astExprUnaryNode* newAstExprUnaryNode(exprOperator, astNode*);
astExprBinaryNode* newAstExprBinaryNode(exprOperator, astNode*, astNode*);
astExprCallNode* newAstExprCallNode(const char* name, int8_t aggr, astNode* var);

void initInput(const char* fileName);
void initLexan(const char * fileName);
void initParser(const char* fileName);

char getChar();
LexicalSymbol readLexem();

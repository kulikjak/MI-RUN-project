#ifndef __AST__
#define __AST__

typedef enum astTag {
  N_BLOCK,
  N_STATEMENT,
  N_ASIGN,
  N_IF,
  N_WHILE,
  N_READ,
  N_WRITE,
  N_INCREMENT,
  N_TYPE,
  N_INT,
  N_STRING,
  N_VAR,
  N_UNARY,
  N_BINARY
} astTag;

typedef enum varType { T_INTEGER } varType;

typedef enum exprOperator {
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
  astNode* type;
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

typedef struct astTypeNode {
  enum astTag tag;
  enum varType type;
} astTypeNode;

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
astStatementNode* newAstStatementNode(astNode*, astStatementNode*);
astAsignNode* newAstAsignNode(astExprVariableNode*, astNode*);
astReadNode* newAstReadNode(astTypeNode*, astExprVariableNode* _v);
astWriteNode* newAstWriteNode(astNode*);
astIncrementNode* newAstIncrementNode(astExprVariableNode*, int);
astTypeNode* newAstTypeNode(varType);
astExprVariableNode* newAstExprVariableNode(char*);
astExprIntegerNode* newAstExprIntegerNode(int);
astExprUnaryNode* newAstExprUnaryNode(exprOperator, astNode*);
astExprBinaryNode* newAstExprBinaryNode(exprOperator, astNode*, astNode*);

#endif /* __AST__ */

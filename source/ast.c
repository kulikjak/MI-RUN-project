#include <string.h>

#include "lang.h"

void printAstTree(astNode* node) {  // TODO
  if (node == NULL) return;
  switch (node->tag) {
    case N_STATEMENT: {
      if (((astStatementNode*)node)->statement->tag == N_STATEMENT) {
        printf("BEGIN\n");
        printAstTree(((astStatementNode*)node)->statement);
        printf("END\n");
      } else {
        printAstTree(((astStatementNode*)node)->statement);
      }
      printf("\n");
      printAstTree(((astStatementNode*)node)->next);
      break;
    }
    case N_IF: {
      printf("IF ");
      printAstTree(((astIfNode*)node)->cond);
      printf("\n");
      printAstTree(((astIfNode*)node)->thenBlk);
      if (((astIfNode*)node)->elseBlk) {
        printf("ELSE\n");
        printAstTree(((astIfNode*)node)->elseBlk);
      }
      printf("DONE\n");
      break;
    }
    case N_WHILE: {
      printf("WHILE ");
      printAstTree(((astWhileNode*)node)->cond);
      printf("\n");
      printAstTree(((astWhileNode*)node)->block);
      printf("DONE\n");
      break;
    }
    case N_DO: {
      printf("DO ");
      printAstTree(((astDoNode*)node)->amount);
      printf("TIMES\n");
      printAstTree(((astDoNode*)node)->block);
      printf("DONE\n");
      break;
    }
    case N_READ: {
      printf("READ ");
      printf(" INTO ");
      printAstTree(((astReadNode*)node)->var);
      break;
    }
    case N_WRITE: {
      printf("WRITE ");
      printAstTree(((astWriteNode*)node)->expr);
      break;
    }
    case N_INT: {
      printf("%ld", ((astExprIntegerNode*)node)->value);
      break;
    }
    case N_STRING: {
      printf("%s", ((astExprStringNode*)node)->string);
      break;
    }
    case N_VAR: {
      printf("_%s", ((astExprVariableNode*)node)->name);
      break;
    }
    case N_UNARY: {
      switch (((astExprUnaryNode*)node)->op) {
        case OP_MINUS:
          printf("-");
          break;
        default:
          break;
      }
      printAstTree(((astExprUnaryNode*)node)->expr);
      break;
    }
    case N_BINARY: {
      printf("(");
      printAstTree(((astExprBinaryNode*)node)->left);
      switch (((astExprBinaryNode*)node)->op) {
        case OP_ASSIGN:
          printf(" := ");
          break;
        case OP_PLUS:
          printf(" + ");
          break;
        case OP_MINUS:
          printf(" - ");
          break;
        case OP_MUL:
          printf(" * ");
          break;
        case OP_DIV:
          printf(" / ");
          break;
        case OP_MOD:
          printf(" mod ");
          break;
        case OP_EQ:
          printf(" == ");
          break;
        case OP_NEQ:
          printf(" != ");
          break;
        case OP_LT:
          printf(" < ");
          break;
        case OP_LTE:
          printf(" <= ");
          break;
        case OP_GT:
          printf(" > ");
          break;
        case OP_GTE:
          printf(" >= ");
          break;
        case OP_AND:
          printf(" AND ");
          break;
        case OP_OR:
          printf(" OR ");
          break;
      }
      printAstTree(((astExprBinaryNode*)node)->right);
      printf(")");
      break;
    }
    default:
      break;
  }
}

void burnAstTree(astNode* node) {
  if (node == NULL) return;
  switch (node->tag) {
    case N_STATEMENT: {
      burnAstTree(((astStatementNode*)node)->statement);
      burnAstTree(((astStatementNode*)node)->next);
      break;
    }
    case N_IF: {
      burnAstTree(((astIfNode*)node)->cond);
      burnAstTree(((astIfNode*)node)->thenBlk);
      burnAstTree(((astIfNode*)node)->elseBlk);
      break;
    }
    case N_WHILE: {
      burnAstTree(((astWhileNode*)node)->cond);
      burnAstTree(((astWhileNode*)node)->block);
      break;
    }
    case N_DO: {
      burnAstTree(((astDoNode*)node)->amount);
      burnAstTree(((astDoNode*)node)->block);
      break;
    }
    case N_READ: {
      burnAstTree(((astReadNode*)node)->var);
      break;
    }
    case N_WRITE: {
      burnAstTree(((astWriteNode*)node)->expr);
      break;
    }
    case N_UNARY: {
      burnAstTree(((astExprUnaryNode*)node)->expr);
      break;
    }
    case N_BINARY: {
      burnAstTree(((astExprBinaryNode*)node)->left);
      burnAstTree(((astExprBinaryNode*)node)->right);
      break;
    }
    case N_CALL: {
      burnAstTree(((astExprCallNode*)node)->var);
      break;
    }
    default:
      break;
  }
  free(node);
}

astStatementNode* newAstStatementNode(astNode* __s, astStatementNode* __n,
                                      int8_t aggregator) {
  astStatementNode* _n = (astStatementNode*)malloc(sizeof(astStatementNode));
  _n->tag = N_STATEMENT;
  _n->aggregator = aggregator;
  _n->statement = __s;
  _n->next = (astNode*)__n;
  return _n;
}

astIfNode* newAstIfNode(astNode* condition, astNode* thenBlk, astNode* elseBlk) {
  astIfNode* _n = (astIfNode*)malloc(sizeof(astIfNode));
  _n->tag = N_IF;
  _n->cond = condition;
  _n->thenBlk = thenBlk;
  _n->elseBlk = elseBlk;
  return _n;
}

astWhileNode* newAstWhileNode(astNode* condition, astNode* block) {
  astWhileNode* _n = (astWhileNode*)malloc(sizeof(astWhileNode));
  _n->tag = N_WHILE;
  _n->cond = condition;
  _n->block = block;
  return _n;  
}

astDoNode* newAstDoNode(astNode* amount, astNode* block) {
  astDoNode* _n = (astDoNode*)malloc(sizeof(astDoNode));
  _n->tag = N_DO;
  _n->amount = amount;
  _n->block = block;
  return _n;   
}

astReadNode* newAstReadNode(astExprVariableNode* __v) {
  astReadNode* _n = (astReadNode*)malloc(sizeof(astReadNode));
  _n->tag = N_READ;
  _n->var = (astNode*)__v;
  return _n;
}

astWriteNode* newAstWriteNode(astNode* __e) {
  astWriteNode* _n = (astWriteNode*)malloc(sizeof(astWriteNode));
  _n->tag = N_WRITE;
  _n->expr = __e;
  return _n;
}

astExprIntegerNode* newAstExprIntegerNode(int64_t __v) {
  astExprIntegerNode* _n =
      (astExprIntegerNode*)malloc(sizeof(astExprIntegerNode));
  _n->tag = N_INT;
  _n->value = __v;
  return _n;
}

astExprBooleanNode* newAstExprBooleanNode(bool __v) {
  astExprBooleanNode* _n =
      (astExprBooleanNode*)malloc(sizeof(astExprBooleanNode));
  _n->tag = N_BOOL;
  _n->value = __v;
  return _n;
}

astExprStringNode* newAstExprStringNode(char* __b) {
  int32_t len = strlen(__b);
  astExprStringNode* _n =
      (astExprStringNode*)malloc(sizeof(astExprStringNode) + len);
  _n->tag = N_STRING;
  strncpy(_n->string, __b, len + 1);
  return _n;
}

astExprVariableNode* newAstExprVariableNode(char* __b) {
  int32_t len = strlen(__b);
  astExprVariableNode* _n =
      (astExprVariableNode*)malloc(sizeof(astExprVariableNode) + len);
  _n->tag = N_VAR;
  strncpy(_n->name, __b, len + 1);
  return _n;
}

astExprUnaryNode* newAstExprUnaryNode(exprOperator __op, astNode* __e) {
  astExprUnaryNode* _n = (astExprUnaryNode*)malloc(sizeof(astExprUnaryNode));
  _n->tag = N_UNARY;
  _n->op = __op;
  _n->expr = __e;
  return _n;
}

astExprBinaryNode* newAstExprBinaryNode(exprOperator __op, astNode* __l,
                                        astNode* __r) {
  astExprBinaryNode* _n = (astExprBinaryNode*)malloc(sizeof(astExprBinaryNode));
  _n->tag = N_BINARY;
  _n->op = __op;
  _n->left = __l;
  _n->right = __r;
  return _n;
}

astExprCallNode* newAstExprCallNode(const char* name, int8_t aggr, astNode* var) {
  int32_t len = strlen(name);
  astExprCallNode* node = (astExprCallNode*)malloc(sizeof(astExprCallNode) + len);
  node->tag = N_CALL;
  node->aggr = aggr;
  node->var = var;
  strncpy(node->name, name, len + 1);
  return node;
}

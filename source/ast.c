#include <string.h>

#include "lang.h"

void printAstTree(astNode* node) {
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
    case N_ASSERT: {
      printf("ASSERT ");
      printAstTree(((astAssertNode*)node)->expr);
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
    case N_CALL: {
      printf("CALL %s (aggregator %d)", ((astExprCallNode*)node)->name,
             ((astExprCallNode*)node)->aggr);
      if (((astExprCallNode*)node)->var) {
        printf(" -> ");
        printAstTree(((astExprCallNode*)node)->var);
      }
      printf("\n");
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
    case N_ASSERT: {
      burnAstTree(((astAssertNode*)node)->expr);
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

astStatementNode* newAstStatementNode(astNode* statement,
                                      astStatementNode* next,
                                      int8_t aggregator) {
  astStatementNode* node = (astStatementNode*)malloc(sizeof(astStatementNode));
  node->tag = N_STATEMENT;
  node->aggregator = aggregator;
  node->statement = statement;
  node->next = (astNode*)next;
  return node;
}

astIfNode* newAstIfNode(astNode* condition, astNode* thenBlk,
                        astNode* elseBlk) {
  astIfNode* node = (astIfNode*)malloc(sizeof(astIfNode));
  node->tag = N_IF;
  node->cond = condition;
  node->thenBlk = thenBlk;
  node->elseBlk = elseBlk;
  return node;
}

astWhileNode* newAstWhileNode(astNode* condition, astNode* block) {
  astWhileNode* node = (astWhileNode*)malloc(sizeof(astWhileNode));
  node->tag = N_WHILE;
  node->cond = condition;
  node->block = block;
  return node;
}

astDoNode* newAstDoNode(astNode* amount, astNode* block) {
  astDoNode* node = (astDoNode*)malloc(sizeof(astDoNode));
  node->tag = N_DO;
  node->amount = amount;
  node->block = block;
  return node;
}

astReadNode* newAstReadNode(astExprVariableNode* var) {
  astReadNode* node = (astReadNode*)malloc(sizeof(astReadNode));
  node->tag = N_READ;
  node->var = (astNode*)var;
  return node;
}

astWriteNode* newAstWriteNode(astNode* expr) {
  astWriteNode* node = (astWriteNode*)malloc(sizeof(astWriteNode));
  node->tag = N_WRITE;
  node->expr = expr;
  return node;
}

astAssertNode* newAstAssertNode(astNode* expr) {
  astAssertNode* node = (astAssertNode*)malloc(sizeof(astAssertNode));
  node->tag = N_ASSERT;
  node->expr = expr;
  return node;
}

astExprIntegerNode* newAstExprIntegerNode(int64_t value) {
  astExprIntegerNode* node =
      (astExprIntegerNode*)malloc(sizeof(astExprIntegerNode));
  node->tag = N_INT;
  node->value = value;
  return node;
}

astExprBooleanNode* newAstExprBooleanNode(bool value) {
  astExprBooleanNode* node =
      (astExprBooleanNode*)malloc(sizeof(astExprBooleanNode));
  node->tag = N_BOOL;
  node->value = value;
  return node;
}

astExprStringNode* newAstExprStringNode(char* buffer) {
  int32_t len = strlen(buffer);
  astExprStringNode* node =
      (astExprStringNode*)malloc(sizeof(astExprStringNode) + len);
  node->tag = N_STRING;
  strncpy(node->string, buffer, len + 1);
  return node;
}

astExprVariableNode* newAstExprVariableNode(char* buffer) {
  int32_t len = strlen(buffer);
  astExprVariableNode* node =
      (astExprVariableNode*)malloc(sizeof(astExprVariableNode) + len);
  node->tag = N_VAR;
  strncpy(node->name, buffer, len + 1);
  return node;
}

astExprUnaryNode* newAstExprUnaryNode(exprOperator op, astNode* expr) {
  astExprUnaryNode* node = (astExprUnaryNode*)malloc(sizeof(astExprUnaryNode));
  node->tag = N_UNARY;
  node->op = op;
  node->expr = expr;
  return node;
}

astExprBinaryNode* newAstExprBinaryNode(exprOperator op, astNode* left,
                                        astNode* right) {
  astExprBinaryNode* node =
      (astExprBinaryNode*)malloc(sizeof(astExprBinaryNode));
  node->tag = N_BINARY;
  node->op = op;
  node->left = left;
  node->right = right;
  return node;
}

astExprCallNode* newAstExprCallNode(const char* name, int8_t aggr,
                                    astNode* var) {
  int32_t len = strlen(name);
  astExprCallNode* node =
      (astExprCallNode*)malloc(sizeof(astExprCallNode) + len);
  node->tag = N_CALL;
  node->aggr = aggr;
  node->var = var;
  strncpy(node->name, name, len + 1);
  return node;
}

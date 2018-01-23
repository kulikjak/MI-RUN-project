#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"

void printAstTree(astNode* node) {  // TODO
  if (node == NULL) return;
  switch (node->tag) {
    case N_BLOCK: {
      printAstTree(((astBlockNode*)node)->statement);
      break;
    }
    case N_STATEMENT: {
      printAstTree(((astStatementNode*)node)->statement);
      printf("\n");
      printAstTree(((astStatementNode*)node)->next);
      break;
    }
    case N_ASIGN: {
      printAstTree(((astAsignNode*)node)->left);
      printf(" = ");
      printAstTree(((astAsignNode*)node)->right);
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
    case N_INCREMENT: {
      printf("INCREMENT \n");
      printAstTree(((astIncrementNode*)node)->var);
      printf(" BY %d", ((astIncrementNode*)node)->amount);
      break;
    }
    case N_INT: {
    	printf("%d", ((astExprIntegerNode*)node)->value);
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
      switch (((astExprUnaryNode*)node)->op) {
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
    case N_BLOCK: {
      burnAstTree(((astBlockNode*)node)->statement);
      break;
    }
    case N_STATEMENT: {
      burnAstTree(((astStatementNode*)node)->statement);
      burnAstTree(((astStatementNode*)node)->next);
      break;
    }
    case N_ASIGN: {
      burnAstTree(((astAsignNode*)node)->left);
      burnAstTree(((astAsignNode*)node)->right);
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
    case N_READ: {
      burnAstTree(((astReadNode*)node)->var);
      break;
    }
    case N_WRITE: {
      burnAstTree(((astWriteNode*)node)->expr);
      break;
    }
    case N_INCREMENT: {
      burnAstTree(((astIncrementNode*)node)->var);
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
    default:
      break;
  }
  free(node);
}

astBlockNode* newAstBlockNode(astStatementNode* __s) {
  astBlockNode* _n = (astBlockNode*)malloc(sizeof(astBlockNode));
  _n->tag = N_BLOCK;
  _n->statement = (astNode*)__s;
  return _n;
}

astStatementNode* newAstStatementNode(astNode* __s, astStatementNode* __n, int8_t aggregator) {
  astStatementNode* _n = (astStatementNode*)malloc(sizeof(astStatementNode));
  _n->tag = N_STATEMENT;
  _n->aggregator = aggregator;
  _n->statement = __s;
  _n->next = (astNode*)__n;
  return _n;
}

astAsignNode* newAstAsignNode(astExprVariableNode* __v, astNode* __e) {
  astAsignNode* _n = (astAsignNode*)malloc(sizeof(astAsignNode));
  _n->tag = N_ASIGN;
  _n->left = (astNode*)__v;
  _n->right = __e;
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

astIncrementNode* newAstIncrementNode(astExprVariableNode* __v, int __a) {
  astIncrementNode* _n = (astIncrementNode*)malloc(sizeof(astIncrementNode));
  _n->tag = N_INCREMENT;
  _n->var = (astNode*)__v;
  _n->amount = __a;
  return _n;
}

/*astTypeNode* newAstTypeNode(varType __t) {
  astTypeNode* _n = (astTypeNode*)malloc(sizeof(astTypeNode));
  _n->tag = N_TYPE;
  _n->type = __t;
  return _n;
}*/

astExprIntegerNode* newAstExprIntegerNode(int __v) {
  astExprIntegerNode* _n =
      (astExprIntegerNode*)malloc(sizeof(astExprIntegerNode));
  _n->tag = N_INT;
  _n->value = __v;
  return _n;
}

astExprVariableNode* newAstExprVariableNode(char* __b) {
  astExprVariableNode* _n =
      (astExprVariableNode*)malloc(sizeof(astExprVariableNode) + sizeof(__b));
  _n->tag = N_VAR;
  strncpy(_n->name, __b, sizeof(__b) + 1);
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

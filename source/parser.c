#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexan.h"
#include "prototypes.h"
#include "utils.h"

astBlockNode *Block();
astStatementNode *Line();

astNode *Expression();
astNode *ExpressionRest(astNode *du);
astNode *SimpleExpression();
astNode *SimpleExpressionRest(astNode *du);
astNode *Term();
astNode *TermRest(astNode *du);
astNode *Factor();

LexicalSymbol Symb;

void Match(LexSymbolType token) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  Symb = readLexem();
}

void MatchGetString(LexSymbolType token, char *id) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  strcpy(id, Symb.ident);
  Symb = readLexem();
}

void MatchGetNumber(LexSymbolType token, int *h) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  *h = Symb.number;
  Symb = readLexem();
}

bool Check(LexSymbolType s) { return (Symb.type == s); }

astBlockNode *Block() {
  debug("Block");

  astStatementNode *first = Line();
  astStatementNode *last = first;
  Match(ENDL);

  while (!Check(kwDONE) && !Check(EOI)) {
    astStatementNode *curr = Line();
    Match(ENDL);

    last->next = (astNode *)curr;
    last = curr;
  }
  return newAstBlockNode(first);
}

astStatementNode *Line() {
  debug("Line");

  switch (Symb.type) {
    case kwREAD: {
      Symb = readLexem();

      // Check for read type identifier
      astTypeNode *type = NULL;
      if (Symb.type == kwINTEGER) {
        type = newAstTypeNode(T_INTEGER);
        Symb = readLexem();
      }
      Match(kwINTO);

      // read desc variable name
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      // create nodes for read statement
      astExprVariableNode *var = newAstExprVariableNode(buffer);
      astReadNode *rnode = newAstReadNode(type, var);

      astStatementNode *statement = newAstStatementNode((astNode *)rnode, NULL);
      return statement;
    }
    case kwWRITE: {
      Symb = readLexem();

      // parse expression after write keyword
      astNode *expr = Expression();

      // create nodes for write statement
      astWriteNode *wnode = newAstWriteNode(expr);
      astStatementNode *statement = newAstStatementNode((astNode *)wnode, NULL);
      return statement;
    }
    case kwWHILE: {
      Symb = readLexem();
      fatal("Not implemented");
      // TODO
      break;
    }
    case kwIF: {
      Symb = readLexem();
      fatal("Not implemented");
      // TODO
      break;
    }
    case kwDO: {
      Symb = readLexem();
      fatal("Not implemented");
      // TODO
      break;
    }
    case kwINCREMENT:
    case kwDECREMENT: {
      // set default amount based on keyword
      int amount = (Symb.type == kwINCREMENT) ? 1 : -1;

      Symb = readLexem();

      // read desc variable name
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      // create node for variable
      astExprVariableNode *var = newAstExprVariableNode(buffer);

      // check for additional info about increment / decrement size
      if (Check(kwBY)) {
        Match(kwBY);
        if (amount == -1) {
          MatchGetNumber(NUMB, &amount);
          amount *= -1;
        } else {
          MatchGetNumber(NUMB, &amount);
        }
      }

      // create nodes for increment / decrement statement
      astIncrementNode *incr = newAstIncrementNode(var, amount);
      astStatementNode *statement = newAstStatementNode((astNode *)incr, NULL);
      return statement;
    }
    case IDENT: {
      // read variable name of identificator
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      // create node for variable
      astExprVariableNode *var = newAstExprVariableNode(buffer);

      Match(kwIS);

      // parse expression after write keyword
      astNode *expr = Expression();

      // create nodes for write statement
      astAsignNode *anode = newAstAsignNode(var, (astNode *)expr);
      astStatementNode *statement = newAstStatementNode((astNode *)anode, NULL);
      return statement;
    }
    case ENDL: {
      Symb = readLexem();
      return Line();
    }
    default: {}
  }
  _UNREACHABLE;
  return newAstStatementNode(NULL, NULL);
}

astNode *Expression(void) {
  debug("Expression");
  return ExpressionRest(SimpleExpression());
}

astNode *ExpressionRest(astNode *du) {
  debug("ExpressionRest");
  if (Check(kwIS)) {
    Symb = readLexem();
    switch (Symb.type) {
      case kwLESS: {
        Symb = readLexem();
        if (Check(kwOR)) {
          Symb = readLexem();
          Match(kwEQUAL);
          Match(kwTO);
          return ExpressionRest(
              (astNode *)newAstExprBinaryNode(OP_LTE, du, SimpleExpression()));
        } else {
          Match(kwTHAN);
          return ExpressionRest(
              (astNode *)newAstExprBinaryNode(OP_LT, du, SimpleExpression()));
        }
      }
      case kwGREATER: {
        Symb = readLexem();
        if (Check(kwOR)) {
          Symb = readLexem();
          Match(kwEQUAL);
          Match(kwTO);
          return ExpressionRest(
              (astNode *)newAstExprBinaryNode(OP_GTE, du, SimpleExpression()));
        } else {
          Match(kwTHAN);
          return ExpressionRest(
              (astNode *)newAstExprBinaryNode(OP_GT, du, SimpleExpression()));
        }
      }
      case kwNOT:
        Symb = readLexem();
        Match(kwEQUAL);
        Match(kwTO);
        return ExpressionRest(
            (astNode *)newAstExprBinaryNode(OP_NEQ, du, SimpleExpression()));
      case kwEQUAL:
        Symb = readLexem();
        Match(kwTO);
        return ExpressionRest(
            (astNode *)newAstExprBinaryNode(OP_EQ, du, SimpleExpression()));
      default:
        fprintf(stderr, "error here");
        exit(EXIT_FAILURE);
    }
  }
  return du;
}

astNode *SimpleExpression(void) {
  debug("SimpleExpression");
  return SimpleExpressionRest(Term());
}

astNode *SimpleExpressionRest(astNode *du) {
  debug("SimpleExpressionRest");
  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return SimpleExpressionRest(
          (astNode *)newAstExprBinaryNode(OP_PLUS, du, Term()));
    case MINUS:
      Symb = readLexem();
      return SimpleExpressionRest(
          (astNode *)newAstExprBinaryNode(OP_MINUS, du, Term()));
    case kwOR:
      Symb = readLexem();
      return SimpleExpressionRest(
          (astNode *)newAstExprBinaryNode(OP_OR, du, Term()));
    default:
      return du;
  }
}

astNode *Term(void) {
  debug("Term");
  return TermRest(Factor());
}

astNode *TermRest(astNode *du) {
  debug("TermRest");
  switch (Symb.type) {
    case MUL:
      Symb = readLexem();
      return TermRest((astNode *)newAstExprBinaryNode(OP_MUL, du, Factor()));
    case DIV:
      Symb = readLexem();
      return TermRest((astNode *)newAstExprBinaryNode(OP_DIV, du, Factor()));
    case MOD:
      Symb = readLexem();
      return TermRest((astNode *)newAstExprBinaryNode(OP_MOD, du, Factor()));
    case kwAND:
      Symb = readLexem();
      return TermRest((astNode *)newAstExprBinaryNode(OP_AND, du, Factor()));
    default:
      return du;
  }
}

astNode *Factor(void) {
  debug("Factor");
  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return Factor();
    case MINUS:
      Symb = readLexem();
      return (astNode *)newAstExprUnaryNode(OP_MINUS, Factor());
    case LPAR: {
      Symb = readLexem();
      astNode *expr = Expression();
      Match(RPAR);
      return expr;
    }
    case NUMB: {
      int value;
      MatchGetNumber(NUMB, &value);

      return (astNode *)newAstExprIntegerNode(value);
    }
    case IDENT: {
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      return (astNode *)newAstExprVariableNode(buffer);
    }
    default:
      printf("Error in expression expansion");
      printf("%s\n", symbTable[Symb.type]);
      exit(EXIT_FAILURE);
  }
}

/*
void Block(TreeBlockNode *b) {
  Debug("Block");
  LocalDeclarations(b);
  CompoundStatement(b);
}

void LocalDeclarations(TreeBlockNode *b) {
  Debug("LocalDeclarations");
  switch (Symb.type) {
    case kwCONST: {
      Symb = readLexem();

      int value;
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      Match(EQ);
      if (Check(MINUS)) {
        Symb = readLexem();
        MatchGetNumber(NUMB, value);
        value = value * (-1);
      } else
        MatchGetNumber(NUMB, value);
      Match(SEMICOLON);

      b->addDecl(new ConstDeclNode(string, value));
      RestConstDeclarations(b);
      LocalDeclarations(b);
      break;
    }
    case kwVAR: {
      Symb = readLexem();
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);

      TreeTypeNode *t = RestIdents(b);
      b->addDecl(new VarDeclNode(string, t));

      RestVarDeclarations(b);
      LocalDeclarations(b);
      break;
    }
    default:
      break;
  }
}

void RestConstDeclarations(TreeBlockNode *b) {
  Debug("RestConstDeclarations");
  if (Check(IDENT)) {
    int value;
    char string[MAX_IDENT_LEN];
    MatchGetString(IDENT, string);
    Match(EQ);
    if (Check(MINUS)) {
      Symb = readLexem();
      MatchGetNumber(NUMB, value);
      value = value * (-1);
    } else
      MatchGetNumber(NUMB, value);
    Match(SEMICOLON);

    b->addDecl(new ConstDeclNode(string, value));
    RestConstDeclarations(b);
  }
}

void RestVarDeclarations(TreeBlockNode *b) {
  Debug("RestVarDeclarations");
  if (Check(IDENT)) {
    char string[MAX_IDENT_LEN];
    MatchGetString(IDENT, string);

    TreeTypeNode *t = RestIdents(b);
    b->addDecl(new VarDeclNode(string, t));

    RestVarDeclarations(b);
  }
}

TreeTypeNode *RestIdents(TreeBlockNode *b) {
  Debug("RestIdents");
  switch (Symb.type) {
    case COMMA: {
      Symb = readLexem();
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);

      TreeTypeNode *t = RestIdents(b);
      b->addDecl(new VarDeclNode(string, t));

      return t->clone();
    }
    case COLON: {
      Symb = readLexem();
      TreeTypeNode *t = Type();
      Match(SEMICOLON);

      return t;
    }
    default:
      return NULL;
  }
}

void CompoundStatement(TreeBlockNode *b) {
  Debug("CompoundStatement");
  Match(kwBEGIN);
  Statement(b);
  CompoundStatementRest(b);
  Match(kwEND);
}

void CompoundStatementRest(TreeBlockNode *b) {
  Debug("CompoundStatementRest");
  if (Symb.type == SEMICOLON) {
    Symb = readLexem();
    Statement(b);
    CompoundStatementRest(b);
  }
}

void Statement(TreeBlockNode *b) {
  Debug("Statement");
  switch (Symb.type) {
    case IDENT: {
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      switch (Symb.type) {
        case LPAR:  // function call
        {
          Symb = readLexem();
          TreeFunctionENode *f = new TreeFunctionENode(string);
          FunctionArguments(f);
          Match(RPAR);
          b->addStmt(new TreeCallNode(f));
          break;
        }
        case LBRAC: {
          Symb = readLexem();
          TreeExpressionNode *e = Expression();
          Match(RBRAC);
          TreeExpressionNode *expr = new TreeArrayENode(string, e);

          Match(ASSIGN);
          TreeExpressionNode *f = Expression();
          b->addStmt(new TreeAsignNode(expr, f));
          break;
        }
        case ASSIGN: {
          Symb = readLexem();
          TreeExpressionNode *e = Expression();
          TreeExpressionNode *expr = new TreeVariableENode(string);
          b->addStmt(new TreeAsignNode(expr, e));
          break;
        }
        default:
          break;
      }
      break;
    }
    case kwIF: {
      Symb = readLexem();
      TreeExpressionNode *e = Expression();
      Match(kwTHEN);
      TreeBlockNode *body = new TreeBlockNode();
      Statement(body);

      if (Check(kwELSE)) {
        Symb = readLexem();
        TreeBlockNode *elbody = new TreeBlockNode();
        Statement(elbody);
        b->addStmt(new TreeIfNode(e, body, elbody));
      } else
        b->addStmt(new TreeIfNode(e, body, NULL));
      break;
    }
    case kwWHILE: {
      Symb = readLexem();
      TreeExpressionNode *e = Expression();
      Match(kwDO);

      TreeBlockNode *body = new TreeBlockNode();
      body->addStmt(new BreakNode(new TreeUnaryENode(kwNOT, e)));
      Statement(body);
      b->addStmt(new TreeWhileNode(body));
      break;
    }
    case kwFOR: {
      Symb = readLexem();

      bool direction;
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      Match(ASSIGN);
      TreeExpressionNode *e = Expression();
      switch (Symb.type) {
        case kwTO: {
          Symb = readLexem();
          direction = true;
          break;
        }
        case kwDOWNTO: {
          Symb = readLexem();
          direction = false;
          break;
        }
        default:
          break;
      }
      TreeExpressionNode *f = Expression();
      Match(kwDO);

      TreeBlockNode *body = new TreeBlockNode();
      body->addStmt(new BreakNode(new TreeBinaryENode(
          (direction ? GT : LT), new TreeVariableENode(string), f)));

      Statement(body);

      body->addStmt(new TreeStatementBopNode((direction ? kwINC : kwDEC),
                                             new TreeVariableENode(string),
                                             new TreeIntegerENode(1)));

      b->addStmt(new TreeAsignNode(new TreeVariableENode(string), e));
      b->addStmt(new TreeWhileNode(body));

      break;
    }
    case kwREADLN: {
      Symb = readLexem();
      Match(LPAR);

      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      TreeExpressionNode *expr;
      if (Check(LBRAC)) {
        Symb = readLexem();
        TreeExpressionNode *e = Expression();
        Match(RBRAC);
        expr = new TreeArrayENode(string, e);
      } else
        expr = new TreeVariableENode(string);
      Match(RPAR);

      b->addStmt(new ReadNode(expr));
      break;
    }
    case kwWRITELN: {
      Symb = readLexem();
      Match(LPAR);
      TreeExpressionNode *e = Expression();
      Match(RPAR);

      b->addStmt(new WriteNode(true, e));
      break;
    }
    case kwWRITE: {
      Symb = readLexem();
      Match(LPAR);

      char string[MAX_IDENT_LEN];
      MatchGetString(STRING, string);
      Match(RPAR);

      b->addStmt(new WriteNode(false, new TreeStringENode(string)));
      break;
    }
    case kwBEGIN: {
      Block(b);
      break;
    }
    case kwINC: {
      Symb = readLexem();
      Match(LPAR);

      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      TreeExpressionNode *expr;
      if (Check(LBRAC)) {
        Symb = readLexem();
        TreeExpressionNode *e = Expression();
        Match(RBRAC);
        expr = new TreeArrayENode(string, e);
      } else
        expr = new TreeVariableENode(string);
      Match(RPAR);

      b->addStmt(
          new TreeStatementBopNode(kwINC, expr, new TreeIntegerENode(1)));
      break;
    }
    case kwDEC: {
      Symb = readLexem();
      Match(LPAR);

      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);
      TreeExpressionNode *expr;
      if (Check(LBRAC)) {
        Symb = readLexem();
        TreeExpressionNode *e = Expression();
        Match(RBRAC);
        expr = new TreeArrayENode(string, e);
      } else
        expr = new TreeVariableENode(string);
      Match(RPAR);

      b->addStmt(
          new TreeStatementBopNode(kwDEC, expr, new TreeIntegerENode(1)));
      break;
    }
    case kwEXIT: {
      b->addStmt(new ExitNode());
      Symb = readLexem();
      break;
    }
    default:
      break;
  }
}*/

void initParser(const char *fileName) {
  initLexan(fileName);
  Symb = readLexem();
}

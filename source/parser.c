#include <string.h>

#include "lang.h"

int8_t aggregator();

appContext* Program();
void GlobalDeclarations(appContext*);
astBlockNode* Block();
astStatementNode* Line();

astNode* Expression();
astNode* AssignmentExpr();
astNode* AssignmentExprRest(astNode*);
astNode* LogicalOrExpr();
astNode* LogicalOrExprRest(astNode*);
astNode* LogicalAndExpr();
astNode* LogicalAndExprRest(astNode*);
astNode* EqualityExpr();
astNode* EqualityExprRest(astNode*);
astNode* RelationalExpr();
astNode* RelationalExprRest(astNode*);
astNode* AdditiveExpr();
astNode* AdditiveExprRest(astNode*);
astNode* MultiplicativeExpr();
astNode* MultiplicativeExprRest(astNode*);
astNode* UnaryExpr();

LexicalSymbol Symb;

void Match(LexSymbolType token) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  Symb = readLexem();
}

void MatchGetString(LexSymbolType token, char* id) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  strcpy(id, Symb.ident);
  Symb = readLexem();
}

void MatchGetNumber(LexSymbolType token, int* h) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  *h = Symb.number;
  Symb = readLexem();
}

bool Check(LexSymbolType s) { return (Symb.type == s); }

appContext* Program() {
  debug("Program");

  appContext* ctx = initAppContext();

  GlobalDeclarations(ctx);

  SKIP_ENDL;

  Match(kwMAIN);
  astBlockNode* body = Block();
  Match(ENDL);

  ctx->main = body;

  SKIP_ENDL;

  if (Symb.type != EOI) {
    fprintf(stderr, "Program does not end with main function.");
    exit(EXIT_FAILURE);
  }

  return ctx;
}

void GlobalDeclarations(appContext* ctx) {
  debug("GlobalDeclarations");

  SKIP_ENDL;

  switch (Symb.type) {
    case kwGLOBAL: {
      Symb = readLexem();
      Match(kwVARIABLE);

      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      newGlobalVariable(ctx, buffer);

      Match(ENDL);
      GlobalDeclarations(ctx);
    }
    /*case kwFUNCTION: {
      Symb = readLexem();
      char string[MAX_IDENT_LEN];
      MatchGetString(IDENT, string);

      if (Symb.type == kwARGS) {
        Symb = readLexem();
        // TODO
        FunctionArguments( mem );
      }

      astBlockNode* body = Block();

      Match(ENDL);
      GlobalDeclarations(ctx);
    }*/
    default:
      break;
  }
}

/*void FunctionArguments( xxx* args ) {

  debug( "FuncArguments" );
  if(Check(IDENT)){

    char string[MAX_IDENT_LEN];
    MatchGetString(IDENT, string);

    // TOOD f->addArgument( string, t );
    FunctionArguments( args );
  }
}*/

astBlockNode* Block() {
  debug("Block");

  Match(kwBEGIN);
  Match(ENDL);

  astStatementNode* first = Line();
  astStatementNode* last = first;

  while (!Check(kwEND) && !Check(EOI)) {
    astStatementNode* curr = Line();

    last->next = (astNode*)curr;
    last = curr;
  }
  Match(kwEND);

  return newAstBlockNode(first);
}

int8_t aggregator() {
  int8_t value = -1;

  if (Check(AGGREGATOR)) {
    value = Symb.number;
    Symb = readLexem();
  }
  return value;
}

astStatementNode* Line() {
  debug("Line");

  SKIP_ENDL;

  int8_t agg = aggregator();

  switch (Symb.type) {
    case kwREAD: {
      Symb = readLexem();

      // Check for read type identifier
      Match(kwINTO);

      // read desc variable name
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);
      Match(ENDL);

      // create nodes for read statement
      astExprVariableNode* var = newAstExprVariableNode(buffer);
      astReadNode* rnode = newAstReadNode(var);

      astStatementNode* statement =
          newAstStatementNode((astNode*)rnode, NULL, agg);
      return statement;
    }
    case kwWRITE: {
      Symb = readLexem();

      // parse expression after write keyword
      astNode* expr = Expression();
      Match(ENDL);

      // create nodes for write statement
      astWriteNode* wnode = newAstWriteNode(expr);
      astStatementNode* statement =
          newAstStatementNode((astNode*)wnode, NULL, agg);
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
      astExprVariableNode* var = newAstExprVariableNode(buffer);

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
      Match(ENDL);

      // create nodes for increment / decrement statement
      astIncrementNode* incr = newAstIncrementNode(var, amount);
      astStatementNode* statement =
          newAstStatementNode((astNode*)incr, NULL, agg);
      return statement;
    }
    default: {
      astStatementNode* statement =
          newAstStatementNode((astNode*)Expression(), NULL, agg);
      Match(ENDL);

      return statement;
    }
  }
  _UNREACHABLE;
  return newAstStatementNode(NULL, NULL, 0);
}

astNode* Expression() { return AssignmentExpr(); }

astNode* AssignmentExpr() {
  debug("AssignmentExpr");
  return AssignmentExprRest(LogicalOrExpr());
}

astNode* AssignmentExprRest(astNode* du) {
  debug("AssignmentExprRest");

  if (Symb.type == ASSIGN) {
    Symb = readLexem();
    return AssignmentExprRest(
        (astNode*)newAstExprBinaryNode(OP_ASSIGN, du, LogicalOrExpr()));
  }
  return du;
}

astNode* LogicalOrExpr() {
  debug("LogicalOrExpr");
  return LogicalOrExprRest(LogicalAndExpr());
}

astNode* LogicalOrExprRest(astNode* du) {
  debug("LogicalOrExprRest");

  if (Symb.type == kwOR) {
    Symb = readLexem();
    return LogicalOrExprRest(
        (astNode*)newAstExprBinaryNode(OP_OR, du, LogicalAndExpr()));
  }
  return du;
}

astNode* LogicalAndExpr() {
  debug("LogicalAndExpr");
  return LogicalAndExprRest(EqualityExpr());
}

astNode* LogicalAndExprRest(astNode* du) {
  debug("LogicalAndExprRest");

  if (Symb.type == kwAND) {
    Symb = readLexem();
    return LogicalAndExprRest(
        (astNode*)newAstExprBinaryNode(OP_AND, du, EqualityExpr()));
  }
  return du;
}

astNode* EqualityExpr() {
  debug("EqualityExpr");
  return EqualityExprRest(RelationalExpr());
}

astNode* EqualityExprRest(astNode* du) {
  debug("EqualityExprRest");

  switch (Symb.type) {
    case EQ:
      Symb = readLexem();
      return EqualityExprRest(
          (astNode*)newAstExprBinaryNode(OP_EQ, du, RelationalExpr()));
    case NEQ:
      Symb = readLexem();
      return EqualityExprRest(
          (astNode*)newAstExprBinaryNode(OP_NEQ, du, RelationalExpr()));
    default:
      return du;
  }
}

astNode* RelationalExpr() {
  debug("RelationalExpr");
  return RelationalExprRest(AdditiveExpr());
}

astNode* RelationalExprRest(astNode* du) {
  debug("RelationalExprRest");

  switch (Symb.type) {
    case LTE:
      Symb = readLexem();
      return RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_LTE, du, AdditiveExpr()));
    case LT:
      Symb = readLexem();
      return RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_LT, du, AdditiveExpr()));
    case GTE:
      Symb = readLexem();
      return RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_GTE, du, AdditiveExpr()));
    case GT:
      Symb = readLexem();
      return RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_GT, du, AdditiveExpr()));
    default:
      return du;
  }
}

astNode* AdditiveExpr() {
  debug("AdditiveExpr");
  return AdditiveExprRest(MultiplicativeExpr());
}

astNode* AdditiveExprRest(astNode* du) {
  debug("AdditiveExprRest");

  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return AdditiveExprRest(
          (astNode*)newAstExprBinaryNode(OP_PLUS, du, MultiplicativeExpr()));
    case MINUS:
      Symb = readLexem();
      return AdditiveExprRest(
          (astNode*)newAstExprBinaryNode(OP_MINUS, du, MultiplicativeExpr()));
    default:
      return du;
  }
}

astNode* MultiplicativeExpr() {
  debug("MultiplicativeExpr");
  return MultiplicativeExprRest(UnaryExpr());
}

astNode* MultiplicativeExprRest(astNode* du) {
  debug("MultiplicativeExprRest");

  switch (Symb.type) {
    case MUL:
      Symb = readLexem();
      return MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_MUL, du, UnaryExpr()));
    case DIV:
      Symb = readLexem();
      return MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_DIV, du, UnaryExpr()));
    case MOD:
      Symb = readLexem();
      return MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_MOD, du, UnaryExpr()));
    default:
      return du;
  }
}

astNode* UnaryExpr() {
  debug("UnaryExpr");

  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return UnaryExpr();
    case MINUS:
      Symb = readLexem();
      return (astNode*)newAstExprUnaryNode(OP_MINUS, UnaryExpr());
    case LPAR: {
      Symb = readLexem();
      astNode* expr = Expression();
      Match(RPAR);
      return expr;
    }
    case NUMB: {
      int value;
      MatchGetNumber(NUMB, &value);

      return (astNode*)newAstExprIntegerNode(value);
    }
    case IDENT: {
      char buffer[MAX_IDENT_LEN];
      MatchGetString(IDENT, buffer);

      switch (Symb.type) {
        case LPAR: {
          fatal("Not implemented");
          /*Symb = readLexem();
          TreeFunctionENode *f = new TreeFunctionENode( string );
          FunctionArguments( f );
          Match(RPAR);

          return f;*/
        }
        default: { return (astNode*)newAstExprVariableNode(buffer); }
      }
    }
    default:
      printf("Error in expression expansion");
      printf("%s\n", symbTable[Symb.type]);
      exit(EXIT_FAILURE);
  }
}

/*

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
*/

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

void initParser(const char* fileName) {
  initLexan(fileName);
  Symb = readLexem();
}

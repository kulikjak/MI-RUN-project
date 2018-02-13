#include <string.h>

#include "lang.h"

int8_t _Aggregator();
void _GlobalDeclarations();
astStatementNode* _Block();
astStatementNode* _Line();

astNode* _Expression();
astNode* _AssignmentExpr();
astNode* _AssignmentExprRest(astNode*);
astNode* _LogicalOrExpr();
astNode* _LogicalOrExprRest(astNode*);
astNode* _LogicalAndExpr();
astNode* _LogicalAndExprRest(astNode*);
astNode* _EqualityExpr();
astNode* _EqualityExprRest(astNode*);
astNode* _RelationalExpr();
astNode* _RelationalExprRest(astNode*);
astNode* _AdditiveExpr();
astNode* _AdditiveExprRest(astNode*);
astNode* _MultiplicativeExpr();
astNode* _MultiplicativeExprRest(astNode*);
astNode* _UnaryExpr();

LexicalSymbol Symb;

void _Match(LexSymbolType token) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  Symb = readLexem();
}

void _MatchGetString(LexSymbolType token, char* id) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  strcpy(id, Symb.ident);
  Symb = readLexem();
}

void _MatchGetNumber(LexSymbolType token, int64_t* h) {
  if (Symb.type != token) {
    printf("Match error: expected %s, got %s ...\n", symbTable[token],
           symbTable[Symb.type]);
    exit(EXIT_FAILURE);
  }
  *h = Symb.number;
  Symb = readLexem();
}

bool _Check(LexSymbolType s) { return (Symb.type == s); }

void ParseProgram() {
  debug("ParseProgram");

  _GlobalDeclarations();
  SKIP_ENDL;

  _Match(kwMAIN);
  astStatementNode* body = _Block();
  _Match(ENDL);

  gAppCtx.main = body;
  SKIP_ENDL;

  if (Symb.type != EOI) {
    fprintf(stderr, "Program does not end with main function.");
    exit(EXIT_FAILURE);
  }
}

void _GlobalDeclarations() {
  debug("_GlobalDeclarations");

  SKIP_ENDL;

  switch (Symb.type) {
    case kwGLOBAL: {
      Symb = readLexem();
      _Match(kwVARIABLE);

      char buffer[MAX_IDENT_LEN];
      _MatchGetString(IDENT, buffer);

      newGlobalVariable(buffer);

      _Match(ENDL);
      _GlobalDeclarations();
    }
    default:
      break;
  }
}

astStatementNode* _Block() {
  debug("_Block");

  _Match(kwBEGIN);
  _Match(ENDL);

  astStatementNode* first = _Line();
  astStatementNode* last = first;

  while (!_Check(kwEND) && !_Check(EOI)) {
    astStatementNode* curr = _Line();

    last->next = (astNode*)curr;
    last = curr;

    SKIP_ENDL;
  }
  _Match(kwEND);

  return first;
}

int8_t _Aggregator() {
  debug("_Aggregator");
  int8_t value = -1;

  if (_Check(AGGREGATOR)) {
    value = Symb.number;
    Symb = readLexem();
  }
  return value;
}

astStatementNode* _Line() {
  debug("_Line");

  SKIP_ENDL;

  int8_t agg = _Aggregator();

  switch (Symb.type) {
    case kwREAD: {
      Symb = readLexem();

      // Check for read type identifier
      _Match(kwINTO);

      // read desc variable name
      char buffer[MAX_IDENT_LEN];
      _MatchGetString(IDENT, buffer);
      _Match(ENDL);

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
      astNode* expr = _Expression();
      _Match(ENDL);

      // create nodes for write statement
      astWriteNode* wnode = newAstWriteNode(expr);
      astStatementNode* statement =
          newAstStatementNode((astNode*)wnode, NULL, agg);
      return statement;
    }
    case kwIF: {
      Symb = readLexem();

      astNode* condition = (astNode*)_Expression();
      astNode* thenBlk = (astNode*)_Block();
      astNode* elseBlk = NULL;

      if (_Check(kwELSE)) {
        Symb = readLexem();
        elseBlk = (astNode*)_Block();
      }
      _Match(ENDL);

      astIfNode* statement = newAstIfNode(condition, thenBlk, elseBlk);
      return newAstStatementNode((astNode*)statement, NULL, agg);
    }
    case kwWHILE: {
      Symb = readLexem();

      astNode* condition = (astNode*)_Expression();
      astNode* block = (astNode*)_Block();
      astWhileNode* statement = newAstWhileNode(condition, block);

      _Match(ENDL);

      return newAstStatementNode((astNode*)statement, NULL, agg);
    }
    case kwDO: {
      Symb = readLexem();
      astNode* amount = (astNode*)_Expression();

      _Match(kwTIMES);
      astNode* block = (astNode*)_Block();
      astDoNode* statement = newAstDoNode(amount, block);

      _Match(ENDL);

      return newAstStatementNode((astNode*)statement, NULL, agg);
    }
    case kwASSERT: {
      Symb = readLexem();

      // parse expression after assert keyword
      astNode* expr = _Expression();
      _Match(ENDL);

      // create nodes for assertion statement
      astAssertNode* node = newAstAssertNode(expr);
      astStatementNode* statement =
          newAstStatementNode((astNode*)node, NULL, agg);
      return statement;
    }
    case kwINCREMENT:
    case kwDECREMENT: {
      // set default amount based on keyword
      int64_t amount = (Symb.type == kwINCREMENT) ? 1 : -1;

      Symb = readLexem();

      // read desc variable name
      char buffer[MAX_IDENT_LEN];
      _MatchGetString(IDENT, buffer);

      // check for additional info about increment / decrement size
      if (_Check(kwBY)) {
        _Match(kwBY);
        if (amount == -1) {
          _MatchGetNumber(NUMB, &amount);
          amount *= -1;
        } else {
          _MatchGetNumber(NUMB, &amount);
        }
      }
      _Match(ENDL);

      astNode* expr = (astNode*)newAstExprBinaryNode(
          OP_PLUS, (astNode*)newAstExprVariableNode(buffer),
          (astNode*)newAstExprIntegerNode(amount));
      astNode* incr = (astNode*)newAstExprBinaryNode(
          OP_ASSIGN, (astNode*)newAstExprVariableNode(buffer), expr);
      astStatementNode* statement = newAstStatementNode(incr, NULL, agg);

      return statement;
    }
    default: {
      astStatementNode* statement =
          newAstStatementNode((astNode*)_Expression(), NULL, agg);
      _Match(ENDL);

      return statement;
    }
  }
  _UNREACHABLE;
  return newAstStatementNode(NULL, NULL, 0);
}

astNode* _Expression() { return _AssignmentExpr(); }

astNode* _AssignmentExpr() {
  debug("_AssignmentExpr");
  return _AssignmentExprRest(_LogicalOrExpr());
}

astNode* _AssignmentExprRest(astNode* du) {
  debug("_AssignmentExprRest");

  if (Symb.type == ASSIGN) {
    Symb = readLexem();
    return _AssignmentExprRest(
        (astNode*)newAstExprBinaryNode(OP_ASSIGN, du, _LogicalOrExpr()));
  }
  return du;
}

astNode* _LogicalOrExpr() {
  debug("LogicalOrExpr");
  return _LogicalOrExprRest(_LogicalAndExpr());
}

astNode* _LogicalOrExprRest(astNode* du) {
  debug("_LogicalOrExprRest");

  if (Symb.type == kwOR) {
    Symb = readLexem();
    return _LogicalOrExprRest(
        (astNode*)newAstExprBinaryNode(OP_OR, du, _LogicalAndExpr()));
  }
  return du;
}

astNode* _LogicalAndExpr() {
  debug("LogicalAndExpr");
  return _LogicalAndExprRest(_EqualityExpr());
}

astNode* _LogicalAndExprRest(astNode* du) {
  debug("_LogicalAndExprRest");

  if (Symb.type == kwAND) {
    Symb = readLexem();
    return _LogicalAndExprRest(
        (astNode*)newAstExprBinaryNode(OP_AND, du, _EqualityExpr()));
  }
  return du;
}

astNode* _EqualityExpr() {
  debug("EqualityExpr");
  return _EqualityExprRest(_RelationalExpr());
}

astNode* _EqualityExprRest(astNode* du) {
  debug("_EqualityExprRest");

  switch (Symb.type) {
    case EQ:
      Symb = readLexem();
      return _EqualityExprRest(
          (astNode*)newAstExprBinaryNode(OP_EQ, du, _RelationalExpr()));
    case NEQ:
      Symb = readLexem();
      return _EqualityExprRest(
          (astNode*)newAstExprBinaryNode(OP_NEQ, du, _RelationalExpr()));
    default:
      return du;
  }
}

astNode* _RelationalExpr() {
  debug("RelationalExpr");
  return _RelationalExprRest(_AdditiveExpr());
}

astNode* _RelationalExprRest(astNode* du) {
  debug("_RelationalExprRest");

  switch (Symb.type) {
    case LTE:
      Symb = readLexem();
      return _RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_LTE, du, _AdditiveExpr()));
    case LT:
      Symb = readLexem();
      return _RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_LT, du, _AdditiveExpr()));
    case GTE:
      Symb = readLexem();
      return _RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_GTE, du, _AdditiveExpr()));
    case GT:
      Symb = readLexem();
      return _RelationalExprRest(
          (astNode*)newAstExprBinaryNode(OP_GT, du, _AdditiveExpr()));
    default:
      return du;
  }
}

astNode* _AdditiveExpr() {
  debug("AdditiveExpr");
  return _AdditiveExprRest(_MultiplicativeExpr());
}

astNode* _AdditiveExprRest(astNode* du) {
  debug("_AdditiveExprRest");

  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return _AdditiveExprRest(
          (astNode*)newAstExprBinaryNode(OP_PLUS, du, _MultiplicativeExpr()));
    case MINUS:
      Symb = readLexem();
      return _AdditiveExprRest(
          (astNode*)newAstExprBinaryNode(OP_MINUS, du, _MultiplicativeExpr()));
    default:
      return du;
  }
}

astNode* _MultiplicativeExpr() {
  debug("MultiplicativeExpr");
  return _MultiplicativeExprRest(_UnaryExpr());
}

astNode* _MultiplicativeExprRest(astNode* du) {
  debug("_MultiplicativeExprRest");

  switch (Symb.type) {
    case MUL:
      Symb = readLexem();
      return _MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_MUL, du, _UnaryExpr()));
    case DIV:
      Symb = readLexem();
      return _MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_DIV, du, _UnaryExpr()));
    case MOD:
      Symb = readLexem();
      return _MultiplicativeExprRest(
          (astNode*)newAstExprBinaryNode(OP_MOD, du, _UnaryExpr()));
    default:
      return du;
  }
}

astNode* _UnaryExpr() {
  debug("_UnaryExpr");

  switch (Symb.type) {
    case PLUS:
      Symb = readLexem();
      return _UnaryExpr();
    case MINUS:
      Symb = readLexem();
      return (astNode*)newAstExprUnaryNode(OP_MINUS, _UnaryExpr());
    case LPAR: {
      Symb = readLexem();
      astNode* expr = _LogicalOrExpr();
      _Match(RPAR);
      return expr;
    }
    case NUMB: {
      int64_t value;
      _MatchGetNumber(NUMB, &value);

      return (astNode*)newAstExprIntegerNode(value);
    }
    case kwTRUE: {
      Symb = readLexem();
      return (astNode*)newAstExprBooleanNode(1);
    }
    case kwFALSE: {
      Symb = readLexem();
      return (astNode*)newAstExprBooleanNode(0);
    }
    case STRING: {
      char buffer[MAX_IDENT_LEN];
      _MatchGetString(STRING, buffer);

      return (astNode*)newAstExprStringNode(buffer);
    }
    case IDENT: {
      char buffer[MAX_IDENT_LEN];
      _MatchGetString(IDENT, buffer);

      switch (Symb.type) {
        case LPAR: {
          Symb = readLexem();

          int64_t aggr;
          _MatchGetNumber(AGGREGATOR, &aggr);

          astNode* var = NULL;
          if (_Check(ARROW)) {
            Symb = readLexem();
            var = (astNode*)_Expression();
          }
          _Match(RPAR);

          return (astNode*)newAstExprCallNode(buffer, (int8_t)aggr, var);
        }
        default: { return (astNode*)newAstExprVariableNode(buffer); }
      }
    }
    default:
      fatalExt("Error in expression expansion", symbTable[Symb.type]);
      return NULL;
  }
}

void initParser(const char* fileName) {
  initLexan(fileName);
  Symb = readLexem();
}

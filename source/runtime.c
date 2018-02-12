#include <string.h>

#include "lang.h"

void runProgram() {
  // prepare memory
  pushLocalContext(TRUE);

  // start program execution
  statementLoop((astNode*)(gAppCtx.main));

  // finish everything
  popLocalContext();
}

void statementLoop(astNode* node) {
  if (node->tag != N_STATEMENT) fatal("Unexpected tree sequence.");

  astStatementNode* statement = (astStatementNode*)node;
  while (statement) {
    OBJ res = eval(statement->statement);
    if (statement->aggregator != -1)
      memoryAggregatorAdd(statement->aggregator, res);
    statement = (astStatementNode*)statement->next;
    heapMarkAndSweep();
  }
}

bool _evalToBoolean(OBJ object) {
  switch (object->tag) {
    case T_UNINITIALIZED:
      fatal("If cannot be decided by undefined variable.");
    case T_BOOLEAN:
      return ((objBoolean*)object)->val;
    case T_INTEGER:
      return (bool)((objInteger*)object)->val;
    case T_BIGINTEGER:
      return (bool)((objBigInteger*)object)->val;
    case T_STRING:
      return (((objString*)object)->string) ? TRUE : FALSE;
    default:
      fatal("Unexpected variable type: cannot eval to boolean.");
  }
  return FALSE;
}

inline OBJ _handleUnaryMinus(OBJ obj) {
  INT_ONLY_CHECK(obj, "Unary minus");

  int64_t val = GET_INT_VALUE(obj);
  return newAutoInteger(val * -1);
}
/*OBJ _handleBinaryPlus(OBJ left, OBJ right) {
  if (left->tag == T_UNINITIALIZED || right->tag == T_UNINITIALIZED)
    fatal("Cannot work with uninitialized objects.");
  if (left->tag == T_BOOLEAN || right->tag == T_BOOLEAN)
    fatal("Binary plus cannot be used with Boolean objects.");
  if (left->tag == T_STRING || right->tag == T_STRING)
    fatal("Binary plus cannot be used with String objects.");

  if ((left->tag == T_INTEGER || left->tag == T_BIGINTEGER) &&
      (right->tag == T_INTEGER || right->tag == T_BIGINTEGER)) {

    int64_t leftVal = GET_INT_VALUE(left);
    int64_t rightVal = GET_INT_VALUE(right);
    return newAutoInteger(leftVal + rightVal);
  }
  fatal("Unknown variable type.");
  return NULL;
}*/
inline OBJ _handleBinaryPlus(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary plus");
  INT_ONLY_CHECK(right, "Binary plus");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);
  return newAutoInteger(leftVal + rightVal);
}
inline OBJ _handleBinaryMinus(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary minus");
  INT_ONLY_CHECK(right, "Binary minus");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);
  return newAutoInteger(leftVal - rightVal);
}
inline OBJ _handleBinaryMul(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary multiplication");
  INT_ONLY_CHECK(right, "Binary multiplication");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);
  return newAutoInteger(leftVal * rightVal);
}
inline OBJ _handleBinaryDiv(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary division");
  INT_ONLY_CHECK(right, "Binary division");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);
  return newAutoInteger(leftVal / rightVal);
}
inline OBJ _handleBinaryMod(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary modulo");
  INT_ONLY_CHECK(right, "Binary modulo");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);
  return newAutoInteger(leftVal % rightVal);
}
inline OBJ _handleBinaryEq(OBJ left, OBJ right) {
  if (left->tag != right->tag) return newBoolean(FALSE);
  if (left->tag == T_UNINITIALIZED || right->tag == T_UNINITIALIZED)
    fatal("Cannot compare uninitialized values.");

  switch (left->tag) {
    case T_BOOLEAN:
      if (((objBoolean*)left)->val == ((objBoolean*)right)->val)
        return newBoolean(TRUE);
      return newBoolean(FALSE);
    case T_INTEGER:
      if (((objInteger*)left)->val == ((objInteger*)right)->val)
        return newBoolean(TRUE);
      return newBoolean(FALSE);
    case T_BIGINTEGER:
      if (((objBigInteger*)left)->val == ((objBigInteger*)right)->val)
        return newBoolean(TRUE);
      return newBoolean(FALSE);
    case T_STRING:
      if (!strcmp(((objString*)left)->string, ((objString*)right)->string))
        return newBoolean(TRUE);
      return newBoolean(FALSE);
    default:
      fatal("Unknown variable type.");
      return NULL;
  }
}
inline OBJ _handleBinaryNeq(OBJ left, OBJ right) {
  if (left->tag == T_UNINITIALIZED || right->tag == T_UNINITIALIZED)
    fatal("Cannot compare uninitialized values.");
  if (left->tag != right->tag) return newBoolean(TRUE);

  switch (left->tag) {
    case T_BOOLEAN:
      if (((objBoolean*)left)->val == ((objBoolean*)right)->val)
        return newBoolean(FALSE);
      return newBoolean(TRUE);
    case T_INTEGER:
      if (((objInteger*)left)->val == ((objInteger*)right)->val)
        return newBoolean(FALSE);
      return newBoolean(TRUE);
    case T_BIGINTEGER:
      if (((objBigInteger*)left)->val == ((objBigInteger*)right)->val)
        return newBoolean(FALSE);
      return newBoolean(TRUE);
    case T_STRING:
      if (!strcmp(((objString*)left)->string, ((objString*)right)->string))
        return newBoolean(FALSE);
      return newBoolean(TRUE);
    default:
      fatal("Unknown variable type.");
      return NULL;
  }
}
inline OBJ _handleBinaryLt(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary LT");
  INT_ONLY_CHECK(right, "Binary LT");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);

  if (leftVal < rightVal) return newBoolean(TRUE);
  return newBoolean(FALSE);
}
inline OBJ _handleBinaryLte(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary LTE");
  INT_ONLY_CHECK(right, "Binary LTE");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);

  if (leftVal <= rightVal) return newBoolean(TRUE);
  return newBoolean(FALSE);
}
inline OBJ _handleBinaryGt(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary GT");
  INT_ONLY_CHECK(right, "Binary GT");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);

  if (leftVal > rightVal) return newBoolean(TRUE);
  return newBoolean(FALSE);
}
inline OBJ _handleBinaryGte(OBJ left, OBJ right) {
  INT_ONLY_CHECK(left, "Binary GTE");
  INT_ONLY_CHECK(right, "Binary GTE");

  int64_t leftVal = GET_INT_VALUE(left);
  int64_t rightVal = GET_INT_VALUE(right);

  if (leftVal >= rightVal) return newBoolean(TRUE);
  return newBoolean(FALSE);
}
inline OBJ _handleBinaryAnd(OBJ left, OBJ right) {
  if (_evalToBoolean(left) && _evalToBoolean(right)) return newBoolean(TRUE);
  return newBoolean(FALSE);
}
inline OBJ _handleBinaryOr(OBJ left, OBJ right) {
  if (_evalToBoolean(left) || _evalToBoolean(right)) return newBoolean(TRUE);
  return newBoolean(FALSE);
}

OBJ eval(astNode* node) {
  switch (node->tag) {
    case N_STATEMENT:
      pushLocalContext(FALSE);
      statementLoop(node);
      popLocalContext();

      return NULL;
    case N_IF: {
      OBJ cond = eval(((astIfNode*)node)->cond);
      bool res = _evalToBoolean(cond);
      if (res) {
        eval(((astIfNode*)node)->thenBlk);
        return newBoolean(TRUE);
      }
      if (((astIfNode*)node)->elseBlk) eval(((astIfNode*)node)->elseBlk);

      return newBoolean(FALSE);
    }
    case N_ASSERT: {
      OBJ res = eval(((astAssertNode*)node)->expr);
      if (_evalToBoolean(res)) return newBoolean(TRUE);
      fatal("Assertion failed.");
    }
    case N_WHILE: {
      OBJ cond = eval(((astWhileNode*)node)->cond);
      while (_evalToBoolean(cond)) {
        eval(((astWhileNode*)node)->block);
        cond = eval(((astWhileNode*)node)->cond);
      }
      return newBoolean(TRUE);
    }
    case N_DO: {
      OBJ amount = eval(((astDoNode*)node)->amount);
      if (amount->tag != T_INTEGER && amount->tag != T_BIGINTEGER)
        fatal("Do loop must have integer input.");

      int64_t val = GET_INT_VALUE(amount);
      for (int64_t i = 0; i < val; i++) {
        eval(((astWhileNode*)node)->block);
      }
      return newBoolean(TRUE);
    }
    case N_READ: {
      char buffer[READ_BUFFER_SIZE];
      if (fgets(buffer, READ_BUFFER_SIZE, stdin) == NULL)
        fatal("Nothing to read.");

      OBJ right = getObjectFromBuffer(buffer);

      astNode* ident = ((astReadNode*)node)->var;
      if (ident->tag != N_VAR) fatal("Objects must be read to variable names.");

      OBJEntry* var = memoryGetObjectEntry(((astExprVariableNode*)ident)->name);
      var->value = right;
      return cloneObject(right);
    }
    case N_WRITE: {
      OBJ obj = eval(((astWriteNode*)node)->expr);
      printObject(obj);
      printf("\n");
      return cloneObject(obj);
    }
    case N_CALL: {
      if (((astExprCallNode*)node)->var == NULL)
        return handleFunctionCall(((astExprCallNode*)node)->name,
                                  ((astExprCallNode*)node)->aggr);

      astNode* ident = ((astExprCallNode*)node)->var;
      if (ident->tag != N_VAR)
        fatal("Aggregator must have variable as a target.");

      OBJ res = handleFunctionCall(((astExprCallNode*)node)->name,
                                   ((astExprCallNode*)node)->aggr);

      OBJEntry* var = memoryGetObjectEntry(((astExprVariableNode*)ident)->name);
      var->value = res;
      return cloneObject(res);
    }
    case N_INT:
      return newAutoInteger(((astExprIntegerNode*)node)->value);

    case N_BOOL:
      return newBoolean(((astExprBooleanNode*)node)->value);

    case N_STRING:
      return newString(((astExprStringNode*)node)->string);

    case N_VAR:
      return memoryGetObject(((astExprVariableNode*)node)->name);

    case N_UNARY: {
      OBJ expr = eval(((astExprUnaryNode*)node)->expr);
      if (((astExprUnaryNode*)node)->op == OP_MINUS) {
        return _handleUnaryMinus(expr);
      }
      fatal("Unexpected unary operator.");
    }

    case N_BINARY: {
      OBJ right = eval(((astExprBinaryNode*)node)->right);
      if (((astExprBinaryNode*)node)->op == OP_ASSIGN) {  // TODO

        astNode* ident = ((astExprBinaryNode*)node)->left;
        if (ident->tag != N_VAR)
          fatal(" Objects must be assigned to variable names.");

        OBJEntry* var =
            memoryGetObjectEntry(((astExprVariableNode*)ident)->name);
        var->value = right;
        return cloneObject(right);
      }

      OBJ left = eval(((astExprBinaryNode*)node)->left);
      switch (((astExprBinaryNode*)node)->op) {
        case OP_PLUS:
          return _handleBinaryPlus(left, right);
        case OP_MINUS:
          return _handleBinaryMinus(left, right);
        case OP_MUL:
          return _handleBinaryMul(left, right);
        case OP_DIV:
          return _handleBinaryDiv(left, right);
        case OP_MOD:
          return _handleBinaryMod(left, right);
        case OP_EQ:
          return _handleBinaryEq(left, right);
        case OP_NEQ:
          return _handleBinaryNeq(left, right);
        case OP_LT:
          return _handleBinaryLt(left, right);
        case OP_LTE:
          return _handleBinaryLte(left, right);
        case OP_GT:
          return _handleBinaryGt(left, right);
        case OP_GTE:
          return _handleBinaryGte(left, right);
        case OP_AND:
          return _handleBinaryAnd(left, right);
        case OP_OR:
          return _handleBinaryOr(left, right);
        default:
          fatal("Unexpected binary operator.");
      }
    }
  }
  fatal("Unexpected ast node.");
  return NULL;
}

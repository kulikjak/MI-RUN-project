#include <string.h>

#include "lang.h"

int64_t _hashFunctionName(const char* name) {
  int32_t i, len = strlen(name);
  int64_t hash = 0;

  for (i = 0; i < len; i++) {
    hash = (hash << 2) | (name[i] & 0x3);
  }
  return hash;
}

OBJ _funcSum(int8_t aggrNum) {
  int64_t i, counter = 0;
  appAggregator* aggr = memoryGetAggregator(aggrNum);

  for (i = 0; i < aggr->count; i++) {
    if (aggr->array[i]->tag != T_INTEGER && aggr->array[i]->tag != T_BIGINTEGER)
      fatal("Aggregators can right now work only with integers");

    OBJ temp = aggr->array[i];
    counter += (temp->tag == T_INTEGER) ? (int64_t)(((struct objInteger*)temp)->val) : ((struct objBigInteger*)temp)->val;
  }
  return newAutoInteger(counter);
}

OBJ _funcClear(int8_t aggrNum) {
  appAggregator* aggr = memoryGetAggregator(aggrNum);
  aggr->count = 0;
  return newUninitialized();
}

OBJ _funcAverage(int8_t aggrNum) {
  int64_t i, counter = 0;
  appAggregator* aggr = memoryGetAggregator(aggrNum);

  for (i = 0; i < aggr->count; i++) {
    if (aggr->array[i]->tag != T_INTEGER && aggr->array[i]->tag != T_BIGINTEGER)
      fatal("Aggregators can right now work only with integers");

    OBJ temp = aggr->array[i];
    counter += (temp->tag == T_INTEGER) ? (int64_t)(((struct objInteger*)temp)->val) : ((struct objBigInteger*)temp)->val;
  }
  return newAutoInteger((int64_t)(counter/(aggr->count)));
}

OBJ _funcMultiply(int8_t aggrNum) {
  int64_t i, counter = 1;
  appAggregator* aggr = memoryGetAggregator(aggrNum);

  for (i = 0; i < aggr->count; i++) {
    if (aggr->array[i]->tag != T_INTEGER && aggr->array[i]->tag != T_BIGINTEGER)
      fatal("Aggregators can right now work only with integers");

    OBJ temp = aggr->array[i];
    counter *= (temp->tag == T_INTEGER) ? (int64_t)(((struct objInteger*)temp)->val) : ((struct objBigInteger*)temp)->val;
  }
  return newAutoInteger(counter);
}

OBJ handleFunctionCall(const char* name, int8_t aggr) {
  int64_t function = _hashFunctionName(name);
  switch(function) {
    case 53:
      return _funcSum(aggr);
    case 790:
      return _funcClear(aggr);
    case 6557:
      return _funcAverage(aggr);
    case 20545:
      return _funcMultiply(aggr);
  }
  fatalExt("Unknown function called: ", name);
  return NULL;
}


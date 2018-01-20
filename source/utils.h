#ifndef __UTILS__
#define __UTILS__

#include <stdlib.h>
#include <assert.h>

#define bool int8_t
#define true 1
#define false 0

#define DEBUG__ false

#define _UNREACHABLE assert(0)

void fatal(const char*);
void debug(const char*);

#endif /* __UTILS__ */

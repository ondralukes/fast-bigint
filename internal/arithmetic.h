#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdint.h>

#include "types.h"
bigint_t* add(bigint_t* a, bigint_t* b);
bigint_t* sub(bigint_t* a, bigint_t* b);
int compare(bigint_t* a, bigint_t* b);
#endif

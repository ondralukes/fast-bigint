#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
bigint_t* add(bigint_t* a, bigint_t* b);
bigint_t* sub(bigint_t* a, bigint_t* b);
bigint_t* mul(bigint_t* a, bigint_t* b);
int compare(bigint_t* a, bigint_t* b);
void shift(bigint_t* a, uint64_t n);
#endif

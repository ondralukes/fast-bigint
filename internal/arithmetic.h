#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "types.h"

bigint_t* add(bigint_t* a, bigint_t* b);
bigint_t* sub(bigint_t* a, bigint_t* b);
bigint_t* mul(bigint_t* a, bigint_t* b);
void addTo(bigint_t* a, bigint_t* b, bigint_t * res);
void addToShifted(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift);
void subTo(bigint_t* a, bigint_t* b, bigint_t * res);
void mulTo(bigint_t* a, bigint_t* b, bigint_t * res);
int compare(bigint_t* a, bigint_t* b);
void shift(bigint_t* a, uint64_t n);
#endif

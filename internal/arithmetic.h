#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "types.h"

bigint_t* add(bigint_t* a, bigint_t* b);
bigint_t* sub(bigint_t* a, bigint_t* b);
bigint_t* mul(bigint_t* a, bigint_t* b);

//div is already defined in stdlib.h
bigint_t* divide(bigint_t* a, bigint_t* b);
void addTo(bigint_t* a, bigint_t* b, bigint_t * res);

//b is right-shifted
//shift is in 8-bytes
void addToShifted(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift);
void subTo(bigint_t* a, bigint_t* b, bigint_t * res);

//a is right-shifted
//shift in in bits
void subToShifted(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift);

void mulTo(bigint_t* a, bigint_t* b, bigint_t* res);
void divTo(bigint_t* n, bigint_t* d, bigint_t * res);
int compare(bigint_t* a, bigint_t* b);
void shift(bigint_t* a, uint64_t n);
#endif

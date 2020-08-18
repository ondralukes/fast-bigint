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

//b is right-shifted
//shift is in bytes
void subToShifted8(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift);

void mulTo(bigint_t* a, bigint_t* b, bigint_t* res);
void divTo(bigint_t* x, bigint_t* y, bigint_t * q);
int compare(bigint_t* a, bigint_t* b);
int compareShifted8(bigint_t* a, bigint_t* b, uint64_t shift);
void shift8(bigint_t* a, uint64_t n);
#endif

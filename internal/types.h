#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <js_native_api.h>
#include <node_api.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define WRITE_UINT64_LE(ptr, number) *(ptr) = (number);
  #define READ_UINT64_LE(ptr) *(ptr)
#else
  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define WRITE_UINT64_LE(ptr, number) *(ptr) = bswap_64((number));
    #define READ_UINT64_LE(ptr) bswap64(*(ptr))
  #else
    #error PDP-endian not supported
  #endif
#endif

typedef struct {
  uint64_t* base;
  uint64_t allocated;
  uint64_t length;
} bigint_t;

typedef struct {
  bigint_t* i;
  uint64_t exp;
} bigfloat_t;

enum op_type{Add, Sub, Mul};

typedef struct {
  enum op_type type;
  bigint_t** argv;
  size_t argc;
  napi_threadsafe_function callback;
} async_op_t;

bigint_t* createEmptyBigint(uint64_t size);
void copy(bigint_t* d, bigint_t* s);
void createSubBigint(bigint_t * src, uint64_t start, uint64_t end, bigint_t* res);
bigint_t* createMaxBigint(uint64_t size);
void resizeBigint(bigint_t* x, uint64_t size);
uint64_t contentLength(bigint_t* x);
void destroyBigint(bigint_t* bigint);

uint64_t getAtBit(bigint_t*x, int64_t n);
uint64_t bitLength(bigint_t *x);

uint8_t get8(bigint_t* x, uint64_t n);
void set8(bigint_t* x, uint64_t n, uint8_t val);

uint64_t get64at8(bigint_t* x, int64_t p);
#endif

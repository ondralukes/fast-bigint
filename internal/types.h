#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <js_native_api.h>
#include <node_api.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define WRITE_UINT64_LE(ptr, number) *ptr = number;
  #define READ_UINT64_LE(ptr) *ptr;
#else
  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define WRITE_UINT64_LE(ptr, number) *ptr = bswap_64(number);
    #define READ_UINT64_LE(ptr) bswap64(*ptr);
  #else
    #error PDP-endian not supported
  #endif
#endif

typedef struct {
  uint64_t* base;
  size_t length;
} bigint_t;

enum op_type{Add};

typedef struct {
  enum op_type type;
  bigint_t** argv;
  size_t argc;
  napi_threadsafe_function callback;
} async_op_t;

bigint_t* createEmptyBigint(uint64_t size);
void destroyBigint(bigint_t* bigint);
#endif

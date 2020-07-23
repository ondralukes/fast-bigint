#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

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
#endif

#include "types.h"

bigint_t* createEmptyBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc(size * sizeof(uint64_t));
  res->length = size;
  return res;
}

bigint_t* createMaxBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc(size * sizeof(uint64_t));
  res->length = size;
  memset(res->base, 0xff, sizeof(uint64_t) * size);
  return res;
}

bigint_t* createSubBigint(bigint_t * src, uint64_t start, uint64_t end){
  if(start > src->length) start = src->length;
  if(end > src->length) end = src->length;
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = src->base + start;
  res->length = end - start;
  return res;
}

void destroyBigint(bigint_t* bigint){
  free(bigint->base);
  free(bigint);
}

uint64_t contentLength(bigint_t* x){
  uint64_t len = x->length;
  while(len != 0){
    len--;
    if(x->base[len] != 0) return len+1;
  }
  return len;
}

#include "types.h"

bigint_t* createEmptyBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc((size + 256) * sizeof(uint64_t));
  res->allocated = size + 256;
  res->length = 0;
  return res;
}

bigint_t* createMaxBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc(size * sizeof(uint64_t));
  res->length = size;
  memset(res->base, 0xff, sizeof(uint64_t) * size);
  return res;
}

void createSubBigint(bigint_t * src, uint64_t start, uint64_t end, bigint_t* res){
  if(start > src->length) start = src->length;
  if(end > src->length) end = src->length;
  res->base = src->base + start;
  res->length = end - start;
}

void destroyBigint(bigint_t* bigint){
  free(bigint->base);
  free(bigint);
}

void resizeBigint(bigint_t* x, uint64_t size){
  if(size > x->allocated){
    uint64_t newSize = size + 256;
    x->base = realloc(x->base, newSize * sizeof(uint64_t));
    x->allocated = newSize;
  }
}

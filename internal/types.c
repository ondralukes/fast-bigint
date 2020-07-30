#include "types.h"

bigint_t* createEmptyBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc(size * sizeof(uint64_t));
  res->length = size;
  return res;
}

void destroyBigint(bigint_t* bigint){
  free(bigint->base);
  free(bigint);
}

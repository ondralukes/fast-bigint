#include "types.h"

bigint_t* createEmptyBigint(uint64_t size){
  bigint_t * res = malloc(sizeof(bigint_t));
  res->base = malloc((size + 256) * sizeof(uint64_t));
  res->allocated = size + 256;
  res->length = 0;
  return res;
}

void copy(bigint_t* d, bigint_t* s){
  resizeBigint(d, s->length);
  memcpy(d->base, s->base, s->length*sizeof(uint64_t));
  d->length = s->length;
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

uint64_t getAtBit(bigint_t*x, int64_t n){
  int64_t li = n<0?((n-63)/64):(n/64);
  uint64_t low = 0;
  if(li >= 0 && li < (int64_t)x->length) low = READ_UINT64_LE(&x->base[li]);

  int64_t hi = n<0?(n/64):((n+63)/64);

  uint64_t high = 0;
  if(hi < (int64_t)x->length && hi >= 0) high = READ_UINT64_LE(&x->base[hi]);

  int8_t bits = n % 64;
  if(bits < 0) bits += 64;

  uint64_t lp = low >> (uint64_t)bits;
  uint64_t hp = (high & ((1UL << bits) - 1)) << (64-bits);

  return hp|lp;
}

uint64_t bitLength(bigint_t *x){
  uint64_t last = READ_UINT64_LE(&x->base[x->length - 1]);
  uint64_t bit = 63;
  while((last & (1UL << bit)) == 0 && bit > 0) bit--;
  bit++;

  return x->length * 64 - 64 + bit;
}

uint8_t get8(bigint_t* x, uint64_t n){
  uint64_t b = x->base[n >> 3];
  return (b >> (8* (n & 0b111))) & 0xff;
}

void set8(bigint_t* x, uint64_t n, uint8_t val){
  uint64_t b = READ_UINT64_LE(&x->base[n >> 3]);

  uint64_t mask = ~((uint64_t)0xff << (uint64_t)(8*(n & 0b111)));
  WRITE_UINT64_LE(
    &x->base[n >> 3],
    (b & mask) | ((uint64_t)val << (uint64_t)(8*(n & 0b111)))
  );
}

uint64_t get64at8(bigint_t* x, int64_t p){
  uint64_t lsh = (p & 0b111);
  uint64_t hsh = 8 - lsh;

  uint64_t l = 0;
  if((p>>3) >= 0 && (p>>3) < (int64_t)x->length) l = READ_UINT64_LE(&x->base[p >> 3]);
  uint64_t h = 0;
  if((p+7)/8 < (int64_t)x->length) h = READ_UINT64_LE(&x->base[(p+7)/8]);

  return (l >> (8*lsh)) | (h << (8*hsh));
}

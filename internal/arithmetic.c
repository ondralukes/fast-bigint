#include "arithmetic.h"

bigint_t* add(bigint_t* a, bigint_t* b){
  uint64_t resLength = a->length;
  if(b->length > resLength) resLength = b->length;


  uint64_t lastByteSum = 0;

  if(resLength == a->length){
    uint8_t * lastByte = (uint8_t*) a->base;
    lastByte += resLength * 8 - 1;
    lastByteSum += *lastByte;
  }

  if(resLength == b->length){
    uint8_t * lastByte = (uint8_t*) b->base;
    lastByte += resLength * 8 - 1;
    lastByteSum += *lastByte;
  }

  if(lastByteSum > 254) resLength++;

  bigint_t* res = createEmptyBigint(resLength);
  uint64_t * aptr = a->base;
  uint64_t * bptr = b->base;
  uint64_t * resptr = res->base;

  uint64_t carry = 0;
  uint64_t temp;
  for(uint64_t i = 0; i < res->length;i++){
    uint64_t val = carry;
    carry = 0;
    if(i < a->length){
      uint64_t a_val = READ_UINT64_LE(aptr);
      temp = val;
      val += a_val;
      if(val < temp){
        carry++;
      }
    }
    if(i < b->length){
      uint64_t b_val = READ_UINT64_LE(bptr);
      temp = val;
      val += b_val;
      if(val < temp){
        carry++;
      }
    }

    WRITE_UINT64_LE(resptr, val);
    resptr++;
    bptr++;
    aptr++;
  }

  return res;
}

bigint_t* sub(bigint_t* a, bigint_t* b){
  uint64_t resLength = a->length-1;
  while(resLength < b->length){
    if(a->base[resLength] > b->base[resLength]) break;
    resLength--;
  }

  resLength++;

  bigint_t* res = createEmptyBigint(resLength);

  uint64_t* resptr = res->base;
  uint64_t* aptr = a->base;
  uint64_t* bptr = b->base;

  uint64_t carry = 0;

  uint64_t val,tmp;
  for(uint64_t i = 0; i < res->length;i++){
    val = READ_UINT64_LE(aptr);

    if(i < b->length){
      tmp = val;
      val -= READ_UINT64_LE(bptr);
      val -= carry;
      if(val > tmp){
        carry = 1;
      } else {
        carry = 0;
      }
    } else {
      val -= carry;
      carry = 0;
    }

    WRITE_UINT64_LE(resptr, val);
    resptr++;
    aptr++;
    bptr++;
  }

  return res;
}

int compare(bigint_t* a, bigint_t* b){
  uint64_t i = (a->length>b->length?a->length:b->length);

  uint64_t* aptr = a->base + i;
  uint64_t* bptr = b->base + i;

  while(i > 0){
    aptr--;
    bptr--;
    i--;

    uint64_t av = i < a->length?READ_UINT64_LE(aptr):0;
    uint64_t bv = i < b->length?READ_UINT64_LE(bptr):0;

    if(av > bv) return 1;
    if(av < bv) return -1;
  }
  return 0;
}

bigint_t* karatsuba(bigint_t* a, bigint_t* b){
  uint64_t alen = contentLength(a);
  uint64_t blen = contentLength(b);
  if(alen <= 1 && blen <= 1){
    if(alen == 1 && blen == 1){
      uint64_t numa = READ_UINT64_LE(a->base);
      uint64_t numb = READ_UINT64_LE(b->base);

      uint64_t la = numa & 0xffffffff;
      uint64_t ha = numa >> 32;

      uint64_t lb = numb & 0xffffffff;
      uint64_t hb = numb >> 32;

      uint64_t low = la * lb;

      uint64_t lahb = la * hb;
      uint64_t halb = ha * lb;


      uint64_t mid = lahb + halb;
      uint64_t high = ha * hb;
      if(mid < lahb){
        high += (uint64_t)1 << 32;
      }

      uint64_t f0 = low + (mid << 32);
      uint64_t f1 = high + (mid >> 32);
      if(f0 < low){
        f1++;
      }
      bigint_t* res = createEmptyBigint(f1==0?1:2);

      WRITE_UINT64_LE(res->base, f0);
      if(f1!=0) WRITE_UINT64_LE(res->base + 1, f1);
      return res;
    } else {
      bigint_t* res = createEmptyBigint(1);
      res->base[0] = 0;
      return res;
    }
  }
  if(alen == 0 || blen == 0){
    bigint_t* res = createEmptyBigint(1);
    res->base[0] = 0;
    return res;
  }
  uint64_t base = ((alen<blen?alen:blen) + 1) / 2;
  bigint_t* a1 = createSubBigint(a, 0, base);
  bigint_t* b1 = createSubBigint(b, 0, base);

  bigint_t* a2 = createSubBigint(a, base, alen);
  bigint_t* b2 = createSubBigint(b, base, blen);

  bigint_t* a3 = add(a1, a2);

  bigint_t* b3 = add(b1, b2);


  bigint_t* r1 = karatsuba(a1, b1);
  bigint_t* r2 = karatsuba(a2, b2);
  bigint_t* r3 = karatsuba(a3, b3);

  bigint_t* tmp = r3;
  r3 = sub(r3, r1);
  destroyBigint(tmp);
  tmp = r3;
  r3 = sub(r3, r2);
  destroyBigint(tmp);
  shift(r2, base*2);
  shift(r3, base);

  bigint_t* r12 = add(r1, r2);
  bigint_t* r = add(r12, r3);

  destroyBigint(r1);
  destroyBigint(r2);
  destroyBigint(r12);
  destroyBigint(r3);

  free(a1);
  free(b1);

  free(a2);
  free(b2);

  destroyBigint(a3);
  destroyBigint(b3);

  return r;
}

bigint_t* mul(bigint_t* a, bigint_t* b){
  return karatsuba(a, b);
}

void shift(bigint_t* a, uint64_t n){
  uint64_t newLen = a->length + n;
  uint64_t* old = a->base;
  a->base = malloc(newLen * sizeof(uint64_t));
  memcpy(a->base + n, old, a->length*sizeof(uint64_t));
  memset(a->base, 0, sizeof(uint64_t) * n);
  free(old);
  a->length = newLen;
}

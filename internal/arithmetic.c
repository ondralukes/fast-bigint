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

  if(lastByteSum > 255) resLength++;

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

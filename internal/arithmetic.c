#include "arithmetic.h"

void add(bigint_t* a, bigint_t* b, bigint_t* res){
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
}

#include "arithmetic.h"

void addTo(bigint_t* a, bigint_t* b, bigint_t * res){
  uint64_t resLength = a->length;
  if(b->length > resLength) resLength = b->length;

  resLength++;

  resizeBigint(res, resLength);
  uint64_t * aptr = a->base;
  uint64_t * bptr = b->base;
  uint64_t * resptr = res->base;

  uint64_t carry = 0;
  uint64_t temp;
  for(uint64_t i = 0; i < resLength;i++){
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

  res->length = resLength;

  if(res->base[resLength - 1] == 0) res->length--;
}

void addToShifted(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift){
  uint64_t resLength = a->length;
  if((b->length+shift) > resLength) resLength = b->length + shift;

  resLength++;

  resizeBigint(res, resLength);
  uint64_t * aptr = a->base;
  uint64_t * bptr = b->base - shift;
  uint64_t * resptr = res->base;

  uint64_t carry = 0;
  uint64_t temp;
  for(uint64_t i = 0; i < resLength;i++){
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
    if(i >= shift && i < (b->length + shift)){
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

  res->length = resLength;

  if(res->base[resLength - 1] == 0) res->length--;
}

void subTo(bigint_t* a, bigint_t* b, bigint_t * res){
  uint64_t* resptr = res->base;
  uint64_t* aptr = a->base;
  uint64_t* bptr = b->base;

  resizeBigint(res, a->length);
  uint64_t carry = 0;

  uint64_t finalLength = 0;
  uint64_t val,tmp;
  for(uint64_t i = 0; i < a->length;i++){
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
    if(val != 0) finalLength = i;
    resptr++;
    aptr++;
    bptr++;
  }

  res->length = finalLength+1;
}

void mulTo(bigint_t* a, bigint_t* b, bigint_t * res);
bigint_t* add(bigint_t* a, bigint_t* b){
  bigint_t* res = createEmptyBigint(a->length>b->length?a->length:b->length);
  addTo(a, b, res);
  return res;
}

bigint_t* sub(bigint_t* a, bigint_t* b){
  bigint_t* res = createEmptyBigint(a->length);
  subTo(a, b, res);
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

void karatsuba(bigint_t* a, bigint_t* b, bigint_t* res, bigint_t* temp){
  if(a->length <= 1 && b->length <= 1){
    if(a->length == 1 && b->length == 1){
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
      res->length = f1==0?1:2;

      WRITE_UINT64_LE(res->base, f0);
      if(f1!=0) WRITE_UINT64_LE(res->base + 1, f1);
    } else {
      res->length = 0;
    }
    return;
  }

  if(a->length == 0 || b->length == 0){
    res->length = 0;
    return;
  }

  bigint_t a1,a2,b1,b2;
  bigint_t* a3 = &temp[0];
  bigint_t* b3 = &temp[1];
  bigint_t* r2 = &temp[2];
  bigint_t* r3 = &temp[3];

  uint64_t base = (a->length>b->length?a->length:b->length) / 2;

  createSubBigint(a, 0, base, &a1);
  createSubBigint(b, 0, base, &b1);

  createSubBigint(a, base, a->length, &a2);
  createSubBigint(b, base, b->length, &b2);

  addTo(&a1, &a2, a3);
  addTo(&b1, &b2, b3);

  karatsuba(&a1, &b1, res, &temp[4]);
  karatsuba(&a2, &b2, r2, &temp[4]);
  karatsuba(a3, b3, r3, &temp[4]);

  subTo(r3, res, r3);
  subTo(r3, r2, r3);

  addToShifted(res, r2, res, base*2);
  addToShifted(res, r3, res, base);
}

bigint_t* mul(bigint_t* a, bigint_t* b){
  uint64_t len = a->length>b->length?a->length:b->length;
  uint64_t layerLength = len;
  uint64_t depth = 0;

  while(len > 1){
    uint64_t base = layerLength/2;
    layerLength = base+1;
    depth++;
    if(layerLength == 2){
      depth++;
      break;
    }
  }

  uint64_t tempLength = 5* depth * (sizeof(bigint_t) + len*2*sizeof(uint64_t));
  bigint_t* temp = malloc(tempLength);
  uint64_t * base = (uint64_t*)((uint64_t)temp + 5 * depth * sizeof(bigint_t));
  for(uint64_t i = 0;i<5*depth;i++){
    temp[i].base = base;
    temp[i].length = 0;
    temp[i].allocated = len*2;
    base += len*2;
  }

  bigint_t* res = createEmptyBigint(len*2);
  karatsuba(a, b, res, temp);
  free(temp);
  return res;
}

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
  resizeBigint(res, a->length);
  uint64_t* resptr = res->base;
  uint64_t* aptr = a->base;
  uint64_t* bptr = b->base;

  uint64_t carry = 0;

  uint64_t finalLength = 0;
  uint64_t val,tmp;
  for(uint64_t i = 0; i < a->length;i++){
    val = READ_UINT64_LE(aptr);

    if(i < b->length){
      uint64_t x = READ_UINT64_LE(bptr);
      tmp = val;
      val -= x;
      val -= carry;
      int8_t cmp = (tmp < val) ? -1 : (tmp > val);
      if(cmp == -1){
        carry = 1;
      } else if(carry == 1 && cmp == 0){
        carry = 1;
      } else {
        carry = 0;
      }

    } else if(carry == 1){
      val -= carry;
      if(val != 0xffffffffffffffff) carry = 0;
    }

    WRITE_UINT64_LE(resptr, val);
    if(val != 0) finalLength = i;
    resptr++;
    aptr++;
    bptr++;
  }

  res->length = finalLength+1;
}

void subToShifted8(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift){
  resizeBigint(res, a->length);
  uint64_t* resptr = res->base;
  uint64_t* aptr = a->base;
  int64_t bpos = -shift;

  uint64_t carry = 0;

  uint64_t finalLength = 0;
  uint64_t val,tmp;
  for(uint64_t i = 0; i < a->length;i++){
    val = READ_UINT64_LE(aptr);
    if(i >= shift/8 && i < (b->length + (shift+7)/8)){
      uint64_t x = get64at8(b, bpos);
      tmp = val;
      val -= x;
      val -= carry;
      int8_t cmp = (tmp < val) ? -1 : (tmp > val);
      if(cmp == -1){
        carry = 1;
      } else if(carry == 1 && cmp == 0){
        carry = 1;
      } else {
        carry = 0;
      }

    } else if(carry == 1){
      val -= carry;
      if(val != 0xffffffffffffffff) carry = 0;
    }

    WRITE_UINT64_LE(resptr, val);
    if(val != 0) finalLength = i;
    resptr++;
    aptr++;
    bpos+=8;
  }

  res->length = finalLength+1;
}

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

int compareShifted8(bigint_t* a, bigint_t* b, uint64_t shift){
  uint64_t i = (a->length>(b->length + (shift+7)/8))?a->length:(b->length + (shift+7)/8);

  uint64_t* aptr = a->base + i;
  uint64_t bpos = i*8 - shift;

  while(i > 0){
    aptr--;
    bpos-=8;
    i--;

    uint64_t av = i < a->length?READ_UINT64_LE(aptr):0;
    uint64_t bv = (i < (b->length+(shift+7)/8) && i >= shift/8)?get64at8(b, bpos):0;

    if(av > bv){
       return 1;
     }
    if(av < bv){
      return -1;
    }
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

void mulTo(bigint_t* a, bigint_t* b, bigint_t* res){
  uint64_t len = a->length>b->length?a->length:b->length;
  uint64_t layerLength = len;
  uint64_t depth = 1;

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

  resizeBigint(res,len*2);
  karatsuba(a, b, res, temp);
  free(temp);
}

bigint_t* mul(bigint_t* a, bigint_t* b){
  uint64_t len = a->length>b->length?a->length:b->length;
  bigint_t* res = createEmptyBigint(len*2);
  mulTo(a,b,res);
  return res;
}

bigint_t* divide(bigint_t* a, bigint_t* b){
  bigint_t* q = createEmptyBigint(a->length);
  bigint_t* r = createEmptyBigint(b->length);
  divTo(a,b,q,r);
  destroyBigint(r);
  return q;
}

bigint_t* mod(bigint_t* a, bigint_t* b){
  bigint_t* q = createEmptyBigint(a->length);
  bigint_t* r = createEmptyBigint(b->length);
  divTo(a,b,q,r);
  destroyBigint(q);
  return r;
}


void divTo(bigint_t* x, bigint_t* y, bigint_t * q, bigint_t* r){
  copy(r,x);
  uint64_t n = (x->length << 3) - 1;
  uint64_t t = (y->length << 3) - 1;

  while(get8(x, n) == 0) n--;
  while(get8(y, t) == 0) t--;

  uint64_t qlen64 = (n - t) / 8+1;
  if(qlen64 == 0) qlen64++;
  resizeBigint(q, qlen64);
  q->length = qlen64;
  bigint_t* swap = createEmptyBigint(y->length);
  bigint_t* sgmulSwap = createEmptyBigint(1);
  sgmulSwap->length = 1;

  for(uint64_t i = 0;i<qlen64;i++){
    q->base[i] = 0;
  }

  uint64_t cmps = 0;
  while(compareShifted8(r, y, n-t) != -1){
    uint64_t tmp = get8(q, n-t);
    set8(q, n-t, tmp + 1);

    subToShifted8(r, y, r, n-t);
    cmps++;
  }


  for(uint64_t i = n;i >= t+1;i--){
    if(get8(r, i) == get8(y, t)){
      set8(q, i-t-1, 0xff);
    } else {
      uint64_t tmp = ((uint64_t)get8(r, i) << 8) | get8(r, i-1);
      set8(q, i-t-1, tmp/get8(y, t));
    }

    while(true){
      uint64_t m = ((get8(y, t) << 8) | get8(y, t-1)) * get8(q, i-t-1);
      uint64_t rm = (get8(r, i) << 16) | (get8(r, i-1) << 8) | get8(r, i-2);

      if(m <= rm) break;

      set8(q, i-t-1, get8(q,i-t-1)-1);
    }

    sgmulSwap->base[0] = get8(q, i-t-1);
    mulTo(y, sgmulSwap, swap);
    shift8(swap, i-t-1);

    if(compare(r, swap) == -1){
      subToShifted8(swap, y, swap, i-t-1);
      set8(q, i-t-1, get8(q,i-t-1)-1);
    }

    subTo(r, swap, r);
  }
  destroyBigint(swap);
  destroyBigint(sgmulSwap);

  r->length = y->length;
  while(r->base[r->length -1] == 0) r->length--;
}

void shift8(bigint_t* a, uint64_t n){
  uint64_t resz = (n+7)/8;
  resizeBigint(a, a->length + resz);
  memset(&a->base[a->length], 0, resz*sizeof(uint64_t));
  a->length += resz;
  uint8_t * base8 = (uint8_t*)a->base;
  memmove(&base8[n], base8, a->length*sizeof(uint64_t));
  memset(base8, 0, n*sizeof(uint8_t));
  if(a->base[a->length-1] == 0) a->length--;
}

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

void subToShifted(bigint_t* a, bigint_t* b, bigint_t * res, uint64_t shift){
  uint64_t shiftBytes = (shift+63)/64;
  uint64_t len = (a->length+shiftBytes)>b->length?(a->length+shiftBytes):b->length;
  resizeBigint(res, len);

  uint64_t* resptr = res->base;
  int64_t apos = -shift;
  uint64_t* bptr = b->base;


  uint64_t carry = 0;

  uint64_t finalLength = 0;
  uint64_t val,tmp;
  for(uint64_t i = 0; i < len;i++){
    if(apos >= -64 && i < a->length+shiftBytes){
      val = getAtBit(a, apos);
    } else {
      val = 0;
    }

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
    apos+=64;
    bptr++;
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
  bigint_t* res = createEmptyBigint(a->length);
  divTo(a,b,res);
  return res;
}

void divTo(bigint_t* n, bigint_t* d, bigint_t * res){
  //Convert to bigfloat
  bigfloat_t nf, df;
  nf.i = n;
  df.i = d;

  //Shift to 0.5 - 1.0
  uint64_t last = d->base[d->length - 1];
  uint64_t bit = 63;
  while((last & (1UL << bit)) == 0 && bit > 0) bit--;
  bit++;

  df.exp = (d->length - 1) * 64 + bit;
  nf.exp = (n->length - 1) * 64 + bit;

  //Create temporary values
  bigfloat_t t1, t2;

  //48/17
  t1.i = createEmptyBigint(2);
  t1.i->base[0] = 0xd2d2d2d2d2d2d2d3;
  t1.i->base[1] = 0x2;
  t1.i->length = 2;
  t1.exp = 64;

  //32/17
  t2.i = createEmptyBigint(2);
  t2.i->base[0] = 0xe1e1e1e1e1e1e2bb;
  t2.i->base[1] = 0x1;
  t2.i->length = 2;
  t2.exp = 64;

  //Calculate first x
  bigfloat_t t3;
  t3.i = mul(t2.i, df.i);
  t3.exp = t2.exp + df.exp;

  bigint_t* x = createEmptyBigint(t2.i->length);
  subToShifted(t1.i, t3.i, x, t3.exp - t1.exp);

  //Convert x to bigfloat
  bigfloat_t xf;
  xf.i = x;
  xf.exp = t3.exp;

  bigfloat_t tempf;
  tempf.i = createEmptyBigint(t2.i->length);

  //Create temporary float for 2 (1 later)
  bigfloat_t twof;
  twof.i = createEmptyBigint(1);
  twof.i->base[0] = 0x2;
  twof.i->length = 1;
  twof.exp = 0;

  //Create temporary bigfloat for multiplication
  bigfloat_t mult;
  mult.i = createEmptyBigint(xf.i->length);

  uint64_t p64 = (n->length - d->length + 1);
  double p =  p64 * 64;

  uint64_t steps = (uint64_t)ceil(
    log2((p+1)/log2(17))
  );

  bigint_t* swap = createEmptyBigint(xf.i->length);
  //Calculate precise x
  for(uint64_t i = 0;i<steps;i++){
    mulTo(df.i,xf.i,mult.i);
    mult.exp = df.exp + xf.exp;

    subToShifted(twof.i, mult.i, tempf.i, mult.exp - twof.exp);
    tempf.exp = mult.exp;

    xf.exp += tempf.exp;
    mulTo(xf.i,tempf.i,swap);
    bigint_t* tmp = xf.i;
    xf.i = swap;
    swap = tmp;
  }

  mulTo(nf.i,xf.i,res);
  bigfloat_t rf;
  rf.i = res;
  rf.exp = nf.exp + xf.exp;

  //Ceil if needed
  uint64_t firstDigit = 0;
  for(uint64_t i = res->length - 1;i < res->length;i--){
    uint64_t x = READ_UINT64_LE(&res->base[i]);
    if(x != 0){
      for(uint8_t bit = 0;bit<64;bit++){
        if(x == 1){
          firstDigit = i*64+bit;
          break;
        }
        x = x >> 1;
      }
      break;
    }
  }
  uint64_t lastPrecise = firstDigit - p;

  bool ceil = true;
  //Check full 8-bytes
  uint64_t i;
  for(i = lastPrecise;i<rf.exp - 64;i+=64){
    if(getAtBit(res,i) != 0xffffffffffffffff){
      ceil = false;
      break;
    }
  }

  //Check remaining bits
  if(ceil){
    uint64_t rem = getAtBit(res,i);
    uint64_t remMask = (1UL<<(rf.exp - i)) - 1;
    if((rem & remMask) != remMask){
      ceil = false;
    }
  }

  uint64_t intlength = rf.i->length - rf.exp / 64;
  uint64_t bitpos = rf.exp;

  for(uint64_t i = 0;i<intlength;i++){
    res->base[i] = getAtBit(res, bitpos);
    bitpos+=64;
  }
  res->length = intlength;

  if(ceil){
    twof.i->base[0] = 1;
    addTo(res,twof.i,res);
  }

  destroyBigint(t1.i);
  destroyBigint(t2.i);
  destroyBigint(t3.i);
  destroyBigint(x);
  destroyBigint(mult.i);
  destroyBigint(swap);
  destroyBigint(twof.i);
}

#ifndef ASYNC_H
#define ASYNC_H

#include <pthread.h>
#include <stdio.h>

#include "types.h"
#include "napi_utils.h"
#include "arithmetic.h"

struct async_op_node{
  struct async_op_node* next;
  async_op_t* op;
};
void runAsync(async_op_t* op);
void initAsync();

void setMaxThreads(uint64_t n);
#endif

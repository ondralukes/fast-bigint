#ifndef NAPI_UTILS
#define NAPI_UTILS
#include <js_native_api.h>
#include <node_api.h>

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

void getBigint(napi_env env, napi_value buf, bigint_t *res);
void createThreadsafeFunc(napi_env env, napi_value func, napi_threadsafe_function * res);

void callThreadsafeFunc(napi_threadsafe_function func);
void releaseThreadsafeFunc(napi_threadsafe_function func);
#endif

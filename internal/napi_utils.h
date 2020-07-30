#ifndef NAPI_UTILS
#define NAPI_UTILS
#include <js_native_api.h>
#include <node_api.h>

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

void getBigint(napi_env env, napi_value buf, bigint_t *res);

bigint_t* getBigintPtr(napi_env env, napi_value val);
napi_value fromBigintPtr(napi_env env, bigint_t * ptr);
void createThreadsafeFunc(napi_env env, napi_value func, napi_threadsafe_function * res);

void callThreadsafeFunc(napi_threadsafe_function func, void* data);
void releaseThreadsafeFunc(napi_threadsafe_function func);
#endif

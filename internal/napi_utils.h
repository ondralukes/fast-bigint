#ifndef NAPI_UTILS
#define NAPI_UTILS
#include <js_native_api.h>
#include <node_api.h>

#include "types.h"

void getBigint(napi_env env, napi_value buf, bigint_t *res);
#endif

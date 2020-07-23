#include "napi_utils.h"
#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

void getBigint(napi_env env, napi_value buf, bigint_t *res){
  NAPI_CALL(
    env,
    napi_get_buffer_info(
      env,
      buf,
      &res->base,
      &res->length
    )
  );

  res->length /= sizeof(uint64_t);
}

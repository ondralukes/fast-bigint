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

void createThreadsafeFunc(napi_env env, napi_value func, napi_threadsafe_function * res){
  napi_value resource_name;
  NAPI_CALL(
    env,
    napi_create_string_utf8(
      env,
      "engine",
      NAPI_AUTO_LENGTH,
      &resource_name
    )
  );

  NAPI_CALL(
    env,
    napi_create_threadsafe_function(
      env,  //env
      func, //func
      NULL, //async_resource
      resource_name, //async_resource_name
      0,    //max_queue_size
      1,    //initial_thread_count
      NULL, //thread_finalize_data
      NULL, //thread_finalize_cb
      NULL, //context
      NULL, //call_js_cb
      res   //result
    )
  );
}

void callThreadsafeFunc(napi_threadsafe_function func){
  napi_status res = napi_call_threadsafe_function(
    func,
    NULL,
    napi_tsfn_blocking
  );
}

void releaseThreadsafeFunc(napi_threadsafe_function func){
  napi_release_threadsafe_function(
    func,
    napi_tsfn_release
  );
}

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
      }                                                           \
    }                                                             \
  } while(0)

void getBigint(napi_env env, napi_value buf, bigint_t *res){
  NAPI_CALL(
    env,
    napi_get_buffer_info(
      env,
      buf,
      (void**)&res->base,
      &res->length
    )
  );

  res->length /= sizeof(uint64_t);
}

bigint_t* getBigintPtr(napi_env env, napi_value val){
  int64_t res;

  NAPI_CALL(
    env,
    napi_get_value_int64(
      env,
      val,
      &res
    )
  );

  return (bigint_t*)res;
}

napi_value fromBigintPtr(napi_env env, bigint_t * ptr){
  napi_value res;

  NAPI_CALL(
    env,
    napi_create_int64(
      env,
      (uint64_t) ptr,
      &res
    )
  );

  return res;
}

void tsfn_result_cb(
  napi_env env,
  napi_value js_callback,
  void* context,
  void* data){
    uint64_t ptr = *((uint64_t*)data);
    napi_value res;
    NAPI_CALL(
      env,
      napi_create_int64(
        env,
        ptr,
        &res
      )
    );
    
    NAPI_CALL(
      env,
      napi_call_function(
        env,
        js_callback,
        js_callback,
        1,
        &res,
        NULL
      )
    );
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
      tsfn_result_cb, //call_js_cb
      res   //result
    )
  );
}

void callThreadsafeFunc(napi_threadsafe_function func, void* data){
  napi_call_threadsafe_function(
    func,
    data,
    napi_tsfn_blocking
  );
}

void releaseThreadsafeFunc(napi_threadsafe_function func){
  napi_release_threadsafe_function(
    func,
    napi_tsfn_release
  );
}

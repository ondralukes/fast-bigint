#include "internal.h"

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


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define WRITE_UINT64_LE(ptr, number) *ptr = number;
  #define READ_UINT64_LE(ptr) *ptr;
#else
  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define WRITE_UINT64_LE(ptr, number) *ptr = bswap_64(number);
    #define READ_UINT64_LE(ptr) bswap64(*ptr);
  #else
    #error PDP-endian not supported
  #endif
#endif

void * getBufferData(napi_env env, napi_value buffer, size_t* length){
  void* res;
  NAPI_CALL(
    env,
    napi_get_buffer_info(
      env,
      buffer,
      &res,
      length
    )
  );
  return res;
}

static napi_value
setUInt64(napi_env env, napi_callback_info info){

  napi_value argv[2];
  size_t argc = 2;

  NAPI_CALL(
    env,
    napi_get_cb_info(
      env,
      info,
      &argc,
      argv,
      NULL,
      NULL
    )
  );

  napi_value buffer = argv[0];
  napi_value num = argv[1];


  size_t length;
  uint64_t * ptr = getBufferData(env, buffer, &length);

  uint64_t number;

  NAPI_CALL(
    env,
    napi_get_value_int64(
      env,
      num,
      &number
    )
  );

  WRITE_UINT64_LE(ptr, number);
  return NULL;
}

static napi_value
add(napi_env env, napi_callback_info info){

  napi_value argv[3];
  size_t argc = 3;

  NAPI_CALL(
    env,
    napi_get_cb_info(
      env,
      info,
      &argc,
      argv,
      NULL,
      NULL
    )
  );

  napi_value js_result = argv[0];
  napi_value js_a = argv[1];
  napi_value js_b = argv[2];

  size_t ressz;
  size_t asz;
  size_t bsz;

  uint64_t * res = getBufferData(env, js_result, &ressz);
  uint64_t * a = getBufferData(env, js_a, &asz);
  uint64_t * b = getBufferData(env, js_b, &bsz);

  ressz /= sizeof(uint64_t);
  asz /= sizeof(uint64_t);
  bsz /= sizeof(uint64_t);

  uint64_t carry = 0;
  uint64_t temp;
  for(uint64_t i = 0; i < ressz;i++){
    uint64_t val = carry;
    carry = 0;
    if(i < asz){
      uint64_t a_val = READ_UINT64_LE(a);
      temp = val;
      val += a_val;
      if(val < temp){
        carry++;
      }
    }
    if(i < bsz){
      uint64_t b_val = READ_UINT64_LE(b);
      temp = val;
      val += b_val;
      if(val < temp){
        carry++;
      }
    }

    WRITE_UINT64_LE(res, val);
    res++;
    b++;
    a++;
  }



  return NULL;
}

void*
add_function(
  napi_env env,
  napi_value obj,
  napi_value(*func)(napi_env, napi_callback_info),
  const char * name
){

  napi_value exported_func;
  NAPI_CALL(
    env,
    napi_create_function(
      env,
      name,
      NAPI_AUTO_LENGTH,
      func,
      NULL,
      &exported_func
    )
  );

  NAPI_CALL(
    env,
    napi_set_named_property(
      env,
      obj,
      name,
      exported_func
    )
  );

  return NULL;
}

napi_value create_addon(napi_env env){
  napi_value res;
  NAPI_CALL(env, napi_create_object(env, &res));

  add_function(
    env,
    res,
    setUInt64,
    "setUInt64"
  );

  add_function(
    env,
    res,
    add,
    "add"
  );

  return res;
}

NAPI_MODULE_INIT(){
  return create_addon(env);
}

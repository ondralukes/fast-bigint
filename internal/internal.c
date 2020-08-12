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
      }                                                           \
    }                                                             \
  } while(0)


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
ex_setUInt64(napi_env env, napi_callback_info info){

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

  bigint_t * bigint = getBigintPtr(env, argv[0]);

  uint64_t number;

  NAPI_CALL(
    env,
    napi_get_value_int64(
      env,
      argv[1],
      (int64_t*)&number
    )
  );

  resizeBigint(bigint, 1);
  bigint->length = 1;
  WRITE_UINT64_LE(bigint->base, number);
  return NULL;
}

static napi_value
ex_add(napi_env env, napi_callback_info info){

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

  bigint_t* a = getBigintPtr(env, argv[0]);
  bigint_t* b = getBigintPtr(env, argv[1]);

  bigint_t* res = add(a, b);
  return fromBigintPtr(env, res);
}

static napi_value
ex_sub(napi_env env, napi_callback_info info){

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

  bigint_t* a = getBigintPtr(env, argv[0]);
  bigint_t* b = getBigintPtr(env, argv[1]);

  bigint_t* res = sub(a, b);
  return fromBigintPtr(env, res);
}

static napi_value
ex_mul(napi_env env, napi_callback_info info){

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

  bigint_t* a = getBigintPtr(env, argv[0]);
  bigint_t* b = getBigintPtr(env, argv[1]);

  bigint_t* res = mul(a, b);
  return fromBigintPtr(env, res);
}

static napi_value
ex_div(napi_env env, napi_callback_info info){

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

  bigint_t* a = getBigintPtr(env, argv[0]);
  bigint_t* b = getBigintPtr(env, argv[1]);

  bigint_t* res = divide(a, b);
  return fromBigintPtr(env, res);
}

static napi_value
ex_addAsync(napi_env env, napi_callback_info info){

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

  async_op_t* op = malloc(sizeof(async_op_t));
  op->type = Add;

  op->argv = malloc(sizeof(bigint_t) * 3);
  for(int i = 0;i<2;i++){
    op->argv[i] = getBigintPtr(env, argv[i]);
  }

  op->argc = 2;
  createThreadsafeFunc(env, argv[2], &op->callback);
  runAsync(op);
  return NULL;
}

static napi_value
ex_subAsync(napi_env env, napi_callback_info info){

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

  async_op_t* op = malloc(sizeof(async_op_t));
  op->type = Sub;

  op->argv = malloc(sizeof(bigint_t) * 3);
  for(int i = 0;i<2;i++){
    op->argv[i] = getBigintPtr(env, argv[i]);
  }

  op->argc = 2;
  createThreadsafeFunc(env, argv[2], &op->callback);
  runAsync(op);
  return NULL;
}

static napi_value
ex_mulAsync(napi_env env, napi_callback_info info){

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

  async_op_t* op = malloc(sizeof(async_op_t));
  op->type = Mul;

  op->argv = malloc(sizeof(bigint_t) * 3);
  for(int i = 0;i<2;i++){
    op->argv[i] = getBigintPtr(env, argv[i]);
  }

  op->argc = 2;
  createThreadsafeFunc(env, argv[2], &op->callback);
  runAsync(op);
  return NULL;
}

static napi_value
ex_compare(napi_env env, napi_callback_info info){

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

  bigint_t* a = getBigintPtr(env, argv[0]);
  bigint_t* b = getBigintPtr(env, argv[1]);

  int res = compare(a, b);
  napi_value resjs;
  NAPI_CALL(
    env,
    napi_create_int32(
      env,
      res,
      &resjs
    )
  );
  return resjs;
}

static napi_value
ex_setMaxThreads(napi_env env, napi_callback_info info){
  napi_value arg;
  size_t argc = 1;

  NAPI_CALL(
    env,
    napi_get_cb_info(
      env,
      info,
      &argc,
      &arg,
      NULL,
      NULL
    )
  );

  uint64_t threads;
  NAPI_CALL(
    env,
    napi_get_value_int64(
      env,
      arg,
      (int64_t*)&threads
    )
  );

  setMaxThreads(threads);
  return NULL;
}

void destructor(napi_env env,void* finalize_data,void* finalize_hint){
  destroyBigint((bigint_t * ) finalize_data);
}

static napi_value
ex_create(napi_env env, napi_callback_info info){
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

  bigint_t* bigint;
  if(argc == 1){
    bigint = createEmptyBigint(256);
  } else {
    size_t size;
    void* data = getBufferData(env, argv[1], &size);

    size_t alignedSize = (size + 7)/8;

    //Allocate buffer
    bigint = createEmptyBigint(alignedSize);

    //Clear non-copied part
    bigint->base[alignedSize - 1] = 0;

    //Copy
    memcpy(bigint->base, data, size);

    bigint->length = alignedSize;
  }

  napi_value js_ptr;
  NAPI_CALL(
    env,
    napi_create_int64(
      env,
      (int64_t) bigint,
      &js_ptr
    )
  );

  NAPI_CALL(
    env,
    napi_add_finalizer(
      env,
      argv[0],
      bigint,
      destructor,
      NULL,
      NULL
    )
  );

  return js_ptr;
}

static napi_value
ex_fromPtr(napi_env env, napi_callback_info info){
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

  bigint_t* bigint = getBigintPtr(env, argv[1]);

  NAPI_CALL(
    env,
    napi_add_finalizer(
      env,
      argv[0],
      bigint,
      destructor,
      NULL,
      NULL
    )
  );

  return NULL;
}


static napi_value
ex_getBuffer(napi_env env, napi_callback_info info){
  napi_value arg;
  size_t argc = 1;

  NAPI_CALL(
    env,
    napi_get_cb_info(
      env,
      info,
      &argc,
      &arg,
      NULL,
      NULL
    )
  );

  bigint_t* bigint = getBigintPtr(env, arg);

  napi_value buf;
  NAPI_CALL(
    env,
    napi_create_buffer_copy(
      env,
      bigint->length * 8,
      bigint->base,
      NULL,
      &buf
    )
  );

  return buf;
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
    ex_setUInt64,
    "setUInt64"
  );

  add_function(
    env,
    res,
    ex_add,
    "add"
  );

  add_function(
    env,
    res,
    ex_sub,
    "sub"
  );

  add_function(
    env,
    res,
    ex_mul,
    "mul"
  );

  add_function(
    env,
    res,
    ex_div,
    "div"
  );

  add_function(
    env,
    res,
    ex_compare,
    "compare"
  );

  add_function(
    env,
    res,
    ex_addAsync,
    "addAsync"
  );

  add_function(
    env,
    res,
    ex_subAsync,
    "subAsync"
  );

  add_function(
    env,
    res,
    ex_mulAsync,
    "mulAsync"
  );

  add_function(
    env,
    res,
    ex_setMaxThreads,
    "setMaxThreads"
  );

  add_function(
    env,
    res,
    ex_create,
    "create"
  );

  add_function(
    env,
    res,
    ex_fromPtr,
    "fromPtr"
  );

  add_function(
    env,
    res,
    ex_getBuffer,
    "getBuffer"
  );
  return res;
}

NAPI_MODULE_INIT(){
  initAsync();
  return create_addon(env);
}

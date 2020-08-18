#include "async.h"

uint64_t threadCount = 0;
uint64_t maxThreads = 8;

struct async_op_node* enqueue = NULL;
struct async_op_node* dequeue = NULL;

pthread_mutex_t queueMutex;

void initAsync(){
  pthread_mutex_init(&queueMutex, NULL);
}

void setMaxThreads(uint64_t n){
  maxThreads = n;
}

void execute(async_op_t * op){
  bigint_t* res = NULL;
  if(op->type == Add){
    res = add(op->argv[0],op->argv[1]);
  } else if(op->type == Sub){
    res = sub(op->argv[0],op->argv[1]);
  } else if(op->type == Mul){
    res = mul(op->argv[0],op->argv[1]);
  } else if(op->type == Div){
    res = divide(op->argv[0],op->argv[1]);
  } else if(op->type == Mod){
    res = mod(op->argv[0],op->argv[1]);
  }

  callThreadsafeFunc(op->callback, &res);
  releaseThreadsafeFunc(op->callback);
};

void* executor(void* data){
  async_op_t* op = (async_op_t*)data;

  while(op!=NULL){
    execute(op);
    pthread_mutex_lock(&queueMutex);
    if(dequeue != NULL){
      op = dequeue->op;
      dequeue = dequeue->next;
      if(dequeue == NULL) enqueue = NULL;
    } else {
      op = NULL;
    }
    pthread_mutex_unlock(&queueMutex);
  }
  __atomic_fetch_sub(&threadCount, 1, __ATOMIC_SEQ_CST);
  return NULL;
}

void runAsync(async_op_t* op){
  uint64_t threads = __atomic_load_n(&threadCount, __ATOMIC_SEQ_CST);
  if(threads < maxThreads){
    __atomic_fetch_add(&threadCount, 1, __ATOMIC_SEQ_CST);
    pthread_t id;
    pthread_create(&id, NULL, executor, op);
  } else {
    struct async_op_node* node = malloc(sizeof(struct async_op_node));
    node->next = NULL;
    node->op = op;

    pthread_mutex_lock(&queueMutex);
    if(enqueue == NULL){
      enqueue = node;
      dequeue = node;
    } else {
      enqueue->next = node;
      enqueue = node;
    }
    pthread_mutex_unlock(&queueMutex);
  }
}

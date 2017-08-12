#include <pthread.h>

//
// Created by Administrator on 2017/8/11.
//
typedef struct _AVQueue AVQueue;
//分配队列元素的函数
typedef void* (*queue_fill_fun)();
//稀放队列元素所占的内存
typedef void* (*queue_free_fun)(void* elem);

AVQueue *queue_init(int size,queue_fill_fun fill_fun);

void queue_free(AVQueue *queue,queue_free_fun free_fun);

int get_next(AVQueue *queue, int current);
//压入对列
void *queue_push(AVQueue *queue, pthread_mutex_t *ptr, pthread_cond_t *ptr1);
//弹出对列
void *queue_pop(AVQueue *queue, pthread_mutex_t *ptr, pthread_cond_t *ptr1);



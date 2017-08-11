//
// Created by Administrator on 2017/8/11.
//
typedef struct _AVQueue AVQueue;

AVQueue *queue_init(int size);

void queue_free(AVQueue *queue);

int get_next(AVQueue *queue, int current);
//压入对列
void *queue_push(AVQueue *queue);
//弹出对列
void* queue_pop(AVQueue *queue);



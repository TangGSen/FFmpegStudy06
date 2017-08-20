//
// Created by Administrator on 2017/8/16.
//

#ifndef FFMPEGSTUDY07_QUEUE_H
#define FFMPEGSTUDY07_QUEUE_H

//新建双向链表。成功返回表头，否则返回NULL
extern  int create_queue();

//销毁双向链表。成功返回0；否-1
extern int destroy_queue();

//双向链表是否为空，为空的话返回1，否则返回0；
extern int queue_is_empty();

//返回大小
extern int queue_size();

//获取双向链表中第index 位置数据，成功返回节点指针，否NULL
extern void* queue_get(int index);

//获取双向链表中第一个元素，同上
extern void* queue_get_first();
//最后一个元素
extern void* queuq_get_last();

//将value 插入index 位置
extern int queue_inster(int index, void* value);

//value 插入表头
extern int queue_insert_first(void *value);
//插入末尾
extern int queue_insert_last(void* value);

//删除index 位置
extern int queue_delete(int index);

//删除第一个
extern  int queue_delete_first();

//删除最后一个
extern int queue_delete_last();




#endif //FFMPEGSTUDY07_QUEUE_H

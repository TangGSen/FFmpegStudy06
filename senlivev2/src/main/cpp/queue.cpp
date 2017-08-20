//
// Created by Administrator on 2017/8/16.
//

#include "queue.h"
#include <string>

typedef struct queue_node {
    queue_node *pre;
    queue_node *next;
    void *value;//节点的值

} node;

//表头 表头不放元素值
static node *header = NULL;
static int count = 0;

//创建节点
static node *create_node(void *value) {
    node *pnode = NULL;
    pnode = (node *) malloc(sizeof(node));
    if (pnode) {
        //默认 pnode 前一个节点和后一个节点都指向它本身
        pnode->next = pnode->pre = pnode;
        pnode->value = value;
    }
    return pnode;
}


//新建双向链表。成功返回表头，否则返回NULL
int create_queue() {
    header = create_node(NULL);
    if (!header) {
        return -1;
    }
    //设置节点个数为0
    count = 0;
    return 0;
};


//销毁双向链表。成功返回0；否-1
int destroy_queue(){
    if (!header) {
        return -1;
    }
    node *pnode = header->next;
    node *ptmp = NULL;
    while (pnode != header) {
        ptmp = pnode;
        pnode = pnode->next;
        free(ptmp);
    }
    free(header);
    header = NULL;
    count = 0;
    return 0;
}

//双向链表是否为空，为空的话返回1，否则返回0；
int queue_is_empty() {
    return count == 0;
}

//返回大小
int queue_size() {
    return count;
}

static node *get_node(int index) {
    if (index < 0 || index > count) {
        return NULL;
    }

    if(index <=(count /2)){
        int i = 0;
        node *pnode = header->next;
        while(i++<index){
            pnode =pnode->next;
        }
        return pnode;
    }
    //前一半找不到的话，重后面开始找
    int j = 0;
    int rindex = count -index -1;
    node *rnode = header->pre;
    while(j++<rindex){
        rnode =rnode->pre;
    }
    return rnode;

}

//获取双向链表中第index 位置数据，成功返回节点值，否NULL
void *queue_get(int index){
    node * get =get_node(index);
    if(!get){
        return NULL;
    }
    return get->value;
};

//获取双向链表中第一个元素，同上
void *queue_get_first(){
    return get_node(0);
};

//最后一个元素
void *queuq_get_last(){
      return get_node(count -1);
};

//将value 插入index 位置
int queue_inster(int index, void *value){
    //插入表头
    if(index ==0){
        return queue_insert_first(value);
    }

    //获取要插入的位置对应的节点
    node *pindex = get_node(index);
    if (!pindex){
        return -1;
    }
    //找到要插入的位置
    //创建节点,
    node *pnode = create_node(value);
    if (!pnode)
        return -1;
    pnode->pre = pindex->pre;
    pnode->next = pindex;
    pindex->pre->next = pnode;
    pindex->pre = pnode;
    // 节点个数+1
    count++;
    return 0;
}

//value 插入表头
int queue_insert_first(void *value){
    node *pnode = create_node(value);
    if (!pnode)
        return -1;
    pnode->pre = header;
    pnode->next = header->next;
    header->next->pre = pnode;
    header->next = pnode;
    count++;
    return 0;
}

//插入末尾
int queue_insert_last(void *value){
    node *pnode = create_node(value);
    if (!pnode)
        return -1;
    pnode->next = header;
    pnode->pre = header->pre;
    header->pre->next = pnode;
    header->pre = pnode;
    count++;
    return 0;
}

//删除index 位置
int queue_delete(int index){
    node *pindex = get_node(index);
    if (!pindex) {
        return -1;
    }
    pindex->next->pre = pindex->pre;
    pindex->pre->next = pindex->next;
    free(pindex);
    count--;
    return 0;
}

//删除第一个
int queue_delete_first(){
    return  queue_delete(0);
}

//删除最后一个
int queue_delete_last(){
    return queue_delete(count - 1);
}

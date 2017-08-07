#include "thread.h"
#include "android/log.h"
#include "unistd.h"
#include "pthread.h"
#include <string>

#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

pthread_t pthreadT;

void* run(void *arg){

    for (int i = 0; i < 20; ++i) {
        LOGE("i= %d",i);

    }
    return NULL;
}
JNIEXPORT void JNICALL Java_sen_com_thread_PThread_createThread
        (JNIEnv *env, jobject jobj) {
   /** 第一个参数：指向线程标示符pthread_t的指针；
    第二个参数：设置线程的属性
    第三个参数：线程运行函数的起始地址
    第四个参数：运行函数的参数*/
   pthread_create(&pthreadT,NULL,run,NULL);
};
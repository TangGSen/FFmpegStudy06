#include "thread.h"
#include "android/log.h"
#include "unistd.h"
#include "pthread.h"
#include <string>

#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

pthread_t pthreadT;
JavaVM *javaVM ;
jclass uuidClassGR;
jmethodID methodId ;


void * run(void *arg){

    JNIEnv *env ;
    javaVM->AttachCurrentThread(&env,NULL);
    for (int i = 0; i < 20; ++i) {
       jobject  jstrObj= env->CallStaticObjectMethod(uuidClassGR, (jmethodID) methodId);
        const  char* uuid =env->GetStringUTFChars((jstring) jstrObj, NULL);
        LOGE("i= %s", uuid);
        if (i ==10){
            goto end;
        }
        sleep(1);
    }
    end:
    //必须先取消绑定才退出线程
    javaVM->DetachCurrentThread();
    pthread_exit((void*)0);
    return NULL;
}


JNIEXPORT void JNICALL Java_sen_com_thread_PThread_destroy
        (JNIEnv *env, jobject jobj){
    env->DeleteGlobalRef(uuidClassGR);
};
JNIEXPORT void JNICALL Java_sen_com_thread_PThread_init
        (JNIEnv *env, jobject jobj){
    env->GetJavaVM(&javaVM);
    //获取class 必须在主线程中
    jclass uuidClass = env->FindClass("sen/com/other/UUIDUtils");
    //创建全局引用
    uuidClassGR = (jclass) env->NewGlobalRef(uuidClass);
    if(uuidClassGR ==NULL){
        LOGE("uuidClassGR ==NULL");
    }else{
        LOGE("uuidClassGR !=NULL");
    }
     methodId = env->GetStaticMethodID(uuidClassGR,"getUuid","()Ljava/lang/String;");



}

JNIEXPORT void JNICALL Java_sen_com_thread_PThread_createThread
        (JNIEnv *env, jobject jobj) {
if(uuidClassGR ==NULL || methodId ==NULL){
    LOGE("请先init");
    return;
}

   /** 第一个参数：指向线程标示符pthread_t的指针；
    第二个参数：设置线程的属性
    第三个参数：线程运行函数的起始地址
    第四个参数：运行函数的参数*/
   pthread_create(&pthreadT,NULL,run,(void *)"No.1");
    pthread_join(pthreadT,NULL);

};
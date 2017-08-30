//
// Created by Administrator on 2017/8/29.
//

#include "fixbug.h"
#include "dalvik.h"
//执行虚拟机libdvm.so 里的方法  hook
typedef Object* (*FindClassObject)(void * thread, jobject jobj);
typedef void *(*FindThread)();
FindClassObject findClassObject;
FindThread findThread;
JNIEXPORT void JNICALL Java_sen_com_fixndk_fixcore_DexManager_replace
        (JNIEnv *env, jobject jobj,jint sdkInt, jobject bug, jobject noBug){

    //找到虚拟机对应的结构体
    Method *bugMethod = (Method *) env->FromReflectedMethod(bug);
    Method *noBugMethod = (Method *) env->FromReflectedMethod(noBug);


    void * dvm_handler = dlopen("libdvm.so",RTLD_NOW);
    findClassObject = (FindClassObject) dlsym(dvm_handler,
                                              sdkInt>10?"_Z20dvmDecodeIndirectRefP6ThreadP8_jobject":"dvmDecodeIndirectRef");
    findThread = (FindThread) dlsym(dvm_handler, sdkInt>10?"_Z13dvmThreadSelfv":"dvmThreadSelf");

    //Method 所声明的class
    jclass methodClass = env->FindClass("java/lang/reflect/Method");
    jmethodID noBugMethodID = env->GetMethodID(methodClass,
                                               "getDeclaringClass","()Ljava/lang/Class;");

    jobject ndkObj = env->CallObjectMethod(noBug,noBugMethodID);

    ClassObject *classObject = (ClassObject *) findClassObject(findThread(), ndkObj);
    //开始修改
    classObject->status =CLASS_INITIALIZED;
    //访问修饰符
    bugMethod->accessFlags|=ACC_PUBLIC;
    bugMethod->methodIndex = noBugMethod->methodIndex;
    bugMethod->jniArgInfo = noBugMethod->jniArgInfo;
    bugMethod->registersSize =noBugMethod->registersSize;
    bugMethod->outsSize = noBugMethod->outsSize;
    //方法参数 原型
    bugMethod->prototype = noBugMethod->prototype;
    bugMethod->insns = noBugMethod->insns;
    bugMethod->nativeFunc = noBugMethod->nativeFunc;

}

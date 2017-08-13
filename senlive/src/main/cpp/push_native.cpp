#include "push_native.h"

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_sendVideo
        (JNIEnv *env, jobject jobj, jbyteArray data){

};


JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_sendAudio
        (JNIEnv *env, jobject jobj, jbyteArray data, jint len){

};


JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_startPush
        (JNIEnv *env, jobject jobj, jstring jUrl){

};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_stopPush
        (JNIEnv *env, jobject jobj){

};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_release
        (JNIEnv *env, jobject jobj){

};


JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_setVideoOptions
        (JNIEnv *env, jobject jobj, jint width , jint height, jint bitrate, jint flp){

};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_setAudioOptions
        (JNIEnv *env, jobject jobj, jint sampleRateInHz, jint chancle){

};


#include "push_native.h"
#include <string>
#include "x264/x264.h"
#include "android/log.h"

#define LOG_TAG    "x264" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

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

/**
 *
 *1.Profile是对视频压缩特性的描述（CABAC呀、颜色采样数等等）。Level是对视频本身特性的描述（码率、分辨率、fps）。
 *简单来说，Profile越高，就说明采用了越高级的压缩特性。Level越高，视频的码率、分辨率、fps越高
 *2.这里的参数：可以下载源码x264 example.c
 */

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_setVideoOptions
        (JNIEnv *env, jobject jobj, jint width , jint height, jint bitrate, jint fps){
   /**** x264_param_t param init satar ****/
    x264_param_t param ;
    x264_param_default_preset(&param ,"ultrafast","zerolatency");


    /* Configure non-default params */
    //编码的输入像素格式，所以上面采集的视频数据得转YUV420
    param.i_csp = X264_CSP_I420;
    param.i_width  = width;
    param.i_height = height;
    //i_rc_method 表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR平均码率
    param.rc.i_rc_method = X264_RC_CRF;
    //码率
    param.rc.i_bitrate =bitrate/1000;
    //瞬时最大码率
    param.rc.i_vbv_max_bitrate =bitrate/1000*1.2;
    //帧率分子
    param.i_fps_num =fps;
    //帧率分母
    param.i_fps_den =1;
    param.i_timebase_den = param.i_fps_num;
    param.i_timebase_num = param.i_fps_den ;
    //并行编码线程数量，0默认多线程
    param.i_threads = 1;
    //通过帧率fps 控制码率，而不是通过timebase 和timestamp
    param.b_vfr_input = 0;
//    put SPS/PPS before each keyframe 是否将sps/pps 放入每一个关键帧
    //SPS Sequenece Parameter Set 序列参数集，PPS Pitrue Parameter Set 图像参数集
    //这样好处，就是前一帧破坏了不影响到别的帧播放，提高了图像的纠错能力
    param.b_repeat_headers = 1;

    //设置level 5.1 最高级别
    param.i_level_idc =51;

    //设置profile，没有B帧
    x264_param_apply_profile( &param, "baseline" );
    /**** x264_param_t param init end ****/

    /**** 输入图像初始化****/
    x264_picture_t pic ;
    x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height);

    /**** 打开编码器  ****/

    x264_t * x264_encoder =x264_encoder_open(&param);
    if(x264_encoder ==NULL){
        LOGE("x264 encoder open fail");
    } else{
        LOGE("x264 encoder open sucess");
    }



};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_setAudioOptions
        (JNIEnv *env, jobject jobj, jint sampleRateInHz, jint chancle){

};


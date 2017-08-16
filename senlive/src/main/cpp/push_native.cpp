#include "push_native.h"
#include <string>
#include "x264/x264.h"
#include "android/log.h"
#include "rtmp.h"
#include "queue.h"
#include <pthread.h>
#define LOG_TAG    "x264" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
//输入图像
x264_picture_t pic_in ;
x264_picture_t pic_out ;
int y_len,v_u_len;
//x264 编码处理器
x264_t * x264_enco_handler;

pthread_mutex_t mutex;
pthread_cond_t cond;

//流媒体地址
char *rtmp_path;
bool  isPushing = false;


unsigned int start_time;

void add_rtmp_packet(RTMPPacket *pPacket);

void JNICALL Java_sen_com_senlive_natives_PushNative_sendVideo
        (JNIEnv *env, jobject jobj, jbyteArray data){

    jbyte* nv21_buffer = env->GetByteArrayElements(data,NULL);
    /**
     *  NV21 ->YUV420P
     *  nv21,yuv420p y个数一致，uv 位置对调
     *  nv21 = yvu
     *  yuv420p = yuv
     *  y= y ,u= y+1+1,v=y+1
     */


    jbyte* u = (jbyte *) pic_in.img.plane[1];
    jbyte* v = (jbyte *) pic_in.img.plane[2];
    memcpy(pic_in.img.plane[0],nv21_buffer,y_len);
    for (int i = 0; i < v_u_len; ++i) {
        *(u+i) =*(nv21_buffer+y_len+i*2+1);
        *(v+i) =*(nv21_buffer+y_len+i*2);
    }
//    x264_t *, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_in, x264_picture_t *pic_out
    //x264编码之后得到的NaLU单元数组
    x264_nal_t *nal = NULL;
    //nal 个数
    int pi_nal = -1;
    int result =  x264_encoder_encode(x264_enco_handler,&nal,&pi_nal,&pic_in,&pic_out);
    if(result <0){
        LOGE("编码失败");
        return;
    }

    //将rtmp协议将数据发送到流媒体服务器
    unsigned char sps[100];
    memset(sps,0,100);
    unsigned char pps[100];
    memset(pps,0,100);
    int sps_len,pps_len;

    for (int i = 0; i < pi_nal; ++i) {
        if (nal[i].i_type ==NAL_SPS){
            sps_len = nal[i].i_payload-4;
            //不复制4字节起始码
            memcpy(sps,nal[i].p_payload+4,sps_len);
        } else if (nal[i].i_type ==NAL_PPS){
            pps_len = nal[i].i_payload-4;
            memcpy(pps,nal[i].p_payload+4,pps_len);
//            pps 和sps 数据添加到H264 关键帧
            //发送序列信息
            add_sueque_header(sps,pps,sps_len,pps_len);
        } else{
            //发送帧信息
            add_x264_body(nal[i].p_payload,nal[i].i_payload);
        }

    }


}
//消费者线程，发送packet 到服务器
void* push_thread(void* args){
    RTMP * rtmp =RTMP_Alloc();
    if(!rtmp){
        LOGE("rtmp init fail");
        goto end;
    }
    RTMP_Init(rtmp);
    rtmp->Link.timeout=5;
    RTMP_SetupURL(NULL,rtmp_path);
    //首先做这个步
    RTMP_EnableWrite(rtmp);
    //建立链接
   if(!RTMP_Connect(rtmp,0)){
      LOGE("rtmp 链接失败");
       goto end;
   }
    //计时
    start_time = RTMP_GetTime();
    //建立Rtmp 链接
    while(isPushing){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);//阻塞
        //取出队列中元素发送
        RTMPPacket *packet = (RTMPPacket *) queue_get_first();

        if(packet){
            queue_delete_first();//先移除在使用
            packet->m_nInfoField2 =rtmp->m_stream_id;
            int res = RTMP_SendPacket(rtmp,packet, true);//true 放在内部队列，并不是立即发送
            if(!res){
                LOGE("发送失败");
            }
            RTMPPacket_Free(packet);
        }
        pthread_mutex_unlock(&mutex);
    }
    end:
    LOGE("稀放");
    RTMP_Close(rtmp);
    RTMP_Free(rtmp);
    free(rtmp_path);


}



JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_sendAudio
        (JNIEnv *env, jobject jobj, jbyteArray data, jint len){

};


JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_startPush
        (JNIEnv *env, jobject jobj, jstring jUrl){
    //初始化操作

    const char * url = env->GetStringUTFChars(jUrl,NULL);
    rtmp_path = (char *) malloc(strlen(url)+1);
    memset(rtmp_path,0,strlen(url)+1);
    memcpy(rtmp_path,url,strlen(url));

    //创建队列
    create_queue();
    //启动消费者线程（从队列中不断拉取packet发送到流媒体服务器）
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_t  push_thread_id;
    isPushing = true;
    pthread_create(&push_thread_id,NULL,push_thread,NULL);

    env->ReleaseStringUTFChars(jUrl,url);
};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_stopPush
        (JNIEnv *env, jobject jobj){
    isPushing = false;

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

    //初始YUV 格式
     y_len = width * height;
    v_u_len = y_len/4;


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

    x264_picture_alloc( &pic_in, param.i_csp, param.i_width, param.i_height);

    /**** 打开编码器  ****/

    x264_enco_handler =x264_encoder_open(&param);
    if(x264_enco_handler ==NULL){
        LOGE("x264 encoder open fail");
    } else{
        LOGE("x264 encoder open sucess");
    }





};

JNIEXPORT void JNICALL Java_sen_com_senlive_natives_PushNative_setAudioOptions
        (JNIEnv *env, jobject jobj, jint sampleRateInHz, jint chancle){

};


//发送x264帧信息
void add_x264_body(	unsigned char *buf, int len) {
    //去掉起始码（界定符）
    if(buf[2] ==0x00){
        buf+=4;
        len-=4;
    }else if(buf[2]==0x01){
        buf+=3;
        buf-=3;
    }
/*    int body_size = len+9;
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(packet,body_size);
    RTMPPacket_Reset(packet);
    unsigned char *body = (unsigned char *) packet->m_body;

    //当NAL头信息中，type（5位）等于5，说明这是关键帧NAL单元
    //buf[0] NAL Header与运算，获取type，根据type判断关键帧和普通帧
    //00000101 & 00011111(0x1f) = 00000101
    int type = buf[0] & 0x1f;
    //Inter Frame 帧间压缩
    body[0] = 0x27;//VideoHeaderTag:FrameType(2=Inter Frame)+CodecID(7=AVC)
    //IDR I帧图像
    if (type == NAL_SLICE_IDR) {
        body[0] = 0x17;//VideoHeaderTag:FrameType(1=key frame)+CodecID(7=AVC)
    }
    //AVCPacketType = 1
    body[1] = 0x01; *//*nal unit,NALUs（AVCPacketType == 1)*//*
    body[2] = 0x00; //composition time 0x000000 24bit
    body[3] = 0x00;
    body[4] = 0x00;

    //写入NALU信息，右移8位，一个字节的读取？
    body[5] = (len >> 24) & 0xff;
    body[6] = (len >> 16) & 0xff;
    body[7] = (len >> 8) & 0xff;
    body[8] = (len) & 0xff;

    *//*copy data*//*
    memcpy(&body[9], buf, len);


    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = body_size;
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;//当前packet的类型：Video
    packet->m_nChannel = 0x04;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
//	packet->m_nTimeStamp = -1;
    packet->m_nTimeStamp = RTMP_GetTime() - start_time;//记录了每一个tag相对于第一个tag（File Header）的相对时间
    add_rtmp_packet(packet);*/

}
//将pps 和sps 添加到关键帧并发送
void add_sueque_header(unsigned char* sps, unsigned char* pps, int sps_len, int pps_len) {
    //16代表是16字节，是x264配置信息使用到
    int body_size = 16+sps_len+pps_len;
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(packet,body_size);
    RTMPPacket_Reset(packet);

    unsigned char *body = (unsigned char *) packet->m_body;
    int i = 0;
    //二进制表示：00010111
    //VideoHeaderTag:FrameType(1=keyFrame)+CodecId(7=AVC)
    body[i++] = 0x17;
    //AvCPacketType = 0表示设置AVCDecoderConfigurationRecord
    body[i++] = 0x00;

    //composition time 0x0000000 24bit?
   body[i++] =0x00;
   body[i++] =0x00;
   body[i++] =0x00;

    //AVCDecoderConfigurationRecord
    body[i++] = 0x01;//configuredVerion .版本为1
    body[i++] = sps[1];//AVCProfileIndication 用于声明H.264的profile
    body[i++] = sps[2];//progile_compatibility 含义还不是太清楚，一般设置00
    body[i++] = sps[3];//AVClevelIndication 用于声明H.264的level。level决定了解码器的解码能力，即最大多大的分辨率、帧率、码率

    //lengthSizeMinusOne,H264 视频中NALU的长度，计算方法是
    // 1+(lengthSizeMinusOne & 3),实际测试时发现总为FF，计算结果为4
    body[i++] =0xFF;

    //SPS
    ////numOfSequenceParameterSets:SPS的个数，计算方法是 numOfSequenceParameterSets & 0x1F,实际测试时发现总为E1，计算结果为1.
    body[i++]= 0xE1;
    body[i++]=(sps_len>>8)& 0xff;//sequenceParameterSetLength;SPS长度
    body[i++] = sps_len & 0xff;//sequenceParameterSetNALUnits
    memcpy(&body[i],sps,sps_len);

    i+= sps_len;

    ////pps
    body[i++]=0x01; //numOfPictureParameterSets numOfPictureParameterSets & 0x1F,实际测试时发现总为E1，计算结果为1.
    body[i++] = (pps_len) & 0xff;//PPS
    memcpy(&body[i], pps, pps_len);
    i += pps_len;

    //Message Type Rtmp_Packet_type_Video 0x09
    packet->m_packetType =RTMP_PACKET_TYPE_VIDEO;
    //Paload length
    packet->m_nBodySize = body_size;
    //Time Stamp 4个字节
    //记录了每一个tag 相对于第一个tag(File Header) 的相对时间
    //以毫秒为单位，而File header 的time stamp 永远为0
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp =0;
    packet->m_nChannel =0x04; //channel Id Audio Video 通道
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    //将Rtmppacket 加入到队列
    add_rtmp_packet(packet);






}

void add_rtmp_packet(RTMPPacket *pPacket) {
    pthread_mutex_lock(&mutex);
    queue_insert_last(pPacket);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
};

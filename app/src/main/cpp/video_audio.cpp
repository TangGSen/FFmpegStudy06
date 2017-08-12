#include "video_audio_play.h"
#include <string>
#include "android/log.h"
//原生绘制的头文件
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "unistd.h"
#include "vaqueue.h"

/**
 * 未解决部分：音视频同步，和有些视频崩溃
 */

extern "C" {

//编解码库
#include "libavcodec/avcodec.h"
//封装格式处理库
#include "libavformat/avformat.h"
//工具库
#include "libavutil/imgutils.h"
//视频像素格式转换
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"


}
#define SUCCESS 0
#define FAILD -1;
#define MAX_STREAM_ARRAY 2
#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
//44100HZ 16bit =2个字节
#define MAX_AUDIO_FRAME_SIZE 44100 * 2
//队列的长度
#define PACKET_QUEUE_SIZE 50
JavaVM *javaVM;

//定义一个结构体，用于封装一些参数（音视频共用的参数）
struct SenPlayer{
    //封装格式的上下文
    AVFormatContext *avFormatContext;
    //音视频流索引位置
    int audio_stream_index ;
    int video_stream_index;
    //该视频流的总数，有几种流就有几种队列
    int total_stream_num;
    //解码器数据
    AVCodecContext  *input_code_contx[MAX_STREAM_ARRAY];
    //视频转换的上下文
    SwsContext *swsContext;

    //音频转换的上下文
    SwrContext *swrContext;
    //获取输入输出的声道个数
    int out_nb_chanels_size;
    //音频的解码后输出路径
    const char *audioOutFilePath;
    //音频的audio_write_mid
    jmethodID audio_write_mid;
    jobject audio_track_obj;

    ANativeWindow*  aNativeWindow;
    //解码线程数组id
    pthread_t deocde_thread_id[MAX_STREAM_ARRAY];

    AVQueue *packet[MAX_STREAM_ARRAY];

    //生产者线程id
    pthread_t play_read_thread_id;

    //错误信息
    char* errorMsg;
    //错误码
    int code;

    //视频
    AVFrame *in_frame_picture;
    AVFrame *out_frame_picture;
    ANativeWindow_Buffer outBuffer;

    //互斥锁
    pthread_mutex_t mutex;
    //条件变量
    pthread_cond_t cond;
};
//这个在子线程区分是那个线程过来的，
struct DecodeData{
    SenPlayer *player;
    int stream_index;
};



//目前默认返回零，改执行方法为成功的方法，才能继续进行下一步，-1为失败
void init_input_format_comtx(SenPlayer *player ,const char *cFilePath){
    //1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        player->code = FAILD;
//        player->errorMsg ="视频文件打开失败";
        return ;
    }
    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {
        player->code = FAILD;
//        player->errorMsg="获取文件信息失败";
        return ;
    }
    player->avFormatContext=avFormatContext;
    player->code=SUCCESS;
}
//.1遍历查找文件流中的视频流的索引位置
void findVideoAduioIndex(SenPlayer *player){
    AVFormatContext *avFormatContext =player->avFormatContext;
    int i = 0;
    //流的总数
    player->total_stream_num = avFormatContext->nb_streams;
    for (i; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            player->video_stream_index=i;

        }else if(avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            player->audio_stream_index = i;
        }
    }

}

//查找音视频解码器并打开
void init_code_contx_open(SenPlayer *player, int stream_index) {
    //5.查找音视频流的解码器
    //根据视频流获取编码的上下文
    AVCodecParameters *codecpar = player->avFormatContext->streams[stream_index]->codecpar;
    //返回解码器

    AVCodecContext *codecContext = player->avFormatContext->streams[stream_index]->codec;
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);
    if (avCodec == NULL) {
//        player->errorMsg="找不到对应的解码器";
        player->code = FAILD;
        return;
    }
//6.打开解码器
//zero on success, a negative value on error
    int open_codec_result = avcodec_open2(codecContext, avCodec, NULL);
    if (open_codec_result != 0) {
//        player->errorMsg="打开解码器失败";
        player->code = FAILD;
        return;
    }
    player->input_code_contx[stream_index] = codecContext;
    player->code = SUCCESS;

}

/**解码视频的*/
void decodeVideoData(SenPlayer *player, AVPacket *avPacket) {
    AVCodecContext *codecContext = player->input_code_contx[player->video_stream_index];
//    新Api
    /**
     *   avcodec_send_packet() and avcodec_receive_frame()
     *   avcodec_send_packet 发送数据包，通俗来说是，解压一帧数据
     *   avcodec_receive_frame 接收解析成功的一帧像素数据
     */

    int result = avcodec_send_packet(codecContext,avPacket);
    result= avcodec_receive_frame(codecContext,player->in_frame_picture);
    if (result != 0) {
        LOGE("解码失败...");
        return;
    }
    //0位解码完毕，非0为正在解码

    //原生绘制步骤：
    //1.lock
    //设置缓冲区的大小
    ANativeWindow_setBuffersGeometry(player->aNativeWindow,codecContext->width,codecContext->width,WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_lock(player->aNativeWindow,&(player->outBuffer),NULL);
    // render
    sws_scale(player->swsContext,
              (const uint8_t *const *) player->in_frame_picture->data,
              player->in_frame_picture->linesize, 0, codecContext->height,
              player->out_frame_picture->data,
              player->out_frame_picture->linesize
    );
    // 获取stride
    uint8_t * dst = (uint8_t *) player->outBuffer.bits;
    int dstStride = player->outBuffer.stride * 4;
    uint8_t * src = (uint8_t*) (player->out_frame_picture->data[0]);
    int srcStride = player->out_frame_picture->linesize[0];

    // 由于window的stride和帧的stride不同,因此需要逐行复制
    int h;
    for (h = 0; h < codecContext->height; h++) {
        memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
    }

    //unlock
    ANativeWindow_unlockAndPost(player->aNativeWindow);

}

////解码音频
void docodeAudioData(SenPlayer *player, AVPacket *pkt,
                     JNIEnv *env) {
    AVFrame *in_frame = av_frame_alloc();
    uint8_t *audioOutBuffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE);
    int result = -1;
    AVCodecContext *avCodecContext = player->input_code_contx[player->audio_stream_index];

    avcodec_send_packet(avCodecContext, pkt);
    result = avcodec_receive_frame(avCodecContext, in_frame);
    if (result == AVERROR(EAGAIN)) {
        LOGE("解码失败");
    }
    if (result == AVERROR_EOF) {
        LOGE("解码完成，没有任何输出");
    }
    if (result == AVERROR(EINVAL)) {
        LOGE("解码：codec not opened, or it is an encoder");
    }
    if (result == 0) {
        //将音频以pcm 输出

        swr_convert(player->swrContext, &audioOutBuffer, MAX_AUDIO_FRAME_SIZE,
                    (const uint8_t **) in_frame->data, in_frame->nb_samples);
        //每次写这么多
        int outBufferSize = av_samples_get_buffer_size(NULL, player->out_nb_chanels_size,
                                                       in_frame->nb_samples,
                                                       player->input_code_contx[player->audio_stream_index]->sample_fmt,
                                                       1);
        //由于AduidoTrack 需要的参数是三个，并且返回值是Int
        //所以将outBuffer 自定义uint8 类型转成byte数据
        jbyteArray byteArray = env->NewByteArray(outBufferSize);
        //通过 *byte 指针来操作byteArray 里面数据
        jbyte *byte = env->GetByteArrayElements(byteArray, NULL);
        //赋值outBuffer --》byte
        memcpy(byte, audioOutBuffer, outBufferSize);
        env->ReleaseByteArrayElements(byteArray, byte, 0);
        env->CallIntMethod(player->audio_track_obj, player->audio_write_mid, byteArray, 0,
                           outBufferSize);
        //需要释放局部变量，防止溢出（在for循环里创建对象需要释放）
        env->DeleteLocalRef(byteArray);
    }
    free(audioOutBuffer);
    av_frame_free(&in_frame);

}




/**解码音视频子线程*/
void *decodeDataThreadRun(void *args) {
    DecodeData *decodeData =(DecodeData *) args;
    SenPlayer *player = decodeData->player;
    int stream_index = decodeData->stream_index;
    JNIEnv *env;
    javaVM->AttachCurrentThread(&env, NULL);
    //根据stream _index 获取对应的队列
    AVQueue *avQueue = (AVQueue *) player->packet[stream_index];
    LOGE("decodeDataThreadRun  stream_index:%d",stream_index);
    //循环读取每一帧
    for (;;) {
        pthread_mutex_lock(&player->mutex);
        AVPacket *avPacket = (AVPacket *) queue_pop(avQueue, &player->mutex, &player->cond);
        pthread_mutex_unlock(&player->mutex);
        if (avPacket->stream_index == player->video_stream_index) {
            decodeVideoData(player, avPacket);
        }else if(avPacket->stream_index == player->audio_stream_index){
            docodeAudioData(player, avPacket, env);
        }
    }
    javaVM->DetachCurrentThread();
    av_frame_free(&player->in_frame_picture);
    av_frame_free(&player->out_frame_picture);
    sws_freeContext(player->swsContext);
    swr_free(&(player->swrContext));
    ANativeWindow_release(player->aNativeWindow);
    swr_free(&(player->swrContext));
    avcodec_close(player->input_code_contx[player->audio_stream_index]);
    avcodec_close(player->input_code_contx[player->video_stream_index]);
    return NULL;
}


//解码Video前进行初始化准备
void decode_video_prepare(JNIEnv *env, jobject jSurface, SenPlayer *player) {
    //Android native绘制
    player->aNativeWindow =ANativeWindow_fromSurface( env, jSurface);

    AVCodecContext *codecContext =player->input_code_contx[player->video_stream_index];

    //缓存一帧数据（一张图片）
    AVFrame *in_frame_picture = av_frame_alloc();
    /**以下几个参数为了SwsContext 做准备的*/
    //定义输出一帧数据(缓冲区)
    AVFrame *out_frame_picture = av_frame_alloc();

    //指定缓冲区的类型
    //开辟空间是YUV420p 的数据大小
    //OldApi
    // 参数4 字节对齐的方式 1（通用）
    uint8_t *out_buffter = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_RGBA, codecContext->width, codecContext->height,1));
    //指定填充数据
    av_image_fill_arrays( out_frame_picture->data,out_frame_picture->linesize, out_buffter, AV_PIX_FMT_RGBA,
                          codecContext->width, codecContext->height,1);


    SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                            codecContext->pix_fmt, codecContext->width,
                                            codecContext->height, AV_PIX_FMT_RGBA,
                                            SWS_BICUBIC, NULL, NULL, NULL

    );
    player->swsContext =swsContext;
    player->in_frame_picture=in_frame_picture ;
    player->out_frame_picture=out_frame_picture ;

}


void decode_audio_prepare(JNIEnv *env, SenPlayer *player, jobject jobj) {
    AVCodecContext *avctx = player->input_code_contx[player->audio_stream_index];
    //上下文
    SwrContext* swrContext = swr_alloc();
    /**
     * 初始化音频重采样
     * 参数一：上下文
     * 2.输出声道布局，比如立体，环绕
     * 3.输出音频格采样格式
     */
    int out_ch_layout = AV_CH_LAYOUT_STEREO;
    AVSampleFormat out_sample_fmt =AV_SAMPLE_FMT_S16;
    int in_ch_layout = av_get_default_channel_layout(avctx->channels);

    //根据声道布局获取声道数量
    int out_nb_chanels_size = av_get_channel_layout_nb_channels(out_ch_layout);
    player->out_nb_chanels_size=out_nb_chanels_size;
    swr_alloc_set_opts(swrContext,AV_CH_LAYOUT_STEREO,
                       out_sample_fmt,avctx->sample_rate,in_ch_layout,
                       avctx->sample_fmt,avctx->sample_rate,0,NULL);

    swr_init(swrContext);
    player->swrContext = swrContext;
    player->out_nb_chanels_size = out_nb_chanels_size;

    /**
       *    初始化jni 使用Java中AudioTrack 来播放
        */
    jclass audioTestClass = env->GetObjectClass(jobj);
    jmethodID createAudioTrackId = env->GetMethodID(audioTestClass, "createAudioTrack",
                                                    "()Landroid/media/AudioTrack;");
    jobject audio_track_obj = env->CallObjectMethod(jobj, createAudioTrackId);
    jclass audio_track_class = env->GetObjectClass(audio_track_obj);
    jmethodID audio_play_mid = env->GetMethodID(audio_track_class, "play", "()V");

    env->CallVoidMethod(audio_track_obj, audio_play_mid);
    jmethodID audio_write_mid = env->GetMethodID(audio_track_class, "write", "([BII)I");
    player->audio_write_mid = audio_write_mid;
    player->audio_track_obj = env->NewGlobalRef(audio_track_obj);
}
//给AVPacket 开辟空间，后面会将AVPacket栈内存数据拷贝到这里
void* player_fill_packet(){
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
    return packet;
}
void* packet_free_queue(AVPacket *packet){
    av_packet_unref(packet);
    return 0;
}
/**
 * 初始化流的队列
 */
void play_mallco_queue(SenPlayer *player){
    LOGE("play_mallco_queue初始化：%d",player->total_stream_num);
    int i;
    for (int i = 0; i < player->total_stream_num; ++i) {
        if (i>=MAX_STREAM_ARRAY){
            return;
        }
        player->packet[i] =  queue_init(PACKET_QUEUE_SIZE,(queue_fill_fun)player_fill_packet);
    }
}


/**
 * 生成者线程执行的函数
 */
void* player_read_from_stream(void* args){
    SenPlayer *player = (SenPlayer *) args;
    /**
     * 在栈内存上保存一个AVpacket,然后将指针保存到对列的数组里，当对列不用这个指针时，
     * 这样好处是，用完就自动稀放，不需要手动稀放
     * */
    AVPacket packet,*avPacket =&packet;

    int result ;
    for(;;){
        result = av_read_frame(player->avFormatContext,avPacket);
        if (result<0){
            //读完了
            break;
        }
        AVQueue* queue =  player->packet[avPacket->stream_index];
        //需要锁上，如果读的线程也读到这里的话，那么需要等写完才能读
        pthread_mutex_lock(&player->mutex);
        AVPacket* packet_data = (AVPacket *) queue_push(queue, &player->mutex, &player->cond);
        *packet_data =packet;
        pthread_mutex_unlock(&player->mutex);
        LOGE("packet_data:%x******%d",packet_data,avPacket->stream_index);
    }




    return NULL;
}

/**线程音频边解码边播放*/

JNIEXPORT void JNICALL Java_sen_com_video_VideoAudioPlay_videoAudioPlayerV2
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring audioOutFilePath,jobject jSurface) {
    SenPlayer *player = (SenPlayer *) malloc(sizeof(SenPlayer));
    env->GetJavaVM(&javaVM);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);
    const char *cAudioOutFilePath = env->GetStringUTFChars(audioOutFilePath, NULL);
    player->audioOutFilePath = cAudioOutFilePath;
    //1初始化封装格式上下文
    init_input_format_comtx(player,cFilePath);
    LOGE("**********888");
    if (player->code!=0){
        LOGE("%s",player->errorMsg);
        return;
    }

   findVideoAduioIndex(player);

    init_code_contx_open(player,player->audio_stream_index);
    init_code_contx_open(player, player->video_stream_index);
    //初始化jni 和音频重采样
    decode_audio_prepare(env, player, jobj);
    //初始化视频做准备
    decode_video_prepare(env, jSurface, player);
    //初始化队列
    play_mallco_queue(player);

    //初始化互斥锁和条件变量
    pthread_mutex_init(&player->mutex,NULL);
    pthread_cond_init(&player->cond,NULL);
    //生产这线程
    pthread_create(&(player->play_read_thread_id), NULL,
                   player_read_from_stream,
                   (void *) player);
    //消费者线程
    DecodeData *audio = (DecodeData *) malloc(sizeof(DecodeData));
    audio->player =player;
    audio->stream_index= player->audio_stream_index ;

    DecodeData *video = (DecodeData *) malloc(sizeof(DecodeData));
    video->player =player;
    video->stream_index= player->video_stream_index ;
    pthread_create(&(player->deocde_thread_id[player->audio_stream_index]), NULL,
                   decodeDataThreadRun,
                   (void *) audio);
    pthread_create(&(player->deocde_thread_id[player->video_stream_index]), NULL,
                   decodeDataThreadRun,
                   (void *) video);





//    env->ReleaseStringUTFChars(jfilepath, cFilePath);
//    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}
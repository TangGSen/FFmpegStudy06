#include "audio.h"
#include <string>
#include "android/log.h"
#include "unistd.h"
extern "C" {

//编解码库
#include "libavcodec/avcodec.h"
//封装格式处理库
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}
#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

/**音频解码*/
//44100HZ 16bit =2个字节
#define MAX_AUDIO_FRAME_SIZE 44100 * 2
JNIEXPORT void JNICALL Java_sen_com_audio_AudioTest_sound
        (JNIEnv *env, jobject jobj, jstring inFilePath, jstring outFilePath){
    const char *cInFilePath = env->GetStringUTFChars(inFilePath, NULL);
    const char *cOutFilePath = env->GetStringUTFChars(outFilePath, NULL);
    int result = 0;
    av_register_all();
    AVFormatContext * avFormatContext = avformat_alloc_context();
    result = avformat_open_input(&avFormatContext,cInFilePath,NULL,NULL);
    if (result !=0){
        LOGE("文件打开失败%d", result);
        return;
    }
    //获取输入文件信息
    result = avformat_find_stream_info(avFormatContext,NULL);
    if (result <0){
        LOGE("获取输入文件信息%d", result);
        return;
    }


    //查找音频的索引index
    int audio_index = -1;
    int i = 0;
    for(;i< avFormatContext->nb_streams;i++){
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
            break;
        }

    }

    if (audio_index ==-1){
        LOGE("获取音频信息失败%d", audio_index);
        return;
    }
    //查找解码器
    AVCodecParameters * codecParameters =  avFormatContext->streams[audio_index]->codecpar;
    AVCodec *avCodec = avcodec_find_decoder(codecParameters->codec_id);

    if (avCodec ==NULL){
        LOGE("获取解码器失败%d", result);
        return;
    }
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    result = avcodec_open2(avCodecContext,avCodec,NULL);

    if (result !=0){
        LOGE("打开解码器失败%d", result);
        return;
    }

    AVPacket *pkt = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *out_frame = av_frame_alloc();
    AVFrame *in_frame = av_frame_alloc();

    SwrContext *swrContext = swr_alloc() ;
    //重采样设置参数

    enum AVSampleFormat out_sample_fmt =  avCodecContext->sample_fmt;
    enum AVSampleFormat in_sample_fmt =  AV_SAMPLE_FMT_S16;
    int  in_sample_rate = avCodecContext->sample_rate;
    int  out_sample_rate = avCodecContext->sample_rate ;
    int log_offset =0;

    //根据声道个数获取默认的声道布局，默认是立体声strero
//    int64_t  in_ch_layout = av_get_default_channel_layout(avCodecContext->channels);
    int64_t  in_ch_layout = avCodecContext->channel_layout;
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;//立体声

    //获取输入输出的声道个数
    int out_nb_chanels_size = av_get_channel_layout_nb_channels(out_ch_layout);

    swr_alloc_set_opts(swrContext,
                       out_ch_layout,  out_sample_fmt,  out_sample_rate,
                       in_ch_layout,  in_sample_fmt,   in_sample_rate,
                       log_offset,NULL);
    swr_init(swrContext);

    //缓冲区
    uint8_t *outBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    FILE *out_file = fopen(cOutFilePath,"wb+");
    int outBufferSize =0;
    int currentIndex = 0;

    //不断的读取压缩数据
    while(av_read_frame(avFormatContext,pkt)>=0){
        currentIndex++;
        LOGE("当前音频解码到：%d",currentIndex);
        if (pkt->stream_index != audio_index) {
            LOGE("不是音频");
            continue;
        }
        avcodec_send_packet(avCodecContext,pkt);
        result = avcodec_receive_frame(avCodecContext,in_frame);
        if (result ==AVERROR(EAGAIN)){
            LOGE("解码失败");
        }
        if (result ==AVERROR_EOF){
            LOGE("解码完成，没有任何输出");
        }
        if(result ==AVERROR(EINVAL)){
            LOGE("解码：codec not opened, or it is an encoder");
        }
        if (result ==0){
            //将音频以pcm 输出

            swr_convert(swrContext,&outBuffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)in_frame->data,in_frame->nb_samples);
            //每次写这么多
            outBufferSize =av_samples_get_buffer_size(NULL,
                                                      out_nb_chanels_size,
                                                      in_frame->nb_samples,out_sample_fmt,1);
            fwrite(outBuffer,1,outBufferSize,out_file);


        }

    }

    av_packet_free(&pkt);
    fclose(out_file);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame);
    av_frame_free(&out_frame);
    swr_free(&swrContext);
    env->ReleaseStringUTFChars(inFilePath, cInFilePath);
    env->ReleaseStringUTFChars(outFilePath, cOutFilePath);


};



/**音频解码*/

JNIEXPORT void JNICALL Java_sen_com_audio_AudioTest_sound2
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring jFileoutPath) {

    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    av_register_all();

    AVFormatContext *avFormatContext = avformat_alloc_context();
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("文件打开失败");

        return;
    }

    if(avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGE("获取信息失败");
        return;
    }

    int audio_index = -1;
    for(int i = 0;i<avFormatContext->nb_streams;i++){
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audio_index =i;
            break;
        }
    }

    if(audio_index==-1){
        //没有音频流
        return;
    }
    AVCodecParameters *codecpar = avFormatContext->streams[audio_index]->codecpar;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext *avctx = avcodec_alloc_context3(avCodec); //这个无效？
//    AVCodecContext *avctx = avFormatContext->streams[audio_index]->codec;
    int code_result = avcodec_open2(avctx,avCodec,NULL);
    if(code_result!=0){
        LOGE("打开解码器失败");
        return;
    }
    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket));
    AVFrame *in_frame = av_frame_alloc();

    //上下文
    SwrContext* swrContext = swr_alloc();
    /**
     * 参数一：上下文
     * 2.输出声道布局，比如立体，环绕
     * 3.输出音频格采样格式
     */
    int out_ch_layout = AV_CH_LAYOUT_STEREO;
    AVSampleFormat out_sample_fmt =AV_SAMPLE_FMT_S16;
    int in_ch_layout = av_get_default_channel_layout(avctx->channels);

    //根据声道布局获取声道数量
    int out_nb_chanels_size = av_get_channel_layout_nb_channels(out_ch_layout);
    swr_alloc_set_opts(swrContext,AV_CH_LAYOUT_STEREO,
                       out_sample_fmt,avctx->sample_rate,in_ch_layout,
                       avctx->sample_fmt,avctx->sample_rate,0,NULL);

    swr_init(swrContext);
    //缓冲区
    uint8_t *outBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    FILE *out_file = fopen(cFileOutPath,"wb+");
    int outBufferSize =0;
    int currentIndex = 0;
    while(av_read_frame(avFormatContext,pkt)>=0){
        if (pkt->stream_index ==audio_index){

            int result = avcodec_send_packet(avctx,pkt);
//
            result= avcodec_receive_frame(avctx,in_frame);
            if (result != 0) {
                LOGE("解码失败...");
                continue;
            }
            swr_convert(swrContext,&outBuffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)in_frame->data,in_frame->nb_samples);
            outBufferSize =av_samples_get_buffer_size(NULL,
                                                      out_nb_chanels_size,
                                                      in_frame->nb_samples,out_sample_fmt,1);
            fwrite(outBuffer,1,outBufferSize,out_file);
            currentIndex++;
            LOGE("当前音频解码到：%d",currentIndex);
        }
    }
    av_packet_free(&pkt);
    fclose(out_file);
    avcodec_close(avctx);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame);
    swr_free(&swrContext);
    env->ReleaseStringUTFChars(jfilepath, cFilePath);
    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}


/**音频边解码边播放*/

JNIEXPORT void JNICALL Java_sen_com_audio_AudioTest_playSound
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring jFileoutPath) {

    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    av_register_all();

    AVFormatContext *avFormatContext = avformat_alloc_context();
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("文件打开失败");

        return;
    }

    if(avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGE("获取信息失败");
        return;
    }

    int audio_index = -1;
    for(int i = 0;i<avFormatContext->nb_streams;i++){
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audio_index =i;
            break;
        }
    }

    if(audio_index==-1){
        //没有音频流
        return;
    }
    AVCodecParameters *codecpar = avFormatContext->streams[audio_index]->codecpar;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecpar->codec_id);
//    AVCodecContext *avctx = avcodec_alloc_context3(avCodec); //这个无效？
    AVCodecContext *avctx = avFormatContext->streams[audio_index]->codec;
    int code_result = avcodec_open2(avctx,avCodec,NULL);
    if(code_result!=0){
        LOGE("打开解码器失败");
        return;
    }
    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket));
    AVFrame *in_frame = av_frame_alloc();

    //上下文
    SwrContext* swrContext = swr_alloc();
    /**
     * 参数一：上下文
     * 2.输出声道布局，比如立体，环绕
     * 3.输出音频格采样格式
     */
    int out_ch_layout = AV_CH_LAYOUT_STEREO;
    AVSampleFormat out_sample_fmt =AV_SAMPLE_FMT_S16;
    int in_ch_layout = av_get_default_channel_layout(avctx->channels);

    //根据声道布局获取声道数量
    int out_nb_chanels_size = av_get_channel_layout_nb_channels(out_ch_layout);
    swr_alloc_set_opts(swrContext,AV_CH_LAYOUT_STEREO,
                       out_sample_fmt,avctx->sample_rate,in_ch_layout,
                       avctx->sample_fmt,avctx->sample_rate,0,NULL);

    swr_init(swrContext);
    //缓冲区
    uint8_t *outBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    FILE *out_file = fopen(cFileOutPath,"wb+");
    int outBufferSize =0;
    int currentIndex = 0;

    /**
     * 使用Java中AudioTrack 来播放
     */
    jclass  audioTestClass = env->GetObjectClass(jobj);
   jmethodID  createAudioTrackId = env->GetMethodID(audioTestClass,"createAudioTrack","()Landroid/media/AudioTrack;");
    jobject audio_track_obj = env->CallObjectMethod(jobj,createAudioTrackId);
    jclass audio_track_class = env->GetObjectClass(audio_track_obj);
    jmethodID audio_play_mid = env->GetMethodID(audio_track_class,"play","()V");

    env->CallVoidMethod(audio_track_obj,audio_play_mid);
    jmethodID audio_write_mid = env->GetMethodID(audio_track_class,"write","([BII)I");



    while(av_read_frame(avFormatContext,pkt)>=0){
        if (pkt->stream_index ==audio_index){

            int result = avcodec_send_packet(avctx,pkt);
//
            result= avcodec_receive_frame(avctx,in_frame);
            if (result != 0) {
                LOGE("解码失败...");
                continue;
            }
            swr_convert(swrContext,&outBuffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)in_frame->data,in_frame->nb_samples);
            outBufferSize =av_samples_get_buffer_size(NULL,
                                                      out_nb_chanels_size,
                                                      in_frame->nb_samples,out_sample_fmt,1);
            //由于AduidoTrack 需要的参数是三个，并且返回值是Int
            //所以将outBuffer 自定义uint8 类型转成byte数据

            jbyteArray byteArray=env->NewByteArray(outBufferSize);
            //通过 *byte 指针来操作byteArray 里面数据
            jbyte *byte = env->GetByteArrayElements(byteArray,NULL);
            //赋值outBuffer --》byte
            memcpy(byte,outBuffer,outBufferSize);
            env->ReleaseByteArrayElements(byteArray,byte,0);
             env->CallIntMethod(audio_track_obj,audio_write_mid,byteArray,0,outBufferSize);
//            fwrite(outBuffer,1,outBufferSize,out_file);
            currentIndex++;
            LOGE("当前音频解码到：%d",currentIndex);

            //需要释放局部变量，防止溢出（在for循环里创建对象需要释放）
            env->DeleteLocalRef(byteArray);

            //睡眠一下
            usleep(1000*16);
        }
    }
    av_packet_free(&pkt);
    fclose(out_file);
    avcodec_close(avctx);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame);
    swr_free(&swrContext);
    env->ReleaseStringUTFChars(jfilepath, cFilePath);
    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}
JavaVM *javaVM;
struct Player{
    //封装格式的上下文
    AVFormatContext *avFormatContext;
    //音视频流索引位置
    int audio_stream_index ;
    int video_stream_index;
    //解码器数据
    AVCodecContext  *input_code_contx;
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

    //解码线程数组id
    pthread_t deocde_thread_id;


};

void * playSoundInThread(void *arg){
    JNIEnv *env;
    javaVM->AttachCurrentThread(&env, NULL);
    Player *player = (Player *) arg;
    int outBufferSize =0;
    int currentIndex = 0;
    uint8_t *outBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    FILE *out_file = fopen(player->audioOutFilePath,"wb+");
    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket));
    AVFrame *in_frame = av_frame_alloc();
    while(av_read_frame(player->avFormatContext,pkt)>=0){
        if (pkt->stream_index ==player->audio_stream_index){

            int result = avcodec_send_packet(player->input_code_contx,pkt);
//
            result= avcodec_receive_frame(player->input_code_contx,in_frame);
            if (result != 0) {
                LOGE("解码失败...");
                continue;
            }
            swr_convert(player->swrContext,&outBuffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)in_frame->data,in_frame->nb_samples);
            outBufferSize =av_samples_get_buffer_size(NULL,
                                                      player->out_nb_chanels_size,
                                                      in_frame->nb_samples,player->input_code_contx->sample_fmt,1);
            //由于AduidoTrack 需要的参数是三个，并且返回值是Int
            //所以将outBuffer 自定义uint8 类型转成byte数据

            jbyteArray byteArray=env->NewByteArray(outBufferSize);
            //通过 *byte 指针来操作byteArray 里面数据
            jbyte *byte = env->GetByteArrayElements(byteArray,NULL);
            //赋值outBuffer --》byte
            memcpy(byte,outBuffer,outBufferSize);
            env->ReleaseByteArrayElements(byteArray,byte,0);
            env->CallIntMethod(player->audio_track_obj,player->audio_write_mid,byteArray,0,outBufferSize);
//            fwrite(outBuffer,1,outBufferSize,out_file);
            currentIndex++;
            LOGE("当前音频解码到：%d",currentIndex);

            //需要释放局部变量，防止溢出（在for循环里创建对象需要释放）
            env->DeleteLocalRef(byteArray);

            //睡眠一下
            usleep(1000*16);
        }
    }
    javaVM->DetachCurrentThread();
    av_packet_free(&pkt);
    fclose(out_file);
    avcodec_close(player->input_code_contx);
    avformat_free_context(player->avFormatContext);
    av_frame_free(&in_frame);
    swr_free(&player->swrContext);
}
/**线程音频边解码边播放*/

JNIEXPORT void JNICALL Java_sen_com_audio_AudioTest_playSoundInThread
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring jFileoutPath) {
    Player *player = (Player *) malloc(sizeof(Player));
    env->GetJavaVM(&javaVM);
    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);
    player->audioOutFilePath = cFileOutPath;
    av_register_all();

    AVFormatContext *avFormatContext = avformat_alloc_context();

    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    player->avFormatContext =avFormatContext;
    if (file_open_result != 0) {
        LOGE("文件打开失败");

        return;
    }

    if(avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGE("获取信息失败");
        return;
    }

    int audio_index = -1;
    for(int i = 0;i<avFormatContext->nb_streams;i++){
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audio_index =i;
            player->audio_stream_index=i;
            break;
        }
    }

    if(audio_index==-1){
        //没有音频流
        return;
    }
    AVCodecParameters *codecpar = avFormatContext->streams[audio_index]->codecpar;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecpar->codec_id);
//    AVCodecContext *avctx = avcodec_alloc_context3(avCodec); //这个无效？
    AVCodecContext *avctx = avFormatContext->streams[audio_index]->codec;
    player->input_code_contx =avctx;
    int code_result = avcodec_open2(avctx,avCodec,NULL);
    if(code_result!=0){
        LOGE("打开解码器失败");
        return;
    }


    //上下文
    SwrContext* swrContext = swr_alloc();
    /**
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
    player->swrContext =swrContext;
    //缓冲区
    /**
     * 使用Java中AudioTrack 来播放
     */
    jclass  audioTestClass = env->GetObjectClass(jobj);
   jmethodID  createAudioTrackId = env->GetMethodID(audioTestClass,"createAudioTrack","()Landroid/media/AudioTrack;");
    jobject audio_track_obj = env->CallObjectMethod(jobj,createAudioTrackId);
    jclass audio_track_class = env->GetObjectClass(audio_track_obj);
    jmethodID audio_play_mid = env->GetMethodID(audio_track_class,"play","()V");

    env->CallVoidMethod(audio_track_obj,audio_play_mid);
    jmethodID audio_write_mid = env->GetMethodID(audio_track_class,"write","([BII)I");
    player->audio_write_mid = audio_write_mid;
    player->audio_track_obj = env->NewGlobalRef(audio_track_obj);

    pthread_create(&(player->deocde_thread_id), NULL,
                   playSoundInThread,
                   (void *) player);

//    env->ReleaseStringUTFChars(jfilepath, cFilePath);
//    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}



/**线程音频边解码边播放*/

JNIEXPORT void JNICALL Java_sen_com_audio_AudioTest_playVideoSoundInThread
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring jFileoutPath,jobject jSurface) {
    Player *player = (Player *) malloc(sizeof(Player));
    env->GetJavaVM(&javaVM);
    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);
    player->audioOutFilePath = cFileOutPath;
    av_register_all();

    AVFormatContext *avFormatContext = avformat_alloc_context();

    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    player->avFormatContext =avFormatContext;
    if (file_open_result != 0) {
        LOGE("文件打开失败");

        return;
    }

    if(avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGE("获取信息失败");
        return;
    }

    int audio_index = -1;
    for(int i = 0;i<avFormatContext->nb_streams;i++){
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audio_index =i;
            player->audio_stream_index=i;
            break;
        }
    }

    if(audio_index==-1){
        //没有音频流
        return;
    }
    AVCodecParameters *codecpar = avFormatContext->streams[audio_index]->codecpar;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecpar->codec_id);
//    AVCodecContext *avctx = avcodec_alloc_context3(avCodec); //这个无效？
    AVCodecContext *avctx = avFormatContext->streams[audio_index]->codec;
    player->input_code_contx =avctx;
    int code_result = avcodec_open2(avctx,avCodec,NULL);
    if(code_result!=0){
        LOGE("打开解码器失败");
        return;
    }


    //上下文
    SwrContext* swrContext = swr_alloc();
    /**
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
    player->swrContext =swrContext;
    //缓冲区



    /**
     * 使用Java中AudioTrack 来播放
     */
    jclass  audioTestClass = env->GetObjectClass(jobj);
   jmethodID  createAudioTrackId = env->GetMethodID(audioTestClass,"createAudioTrack","()Landroid/media/AudioTrack;");
    jobject audio_track_obj = env->CallObjectMethod(jobj,createAudioTrackId);
    jclass audio_track_class = env->GetObjectClass(audio_track_obj);
    jmethodID audio_play_mid = env->GetMethodID(audio_track_class,"play","()V");

    env->CallVoidMethod(audio_track_obj,audio_play_mid);
    jmethodID audio_write_mid = env->GetMethodID(audio_track_class,"write","([BII)I");
    player->audio_write_mid = audio_write_mid;
    player->audio_track_obj = env->NewGlobalRef(audio_track_obj);

    pthread_create(&(player->deocde_thread_id), NULL,
                   playSoundInThread,
                   (void *) player);

//    env->ReleaseStringUTFChars(jfilepath, cFilePath);
//    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}






#include "sen_com_ffmpegstudy06_FFmpegTest.h"
#include <string>
#include <android/log.h>

// 宏定义类似java 层的定义,不同级别的Log LOGI, LOGD, LOGW, LOGE, LOGF。 对就Java中的 Log.i log.d
#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
//#undef LOG // 取消默认的LOG
#define LOGI(...)  __android_log_print(ANDROID_LOGENFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)

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

JNIEXPORT jstring JNICALL
Java_sen_com_ffmpegstudy06_FFmpegTest_getConfigInfo(
        JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    const char *av_cofing_info = avcodec_configuration();
    return env->NewStringUTF(av_cofing_info);
}
/*读取文件视频的方式：OldApi*/
JNIEXPORT void JNICALL
Java_sen_com_ffmpegstudy06_FFmpegTest_readVideoInfoOldApi(
        JNIEnv *env, jobject jobj, jstring jfilepath) {
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    //1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("视频文件打开失败");
        char *erorrInfo;
        av_strerror(file_open_result, erorrInfo, 1024);
        LOGE("视频文件打开失败：%s", erorrInfo);
        return;
    }
    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {
        char *erorrInfo;
        av_strerror(find_stream_result, erorrInfo, 1024);
        LOGE("获取文件信息失败：%s", erorrInfo);
        return;
    }

    //4.查找解码器
    //4.1遍历查找文件流中的视频流的索引位置
    int avSize = avFormatContext->nb_streams;
    int i = 0;
    int av_stream_index = -1;
    for (i; i <= avSize; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不存在视频流");
    }

    //5.查找视频流的解码器
    //根据视频流获取编码的上下文
    AVCodecContext *codecContext = avFormatContext->streams[av_stream_index]->codec;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == NULL) {
        LOGE("找不到对应的解码器");
        return;
    }

    //6.打开解码器
    //zero on success, a negative value on error
    int open_codec_result = avcodec_open2(codecContext, avCodec, NULL);
    if (open_codec_result != 0) {
        LOGE("打开解码器失败");
        return;
    }

    //获取视频的信息：格式，宽高。。
    LOGE("readVideoInfoOldApi视频的格式：%s", avFormatContext->iformat->name);
    //时长返回时：微秒
    LOGE("视频的时长：%lld", (avFormatContext->duration) / 1000000);
    LOGE("视频的宽高：%d * %d", codecContext->width, codecContext->height);
    LOGE("视频解码器的名字：%s", avCodec->name);


    env->ReleaseStringUTFChars(jfilepath, cFilePath);
}
/*读取文件视频的方式：NewApi*/
JNIEXPORT void JNICALL
Java_sen_com_ffmpegstudy06_FFmpegTest_readVideoInfoNewApi(
        JNIEnv *env, jobject jobj, jstring jfilepath) {
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    //1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("视频文件打开失败");
        char *erorrInfo;
        av_strerror(file_open_result, erorrInfo, 1024);
        LOGE("视频文件打开失败：%s", erorrInfo);
        return;
    }
    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {
        char *erorrInfo;
        av_strerror(find_stream_result, erorrInfo, 1024);
        LOGE("获取文件信息失败：%s", erorrInfo);
        return;
    }

    //4.查找解码器
    //4.1遍历查找文件流中的视频流的索引位置
    int avSize = avFormatContext->nb_streams;
    int i = 0;
    int av_stream_index = -1;
    for (i; i <= avSize; i++) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不存在视频流");
    }

    //5.查找视频流的解码器
    //根据视频流获取编码的上下文
    // AVCodecContext *codecContext = (AVCodecContext *) avFormatContext->streams[av_stream_index]->codecpar;

    //NewAPi AVCodecParameters 解码器的参数列表
    AVCodecParameters *codecpars = avFormatContext->streams[av_stream_index]->codecpar;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecpars->codec_id);

    if (avCodec == NULL) {
        LOGE("找不到对应的解码器");
        return;
    }

    //6.打开解码器
    AVCodecContext *avctx = avcodec_alloc_context3(avCodec);
    //zero on success, a negative value on error
    int open_codec_result = avcodec_open2(avctx, avCodec, NULL);
    if (open_codec_result != 0) {
        LOGE("打开解码器失败");

        return;
    }

    //获取视频的信息：格式，宽高。。
    LOGE("视频的格式：%s", avFormatContext->iformat->name);
    //时长返回时：微秒
    LOGE("视频的时长：%lld", (avFormatContext->duration) / 1000000);
    LOGE("视频的宽高：%d * %d", avctx->width, avctx->height);
    LOGE("视频解码器的名字：%s", avCodec->name);


    env->ReleaseStringUTFChars(jfilepath, cFilePath);
}



//3.NDK音视频编解码：FFmpeg-读取视频信息-新版本实现
JNIEXPORT void JNICALL
Java_sen_com_ffmpegstudy06_FFmpegTest_readFileInfoNewFromLaoShi(JNIEnv *env, jobject jobj,
                                                                jstring jFilePath) {
    //将Java的String转成C的字符串
    const char *cFilePath = env->GetStringUTFChars(jFilePath, NULL);

    //接下来就是读取视频信息
    //分析音视频解码流程
    //第一步：注册组件
    av_register_all();

    //第二步：打开输入视频文件
    //初始化封装格式上下文
    AVFormatContext *avFmtCtx = avformat_alloc_context();
    int fmt_open_result = avformat_open_input(&avFmtCtx, cFilePath, NULL, NULL);
    if (fmt_open_result != 0) {
        LOGE("打开视频文件失败");
        return;
    }

    //第三步：获取视频文件信息（文件流）
    //很多流（例如：视频流、音频流、字幕流等等......）
    //然后我的目的：我只需要视频流信息
    int fmt_fd_info = avformat_find_stream_info(avFmtCtx, NULL);
    if (fmt_fd_info < 0) {
        LOGE("获取视频文件信息失败");
        //打印错误码
        //错误信息
        char *error_info;
        //根据错误码找到对应的错误信息s
        av_strerror(fmt_fd_info, error_info, 1024);
        LOGE("错误信息：%s", error_info);
        return;
    }

    //第四步：查找解码器
    //1.获取视频流的索引位置
    //遍历所有的流，找到视频流
    int av_stream_index = -1;
    //avFmtCtx->nb_streams:返回流的大小
    for (int i = 0; i < avFmtCtx->nb_streams; ++i) {
        //判断流的类型(老的API实现)
        //是否是视频流
        //第一个地方：
        //新的API: avFmtCtx->streams[i]->codecpar->codec_type
        //老的API: avFmtCtx->streams[i]->codec->codec_type
        if (avFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不能存在视频流......");
        return;
    }

    //2.根据视频流的索引位置，查找视频流的解码器
    //根据视频流的索引位置，获取到指定的参数上下文
    //编码方式(编码上下文)
    //老的API:AVCodecContext *avCdCtx = avFmtCtx->streams[av_stream_index]->codec;
    //新的API:AVCodecParameters *avCdPm = avFmtCtx->streams[av_stream_index]->codecpar;
    //解码器参数列表
    AVCodecParameters *avCdPm = avFmtCtx->streams[av_stream_index]->codecpar;
    AVCodec *avCd = avcodec_find_decoder(avCdPm->codec_id);
    if (avCd == NULL) {
        LOGE("没有找到这个解码器");
        return;
    }

    //第五步：打开解码器
    AVCodecContext *avCdCtx = avcodec_alloc_context3(avCd);
    int av_cd_open_result = avcodec_open2(avCdCtx, avCd, NULL);
    if (av_cd_open_result != 0) {
        LOGE("解码器打开失败......%d", av_cd_open_result);
        return;
    }

    //获取配置视频信息
    //文件格式、文件的宽高、解码器的名称等等......
    LOGE("视频文件的格式：%s", avFmtCtx->iformat->name);
    //返回的单位是：微秒(avFmtCtx->duration)
    LOGE("视频的时长：%lld秒", (avFmtCtx->duration) / 1000000);
    //获取宽高
    LOGE("视频的宽高：%d x %d = ", avCdPm->width, avCdPm->height);
    //解码器的名称
    LOGE("解码器的名称：%s", avCd->name);
}


JNIEXPORT void JNICALL Java_sen_com_ffmpegstudy06_FFmpegTest_callFFmpegOldDecode
        (JNIEnv *env, jobject jobj, jstring jfilepath, jstring jFileoutPath) {

    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    //1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("视频文件打开失败");
        char *erorrInfo;
        av_strerror(file_open_result, erorrInfo, 1024);
        LOGE("视频文件打开失败：%s", erorrInfo);
        return;
    }
    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {
        char *erorrInfo;
        av_strerror(find_stream_result, erorrInfo, 1024);
        LOGE("获取文件信息失败：%s", erorrInfo);
        return;
    }

    //4.查找解码器
    //4.1遍历查找文件流中的视频流的索引位置
    int avSize = avFormatContext->nb_streams;
    int i = 0;
    int av_stream_index = -1;
    for (i; i <= avSize; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不存在视频流");
    }

    //5.查找视频流的解码器
    //根据视频流获取编码的上下文
    AVCodecContext *codecContext = avFormatContext->streams[av_stream_index]->codec;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == NULL) {
        LOGE("找不到对应的解码器");
        return;
    }

    //6.打开解码器
    //zero on success, a negative value on error
    int open_codec_result = avcodec_open2(codecContext, avCodec, NULL);
    if (open_codec_result != 0) {
        LOGE("打开解码器失败");
        return;
    }

    //获取视频的信息：格式，宽高。。
    LOGE("readVideoInfoOldApi视频的格式：%s", avFormatContext->iformat->name);
    //时长返回时：微秒
    LOGE("视频的时长：%lld", (avFormatContext->duration) / 1000000);
    LOGE("视频的宽高：%d * %d", codecContext->width, codecContext->height);
    LOGE("视频解码器的名字：%s", avCodec->name);


    //第七步，解码
    //从文件中读取一帧数据（压缩数据，一帧一帧得读）


    //这个是读取帧数缓存在这里，需要开辟空间
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //缓存一帧数据（一张图片）
    AVFrame *in_frame_picture = av_frame_alloc();
    /**以下几个参数为了SwsContext 做准备的*/
    //定义输出一帧数据(缓冲区)
    AVFrame *out_frame_picture = av_frame_alloc();

    //指定缓冲区的类型
    //开辟空间是YUV420p 的数据大小
    uint8_t *out_buffter = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height));
    //指定填充数据 YUV420p
    avpicture_fill((AVPicture *) out_frame_picture, out_buffter, AV_PIX_FMT_YUV420P,
                   codecContext->width, codecContext->height);


    //打开文件,二进制
    FILE *out_file_yuv = fopen(cFileOutPath, "wb");
    if (out_file_yuv == NULL) {
        LOGE("文件不存在");
        return;
    }

    LOGE("out_file_yuv文件存在");
    int got_picture_ptr = 0, y_size = 0, u_size = 0, v_size = 0, frame_index = 0;
    /**
     * 1.输入宽度
     * 2.输入高度
     * 3.输入数据
     * 4.输出宽度
     * 5.输出高度
     * 6.输出数据
     * 7.视频像素数据格式转换算法类型（使用什么算法）
     * 8.一般默认1， 字节对齐：提高读取效率
     */
    SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                            codecContext->pix_fmt, codecContext->width,
                                            codecContext->height, AV_PIX_FMT_YUV420P,
                                            SWS_BICUBIC, NULL, NULL, NULL

    );
    //循环读取每一帧
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        //只需要视频流
        if (avPacket->stream_index == av_stream_index) {
            int deocde_result = avcodec_decode_video2(codecContext, in_frame_picture,
                                                      &got_picture_ptr, avPacket);
            if (deocde_result < 0) {
                LOGE("解码失败...");
                return;
            }
            //0位解码完毕，非0为正在解码
            if (got_picture_ptr) {
                //然后将视频整数据保存为像素数据（YUV420像素数据）

                //需要指定输出文件的类型（格式转换）
                /**
                 * 1.视频像素数据格式的上下文
                 * 2.输入的数据，转格式前的像素数据
                 * 3.输入画面每一行的大小
                 * 4.输入画面每一行的要转码开始的位置
                 * 5.输出画面数据，转格式后的视频数据
                 * 6.输出画面每一行的大小
                 *
                 */
                sws_scale(swsContext,
                          (const uint8_t *const *) in_frame_picture->data,
                          in_frame_picture->linesize, 0, codecContext->height,
                          out_frame_picture->data,
                          out_frame_picture->linesize
                );
                /**
                 * YUV420p 格式规定1：Y结构表示一个像素点（一个像素点就是一个Y）
                 * 规定二：四个Y对应的一个U 和一个V(四个像素点对应一个U 和V)
                    Y默认是灰度
                 */
                //计算Y 的大小
                y_size = codecContext->width * codecContext->height;
                u_size = v_size = y_size / 4;

                //写入文件， 首先写入 Y在U 在V
                fwrite(in_frame_picture->data[0], 1, y_size, out_file_yuv);
                fwrite(in_frame_picture->data[1], 1, u_size, out_file_yuv);
                fwrite(in_frame_picture->data[2], 1, v_size, out_file_yuv);
                frame_index++;
                LOGE("当前是：%d", frame_index);

            }
        }
        //关闭流
        av_free_packet(avPacket);
    }

    //关闭流
    fclose(out_file_yuv);
    avcodec_close(codecContext);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame_picture);
    av_frame_free(&out_frame_picture);

    env->ReleaseStringUTFChars(jfilepath, cFilePath);
    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);


}

JNIEXPORT void JNICALL Java_sen_com_ffmpegstudy06_FFmpegTest_callFFmpegNewDecode
        (JNIEnv *env, jobject jobj, jstring jfilepath, jstring jFileoutPath) {

    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    //1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("视频文件打开失败");
        char *erorrInfo;
        av_strerror(file_open_result, erorrInfo, 1024);
        LOGE("视频文件打开失败：%s", erorrInfo);
        return;
    }
    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {
        char *erorrInfo;
        av_strerror(find_stream_result, erorrInfo, 1024);
        LOGE("获取文件信息失败：%s", erorrInfo);
        return;
    }

    //4.查找解码器
    //4.1遍历查找文件流中的视频流的索引位置
    int avSize = avFormatContext->nb_streams;
    int i = 0;
    int av_stream_index = -1;
    for (i; i <= avSize; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不存在视频流");
    }

    //5.查找视频流的解码器
    //根据视频流获取编码的上下文
    AVCodecContext *codecContext = avFormatContext->streams[av_stream_index]->codec;
    //返回解码器
    AVCodec *avCodec = avcodec_find_decoder(codecContext->codec_id);

    if (avCodec == NULL) {
        LOGE("找不到对应的解码器");
        return;
    }

    //6.打开解码器
    //zero on success, a negative value on error
    int open_codec_result = avcodec_open2(codecContext, avCodec, NULL);
    if (open_codec_result != 0) {
        LOGE("打开解码器失败");
        return;
    }

    //获取视频的信息：格式，宽高。。
    LOGE("readVideoInfoOldApi视频的格式：%s", avFormatContext->iformat->name);
    //时长返回时：微秒
    LOGE("视频的时长：%lld", (avFormatContext->duration) / 1000000);
    LOGE("视频的宽高：%d * %d", codecContext->width, codecContext->height);
    LOGE("视频解码器的名字：%s", avCodec->name);


    //第七步，解码
    //从文件中读取一帧数据（压缩数据，一帧一帧得读）


    //这个是读取帧数缓存在这里，需要开辟空间
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //缓存一帧数据（一张图片）
    AVFrame *in_frame_picture = av_frame_alloc();
    /**以下几个参数为了SwsContext 做准备的*/
    //定义输出一帧数据(缓冲区)
    AVFrame *out_frame_picture = av_frame_alloc();

    //指定缓冲区的类型
    //开辟空间是YUV420p 的数据大小
    //OldApi
//    uint8_t *out_buffter = (uint8_t *) av_malloc(
//            avpicture_get_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height));
//    //指定填充数据 YUV420p
//    avpicture_fill((AVPicture *) out_frame_picture, out_buffter, AV_PIX_FMT_YUV420P,
//                   codecContext->width, codecContext->height);
// 参数4 字节对齐的方式 1（通用）
 uint8_t *out_buffter = (uint8_t *) av_malloc(
         av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height,1));
    //指定填充数据 YUV420p
    av_image_fill_arrays( out_frame_picture->data,out_frame_picture->linesize, out_buffter, AV_PIX_FMT_YUV420P,
                   codecContext->width, codecContext->height,1);


    //打开文件,二进制
    FILE *out_file_yuv = fopen(cFileOutPath, "wb");
    if (out_file_yuv == NULL) {
        LOGE("文件不存在");
        return;
    }

    LOGE("out_file_yuv文件存在");
    int got_picture_ptr = 0, y_size = 0, u_size = 0, v_size = 0, frame_index = 0;
    /**
     * 1.输入宽度
     * 2.输入高度
     * 3.输入数据
     * 4.输出宽度
     * 5.输出高度
     * 6.输出数据
     * 7.视频像素数据格式转换算法类型（使用什么算法）
     * 8.一般默认1， 字节对齐：提高读取效率
     */
    SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                            codecContext->pix_fmt, codecContext->width,
                                            codecContext->height, AV_PIX_FMT_YUV420P,
                                            SWS_BICUBIC, NULL, NULL, NULL

    );
    //循环读取每一帧
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        //只需要视频流
        if (avPacket->stream_index == av_stream_index) {
//            int deocde_result = avcodec_decode_video2(codecContext, in_frame_picture,
//                                                      &got_picture_ptr, avPacket);
//            新Api
         /**
          *   avcodec_send_packet() and avcodec_receive_frame()
          *   avcodec_send_packet 发送数据包，通俗来说是，解压一帧数据
          *   avcodec_receive_frame 接收解析成功的一帧像素数据
          */

           int result = avcodec_send_packet(codecContext,avPacket);
//            if (result!=0 ){
//                LOGE("解压缩一帧数据失败");
//                continue;
//            }
            result= avcodec_receive_frame(codecContext,in_frame_picture);
            if (result != 0) {
                LOGE("解码失败...");
                continue;
            }
            //0位解码完毕，非0为正在解码

                //然后将视频整数据保存为像素数据（YUV420像素数据）

                //需要指定输出文件的类型（格式转换）
                /**
                 * 1.视频像素数据格式的上下文
                 * 2.输入的数据，转格式前的像素数据
                 * 3.输入画面每一行的大小
                 * 4.输入画面每一行的要转码开始的位置
                 * 5.输出画面数据，转格式后的视频数据
                 * 6.输出画面每一行的大小
                 *
                 */
                sws_scale(swsContext,
                          (const uint8_t *const *) in_frame_picture->data,
                          in_frame_picture->linesize, 0, codecContext->height,
                          out_frame_picture->data,
                          out_frame_picture->linesize
                );
                /**
                 * YUV420p 格式规定1：Y结构表示一个像素点（一个像素点就是一个Y）
                 * 规定二：四个Y对应的一个U 和一个V(四个像素点对应一个U 和V)
                    Y默认是灰度
                 */
                //计算Y 的大小
                y_size = codecContext->width * codecContext->height;
                u_size = v_size = y_size / 4;

                //写入文件， 首先写入 Y在U 在V
                fwrite(in_frame_picture->data[0], 1, y_size, out_file_yuv);
                fwrite(in_frame_picture->data[1], 1, u_size, out_file_yuv);
                fwrite(in_frame_picture->data[2], 1, v_size, out_file_yuv);
                frame_index++;
                LOGE("当前是：%d", frame_index);

        }
//        //关闭流
//        av_free_packet(avPacket);
    }
    av_packet_free(&avPacket);
    //关闭流
    fclose(out_file_yuv);
    avcodec_close(codecContext);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame_picture);
    av_frame_free(&out_frame_picture);

    env->ReleaseStringUTFChars(jfilepath, cFilePath);
    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);


}


/**音频解码*/
//44100HZ 16bit =2个字节
#define MAX_AUDIO_FRAME_SIZE 44100 * 2
JNIEXPORT void JNICALL Java_sen_com_ffmpegstudy06_FFmpegTest_callFFmpegDecodeAudio
        (JNIEnv *env, jobject jobj,  jstring jfilepath, jstring jFileoutPath) {

    const char *cFileOutPath = env->GetStringUTFChars(jFileoutPath, NULL);
    const char *cFilePath = env->GetStringUTFChars(jfilepath, NULL);

    av_register_all();

    AVFormatContext *avFormatContext = avformat_alloc_context();
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("文件打开失败");
        char *erorrInfo;
        av_strerror(file_open_result, erorrInfo, 1024);
        LOGE("文件打开失败：%s", erorrInfo);
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

    fclose(out_file);
    avcodec_close(avctx);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame);
    swr_free(&swrContext);
    env->ReleaseStringUTFChars(jfilepath, cFilePath);
    env->ReleaseStringUTFChars(jFileoutPath, cFileOutPath);



}

































/**老师的*/
//4.NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)-老版本
JNIEXPORT void JNICALL Java_sen_com_ffmpegstudy06_FFmpegTest_callFFmpegOldDecodeLaoShi
        (JNIEnv *env, jobject jobj, jstring jInFilePath, jstring jOutFilePath) {
    //将Java的String转成C的字符串
    const char *cInFilePath = env->GetStringUTFChars(jInFilePath, NULL);
    const char *cOutFilePath = env->GetStringUTFChars(jOutFilePath, NULL);

    //接下来就是读取视频信息
    //分析音视频解码流程
    //第一步：注册组件
    av_register_all();

    //第二步：打开输入视频文件
    //初始化封装格式上下文
    AVFormatContext *avFmtCtx = avformat_alloc_context();
    int fmt_open_result = avformat_open_input(&avFmtCtx, cInFilePath, NULL, NULL);
    if (fmt_open_result != 0) {
        LOGE("打开视频文件失败");
        return;
    }

    //第三步：获取视频文件信息（文件流）
    //很多流（例如：视频流、音频流、字幕流等等......）
    //然后我的目的：我只需要视频流信息
    int fmt_fd_info = avformat_find_stream_info(avFmtCtx, NULL);
    if (fmt_fd_info < 0) {
        LOGE("获取视频文件信息失败");
        //打印错误码
        //错误信息
        char *error_info;
        //根据错误码找到对应的错误信息s
        av_strerror(fmt_fd_info, error_info, 1024);
        LOGE("错误信息：%s", error_info);
        return;
    }

    //第四步：查找解码器
    //1.获取视频流的索引位置
    //遍历所有的流，找到视频流
    int av_stream_index = -1;
    //avFmtCtx->nb_streams:返回流的大小
    for (int i = 0; i < avFmtCtx->nb_streams; ++i) {
        //判断流的类型(老的API实现)
        //是否是视频流
        if (avFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    if (av_stream_index == -1) {
        LOGE("不能存在视频流......");
        return;
    }

    //2.根据视频流的索引位置，查找视频流的解码器
    //根据视频流的索引位置，获取到指定的参数上下文
    //编码方式(编码上下文)
    AVCodecContext *avCdCtx = avFmtCtx->streams[av_stream_index]->codec;
    AVCodec *avCd = avcodec_find_decoder(avCdCtx->codec_id);
    if (avCd == NULL) {
        LOGE("没有找到这个解码器");
        return;
    }

    //第五步：打开解码器
    int av_cd_open_result = avcodec_open2(avCdCtx, avCd, NULL);
    if (av_cd_open_result != 0) {
        LOGE("解码器打开失败......");
        return;
    }

    //获取配置视频信息
    //文件格式、文件的宽高、解码器的名称等等......
    LOGE("视频文件的格式：%s", avFmtCtx->iformat->name);
    //返回的单位是：微秒(avFmtCtx->duration)
    LOGE("视频的时长：%lld秒", (avFmtCtx->duration) / 1000000);
    //获取宽高
    LOGE("视频的宽高：%d x %d = ", avCdCtx->width, avCdCtx->height);
    //解码器的名称
    LOGE("解码器的名称：%s", avCd->name);


    //第六步：从输入文件读取一帧压缩数据(解压缩:一帧一帧读取解压缩)
    //循环读取每一帧数据
    //读取的帧数据缓存到那里(开辟一块内存空间用于保存)
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //缓存一帧数据(就是一张图片)
    AVFrame *in_frame_picture = av_frame_alloc();

    //定义输出一帧数据(缓冲区:YUV420p类型)
    AVFrame *out_frame_picture_YUV42P = av_frame_alloc();
    //指定缓冲区的类型(像素格式:YUV420P)
    //开启空间的大小是：YUV420P格式数据大小
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, avCdCtx->width, avCdCtx->height));
    //指定填充数据(YUV420P数据)
    avpicture_fill((AVPicture *) out_frame_picture_YUV42P, out_buffer, AV_PIX_FMT_YUV420P,
                   avCdCtx->width, avCdCtx->height);

    int got_picture_ptr, ret, y_size = 0, u_size = 0, v_size = 0, frame_index = 0;

    //打开文件
    FILE *out_file_yuv = fopen(cOutFilePath, "wb");
    if (out_file_yuv == NULL) {
        LOGE("文件不存在!");
        return;
    }

    //视频像素数据格式转换上下文
    //参数一：输入的宽度
    //参数二：输入的高度
    //参数三：输入的数据
    //参数四：输出的宽度
    //参数五：输出的高度
    //参数六：输出的数据
    //参数七：视频像素数据格式转换算法类型(使用什么算法)
    //参数八：字节对齐类型，一般都是默认1（字节对齐类型：提高读取效率）
    SwsContext *sws_ctx =
            sws_getContext(avCdCtx->width, avCdCtx->height, avCdCtx->pix_fmt,
                           avCdCtx->width, avCdCtx->height, AV_PIX_FMT_YUV420P,
                           SWS_BICUBIC, NULL, NULL, NULL);

    //读取返回值
    //>=0：正在读取
    //<0:读取失败或者说读取完毕
    while (av_read_frame(avFmtCtx, packet) >= 0) {
        //有视频流帧数据、音频流帧数据、字幕流......
        if (packet->stream_index == av_stream_index) {
            //我们只需要解码一帧视频压缩数据，得到视频像素数据
            //老的API
            //参数一：解码器上下文
            //参数二：一帧数据
            //参数三：是否正在解码（0：代表解码完毕，非0：正在解码）
            //返回值：小于0解码失败（错误、异常），否则成功解码一帧数据
            ret = avcodec_decode_video2(avCdCtx, in_frame_picture, &got_picture_ptr, packet);
            if (ret < 0) {
                LOGE("解码失败!");
                return;
            }

            //0：代表解码完毕，非0：正在解码
            if (got_picture_ptr) {

                //接下来我要将解码后的数据（视频像素数据，保存为YUV420P文件）
                //在这个地方需要指定输出文件的类型(格式转换)
                //我要将AVFrame转成视频像素数YUV420P格式
                //参数一：视频像素数据格式上下文(SwsContext)
                //参数二：输入的数据（转格式前的视频像素数据）
                //参数三：输入画面每一行的大小(视频像素数据转换一行一行的转)
                //参数四：输入画面每一行的要转码的开始位置
                //参数五：输出画面数据(转格式后的视频像素数据)
                //参数六：输出画面每一行的大小
                sws_scale(sws_ctx, (const uint8_t *const *) in_frame_picture->data,
                          in_frame_picture->linesize, 0, avCdCtx->height,
                          out_frame_picture_YUV42P->data, out_frame_picture_YUV42P->linesize);


                //普及: YUV420P格式结构
                //Y代表亮度，UV代表色度（人的眼睛对亮度敏感，对色度不敏感）
                //再深入：计算机图像学相关
                //YUV420P格式规定一：Y结构表示一个像素点(一个像素点就是一个Y)
                //YUV420P格式规定二：四个Y对应一个U和一个V(也就是四个像素点，对应一个U和V)
                //Y默认情况下：灰度
                //计算Y大小：y = 宽x高
                y_size = avCdCtx->width * avCdCtx->height;
                u_size = y_size / 4;
                v_size = y_size / 4;



                //写入文件
                //首先写入Y，再是U，再是V
                //in_frame_picture->data[0]表示Y
                fwrite(in_frame_picture->data[0], 1, y_size, out_file_yuv);
                //in_frame_picture->data[1]表示U
                fwrite(in_frame_picture->data[1], 1, u_size, out_file_yuv);
                //in_frame_picture->data[2]表示V
                fwrite(in_frame_picture->data[2], 1, v_size, out_file_yuv);


                frame_index++;

                LOGE("当前是第%d帧", frame_index);
            }
        }

        //关闭流
        av_free_packet(packet);
    }

    //关闭流
    fclose(out_file_yuv);
    av_frame_free(&in_frame_picture);
    av_frame_free(&out_frame_picture_YUV42P);
    avcodec_close(avCdCtx);
    avformat_free_context(avFmtCtx);

}







#include "video.h"
#include <string>
#include "android/log.h"
//原生绘制的头文件
#include "android/native_window.h"
#include "android/native_window_jni.h"

#define LOG_TAG    "sen" // 这个是自定义的LOG的标识
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

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

#include "libyuv.h"


}

JNIEXPORT void JNICALL Java_sen_com_video_VideoPlayContrlor_render
        (JNIEnv *env, jobject jObj,jstring jFilePath,jobject jSurface){

    const char *cFilePath = env->GetStringUTFChars(jFilePath, NULL);
//1.注册解码器
    av_register_all();
    //2.打开输入视频文件
    //2.1初始化封装格式的上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //0 on success, a negative AVERROR on failure.
    int file_open_result = avformat_open_input(&avFormatContext, cFilePath, NULL, NULL);
    if (file_open_result != 0) {
        LOGE("视频文件打开失败:%s",cFilePath);

        return;
    }


    //3.获取文件信息(文件流：视频流，音频流，字幕流)
    //>=0 if OK, AVERROR_xxx on error
    int find_stream_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_result < 0) {

        LOGE("获取文件信息失败");
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
// 参数4 字节对齐的方式 1（通用）
    uint8_t *out_buffter = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_RGBA, codecContext->width, codecContext->height,1));
    //指定填充数据
    av_image_fill_arrays( out_frame_picture->data,out_frame_picture->linesize, out_buffter, AV_PIX_FMT_RGBA,
                          codecContext->width, codecContext->height,1);



    LOGE("out_file_yuv文件存在");
    int  frame_index = 0;
    //Android native绘制
    ANativeWindow*  aNativeWindow =ANativeWindow_fromSurface( env, jSurface);
    ANativeWindow_Buffer* outBuffer ;

    SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                            codecContext->pix_fmt, codecContext->width,
                                            codecContext->height, AV_PIX_FMT_RGBA,
                                            SWS_BICUBIC, NULL, NULL, NULL

    );
    //循环读取每一帧
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        //只需要视频流
        if (avPacket->stream_index == av_stream_index) {
//            新Api
            /**
             *   avcodec_send_packet() and avcodec_receive_frame()
             *   avcodec_send_packet 发送数据包，通俗来说是，解压一帧数据
             *   avcodec_receive_frame 接收解析成功的一帧像素数据
             */

            int result = avcodec_send_packet(codecContext,avPacket);
            result= avcodec_receive_frame(codecContext,in_frame_picture);
            if (result != 0) {
                LOGE("解码失败...");
                continue;
            }
            //0位解码完毕，非0为正在解码

            //原生绘制步骤：
            //1.lock
            //设置缓冲区的大小
            ANativeWindow_setBuffersGeometry(aNativeWindow,codecContext->width,codecContext->width,WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(aNativeWindow,outBuffer,NULL);
            // render
            sws_scale(swsContext,
                      (const uint8_t *const *) in_frame_picture->data,
                      in_frame_picture->linesize, 0, codecContext->height,
                      out_frame_picture->data,
                      out_frame_picture->linesize
            );
            // 获取stride
            uint8_t * dst = (uint8_t *) outBuffer->bits;
            int dstStride = outBuffer->stride * 4;
            uint8_t * src = (uint8_t*) (out_frame_picture->data[0]);
            int srcStride = out_frame_picture->linesize[0];

            // 由于window的stride和帧的stride不同,因此需要逐行复制
            int h;
            for (h = 0; h < codecContext->height; h++) {
                memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
            }

            //unlock
            ANativeWindow_unlockAndPost(aNativeWindow);

            LOGE("Current frame_index：%d",frame_index++);
        }
    }
    //稀放
    sws_freeContext(swsContext);
    ANativeWindow_release(aNativeWindow);
    av_packet_free(&avPacket);
    avcodec_close(codecContext);
    avformat_free_context(avFormatContext);
    av_frame_free(&in_frame_picture);
    av_frame_free(&out_frame_picture);

    env->ReleaseStringUTFChars(jFilePath, cFilePath);
}

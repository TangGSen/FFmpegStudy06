//
// Created by Administrator on 2017/9/4.
//

#include "sen_com_ndk_image_NdkImage.h"
#include <string>
JNIEXPORT jintArray JNICALL Java_sen_com_ndk_1image_NdkImage_ndkimage
        (JNIEnv *env, jclass jc, jintArray buffer, jint width, jint height){
    float SKIN_WHITENING = 0.3f;
    float CONTRAST_RATIO = 0.2f;

    jint *source = env->GetIntArrayElements(buffer,NULL);
    int a, r, g, b = 0;
    int ri ,gi ,bi= 0;
    int whileten = (int) (255 * SKIN_WHITENING);
    float contrast = 1.0f + CONTRAST_RATIO;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {

            int color = source[width*y+x];
            a = color >> 24;
            r = (color >> 16) & 0xFF;
            g = (color >> 8) & 0xFF;
            b = color & 0xFF;

            //处理美白

            ri = r + whileten;
            gi = g + whileten;
            bi = b + whileten;
            //边界检查
            r = ri > 255 ? 255 : (ri < 0 ? 0 : ri);
            g = gi > 255 ? 255 : (gi < 0 ? 0 : gi);
            b = bi > 255 ? 255 : (bi < 0 ? 0 : bi);

            //对比度处理，黑的更黑，白的更白，以128作为中间参考值
            ri = r - 128;
            gi = g - 128;
            bi = b - 128;

            ri = (int) (ri * contrast);
            gi = (int) (gi * contrast);
            bi = (int) (bi * contrast);

            ri = ri + 128;
            gi = gi + 128;
            bi = bi + 128;


            //边界检查
            r = ri > 255 ? 255 : (ri < 0 ? 0 : ri);
            g = gi > 255 ? 255 : (gi < 0 ? 0 : gi);
            b = bi > 255 ? 255 : (bi < 0 ? 0 : bi);
            source[width*y+x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    int size = width *height;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result,0,size,source);
    env->ReleaseIntArrayElements(buffer,source,0);
    return result;

};

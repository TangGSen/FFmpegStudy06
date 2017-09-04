package sen.com.ndk_image;

/**
 * Author : 唐家森
 * Version: 1.0
 * On     : 2017/9/4 10:17
 * Des    :
 */

public class NdkImage {
    static {
        System.loadLibrary("ndkimage");
    }

    public static native int[] ndkimage(int[] buffer,int width,int heigth);
}

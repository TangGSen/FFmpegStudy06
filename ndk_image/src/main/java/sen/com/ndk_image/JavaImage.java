package sen.com.ndk_image;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Author : 唐家森
 * Version: 1.0
 * On     : 2017/9/4 09:42
 * Des    :
 */

public class JavaImage {
    public static final float SKIN_WHITENING = 0.2f;
    public static final float CONTRAST_RATIO = 0.2f;

    public static Bitmap javaImage(Bitmap bitmap) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int a, r, g, b = 0;
        int ri ,gi ,bi= 0;
        Bitmap result = Bitmap.createBitmap(width,height, Bitmap.Config.RGB_565);
        int whileten = (int) (255 * SKIN_WHITENING);
        float contrast = 1.0f + CONTRAST_RATIO;

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {

                int color = bitmap.getPixel(x, y);
                a = Color.alpha(color);
                r = Color.red(color);
                g = Color.green(color);
                b = Color.blue(color);

                //处理美白

                ri = r + whileten;
                gi = g + whileten;
                bi = b + whileten;
                //边界检查
                r = ri > 255 ? 255 : (ri < 0 ? 0 : ri);
                g = gi > 255 ? 255 : (gi< 0 ? 0 : gi);
                b = bi > 255 ? 255 : (bi < 0 ? 0 : bi);

                //对比度处理，黑的更黑，白的更白，以128作为中间参考值
                ri = r -128;
                gi = g -128;
                bi = b -128;

                ri = (int) (ri *contrast);
                gi = (int) (gi *contrast);
                bi = (int) (bi *contrast);

                ri= ri+128;
                gi= gi+128;
                bi= bi+128;


                //边界检查
                r = ri > 255 ? 255 : (ri < 0 ? 0 : ri);
                g = gi > 255 ? 255 : (gi< 0 ? 0 : gi);
                b = bi > 255 ? 255 : (bi < 0 ? 0 : bi);
                result.setPixel(x,y,Color.argb(a,r,g,b));
            }
        }
        return result;
    }
}

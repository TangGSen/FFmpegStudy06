package sen.com.video;

/**
 * 视频播放的控制器
 *
 * Created by Administrator on 2017/8/3.
 */

public class VideoPlayContrlor {
    static {
        System.loadLibrary("native-lib");
    }

    public native void init();
}

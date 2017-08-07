package sen.com.video;

import android.view.Surface;

/**
 * 视频播放的控制器
 *
 * Created by Administrator on 2017/8/3.
 */

public class VideoPlayContrlor {
    static {
        System.loadLibrary("native-lib");
    }
    //绘制到surfaceView 上
    public native void render(String filePath, Surface surface);
    //多线程 音视频同步
    public native void videoAudio(String filePath, Surface surface);


}

package sen.com.video;

import android.view.Surface;

/**
 * Created by Administrator on 2017/8/7.
 */

public class VideoAudioPlay {
    static {
        System.loadLibrary("native-lib");
    }
    //多线程 音视频同步
    public native void videoAudio(String filePath, Surface surface);
}

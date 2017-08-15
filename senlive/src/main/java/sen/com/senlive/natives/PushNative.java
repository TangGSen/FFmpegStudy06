package sen.com.senlive.natives;

/**
 * Created by Administrator on 2017/8/13.
 */

public class PushNative {
    static {
        System.loadLibrary("native-lib");
    }
    /**
     * 发送视频数据
     * @param data
     */
    public native void sendVideo(byte[] data);

    /**
     * 发送音频数据
     * len 音频有个buffer.使用buffer 实质 的长度
     * @param data
     * @param len
     */
    public native void sendAudio(byte[] data, int len);

    /**
     * 开始推流
     * @param url
     */

    public native void startPush(String url);

    /**
     * 停止推流
     */
    public native void stopPush();

    /**
     * 稀放资源
     */
    public native void release();

    /**
     * 设置视频的参数
     * @param width
     * @param height
     * @param bitrate //码率
     * @param flp //帧频
     */

    public native void setVideoOptions(int width,int height,int bitrate,int flp);

    /**
     * 设置音频的参数
     * @param sampleRateInHz
     * @param chancle
     */
    public native void setAudioOptions(int sampleRateInHz,int chancle);

}

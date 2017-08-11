package sen.com.video;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.view.Surface;

/**
 * Created by Administrator on 2017/8/7.
 */

public class VideoAudioPlay {
    static {
        System.loadLibrary("native-lib");
    }
    //多线程 音视频同步
    public native void videoAudioPlayerV2(String filePath,String audioOutFilePath, Surface surface);

    //提供给ＪＮＩ　调用
    public AudioTrack createAudioTrack(){
        int sampleRateInHz = 44100;
        int channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        int streamType = AudioManager.STREAM_MUSIC;
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz,channelConfig,audioFormat);
        AudioTrack audioTrack = new AudioTrack( streamType,  sampleRateInHz,  channelConfig,audioFormat,
                bufferSizeInBytes,  AudioTrack.MODE_STREAM);


        return audioTrack;
    }
}

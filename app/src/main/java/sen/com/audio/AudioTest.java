package sen.com.audio;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

/**
 * Created by Administrator on 2017/8/4.
 */

public class AudioTest {

    static {
        System.loadLibrary("native-lib");
    }


    public native void sound(String filePath,String outFilePath) ;
    public native void sound2(String filePath,String outFilePath) ;
    public native void playSound(String filePath,String outFilePath) ;

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

    /**
     * 这个是AudioTrack 类中的方法，我想把他的签名给叫你使用但是javap -s 报找不到类，只能把他弄到这里来获取了

     */
//    public int write(@NonNull byte[] audioData, int offsetInBytes, int sizeInBytes) {
//        return 0;
//    }


}

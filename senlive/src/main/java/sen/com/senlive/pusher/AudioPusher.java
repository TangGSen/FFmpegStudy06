package sen.com.senlive.pusher;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import sen.com.senlive.config.AudioParams;
import sen.com.senlive.natives.PushNative;

/**
 * Created by Administrator on 2017/8/13.
 */

public class AudioPusher extends Pusher {
    private AudioParams mAudioParams;
    private AudioRecord mAudioRecord;
    private final int bufferSizeInBytes;
    private boolean isPushing;
    private  PushNative mPushNative;

    public AudioPusher(AudioParams audioParams, PushNative pushNative) {
        this.mPushNative =pushNative;
        this.mAudioParams = audioParams;
        int channelConfig = audioParams.getChancle() == 1 ? AudioFormat.CHANNEL_IN_MONO : AudioFormat.CHANNEL_IN_STEREO;
        int sampleRateInHz = audioParams.getSamlpRateInHz();
        bufferSizeInBytes = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT);
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT,
                bufferSizeInBytes);
    }

    @Override
    public void startPusher() {
        mAudioRecord.startRecording();
        isPushing = true;
        //开子线程去不断读取数据并传到native
        new Thread(new AudioReadTask()).start();
    }

    @Override
    public void stopPusher() {
        isPushing = false;
        mAudioRecord.stop();
    }

    @Override
    public void release() {
        if (mAudioRecord != null) {
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }

    class AudioReadTask implements Runnable {

        @Override
        public void run() {
            while (isPushing) {
                byte[] data = new byte[bufferSizeInBytes];
                int len = mAudioRecord.read(data, 0, data.length);
                if (len >= 0) {
                    mPushNative.sendAudio(data,len);
                }
            }
        }
    }
}

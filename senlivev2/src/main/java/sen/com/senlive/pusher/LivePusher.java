package sen.com.senlive.pusher;

import android.hardware.Camera;
import android.view.SurfaceHolder;

import sen.com.senlive.config.AudioParams;
import sen.com.senlive.config.VideoParmas;
import sen.com.senlive.natives.PushNative;

/**
 * Created by Administrator on 2017/8/13.
 */

public class LivePusher  implements SurfaceHolder.Callback {

    private  SurfaceHolder mSurfaceHolder;
    private VideoPusher videoPusher;
    private AudioPusher audioPusher;
    private PushNative pushNative;

    public LivePusher(SurfaceHolder holder){
        this.mSurfaceHolder = holder;
        holder.addCallback(this);
        prepare();
    }

    /**
     * 开始推流
     */
    public void startPusher(String url) {
        videoPusher.startPusher();
        audioPusher.startPusher();
        pushNative.startPush(url);
    }

    /**
     * 停止推流
     */
    public void stopPusher() {
        videoPusher.stopPusher();
        audioPusher.stopPusher();
        pushNative.stopPush();
    }

    /**
     * 稀放资源
     */
    private void release(){
        videoPusher.release();
        audioPusher.release();
        pushNative.release();
    }

    private void prepare() {
        pushNative = new PushNative();
        //如果只有一个摄像头的话，不能切换
        //帧频（手机上一般是25，也就是每秒25个画面）,码率 480000 （480 kpps）
        VideoParmas videoParmas = new VideoParmas(480,320,480000,25, Camera.CameraInfo.CAMERA_FACING_BACK);
        videoPusher = new VideoPusher(mSurfaceHolder,videoParmas, pushNative);
        AudioParams audioParams = new AudioParams(44100,1);
        audioPusher = new AudioPusher(audioParams, pushNative);
    }


    public void switchCamera() {
        videoPusher.switchCamera();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        stopPusher();
        release();
    }
}

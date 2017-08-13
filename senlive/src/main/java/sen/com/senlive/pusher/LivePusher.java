package sen.com.senlive.pusher;

import android.hardware.Camera;
import android.view.SurfaceHolder;

import sen.com.senlive.config.AudioParams;
import sen.com.senlive.config.VideoParmas;

/**
 * Created by Administrator on 2017/8/13.
 */

public class LivePusher  implements SurfaceHolder.Callback {

    private  SurfaceHolder mSurfaceHolder;
    private VideoPusher videoPusher;
    private AudioPusher audioPusher;

    public LivePusher(SurfaceHolder holder){
        this.mSurfaceHolder = holder;
        holder.addCallback(this);
        prepare();
    }

    /**
     * 开始推流
     */
    public void startPusher() {
        videoPusher.startPusher();
        audioPusher.startPusher();
    }

    /**
     * 停止推流
     */
    public void stopPusher() {
        videoPusher.stopPusher();
        audioPusher.stopPusher();
    }

    /**
     * 稀放资源
     */
    private void release(){
        videoPusher.release();
        audioPusher.release();
    }

    private void prepare() {
        //如果只有一个摄像头的话，不能切换
        VideoParmas videoParmas = new VideoParmas(480,320, Camera.CameraInfo.CAMERA_FACING_BACK);
        videoPusher = new VideoPusher(mSurfaceHolder,videoParmas);
        AudioParams audioParams = new AudioParams(44100,1);
        audioPusher = new AudioPusher(audioParams);
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

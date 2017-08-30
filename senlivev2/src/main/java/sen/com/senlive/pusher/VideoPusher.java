package sen.com.senlive.pusher;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.view.SurfaceHolder;

import java.io.IOException;

import sen.com.senlive.config.VideoParmas;
import sen.com.senlive.natives.PushNative;

/**
 * Created by Administrator on 2017/8/13.
 */

public class VideoPusher extends Pusher implements SurfaceHolder.Callback, Camera.PreviewCallback {
    private final VideoParmas mVideoParmas;
    private  PushNative mPushNative;
    private SurfaceHolder mSurfaceHolder;
    private Camera mCamera;
    private byte[] callbackBuffer;
    private boolean isPushing;

    public VideoPusher(SurfaceHolder holder, VideoParmas videoParmas, PushNative pushNative) {
        this.mSurfaceHolder = holder;
        this.mVideoParmas = videoParmas;
        mSurfaceHolder.addCallback(this);
        this.mPushNative =pushNative;

    }

    @Override
    public void startPusher() {
        mPushNative.setVideoOptions(mVideoParmas.getWidth(),
                mVideoParmas.getHeigth(),
                mVideoParmas.getBitrate(),
                mVideoParmas.getFps());
        isPushing = true;
    }

    @Override
    public void stopPusher() {
        isPushing = false;
    }

    @Override
    public void release() {
        stopCamera();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        //surface 初始化完成，开始摄像头预览
//        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
//
//        } else if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
//
//        }

        //使用旧的api

        startCamera();
    }


    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    //打开摄像头
    private void startCamera() {
        try {
            mCamera = Camera.open(mVideoParmas.getCarameId());
            Camera.Parameters parameters = mCamera.getParameters();
            //设置相机预览格式，NV21 是YUV格式不过是YVU
            parameters.setPreviewFormat(ImageFormat.NV21);
            parameters.setPreviewSize(mVideoParmas.getWidth(),mVideoParmas.getHeigth());
            //帧频
//            parameters.setPreviewFpsRange(mVideoParmas.getBitrate()-1,mVideoParmas.getBitrate());
            mCamera.setParameters(parameters);
            mCamera.setDisplayOrientation(90);
            mCamera.setPreviewDisplay(mSurfaceHolder);
            //获取摄像头预览数据
            mCamera.startPreview();
            mCamera.setPreviewCallback(this);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //稀放摄像头
    private void stopCamera() {
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.startPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    public void switchCamera() {
        if (mVideoParmas.getCarameId() == Camera.CameraInfo.CAMERA_FACING_BACK) {
            mVideoParmas.setCarameId(Camera.CameraInfo.CAMERA_FACING_FRONT);
        } else {
            mVideoParmas.setCarameId(Camera.CameraInfo.CAMERA_FACING_BACK);
        }
        stopCamera();
        startCamera();

    }

    //多久回调一次跟帧频有关
    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        if (isPushing){
            //传到native 编码
            mPushNative.sendVideo(data);
        }


    }
}

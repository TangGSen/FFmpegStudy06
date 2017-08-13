package sen.com.senlive;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.View;

import sen.com.senlive.pusher.LivePusher;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private LivePusher livePusher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        //开始时就预览摄像头
        livePusher = new LivePusher(surfaceView.getHolder());

    }

    public void startLive(View view){
        livePusher.startPusher();
    }
    public void stopLive(View view){
        livePusher.stopPusher();
    }
    //切换摄像头
    public void switchCamera(View view){
        livePusher.switchCamera();
    }
}

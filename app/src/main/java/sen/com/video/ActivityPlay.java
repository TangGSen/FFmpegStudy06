package sen.com.video;

import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Surface;
import android.view.View;

import java.io.File;

import sen.com.ffmpegstudy06.R;

/**
 * Created by Administrator on 2017/8/3.
 */

public class ActivityPlay extends AppCompatActivity {

    private VideoView surfaceView;
    private VideoPlayContrlor videoPlayContrlor;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        surfaceView = (VideoView) findViewById(R.id.sufaceView);
        videoPlayContrlor = new VideoPlayContrlor();
    }

    public void start(View view) {
       String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator + "ffmpeg.wmv";

        Surface surface = surfaceView.getHolder().getSurface();
        //surface 传入Nativie 用于绘制
        videoPlayContrlor.render(filePath,surface);

    }

    public void stop(View view) {

    }
}

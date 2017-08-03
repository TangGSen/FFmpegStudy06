package sen.com.video;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import sen.com.ffmpegstudy06.R;

/**
 * Created by Administrator on 2017/8/3.
 */

public class ActivityPlay extends AppCompatActivity {

    private VideoView surfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        surfaceView = (VideoView) findViewById(R.id.sufaceView);
    }

    public void start(View view) {

    }

    public void stop(View view) {

    }
}

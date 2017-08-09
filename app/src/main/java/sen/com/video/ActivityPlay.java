package sen.com.video;

import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.widget.EditText;

import java.io.File;

import sen.com.ffmpegstudy06.R;


/**
 * Created by Administrator on 2017/8/3.
 */

public class ActivityPlay extends AppCompatActivity {

    private VideoView surfaceView;
    private VideoPlayContrlor videoPlayContrlor;
    private EditText editText;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        surfaceView = (VideoView) findViewById(R.id.sufaceView);
        videoPlayContrlor = new VideoPlayContrlor();
        editText = (EditText) findViewById(R.id.path);

    }

    public void start(View view) {
       String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator + editText.getText().toString();
        String audioOutFilePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator + "audio.pcm";
        if (new File(filePath).exists()){
            Surface surface = surfaceView.getHolder().getSurface();
            //surface 传入Nativie 用于绘制
//            videoPlayContrlor.render(filePath,surface);
            new VideoAudioPlay().videoAudio(filePath,audioOutFilePath,surface);
            Log.e("sen","存在");
        }else{
            Log.e("sen","不存在");
        }


    }




    public void stop(View view) {

    }
}

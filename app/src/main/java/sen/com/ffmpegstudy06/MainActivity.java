package sen.com.ffmpegstudy06;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private String filePath;
    private String outFilePath;
    private FFmpegTest fFmpegTest;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

         TextView tv = (TextView) findViewById(R.id.sample_text);
        fFmpegTest = new FFmpegTest();
        tv.setText("获取信息：" + fFmpegTest.getConfigInfo());
        filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator + "ffmpeg1.mp4";
        outFilePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath()+ File.separator+"Download"+File.separator+"ffmpeg1.yuv";

        outFilePath =Environment.getExternalStorageDirectory()
                .getAbsolutePath()+ File.separator+"Download"+File.separator+"ffmpeg_audio.pcm";
    }

    public void run(View view) {
        if (new File(filePath).exists()) {
            Log.e("sen", "文件存在");
//            fFmpegTest.readVideoInfoNewApi(filePath);
//            fFmpegTest.readVideoInfoOldApi(filePath);
//            fFmpegTest.readFileInfoNewFromLaoShi(filePath);
//            fFmpegTest.callFFmpegOldDecode(filePath,outFilePath);
//            fFmpegTest.callFFmpegOldDecodeLaoShi(filePath,outFilePath);
//            fFmpegTest.callFFmpegNewDecode(filePath, outFilePath);

//            fFmpegTest.callFFmpegDecodePlay(filePath,outFilePath);
            fFmpegTest.callFFmpegDecodeAudio(filePath, outFilePath);
           // startActivity(new Intent(this, SDLActivity.class));

        } else {
            Log.e("sen", "文件不存在");
        }
    }
}




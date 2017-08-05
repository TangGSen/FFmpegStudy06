package sen.com.ffmpegstudy06;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.File;

import sen.com.audio.AudioTest;
import sen.com.video.ActivityPlay;

public class MainActivity extends AppCompatActivity {

    private String rootPath;
    private String outFilePath;
    private FFmpegTest fFmpegTest;
    private EditText editText;
    private EditText createFile;
    private String inFilePath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

         TextView tv = (TextView) findViewById(R.id.sample_text);
        fFmpegTest = new FFmpegTest();
        tv.setText("获取信息：" + fFmpegTest.getConfigInfo());
        rootPath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator ;


        editText = (EditText) findViewById(R.id.path);
        createFile = (EditText) findViewById(R.id.createFile);
    }

    public void run(View view) {
        inFilePath =rootPath+editText.getText().toString();
        outFilePath = rootPath+createFile.getText().toString();
        if (new File(inFilePath).exists()) {
            Log.e("sen", "文件存在");
//            fFmpegTest.readVideoInfoNewApi(inFilePath);
//            fFmpegTest.readVideoInfoOldApi(inFilePath);
//            fFmpegTest.readFileInfoNewFromLaoShi(inFilePath);
//            fFmpegTest.callFFmpegOldDecode(inFilePath,outFilePath);
//            fFmpegTest.callFFmpegOldDecodeLaoShi(inFilePath,outFilePath);
//            fFmpegTest.callFFmpegNewDecode(inFilePath, outFilePath);

//            fFmpegTest.callFFmpegDecodePlay(inFilePath,outFilePath);
//            fFmpegTest.callFFmpegDecodeAudio(inFilePath, outFilePath);

//            new AudioTest().sound(inFilePath,outFilePath);
//            new AudioTest().sound2(inFilePath,outFilePath);
            new AudioTest().playSound(inFilePath,outFilePath);

        } else {
            Log.e("sen", "文件不存在");
        }
    }


    public void play(View view){
        Intent intent = new Intent(this,ActivityPlay.class);
        startActivity(intent);
    }
}




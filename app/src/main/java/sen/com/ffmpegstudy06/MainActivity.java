package sen.com.ffmpegstudy06;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    // Example of a call to a native method
    TextView tv = (TextView) findViewById(R.id.sample_text);
        FFmpegTest test = new FFmpegTest();
    tv.setText("获取信息："+test.getConfigInfo());
      String filePath=  Environment.getExternalStorageDirectory()
              .getAbsolutePath()+ File.separator+"Download"+File.separator+"ffmpeg1.mp4";
        String outFilePath=  Environment.getExternalStorageDirectory()
                .getAbsolutePath()+ File.separator+"Download"+File.separator+"ffmpeg1.yuv";
        if (new File(filePath).exists()){
            Log.e("sen","文件存在");
//            test.readVideoInfoNewApi(filePath);
//            test.readVideoInfoOldApi(filePath);
//            test.readFileInfoNewFromLaoShi(filePath);
//            test.callFFmpegOldDecode(filePath,outFilePath);
            test.callFFmpegNewDecode(filePath,outFilePath);
//            test.callFFmpegOldDecodeLaoShi(filePath,outFilePath);
        }else{
            Log.e("sen","文件不存在");
        }

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

}




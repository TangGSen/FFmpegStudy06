package sen.com.ffmpegstudy06;

/**
 * Created by Administrator on 2017/7/25.
 */

public class FFmpegTest {
    static {
        System.loadLibrary("native-lib");
    }

    public native String getConfigInfo();

    public native void readVideoInfoNewApi(String filePath);

    public native void readVideoInfoOldApi(String filePath) ;
    //老师的版本
    public native void readFileInfoNewFromLaoShi(String filePath) ;

    public native void callFFmpegOldDecode(String filePath,String outFilePath) ;

    public native void callFFmpegNewDecode(String filePath,String outFilePath) ;

    public native void callFFmpegOldDecodeLaoShi(String filePath,String outFilePath) ;





    // Used to load the 'native-lib' library on application startup.

}

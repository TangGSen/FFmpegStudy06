package sen.com.thread;

/**
 * Created by Administrator on 2017/8/7.
 */

public class PThread {

    static {
        System.loadLibrary("natives-lib");
    }
    public native void createThread();
}

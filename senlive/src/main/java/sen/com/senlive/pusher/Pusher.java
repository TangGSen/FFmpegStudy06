package sen.com.senlive.pusher;

/**
 * Created by Administrator on 2017/8/13.
 */

public abstract class Pusher {
    public abstract void startPusher();

    public abstract void stopPusher();
    public abstract void release();
}

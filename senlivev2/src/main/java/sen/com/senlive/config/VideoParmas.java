package sen.com.senlive.config;

/**
 * Created by Administrator on 2017/8/13.
 */

public class VideoParmas {
    private int width;
    private int heigth;
    private int carameId;
    //码率 480000 （480 kpps）
    private int bitrate;
    //帧频（手机上一般是25，也就是每秒25个画面）
    private int fps;


    public VideoParmas(int width, int heigth,  int bitrate, int fps,int carameId) {
        this.width = width;
        this.heigth = heigth;
        this.carameId = carameId;
        this.bitrate = bitrate;
        this.fps = fps;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeigth() {
        return heigth;
    }

    public void setHeigth(int heigth) {
        this.heigth = heigth;
    }

    public int getCarameId() {
        return carameId;
    }

    public void setCarameId(int carameId) {
        this.carameId = carameId;
    }

    public int getBitrate() {
        return bitrate;
    }

    public void setBitrate(int bitrate) {
        this.bitrate = bitrate;
    }

    public int getFps() {
        return fps;
    }

    public void setFps(int fps) {
        this.fps = fps;
    }
}

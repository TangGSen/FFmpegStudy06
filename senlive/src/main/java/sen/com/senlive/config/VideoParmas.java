package sen.com.senlive.config;

/**
 * Created by Administrator on 2017/8/13.
 */

public class VideoParmas {
    private int width;
    private int heigth;
    private int carameId;

    public VideoParmas(int width, int heigth, int carameId) {
        this.width = width;
        this.heigth = heigth;
        this.carameId = carameId;
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
}

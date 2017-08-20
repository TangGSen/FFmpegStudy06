package sen.com.senlive.config;

/**
 * Created by Administrator on 2017/8/13.
 */

public class AudioParams {
    //采样率
    private int samlpRateInHz;
    //声道个数
    private int chancle;

    public AudioParams(int samlpRateInHz, int chancle) {
        this.samlpRateInHz = samlpRateInHz;
        this.chancle = chancle;
    }

    public int getSamlpRateInHz() {
        return samlpRateInHz;
    }

    public void setSamlpRateInHz(int samlpRateInHz) {
        this.samlpRateInHz = samlpRateInHz;
    }

    public int getChancle() {
        return chancle;
    }

    public void setChancle(int chancle) {
        this.chancle = chancle;
    }
}

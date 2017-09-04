package sen.com.ndk_image;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity {

    private ImageView image;
    private Bitmap srcBitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        image = (ImageView) findViewById(R.id.image);
        srcBitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.test);
    }

    public void src_imag(View view) {
        if (srcBitmap !=null){
            image.setImageBitmap(srcBitmap);
        }
    }

    public void java_imag(View view) {
        Bitmap bitmap = JavaImage.javaImage(srcBitmap);
        image.setImageBitmap(bitmap);
    }

    public void ndk_imag(View view) {
        int width = srcBitmap.getWidth();
        int height = srcBitmap.getHeight();
        int [] buffer = new int [width*height];
        srcBitmap.getPixels(buffer,0,width,0,0,width-1 ,height-1);
        int [] ndkImage = NdkImage.ndkimage(buffer,width,height);
        Bitmap bitmap = Bitmap.createBitmap(ndkImage,width,height, Bitmap.Config.RGB_565);
        image.setImageBitmap(bitmap);
    }
}

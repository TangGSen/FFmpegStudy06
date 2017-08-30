package sen.com.fixndk;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import java.io.File;

import sen.com.fixndk.fixcore.DexManager;

public class MainActivity extends AppCompatActivity {

    private TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.res);
    }

    public void hasBug(View view) {
        textView.setText(""+  GetData.getData());
    }

    public void fix(View view) {
        String fixDexPath = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+
                "Download"+File.separator+"fix.dex";
        DexManager.getInstance(this).loadFile(new File(fixDexPath));
    }

}

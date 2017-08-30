package sen.com.fixndk.fix;

import sen.com.fixndk.fixcore.Replace;

/**
 * Created by Administrator on 2017/8/29.
 */

public class GetData {
    @Replace(clazz = "sen.com.fixndk.GetData",method = "getData")
    public static int getData(){
        int a = 10;
        int b =1;
        return a/b;
    }
}

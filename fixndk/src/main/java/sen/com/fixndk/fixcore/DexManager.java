package sen.com.fixndk.fixcore;

import android.content.Context;
import android.os.Build;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Enumeration;

import dalvik.system.DexFile;

/**
 * Created by Administrator on 2017/8/29.
 */

public class DexManager {
    private Context context;
    private DexFile dexFile;
    static {
        System.loadLibrary("fix");
    }
    private DexManager(Context context){
        this.context = context.getApplicationContext();
    }

    private static DexManager manager;

    public static DexManager getInstance( Context context){
        if (manager==null){
            synchronized (DexManager.class){
                if (manager ==null){
                    manager =new DexManager(  context);
                }
            }
        }
        return manager;
    }

    public void loadFile(File file){
        try {
            //opt 是设置缓存路径
            DexFile dexFile = DexFile.loadDex(file.getAbsolutePath(),
                    new File(context.getCacheDir(),"opt").getAbsolutePath(),Context.MODE_PRIVATE);
            //下一步得到class 取出修复好的method

            Enumeration<String> entries =dexFile.entries();
            while (entries.hasMoreElements()){
                //获取class全类名，注意这里不能使用Class.forName()来反射，因为这些需要安装apk 才行
                String className =entries.nextElement();
                Class clazz = dexFile.loadClass(className,context.getClassLoader());

                    fixClass(clazz);

            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void fixClass(Class clazz) {
        //取出修复类里所有含有注解的方法
        Method[] methods = clazz.getDeclaredMethods();
        for (Method noBugMethod:methods ){
            Replace replace=  noBugMethod.getAnnotation(Replace.class);
            if (replace ==null){
                continue;
            }

            //拿到错误的方法，和类名
            String bugClassName = replace.clazz();
            String bugMethodName = replace.method();

            try {
                Class bugClazz = Class.forName(bugClassName);
                //需要找到跟没有bug 方法的参数类型一样的方法
                Method bugMethod = bugClazz.getDeclaredMethod(bugMethodName,noBugMethod.getParameterTypes());
                if (Build.VERSION.SDK_INT<=19) {
                    replace(Build.VERSION.SDK_INT,bugMethod,noBugMethod);
                }


            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            }


        }
    }

    private native void replace(int sdkInt ,Method bugMethod, Method noBugMethod);

}

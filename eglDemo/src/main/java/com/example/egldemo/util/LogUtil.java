package com.example.egldemo.util;

import android.util.Log;

public class LogUtil {
    public static void Log(String tag,String msg,int level){
        switch (level) {
            case 0:
                Log.d(tag,msg);break;
            case 1:
                Log.i(tag,msg);break;
            case 2:
                Log.w(tag, msg);break;
            case 3:
                Log.e(tag, msg);break;
            default:
                Log.d(tag, msg);
        }
    }

    public static void log(String msg) {
        Log("huan",msg,0);
    }
}

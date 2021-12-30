package com.example.util;

import android.util.Log;

public class Logger {

    private String TAG;
    private Logger(String tag) {
        TAG = "[" + tag +"] ";
    }
    public static Logger create(String tag) {
        return new Logger(tag);
    }

    public void d(String msg) {
        D(TAG,msg);
    }
    public void i(String msg) {
        I(TAG,msg);
    }
    public void v(String msg) {
        V(TAG,msg);
    }
    public void e(String msg) {
        E(TAG,msg);
    }

    public static void D(String tag,String msg) {
        Log.d(tag,msg);
    }
    public static void E(String tag,String msg) {
        Log.e(tag,msg);
    }
    public static void I(String tag,String msg) {
        Log.i(tag,msg);
    }
    public static void V(String tag,String msg) {
        Log.v(tag,msg);
    }
}

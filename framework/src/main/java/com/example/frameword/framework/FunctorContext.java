package com.example.frameword.framework;

import android.graphics.Bitmap;

import com.example.util.Logger;

public class FunctorContext {
    private long mNativeHandle;
    private Logger mLogger;
    private String mFunctorName;

    protected void onInit(long nativeHandle,String name) {
        mFunctorName = name;
        mLogger = Logger.create(mFunctorName +":functorContext");
        mNativeHandle = nativeHandle;
    }

    protected String getFunctorName() {
        return mFunctorName;
    }

    protected long getFunctorContextHandle() {
        return mNativeHandle;
    }

    protected void runInGLThread(Runnable task) {
        nativeRunInGLThread(mNativeHandle,task);
    }

    protected void executeSelf() {
        nativeExecuteSelf(mNativeHandle);
    }

    // ----------------------getInput------------------------------------

    protected NativeGLFrame getInputGLFrame(String tag,int index) {
        Object res = nativeGetInputGLFrame(mNativeHandle,tag,index);
        if (res == null) {
            mLogger.e("getInputGLFrame return null");
            return null;
        }
        return (NativeGLFrame) res;
    }

    // -----------------------setOutput----------------------------------

    protected void setOutputGLFrame(NativeGLFrame glFrame,String tag,int index) {
        nativeSetOutputGLFrame(mNativeHandle,glFrame,tag,index);
    }

    protected void setOutputBitmap(Bitmap bitmap,String tag,int index) {
        nativeSetOutputBitmap(mNativeHandle,bitmap,tag,index);
    }



    private native Object nativeGetInputGLFrame(long nativeHandle,String tag,int index);

    private native void nativeSetOutputGLFrame(long nativeHandle,Object glFrame,String tag,int index);
    private native void nativeSetOutputBitmap(long nativeHandle,Bitmap bitmap,String tag,int index);

    private native void nativeRunInGLThread(long nativeHandle, Object task);

    private native void nativeExecuteSelf(long nativeHandle);
}

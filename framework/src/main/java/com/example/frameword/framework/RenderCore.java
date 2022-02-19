package com.example.frameword.framework;

import android.view.Surface;

/**
 * 由于渲染环境以及配置都在c++中，java层要做渲染需要转到c++去执行
 */
public class RenderCore {
    private long mNativeFunctorContextHandle;
    private final long mNativeRenderHandle;

    public RenderCore(long nativeHandle,String vertexCode,String fragmentCode) {
        vertexCode = vertexCode == null? "" : vertexCode;
        fragmentCode = fragmentCode == null? "" : fragmentCode;
        mNativeFunctorContextHandle = nativeHandle;
        mNativeRenderHandle = nativeInitRender(nativeHandle,vertexCode,fragmentCode);
    }

    public void release() {
        nativeReleaseRender(mNativeRenderHandle);
    }

    public void releaseTexture(int width,int height,int texture,boolean reuse){
        nativeReleaseTexture(mNativeFunctorContextHandle,width,height,texture,reuse);
    }

    public void setViewPort(int[] viewPort) {
        nativeSetViewPort(mNativeRenderHandle,viewPort);
    }

    public void render() {
        nativeRender(mNativeRenderHandle);
    }

    public void swapEGLBuffer() {
        nativeSwapEGLBuffer(mNativeFunctorContextHandle);
    }

    public void setUniformTexture(String name, int textureId) {
        nativeSetTexture(mNativeRenderHandle,mNativeFunctorContextHandle,name,textureId);
    }

    public void setUniformMat4(String name, float[] matrix) {
        nativeSetMat4(mNativeRenderHandle,name,matrix);
    }

    public void setUniformInt(String name, int value) {
        nativeSetInt(mNativeRenderHandle,name,value);
    }

    public static void setWindowSurface(long nativeHandle, Surface surface,int width,int height) {
        nativeSetWindowSurface(nativeHandle,surface,width,height);
    }

    private native long nativeInitRender(long nativeHandle,String vertexCode,String fragmentCode);
    private native void nativeReleaseTexture(long nativeFunctorContextHandle,int width,int height,int textureId,boolean reuse);
    private native void nativeRender(long nativeRenderHandle);
    private native void nativeSetViewPort(long nativeRenderHandle,int[] viewPort);
    private native void nativeSetTexture(long nativeRenderHandle,long nativeFunctorContextHandle, String name,int textureId);
    private native void nativeSetMat4(long nativeRenderHandle,String name,float[] matrix);
    private native void nativeSetInt(long nativeRenderHandle,String name,int value);
    private native void nativeReleaseRender(long nativeRenderHandle);
    private native void nativeSwapEGLBuffer(long nativeFunctorContext);

    private static native void nativeSetWindowSurface(long nativeHandle, Surface surface,int width,int height);

}

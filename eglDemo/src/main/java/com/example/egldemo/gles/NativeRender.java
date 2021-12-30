package com.example.egldemo.gles;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLSurface;
import android.opengl.GLES30;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import com.example.egldemo.util.LogUtil;
import com.example.egldemo.util.SyncThreadHandler;

public class NativeRender {
    private final SyncThreadHandler mEglHandler;
    private final static int INIT = 0;
    private final static int SET_OUTPUT_TEXTURE = 1;
    private final static int CREATE_INPUT_TEXTURE = 2;
    private final static int FRAME_AVAILABLE = 3;
    // 内部handler包含了一个独立线程，用与切换渲染线程
    private class EGLHandler extends SyncThreadHandler {

        public EGLHandler(String tag) {
            super(tag);
        }
        @Override
        public void handleMsg(Message msg) {
            LogUtil.log("handler what = "+ msg.what);
            switch (msg.what) {
                case INIT: handlerInit(); break;
                case SET_OUTPUT_TEXTURE:handlerSetOutputTexture();break;
                case CREATE_INPUT_TEXTURE:handleCreateInputTexture();break;
                case FRAME_AVAILABLE:handleDraw();break;
                default:break;
            }
        }
    }

    // 输入texture
    private int mInputTextureId;
    private SurfaceTexture mInputSurfaceTexture;
    // 输出surface
    private Surface mOutputSurface;
    // 输出窗口宽高
    int[] viewPort = new int[2];
    private long mNativeEGLSurface;
    private long mNativeEglHandle;
    private long mNativeRenderHandle;
    private long mNativeProgramHandle;


    public NativeRender() {
        mEglHandler = new NativeRender.EGLHandler("EGLContextHandler");
        mEglHandler.sendSyncEmptyMsg(INIT);
    }

    public void setOutputTexture(SurfaceTexture surfaceTexture,int width,int height) {
        mOutputSurface = new Surface(surfaceTexture);
        viewPort[0] = width;
        viewPort[1] = height;
        mEglHandler.sendSyncEmptyMsg(SET_OUTPUT_TEXTURE);
    }

    public void setOutputSurface(Surface surface, int width, int height) {
        mOutputSurface = surface;
        viewPort[0] = width;
        viewPort[1] = height;
        mEglHandler.sendSyncEmptyMsg(SET_OUTPUT_TEXTURE);
    }

    public SurfaceTexture createInputTexture() {
        mEglHandler.sendSyncEmptyMsg(CREATE_INPUT_TEXTURE);
        return mInputSurfaceTexture;
    }

    public void release() {
        if (mEglHandler != null) {
            mEglHandler.release();
        }
    }

    private void handlerInit() {
        mNativeEglHandle = nativeInitEGL();
    }

    private void handlerSetOutputTexture(){
        LogUtil.log("handle set output texture");
        mNativeEGLSurface = nativeCreateWindowSurface(mNativeEglHandle,mOutputSurface);
        nativeMakeCurrentSurface(mNativeEglHandle,mNativeEGLSurface);

        mNativeRenderHandle = nativeInitRender();
        mNativeProgramHandle = nativeCreateProgram(mNativeRenderHandle);
        
        checkEglError("set output texture");
        LogUtil.log("egl setOutput texture success");
    }

    // 创建一个外部纹理
    private void handleCreateInputTexture(){
        LogUtil.log("handle create input texture");
        mInputTextureId = nativeCreateOESTexture(mNativeRenderHandle);
        mInputSurfaceTexture = new SurfaceTexture(mInputTextureId);
        HandlerThread thread = new HandlerThread("setOnFrameAvailableListener");
        thread.start();
        Handler handler = new Handler(thread.getLooper());
        mInputSurfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                mEglHandler.sendSyncEmptyMsg(FRAME_AVAILABLE);
            }
        },handler);
        checkEglError("create input texture");
        LogUtil.log(" egl create input texture success");
    }

    private void handleDraw(){
        LogUtil.log("handler dea");
        if (mInputSurfaceTexture != null) {
            mInputSurfaceTexture.updateTexImage();
        }
        nativeDraw(mNativeEglHandle,mNativeRenderHandle,mInputTextureId,mNativeProgramHandle);
    }

    private void checkEglError(String msg) {
        int error;
        if ((error = EGL14.eglGetError()) != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }

    public void checkGlError(String op) {
        int error;
        if ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            LogUtil.log(op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }

    private native long nativeInitEGL();
    private native long nativeInitRender();
    private native int nativeCreateOESTexture(long nativeRenderHandle);
    private native long nativeCreateWindowSurface(long nativeEglHandle,Surface surface);
    private native void nativeMakeCurrentSurface(long nativeEglHandle,long eglSurface);
    private native long nativeCreateProgram(long nativeRenderHandle);
    private native void nativeDraw(long nativeEglCore,long nativeRenderCore,int textureId,long program);
}

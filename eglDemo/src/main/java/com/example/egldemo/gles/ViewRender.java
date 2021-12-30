package com.example.egldemo.gles;


import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLSurface;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import com.example.egldemo.util.LogUtil;
import com.example.egldemo.util.SyncThreadHandler;

public class ViewRender  {
    private final SyncThreadHandler mEglHandler;
    private final static int INIT = 0;
    private final static int SET_OUTPUT_TEXTURE = 1;
    private final static int CREATE_INPUT_TEXTURE = 2;
    private final static int FRAME_AVAILABLE = 3;
    private final static int MAKE_CURRENT_CONTEXT = 4;
    private final static int UPDATE = 5;
    // 内部handler包含了一个独立线程，用与切换渲染线程
    private class EGLHandler extends SyncThreadHandler {

        public EGLHandler(String tag) {
            super(tag);
        }
        @Override
        public void handleMsg(Message msg) {
            switch (msg.what) {
                case INIT: handlerInit(); break;
                case SET_OUTPUT_TEXTURE:handlerSetOutputTexture();break;
                case CREATE_INPUT_TEXTURE:handleCreateInputTexture();break;
                case FRAME_AVAILABLE:handleDraw();break;
                case MAKE_CURRENT_CONTEXT:handleMakeCurrent();break;
                case UPDATE:handleUpdateTexture();break;
                default:break;
            }
        }
    }

    // 输入texture
    private SurfaceTexture mInputSurfaceTexture;
    private int texId;
    // 输出surface
    private Surface mOutputSurface;
    // 输出窗口宽高
    int[] viewPort = new int[2];
    private EGLSurface mEGLSurface;

    private final EGLCore mEGLCore;
    private final RenderCore mRenderCore;


    public ViewRender() {
        mEglHandler = new EGLHandler("EGLContextHandler");
        mEGLCore = new EGLCore();
        mRenderCore = new RenderCore();
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

    public int getTexId () {
        return texId;
    }

    public void makeCurrentContext() {
        mEglHandler.sendSyncEmptyMsg(MAKE_CURRENT_CONTEXT);
    }

    public EGLContext getEGLContext() {
        return mEGLCore.getEGLContext();
    }

    public void updateTex() {
        mEglHandler.sendSyncEmptyMsg(UPDATE);
    }

    private void handlerInit() {
        mEGLCore.initEgl();
    }

    private void handlerSetOutputTexture(){
        LogUtil.log("handle set output texture");

        mEGLSurface = mEGLCore.createWindowSurface(mOutputSurface);
        mEGLCore.makeCurrent(mEGLSurface);

        mRenderCore.createProgram(viewPort[0],viewPort[1]);
        checkEglError("set output texture");

        LogUtil.log("egl setOutput texture success");
    }

    // 创建一个外部纹理
    private void handleCreateInputTexture(){
        LogUtil.log("handle create input texture");
        texId = mRenderCore.createOESTexture();
        mInputSurfaceTexture = new SurfaceTexture(texId);

        checkEglError("create input texture");
        LogUtil.log(" egl create input texture success");
    }

    private void handleDraw(){
        LogUtil.log("handler dea");
        if (mInputSurfaceTexture != null) {
            mInputSurfaceTexture.updateTexImage();
        }
        mRenderCore.draw(viewPort[0],viewPort[1]);
        mEGLCore.swapBuffer(mEGLSurface);
        LogUtil.log("jdjdd");
    }

    private void handleMakeCurrent() {
        EGLSurface surface = mEGLCore.createPBufferSurface();
        mEGLCore.makeCurrent(surface);
    }

    private void handleUpdateTexture() {
        if (mInputSurfaceTexture!= null) {
            mInputSurfaceTexture.updateTexImage();
        }
    }

    private void checkEglError(String msg) {
        int error;
        if ((error = EGL14.eglGetError()) != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }

}

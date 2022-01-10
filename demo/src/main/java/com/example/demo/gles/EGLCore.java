package com.example.demo.gles;

import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.view.Surface;

import com.example.demo.util.LogUtil;


public class EGLCore {
    private EGLDisplay mEGLDisplay = EGL14.EGL_NO_DISPLAY;
    private EGLConfig mEGLConfig;
    private EGLContext mEGLContext = EGL14.EGL_NO_CONTEXT;

    public EGLCore() {
    }

    // 初始化config、display、context、egl
    public void initEgl() {
        LogUtil.log("egl init~");

        mEGLDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (mEGLDisplay == EGL14.EGL_NO_DISPLAY) {
            LogUtil.log("init display error");
            return ;
        }
        checkEglError("eglGetDisplay");

        int[] version = new int[2];
        if (!EGL14.eglInitialize(mEGLDisplay,version,0,version,1)) {
            LogUtil.log("init fail");
            return;
        }
        checkEglError("eglInitialize");

        mEGLConfig = getConfig(3);
        if (mEGLConfig == null) {
            LogUtil.log("gles 3 is not support");
            return;
        }
        checkEglError("EGLConfig");

        int[] attrib3_list = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL14.EGL_NONE
        };
        mEGLContext = EGL14.eglCreateContext(mEGLDisplay,mEGLConfig,EGL14.EGL_NO_CONTEXT,attrib3_list,0);
        if (mEGLContext == EGL14.EGL_NO_CONTEXT) {
            LogUtil.log("create context fail");
            return ;
        }
        checkEglError("EGLContext");

        int[] values = new int[1];
        EGL14.eglQueryContext(mEGLDisplay, mEGLContext, EGL14.EGL_CONTEXT_CLIENT_VERSION,
                values, 0);
        checkEglError("eglQueryContext");

        LogUtil.log("EGLContext created, client version " + values[0]);
        LogUtil.log("EGL init success");
    }

    public EGLSurface createWindowSurface(Surface surface){
        LogUtil.log("createWindowSurface");
        int[] attr = {
                EGL14.EGL_NONE
        };
        EGLSurface eglSurface = EGL14.eglCreateWindowSurface(mEGLDisplay,mEGLConfig,surface,attr,0);
        if (eglSurface == null) {
            LogUtil.log("create egl surface error");
        }
        return eglSurface;
    }

    public EGLSurface createPBufferSurface() {
        int[] attr = {
                EGL14.EGL_NONE
        };
        EGLSurface surface = EGL14.eglCreatePbufferSurface(mEGLDisplay,mEGLConfig,attr,0);
        return surface;
    }

    public EGLContext getEGLContext() {
        return mEGLContext;
    }

    public void makeCurrent(EGLSurface eglSurface) {
        if (mEGLDisplay == EGL14.EGL_NO_DISPLAY) {
            LogUtil.log("have not create eglDisplay");
            return ;
        }
        if (!EGL14.eglMakeCurrent(mEGLDisplay, eglSurface, eglSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }
    }

    public void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
        if (mEGLDisplay == EGL14.EGL_NO_DISPLAY) {
            LogUtil.log("have not create eglDisplay");
            return ;
        }
        if (!EGL14.eglMakeCurrent(mEGLDisplay, drawSurface, readSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent(draw,read) failed");
        }
    }

    public void swapBuffer(EGLSurface eglSurface) {
        EGL14.eglSwapBuffers(mEGLDisplay,eglSurface);
    }


    private EGLConfig getConfig(int version) {
        int renderableType = EGL14.EGL_OPENGL_ES2_BIT;
        if (version >= 3) {
            renderableType |= EGLExt.EGL_OPENGL_ES3_BIT_KHR;
        }

        int[] attribList = {
                EGL14.EGL_RED_SIZE, 8,
                EGL14.EGL_GREEN_SIZE, 8,
                EGL14.EGL_BLUE_SIZE, 8,
                EGL14.EGL_ALPHA_SIZE, 8,
                //EGL14.EGL_DEPTH_SIZE, 16,
                //EGL14.EGL_STENCIL_SIZE, 8,
                EGL14.EGL_RENDERABLE_TYPE, renderableType,
                EGL14.EGL_NONE, 0,      // placeholder for recordable [@-3]
                EGL14.EGL_NONE
        };
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        if (!EGL14.eglChooseConfig(mEGLDisplay, attribList, 0, configs, 0, configs.length,
                numConfigs, 0)) {
            LogUtil.log("unable to find RGB8888 / " + version + " EGLConfig");
            return null;
        }
        return configs[0];
    }

    private void checkEglError(String msg) {
        int error;
        if ((error = EGL14.eglGetError()) != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }
}

package com.example.frameword.framework;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLSurface;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.view.Surface;
import android.view.TextureView;
import android.view.ViewGroup;
import androidx.annotation.NonNull;

public class GLViewFunctor extends Functor{
    private TextureView mTextureView;
    private Handler mMainHandler = new Handler(Looper.getMainLooper());
    private Surface mSurface;
    private RenderCore mRenderCore;
    private int mWindowWidth;
    private int mWindowHeight;

    private final String fragmentCode = "#version 300 es\n"+
        "out vec4 FragColor;\n"+
        "in vec2 otPos;\n"+
        "uniform mat4 model;\n"+
        "uniform sampler2D tex;\n"+
        "uniform int mirrorY;\n"+
        "void main(){\n"+
        "    vec4 pos = model * vec4(otPos.xy,0.0,1.0);\n"+
        "    if (mirrorY == 1) {\n"+
        "        pos.y = 1.0-pos.y;\n"+
        "    }\n"+
        "    FragColor = texture(tex,vec2(pos.x,pos.y));\n"+
        "}";

    @Override
    public boolean onInitialize() {
        mRenderCore = new RenderCore(getFunctorContextHandle(),null,fragmentCode);
        return true;
    }

    @Override
    public void onUnInitialize() {

    }

    @Override
    public boolean onExecute() {
        NativeGLFrame glFrame = getInputGLFrame("video",0);
        if (glFrame == null) {
            return false;
        }
        boolean[] res = {false};
        runInGLThread(() -> {
            res[0] = onExecuteGLFrame(glFrame);
        });
        return res[0];
    }

    @Override
    public void onSetOption(String key, Object value) {
        if ("viewGroup".equals(key)) {
            initTextureView((ViewGroup) value);
        }
    }

    private void initTextureView(ViewGroup viewGroup) {
        mTextureView = new TextureView(viewGroup.getContext());
        mTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
                mSurface = new Surface(surface);
                mWindowHeight = height;
                mWindowWidth = width;
                RenderCore.setWindowSurface(getFunctorContextHandle(),mSurface,width,height);
            }

            @Override
            public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {

            }

            @Override
            public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {

            }
        });
        mMainHandler.post(()->{
            viewGroup.addView(mTextureView, ViewGroup.LayoutParams.MATCH_PARENT,ViewGroup.LayoutParams.MATCH_PARENT);
        });
    }

    private boolean onExecuteGLFrame(NativeGLFrame glFrame) {
        mRenderCore.setViewPort(new int[]{0,0,mWindowWidth,mWindowHeight});
        mRenderCore.setUniformTexture("tex",glFrame.textureId);
        mRenderCore.setUniformMat4("model",calculateMatrix(glFrame));
        mRenderCore.setUniformInt("mirrorY",glFrame.orientation == -1 ? 1 : 0);
        mRenderCore.render();
        mRenderCore.swapEGLBuffer();
        mRenderCore.releaseTexture(glFrame.width,glFrame.height,glFrame.textureId,true);
        return true;
    }

    private float[] calculateMatrix(NativeGLFrame frame) {
        float windowRatio = mWindowWidth / (mWindowHeight * 1.0f);
        float frameRatio;
        if (frame.orientation == 90 || frame.orientation == 270) {
            frameRatio = frame.height/(frame.width*1.0f);
        }else{
            frameRatio = frame.width/(frame.height*1.0f);
        }
        float ratio = frameRatio/windowRatio;
        float[] mCropMatrix = {1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1};
        if (ratio > 1.0f) {
            mCropMatrix[0] = 1.0f / ratio;
            mCropMatrix[12] = 0.5f * (1.0f - 1.0f / ratio);
        } else if (ratio < 1.0f && ratio > 0) {
            mCropMatrix[5] = ratio;
            mCropMatrix[13] = 0.5f * (1.0f - ratio);
        }
        float[] res = new float[16];
        Matrix.multiplyMM(res,0,frame.matrix,0,mCropMatrix,0);
        return res;
    }

}

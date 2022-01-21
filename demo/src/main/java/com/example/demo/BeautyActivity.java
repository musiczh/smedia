package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.view.Surface;
import android.view.TextureView;
import android.widget.FrameLayout;
import android.widget.SeekBar;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.camera.api.PreviewInfo;
import com.example.demo.gles.ViewRender;
import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeGLFrame;

import java.util.HashMap;

public class BeautyActivity extends AppCompatActivity {
    private CameraCapture mCameraCapture= CameraService.obtainCamera();
    private ViewRender mRender = new ViewRender();
    private Graph mGraph = new Graph();

    private Handler mInputTexHandler;
    private Handler mMainThreadHandler = new Handler(Looper.getMainLooper());

    private PreviewInfo mPreviewInfo = new PreviewInfo(0,0,0);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_beauty);

        HandlerThread thread = new HandlerThread("setOnFrameAvailableListener");
        thread.start();
        mInputTexHandler = new Handler(thread.getLooper());

        mRender.makeCurrentContext();
        HashMap<String,Object> options = new HashMap<>();
        options.put("EGLSharedContext",mRender.getEGLContext().getNativeHandle());
        mGraph.init(Util.getJson("beautyCameraGraph.json",this),options);
        mGraph.run();

        SurfaceTexture inputST = mRender.createInputTexture();
        inputST.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                mMainThreadHandler.post(()->{
                    mRender.updateTex();
                    NativeGLFrame nativeGLFrame = new NativeGLFrame();
                    surfaceTexture.getTransformMatrix(nativeGLFrame.matrix);
                    nativeGLFrame.width = mPreviewInfo.width;
                    nativeGLFrame.height = mPreviewInfo.height;
                    nativeGLFrame.textureId = mRender.getTexId();
                    nativeGLFrame.orientation = mPreviewInfo.orientation;
                    mGraph.setOption("oesNode","DATA",nativeGLFrame);
                });
            }
        },mInputTexHandler);

        mCameraCapture.openCamera(this, CameraConfig.createBuilder()
                .facing(CameraConfig.FACING_FRONT)
                .setSurfaceTexture(inputST)
                .setCameraListener(new CameraListener(){
                    @Override
                    public void onPreviewStart(PreviewInfo info) {
                        mPreviewInfo = info;
                    }
                }).build());

        FrameLayout frameLayout = findViewById(R.id.mainLayout);
        SeekBar seekBar = findViewById(R.id.seekBarBeauty);

        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new SamplerSurfaceTextureListener(){
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
                Surface windowSurface = new Surface(surface);
                mGraph.setOption("renderNode","nativeWindow",windowSurface);
                mGraph.setOption("renderNode","mWindowWidth",width);
                mGraph.setOption("renderNode","mWindowHeight",height);
            }
        });
        frameLayout.addView(textureView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/100.f;
                mGraph.setOption("beautyNode","level",value);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) { }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mCameraCapture.release();
        mInputTexHandler.getLooper().quit();
        mInputTexHandler.removeCallbacksAndMessages(null);
        mMainThreadHandler.removeCallbacksAndMessages(null);
        mGraph.release();
        mRender.release();
    }
}
package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.camera.api.PreviewInfo;
import com.example.demo.gles.ViewRender;
import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeGLFrame;
import com.example.util.Logger;

import java.util.HashMap;

public class CameraDemoActivity extends AppCompatActivity {
    private Graph mGraph;
    private CameraCapture mCameraCapture = CameraService.obtainCamera();
    private ViewRender mRender = new ViewRender();
    private Handler mInputTexHandler;
    // todo 这里必须在主线程操作c++,因为如果在子线程，当activity退出杀了子线程时，内部
    // 会找不到env从而报错。这里后续需要优化JNIService来避免这个问题
    private Handler mMainThreadHandler = new Handler(Looper.getMainLooper());

    private int frameWidth;
    private int frameHeight;
    private int orientation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_demo);

        mGraph = new Graph();
        mRender.makeCurrentContext();
        HashMap<String,Object> options = new HashMap<>();
        options.put("EGLSharedContext",mRender.getEGLContext().getNativeHandle());
        mGraph.init(Util.getJson("cameraGraph.json",this),options);
        mGraph.run();

        initView();
    }

    private void initView() {
        FrameLayout mainLayout = findViewById(R.id.mainLayout);


        HandlerThread thread = new HandlerThread("setOnFrameAvailableListener");
        thread.start();
        mInputTexHandler = new Handler(thread.getLooper());

        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new SamplerSurfaceTextureListener(){
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
                Surface windowSurface = new Surface(surface);
                mGraph.setOption("renderNode","nativeWindow",windowSurface);
                mGraph.setOption("renderNode","mWindowWidth",width);
                mGraph.setOption("renderNode","mWindowHeight",height);

                SurfaceTexture cameraTexture = mRender.createInputTexture();

                cameraTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        // todo 切到主线程，但这里会有一点卡顿，因为消息执行需要先走完前面的消息内容
                        mMainThreadHandler.post(()->{
                            mRender.updateTex();
                            NativeGLFrame frame = new NativeGLFrame();
                            frame.orientation = orientation;
                            frame.width = frameWidth;
                            frame.height = frameHeight;
                            frame.textureId = mRender.getTexId();
                            surfaceTexture.getTransformMatrix(frame.matrix);
                            mGraph.setOption("oesNode","DATA",frame);
                        });
                    }
                },mInputTexHandler);
                CameraConfig config = CameraConfig.createBuilder()
                        .facing(CameraConfig.FACING_BACKGROUND)
                        .setCameraListener(new CameraListener(){
                            @Override
                            public void onPreviewStart(PreviewInfo info) {
                                frameHeight = info.height;
                                frameWidth = info.width;
                                orientation = info.orientation;
                            }
                        })
                        .setSurfaceTexture(cameraTexture)
                        .build();
                mCameraCapture.openCamera(CameraDemoActivity.this,config);
            }
        });
        mainLayout.addView(textureView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);

        Button switchButton = findViewById(R.id.switchButton);
        switchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraCapture.switchCamera();
            }
        });

        Button flashButton = findViewById(R.id.flashButton);
        flashButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraCapture.toggleFlash();
            }
        });


    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mCameraCapture.release();
        // 必须要在render之前释放，否则相机还没完全停止，会继续发送帧，导致发送消息到一个dead线程，
        // todo 此处需要增强syncHandler的健壮性
        mInputTexHandler.getLooper().quit();
        // 删除所有消息
        mInputTexHandler.removeCallbacksAndMessages(null);
        mMainThreadHandler.removeCallbacksAndMessages(null);
        mGraph.release();
        mRender.release();

    }
}
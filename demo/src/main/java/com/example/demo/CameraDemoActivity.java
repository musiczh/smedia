package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
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

import java.util.HashMap;

public class CameraDemoActivity extends AppCompatActivity {
    private Graph mGraph;
    private CameraCapture mCameraCapture = CameraService.obtainCamera();
    private ViewRender mRender = new ViewRender();

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

        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new SamplerSurfaceTextureListener(){
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
                Surface windowSurface = new Surface(surface);
                mGraph.setOption("renderNode","nativeWindow",windowSurface);
                mGraph.setOption("renderNode","windowWidth",width);
                mGraph.setOption("renderNode","windowHeight",height);

                SurfaceTexture cameraTexture = mRender.createInputTexture();
                HandlerThread thread = new HandlerThread("setOnFrameAvailableListener");
                thread.start();
                Handler handler = new Handler(thread.getLooper());
                cameraTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        mRender.updateTex();
                        NativeGLFrame frame = new NativeGLFrame();
                        frame.orientation = orientation;
                        frame.width = frameWidth;
                        frame.height = frameHeight;
                        frame.textureId = mRender.getTexId();
                        surfaceTexture.getTransformMatrix(frame.matrix);
                        mGraph.setOption("oesNode","DATA",frame);
                    }
                },handler);
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
        mGraph.release();
        mRender.release();
    }
}
package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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
import android.widget.Toast;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.demo.gles.ViewRender;
import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeGLFrame;
import com.example.util.Logger;
import com.example.util.PictureCache;

import java.util.HashMap;

public class CameraDemoActivity extends AppCompatActivity {
    private Graph mGraph;
    private final Logger mLogger = Logger.create("huan_CameraDemoActivity");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mLogger.d("cameraActivity onCreate start");
        setContentView(R.layout.activity_camera_demo);

        mGraph = new Graph();
        mGraph.init(Util.getJson("cameraGraph.json",this),null);
        mGraph.run();

        FrameLayout mainLayout = findViewById(R.id.mainLayout);
        mGraph.setOption("renderNode","viewGroup",mainLayout);
        mGraph.setOption("cameraNode","open",this);

        initView();
        mLogger.d("cameraActivity onCreate finish");
    }

    private void initView() {
        Button switchButton = findViewById(R.id.switchButton);
        switchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","switch",0);
            }
        });

        Button flashButton = findViewById(R.id.flashButton);
        flashButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","toggleFlash",0);
            }
        });

        Button takeButton = findViewById(R.id.pictureButton);
        takeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","takePicture",0);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mGraph.release();
        mLogger.d("onStop finish");
    }
}
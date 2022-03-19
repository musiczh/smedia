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
import android.widget.SeekBar;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.demo.gles.ViewRender;
import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeGLFrame;
import com.example.util.Logger;

import java.util.HashMap;

public class BeautyActivity extends AppCompatActivity {
    private Graph mGraph;
    private Handler mMainThreadHandler = new Handler(Looper.getMainLooper());
    private Logger mLogger = Logger.create("huan_BeautyActivity");



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mLogger.d("cameraActivity onCreate start");
        setContentView(R.layout.activity_beauty);

        mGraph = new Graph();
        mGraph.init(Util.getJson("beautyCameraGraph.json",this),null);
        mGraph.run();

        mGraph.setOption("cameraNode","open",this);
        mGraph.setOption("cameraNode","switch",0);
        mGraph.setOption("renderNode","viewGroup",findViewById(R.id.mainLayout));
        initView();
    }

    private void initView() {
        SeekBar seekBar = findViewById(R.id.seekBarBeauty);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/100.f;
                mGraph.setOption("beautyNode","level",value);

            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) { }
        });
        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","switch",0);
            }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMainThreadHandler.removeCallbacksAndMessages(null);
        mGraph.release();
        mLogger.d("onStop finish");
    }
}
package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.SeekBar;

import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeCallback;

public class ProcessImageActivity extends AppCompatActivity {

    private Graph mGraph = new Graph();
    private Bitmap mBitmap = null;
    private Bitmap mRawBitmap = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_process_image);

        mGraph.init(Util.getJson("proocessImageGraph.json",this),null);
        mGraph.run();
        mGraph.setOption("callbackNode", "callback", new NativeCallback() {
            @Override
            public boolean onNativeCallback(String nodeName, String keyName, Object value) {
                mBitmap = (Bitmap) value;
                onBitmapShow();
                return true;
            }
        });
        Bitmap bitmap = Util.GetAssetsBitmap("awesomeface.png",ProcessImageActivity.this);
        mBitmap = bitmap;
        mRawBitmap = bitmap;
        mGraph.setOption("imageSourceNode","Data",bitmap);

        SeekBar seekBar = findViewById(R.id.seekBar);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/120.f+0.1f;
                mGraph.setOption("adjustNode","brightness",value);
                mGraph.setOption("imageSourceNode","Data",mRawBitmap);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    private void onBitmapShow() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ImageView imageView = findViewById(R.id.imageView);
                imageView.setImageBitmap(mBitmap);
            }
        });
    }
}
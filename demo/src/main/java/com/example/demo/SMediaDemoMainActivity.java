package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.SurfaceTexture;
import android.opengl.GLES30;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;
import android.widget.FrameLayout;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.frameword.framework.Graph;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class SMediaDemoMainActivity extends AppCompatActivity {
    CameraCapture capture = CameraService.obtainCamera();
    Graph graph = new Graph();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_smedia_demo_main);

        graph.init(getJson("graph.json",this),null);
        graph.run();
        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
                graph.setOption("RenderFunctor","nativeWindow",new Surface(surfaceTexture));
                int[] textureId = new int[1];
                GLES30.glGenTextures(1,textureId,0);
                SurfaceTexture inputTexture = new SurfaceTexture(textureId[0]);
                inputTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

                    }
                });
                capture.setSurfaceTexture(inputTexture);
                capture.openCamera(SMediaDemoMainActivity.this,null);
            }

            @Override
            public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {

            }

            @Override
            public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture) {
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture) {

            }
        });
        FrameLayout layout = findViewById(R.id.mainLayout);
        layout.addView(textureView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
    }

    public static String getJson(String fileName, Context context) {
        StringBuilder stringBuilder = new StringBuilder();
        BufferedReader bf = null;
        try {
            //获取assets资源管理器
            AssetManager assetManager = context.getAssets();
            //通过管理器打开文件并读取
            bf = new BufferedReader(new InputStreamReader(assetManager.open(fileName)));
            String line;
            while ((line = bf.readLine()) != null) {
                stringBuilder.append(line);
            }
        } catch (IOException e) {
        } finally {
            try {
                if (bf != null) {
                    bf.close();
                }
            } catch (IOException e) {
            }
        }
        return stringBuilder.toString();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        capture.release();
        graph.release();
    }
}
package com.example.egldemo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.opengl.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraListener;
import com.example.camera.api.PreviewInfo;
import com.example.egldemo.gles.EGLCore;
import com.example.egldemo.gles.NativeRender;
import com.example.egldemo.gles.RenderCore;
import com.example.egldemo.gles.ViewRender;
import com.example.egldemo.util.LogUtil;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeCallback;
import com.example.frameword.framework.NativeGLFrame;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;

public class EGLMainActivity extends AppCompatActivity {
    {
        System.loadLibrary("eglDemo");
    }
    CameraCapture capture = CameraService.obtainCamera();
    ViewRender viewRender = new ViewRender();
    Graph graph = new Graph();
    private int mWidth;
    private int mHeight;
    private int mOrientation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_eglmain);

        viewRender.makeCurrentContext();
        HashMap<String,Object> map = new HashMap<>();
        map.put("EGLSharedContext", viewRender.getEGLContext().getNativeHandle());
        graph.init(getJson("graph.json",this),map);
        graph.run();
        graph.setOption("callbackNode", "callback", new NativeCallback() {
            @Override
            public boolean onNativeCallback(String nodeName, String keyName, Object value) {
                Bitmap bmp = (Bitmap) value;
                File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
                File file = new File(path,"eglDemo/pic.jpg");
                file.getParentFile().mkdirs();
                file.mkdir();
                try {
                    bmp.compress(Bitmap.CompressFormat.JPEG, 100, new FileOutputStream(file));
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
                return true;
            }
        });
        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int width, int height) {
                graph.setOption("renderNode","nativeWindow",new Surface(surfaceTexture));
                graph.setOption("renderNode","windowWidth",width);
                graph.setOption("renderNode","windowHeight",height);
                SurfaceTexture inputTexture = viewRender.createInputTexture();
                HandlerThread thread = new HandlerThread("setOnFrameAvailableListener");
                thread.start();
                Handler handler = new Handler(thread.getLooper());
                inputTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        viewRender.updateTex();
                        NativeGLFrame frame = new NativeGLFrame();
                        frame.width = mWidth;
                        frame.height = mHeight;
                        frame.orientation = mOrientation;
                        frame.textureId = viewRender.getTexId();
                        surfaceTexture.getTransformMatrix(frame.matrix);
                        graph.setOption("oesSource","DATA",frame);
                    }
                },handler);
                capture.setCameraListener(new CameraListener(){
                    @Override
                    public void onPreviewStart(PreviewInfo info) {
                        mWidth = info.width;
                        mHeight = info.height;
                        mOrientation = info.orientation;
                    }
                });
                capture.setSurfaceTexture(inputTexture);
                capture.openCamera(EGLMainActivity.this,null);

            }

            @Override
            public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
                graph.setOption("renderNode","windowWidth",i);
                graph.setOption("renderNode","windowHeight",i1);
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

        LinearLayout linearLayout = findViewById(R.id.huan);

        SeekBar seekBar = new SeekBar(this);
        linearLayout.addView(seekBar,FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/25.0f;
                graph.setOption("deNoiseNode","highLight",value);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        SeekBar seekBar2 = new SeekBar(this);
        linearLayout.addView(seekBar2,FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        seekBar2.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/25.0f-2f;
                graph.setOption("deNoiseNode","shadow",value);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        SeekBar seekBar3 = new SeekBar(this);
        linearLayout.addView(seekBar3,FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        seekBar3.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress/100.0f;
                graph.setOption("deNoiseNode","contrast",value);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        Button button = new Button(this);
        button.setText("点我test");
        linearLayout.addView(button, FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                graph.setOption("dispatchNode","imageSignal",true);
            }
        });
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
        viewRender.release();
    }
}
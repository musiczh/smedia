package com.example.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.demo.gles.ViewRender;
import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeCallback;
import com.example.frameword.framework.NativeGLFrame;
import com.example.util.Logger;
import com.example.util.PictureCache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

public class CameraAdjustActivity extends AppCompatActivity {
    static class Effect {
        public Effect(String name,int process) {
            this.name = name;
            this.process = process;
        }
        public String name = "";
        public int process = 0;
    }
    private Map<Integer,Effect> mMap;
    private Graph mGraph;
    private final Logger mLogger = Logger.create("huan_CameraAdjustActivity");
    private SeekBar mSeekBar;

    private int mSelect = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_adjust);

        mMap = new HashMap<>();
        mMap.put(1,new Effect("亮度",0));
        mMap.put(2,new Effect("对比度",0));
        mMap.put(3,new Effect("饱和度",0));
        mMap.put(4,new Effect("锐化",0));
        mMap.put(5,new Effect("高光",0));
        mMap.put(6,new Effect("阴影",0));
        mMap.put(7,new Effect("暖色调",0));
        mMap.put(8,new Effect("冷色调",0));
        mMap.put(9,new Effect("着色(紫)",0));
        mMap.put(10,new Effect("着色(绿)",0));


        mGraph = new Graph();
        mGraph.init(Util.getJson("adjustCameraGraph.json",this),null);
        mGraph.run();
        mGraph.setOption("callbackNode", "callback", new NativeCallback() {
            @Override
            public boolean onNativeCallback(String nodeName, String keyName, Object value) {
                if (value instanceof Bitmap) {
                    Bitmap bitmap = (Bitmap)value;
                    String path = PictureCache.saveBitmapExternal(bitmap,"smedia/cameraAdjust.jpg");
                    runOnUiThread(()->{
                        Toast.makeText(CameraAdjustActivity.this, "save picture to "+path,
                                Toast.LENGTH_SHORT).show();
                    });
                }
                return true;
            }
        });
        mGraph.setOption("renderNode","viewGroup",findViewById(R.id.mainLayout));
        mGraph.setOption("cameraNode","open",this);
        initView();
    }

    private void initView() {
        int i = 0;
        String[] data = new String[mMap.size()];
        for (Effect e : mMap.values()) {
            data[i] = e.name;
            i++;
        }

        TextView textView = findViewById(R.id.text);
        ListView listView = findViewById(R.id.listView);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, data);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                mSelect = position+1;
                mLogger.d("select index = "+mSelect+",text ="+mMap.get(mSelect));
                textView.setText(Objects.requireNonNull(mMap.get(mSelect)).name);
                mSeekBar.setProgress(Objects.requireNonNull(mMap.get(mSelect)).process);
            }
        });


        findViewById(R.id.buttonTakePicture).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","takePicture",0);
            }
        });

        findViewById(R.id.switchButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGraph.setOption("cameraNode","switch",0);
            }
        });

        mSeekBar = findViewById(R.id.seekBar);
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mMap.get(mSelect) == null) {
                    return;
                }
                mMap.get(mSelect).process = progress;
                float value = progress/100.f;
                switch (mSelect) {
                    case 1: {
                        mGraph.setOption("adjustNode","brightness",value/4.0f);break;
                    }
                    case 2: {
                        mGraph.setOption("adjustNode","contrast",value+1.0f);break;
                    }
                    case 3 : {
                        mGraph.setOption("adjustNode","saturation",value+1.0f);break;
                    }
                    case 4 : {
                        mGraph.setOption("sharpenNode","sharpen",value*4.0f);break;
                    }
                    case 5 : {
                        mGraph.setOption("shadowNode","highLight",value*2.0f);break;
                    }
                    case 6 : {
                        mGraph.setOption("shadowNode","shadow",value*2.0f);break;
                    }
                    case 7 : {
                        mGraph.setOption("whiteBalanceNode","temperature",value/2.0f);break;
                    }
                    case 8 : {
                        mGraph.setOption("whiteBalanceNode2","temperature",-(value/2.0f));break;
                    }
                    case 9 : {
                        mGraph.setOption("whiteBalanceNode","tint",value*1.5f);break;
                    }
                    case 10 : {
                        mGraph.setOption("whiteBalanceNode2","tint",-(value*1.5f));break;
                    }
                    default: break;
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) { }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGraph.release();
        mLogger.d("onDestroy finish");
    }
}
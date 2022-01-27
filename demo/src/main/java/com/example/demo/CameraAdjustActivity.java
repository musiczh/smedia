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
    class Effect {
        public Effect(String name,int process) {
            this.name = name;
            this.process = process;
        }
        public String name = "";
        public int process = 0;
    }
    private Map<Integer,Effect> mMap;
    private Graph mGraph;
    private CameraCapture mCameraCapture = CameraService.obtainCamera();
    private ViewRender mRender = new ViewRender();
    private Handler mInputTexHandler;
    private HandlerThread mInputThread;
    // todo 这里必须在主线程操作c++,因为如果在子线程，当activity退出杀了子线程时，内部
    // 会找不到env从而报错。这里后续需要优化JNIService来避免这个问题
    private Handler mMainThreadHandler = new Handler(Looper.getMainLooper());
    private Logger mLogger = Logger.create("huan_CameraAdjustActivity");
    private SeekBar mSeekBar;

    private int frameWidth;
    private int frameHeight;
    private int orientation;

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
        mRender.makeCurrentContext();
        HashMap<String,Object> options = new HashMap<>();
        options.put("EGLSharedContext",mRender.getEGLContext().getNativeHandle());
        mGraph.init(Util.getJson("adjustCameraGraph.json",this),options);
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


        FrameLayout mainLayout = findViewById(R.id.mainLayout);
        mInputThread = new HandlerThread("setOnFrameAvailableListener");
        mInputThread.start();
        mInputTexHandler = new Handler(mInputThread.getLooper());

        findViewById(R.id.buttonTakePicture).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraCapture.takePicture(null);
            }
        });

        findViewById(R.id.switchButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraCapture.switchCamera();
            }
        });

        TextureView textureView = new TextureView(this);
        textureView.setSurfaceTextureListener(new SamplerSurfaceTextureListener(){
            @Override
            public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
                Surface windowSurface = new Surface(surface);
                mLogger.e("set option window to graph");
                mGraph.setOption("renderNode","nativeWindow",windowSurface);
                mGraph.setOption("renderNode","mWindowWidth",width);
                mGraph.setOption("renderNode","mWindowHeight",height);

                SurfaceTexture cameraTexture = mRender.createInputTexture();

                cameraTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        // todo 切到主线程，但这里会有一点卡顿，因为消息执行需要先走完前面的消息内容
                        mMainThreadHandler.post(() -> {
                            mRender.updateTex();
                            NativeGLFrame frame = new NativeGLFrame();
                            frame.orientation = orientation;
                            frame.width = frameWidth;
                            frame.height = frameHeight;
                            frame.textureId = mRender.getTexId();
                            frame.timeStamp = SystemClock.elapsedRealtime();
                            surfaceTexture.getTransformMatrix(frame.matrix);
                            mGraph.setOption("oesNode", "DATA", frame);
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

                            @Override
                            public void onPictureTaken(PictureData result) {
                                Bitmap bitmap = BitmapFactory.decodeByteArray(result.data,0,result.data.length);
                                Matrix matrix = new Matrix();
                                matrix.setRotate(result.orientation);
                                Bitmap newBitmap = Bitmap.createBitmap(bitmap,0,0,bitmap.getWidth(),bitmap.getHeight(),matrix,false);
                                mGraph.setOption("imageSourceNode","data",newBitmap);
                            }
                        })
                        .setSurfaceTexture(cameraTexture)
                        .build();
                mCameraCapture.openCamera(CameraAdjustActivity.this,config);
            }
        });
        mainLayout.addView(textureView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);

        mSeekBar = findViewById(R.id.seekBar);
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
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
        mCameraCapture.release();
        // 必须要在render之前释放，否则相机还没完全停止，会继续发送帧，导致发送消息到一个dead线程，
        // todo 此处需要增强syncHandler的健壮性
        mInputTexHandler.getLooper().quit();
        mInputThread.quit();
        // 删除所有消息
        mInputTexHandler.removeCallbacksAndMessages(null);
        mMainThreadHandler.removeCallbacksAndMessages(null);
        mGraph.release();
        mRender.release();
        mLogger.d("onDestroy finish");
    }
}
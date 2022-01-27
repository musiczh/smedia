package com.example.demo;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.ContentUris;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.SurfaceTexture;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeCallback;
import com.example.util.PictureCache;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicBoolean;

public class ProcessImageActivity extends AppCompatActivity {
    class Effect {
        public Effect(String name,int process) {
            this.name = name;
            this.process = process;
        }
        public String name = "";
        public int process = 0;
    }
    enum ProcessState {
        NONE,PROCESSING,UPDATE
    }
    private Map<Integer,Effect> mMap;
    private Graph mGraph = new Graph();
    private Bitmap mBitmap = null;
    private Bitmap mRawBitmap = null;

    private SeekBar mSeekBar;
    private int mSelect = 0;
    private AtomicBoolean mNeedUpdate = new AtomicBoolean(false);
    private volatile ProcessState mState = ProcessState.NONE;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_process_image);

        mMap = new HashMap<>();
        mMap.put(1,new ProcessImageActivity.Effect("亮度",0));
        mMap.put(2,new ProcessImageActivity.Effect("对比度",0));
        mMap.put(3,new ProcessImageActivity.Effect("饱和度",0));
        mMap.put(4,new ProcessImageActivity.Effect("锐化",0));
        mMap.put(5,new ProcessImageActivity.Effect("高光",0));
        mMap.put(6,new ProcessImageActivity.Effect("阴影",0));
        mMap.put(7,new ProcessImageActivity.Effect("暖色调",0));
        mMap.put(8,new ProcessImageActivity.Effect("冷色调",0));
        mMap.put(9,new ProcessImageActivity.Effect("着色(紫)",0));
        mMap.put(10,new ProcessImageActivity.Effect("着色(绿)",0));

        mGraph.init(Util.getJson("proocessImageGraph.json",this),null);
        mGraph.run();
        mGraph.setOption("callbackNode", "callback", new NativeCallback() {
            @Override
            public boolean onNativeCallback(String nodeName, String keyName, Object value) {
                mBitmap = (Bitmap) value;
                onBitmapShow();
                if (mState == ProcessState.UPDATE) {
                    mState = ProcessState.PROCESSING;
                    mGraph.setOption("imageSourceNode","data",mRawBitmap);
                } else if (mState == ProcessState.PROCESSING) {
                    mState = ProcessState.NONE;
                }
                return true;
            }
        });
        Bitmap bitmap = Util.GetAssetsBitmap("awesomeface.png",ProcessImageActivity.this);
        mBitmap = bitmap;
        mRawBitmap = bitmap;
        mGraph.setOption("imageSourceNode","data",bitmap);

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
                if (mState == ProcessState.NONE) {
                    mState = ProcessState.PROCESSING;
                    mGraph.setOption("imageSourceNode","data",mRawBitmap);
                } else if (mState == ProcessState.PROCESSING) {
                    mState = ProcessState.UPDATE;
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        Button chooseButton = findViewById(R.id.button);
        chooseButton.setOnClickListener(v -> {
            Intent intent;
            intent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            startActivityForResult(intent,8);
        });

        Button saveButton = findViewById(R.id.button2);
        saveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String path = PictureCache.saveBitmapExternal(mBitmap,"smedia/pictureProcess.jpg");
                if (path != null) {
                    Toast.makeText(ProcessImageActivity.this, "save picture to "+path,
                            Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(ProcessImageActivity.this, "保存失败", Toast.LENGTH_SHORT).show();
                }
            }
        });

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
                textView.setText(Objects.requireNonNull(mMap.get(mSelect)).name);
                mSeekBar.setProgress(Objects.requireNonNull(mMap.get(mSelect)).process);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == Activity.RESULT_OK && requestCode == 8) {
            handImage(data);
        }
    }

    private void handImage(Intent data){
        String path =null;
        Uri uri = data.getData();
        //根据不同的uri进行不同的解析
        if (DocumentsContract.isDocumentUri(this,uri)){
            String docId = DocumentsContract.getDocumentId(uri);
            if ("com.android.providers.media.documents".equals(uri.getAuthority())){
                String id = docId.split(":")[1];
                String selection = MediaStore.Images.Media._ID+"="+id;
                path = getImagePath(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,selection);
            }else if("com.android.providers.downloads.documents".equals(uri.getAuthority())){
                Uri contentUri = ContentUris.withAppendedId(Uri.parse("content://downloads/public_downloads"),Long.valueOf(docId));
                path = getImagePath(contentUri,null);
            }
        }else if ("content".equalsIgnoreCase(uri.getScheme())){
            path = getImagePath(uri,null);
        }else if ("file".equalsIgnoreCase(uri.getScheme())){
            path = uri.getPath();
        }
        //展示图片
        displayImage(path);
    }

    //content类型的uri获取图片路径的方法
    private String getImagePath(Uri uri,String selection) {
        String path = null;
        Cursor cursor = getContentResolver().query(uri,null,selection,null,null);
        if (cursor!=null){
            if (cursor.moveToFirst()){
                int index = cursor.getColumnIndex(MediaStore.Images.Media.DATA);
                if (index == -1) {
                    return "";
                }
                path = cursor.getString(index);
            }
            cursor.close();
        }
        return path;
    }

    //根据路径展示图片的方法
    private void displayImage(String imagePath){
        if (imagePath != null){
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
            mRawBitmap = bitmap;
            mGraph.setOption("imageSourceNode","data",mRawBitmap);
        }else{
            Toast.makeText(this,"fail to set image",Toast.LENGTH_SHORT).show();
        }
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGraph.release();
    }
}
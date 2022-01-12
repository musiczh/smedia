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
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.demo.util.SamplerSurfaceTextureListener;
import com.example.frameword.framework.Graph;
import com.example.frameword.framework.NativeCallback;
import com.example.util.PictureCache;

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
            mGraph.setOption("imageSourceNode","Data",mRawBitmap);
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
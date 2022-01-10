package com.example.demo;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.example.util.Logger;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Util {
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
            e.printStackTrace();
        } finally {
            try {
                if (bf != null) {
                    bf.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return stringBuilder.toString();
    }

    public static Bitmap GetAssetsBitmap(String fileName, Context context) {
        InputStream inputStream = null;
        Bitmap resBitmap = null;
        try {
            //获取assets资源管理器
            AssetManager assetManager = context.getAssets();
            //通过管理器打开文件并读取
            inputStream = assetManager.open(fileName);
            resBitmap = BitmapFactory.decodeStream(inputStream);
            if (resBitmap == null) {
                Logger.D("GetAssetsBitmap","get assets bitmap error,file = "+fileName);
                return null;
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (inputStream != null) {
                    inputStream.close();
                }
            }catch (IOException e){
                e.printStackTrace();
            }
        }
        return resBitmap;
    }
}

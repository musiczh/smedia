package com.example.util;

import android.graphics.Bitmap;
import android.os.Environment;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

public class PictureCache {
    private static Logger mLogger = Logger.create("PictureCache");

    /**
     * 保存图片到外部图片存储中
     * @param bitmap 图片
     * @param fileName 图片文件名称，带路径和后缀，如"eglDemo/pic.jpg"
     * @return 存储成功返回绝对路径，否则返回null
     */
    public static String saveBitmapExternal(Bitmap bitmap,String fileName) {
        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
        File file = new File(path,fileName);
        if (!file.mkdirs()) {
            mLogger.d("saveBitmap mkdirs fail");
            return null;
        }
        try {
            ((Bitmap) bitmap).compress(Bitmap.CompressFormat.JPEG, 100, new FileOutputStream(file));
            return file.getAbsolutePath();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }
}

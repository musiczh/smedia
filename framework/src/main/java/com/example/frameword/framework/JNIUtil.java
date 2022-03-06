package com.example.frameword.framework;

import android.graphics.Bitmap;

import com.example.util.Logger;

import java.nio.Buffer;
import java.nio.ByteBuffer;

public class JNIUtil {

    public static Object onNativeCreateBitmap(Object buffer,int width,int height) {
        Bitmap bitmap = Bitmap.createBitmap(width,height,Bitmap.Config.ARGB_8888);
        if (buffer instanceof ByteBuffer) {
            bitmap.copyPixelsFromBuffer((Buffer)buffer);
        } else {
            Logger.D("JNIUtil","the buffer is not byteBuffer");
        }
        return bitmap;
    }

    public static Object onNativeCreateObject(String className) {
        try {
            Class cls = Class.forName(className);
            return cls.newInstance();
        } catch (ClassNotFoundException | IllegalAccessException | InstantiationException e) {
            e.printStackTrace();
        }
        return null;
    }

}

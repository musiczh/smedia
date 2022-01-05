package com.example.frameword.framework;

import android.graphics.Bitmap;

import com.example.util.Logger;

import java.nio.ByteBuffer;

public class JNIUtil {

    public static Object onNativeCreateBitmap(Object buffer,int width,int height) {
        Bitmap bitmap = Bitmap.createBitmap(width,height,Bitmap.Config.ARGB_8888);
        if (buffer instanceof ByteBuffer) {
            bitmap.copyPixelsFromBuffer((ByteBuffer)buffer);
        } else {
            Logger.D("JNIUtil","the buffer is not byteBuffer");
        }
        return bitmap;
    }

}

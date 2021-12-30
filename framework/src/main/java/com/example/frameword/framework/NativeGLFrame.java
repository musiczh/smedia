package com.example.frameword.framework;

public class NativeGLFrame {
    public int width;
    public int height;
    public int textureId;
    public int orientation;
    public float[] matrix = new float[16];

    Object onNativeGetUVMatrix() {
        return matrix;
    }
}

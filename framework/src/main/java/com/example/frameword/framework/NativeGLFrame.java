package com.example.frameword.framework;

public class NativeGLFrame {
    public int width;
    public int height;
    public int textureId;
    public int orientation;
    public double timeStamp;
    public boolean isMirrorY;
    public boolean isOES;
    public float[] matrix = new float[16];

    public NativeGLFrame() {

    }

    public NativeGLFrame(int width,int height,int textureId) {
        this.width = width;
        this.height = height;
        this.textureId = textureId;
    }

    public void setOrientation(int orientation) {
        this.orientation = orientation;
    }

    public void setTimeStamp(double timeStamp) {
        this.timeStamp = timeStamp;
    }

    public void setMirrorY(boolean mirrorY) {
        isMirrorY = mirrorY;
    }

    public void setMatrix(float[] matrix) {
        this.matrix = matrix;
    }

    Object onNativeGetUVMatrix() {
        return matrix;
    }
}

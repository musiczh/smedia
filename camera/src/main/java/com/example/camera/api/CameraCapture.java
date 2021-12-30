package com.example.camera.api;

import android.content.Context;
import android.graphics.SurfaceTexture;

public interface CameraCapture {
    public void openCamera(Context context,CameraConfig config);
    public void setCameraListener(CameraListener listener);
    public void setSurfaceTexture(SurfaceTexture surfaceTexture);
    public void switchCamera();
    public void release();
}

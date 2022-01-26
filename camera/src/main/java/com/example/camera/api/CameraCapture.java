package com.example.camera.api;

import android.content.Context;
import android.graphics.SurfaceTexture;

public interface CameraCapture {
    enum CameraState {
        CAMERA_CREATE,CAMERA_OPEN,CAMERA_PREVIEW,CAMERA_STOP,CAMERA_DESTROY
    }

    public void openCamera(Context context,CameraConfig config);
    public void setCameraListener(CameraListener listener);
    public void setSurfaceTexture(SurfaceTexture surfaceTexture);
    public void takePicture(CameraPictureListener listener);
    public void switchCamera();
    public int getFacing();
    public void toggleFlash();
    public void release();
}

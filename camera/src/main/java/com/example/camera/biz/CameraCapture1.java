package com.example.camera.biz;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.SystemClock;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.camera.api.PreviewInfo;
import com.example.util.Logger;

import java.io.IOException;
import java.util.List;

public class CameraCapture1 implements CameraCapture {
    private Context mContext;
    private Camera mCamera;
    private Camera.CameraInfo mCameraInfo = new Camera.CameraInfo();
    private CameraListener mListener;
    private SurfaceTexture mSurfaceTexture;
    private CameraConfig mCameraConfig;
    private final Logger mLogger = Logger.create("CameraCapture1");

    @Override
    public void openCamera(Context context, CameraConfig config) {
        long startTime = SystemClock.elapsedRealtime();

        if (config == null) {
            config = new CameraConfig();
        }
        mCameraConfig = config;

        mContext = context;
        int cameraCount = Camera.getNumberOfCameras();
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        for (int i=0;i<cameraCount;i++) {
            Camera.getCameraInfo(i,cameraInfo);
            if (cameraInfo.facing == config.facing) {
                break;
            }
        }
        mCameraInfo = cameraInfo;

        if (mCamera != null) {
            this.release();
            mLogger.d("release camera in open");
        }
        mCamera = Camera.open(cameraInfo.facing);

        int cal = calculateOrientation();
        mCamera.setDisplayOrientation(cal);

        Camera.Parameters parameters = mCamera.getParameters();
        Camera.Size size = parameters.getSupportedPreviewSizes().get(0);
        mCamera.setErrorCallback(new android.hardware.Camera.ErrorCallback() {
            @Override
            public void onError(int i, android.hardware.Camera camera) {
                System.out.println("camera error huan i="+i);
            }
        });
        int width = size.width;
        int height = size.height;
        parameters.setPreviewSize(width,height);

        List<String> focusModes = parameters.getSupportedFocusModes();
        for (String mode : focusModes) {
            if (mode.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
        }

        mCamera.setParameters(parameters);

        try {
            mCamera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mListener.onPreviewStart(new PreviewInfo(width,height,mCameraInfo.orientation));
        mCamera.startPreview();

        mLogger.d("camera start success,cost="+(SystemClock.elapsedRealtime()-startTime));
    }

    @Override
    public void setCameraListener(CameraListener listener) {
        mListener = listener;
    }

    @Override
    public void setSurfaceTexture(SurfaceTexture surfaceTexture) {
        mSurfaceTexture = surfaceTexture;
    }

    @Override
    public void switchCamera() {
        if (mCameraConfig.facing == CameraConfig.FACING_FRONT) {
            mCameraConfig.facing = CameraConfig.FACING_BACKGROUND;
        }else {
            mCameraConfig.facing = CameraConfig.FACING_FRONT;
        }
        openCamera(mContext,mCameraConfig);
    }

    int calculateOrientation() {
        WindowManager windowManager = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
        int def = Surface.ROTATION_90;
        if (windowManager != null && windowManager.getDefaultDisplay() != null) {
            def = windowManager.getDefaultDisplay().getRotation();
        }
        int activityOrientation = 0;
        switch (def) {
            case Surface.ROTATION_90: activityOrientation = 90; break;
            case Surface.ROTATION_0: activityOrientation = 0; break;
            case Surface.ROTATION_180: activityOrientation = 180; break;
            case Surface.ROTATION_270: activityOrientation = 270;break;
        }
        if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK){
            return (mCameraInfo.orientation-activityOrientation+360)%360;
        }else {
            return (mCameraInfo.orientation+activityOrientation)%360;
        }
    }

    @Override
    public void release() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
        }
    }
}

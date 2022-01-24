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
    private SurfaceTexture mSurfaceTexture;
    private CameraConfig mCameraConfig = new CameraConfig();
    private final Logger mLogger = Logger.create("CameraCapture1");

    private int mPreviewWidth;
    private int mPreviewHeight;
    private int mOrientation;

    @Override
    public void openCamera(Context context, CameraConfig config) {
        long startTime = SystemClock.elapsedRealtime();
        if (config != null) {
            mCameraConfig = config;
        }
        mContext = context;

        int cameraCount = Camera.getNumberOfCameras();
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        for (int i=0;i<cameraCount;i++) {
            Camera.getCameraInfo(i,cameraInfo);
            if (cameraInfo.facing == mCameraConfig.facing) {
                break;
            }
            if (i == cameraCount-1) {
                mLogger.e("can not open camera,facing = "+mCameraConfig.facing);
                return;
            }
        }
        mCameraInfo = cameraInfo;

        if (mCamera != null) {
            this.release();
            mLogger.d("release camera in open");
        }
        mCamera = Camera.open(cameraInfo.facing);
        mCamera.setErrorCallback((i, camera) -> System.out.println("camera error huan i="+i));

        initCameraParameters();

        try {
            if (mCameraConfig.surfaceTexture != null) {
                mCamera.setPreviewTexture(mCameraConfig.surfaceTexture);
            }else if (mCameraConfig.surfaceHolder != null) {
                mCamera.setPreviewDisplay(mCameraConfig.surfaceHolder);
            }else {
                mCamera.setPreviewTexture(mSurfaceTexture);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (mCameraConfig.cameraListener != null) {
            mCameraConfig.cameraListener.onPreviewStart(new PreviewInfo(mPreviewWidth,mPreviewHeight,
                    mOrientation));
        }
        mCamera.startPreview();

        mLogger.d("camera start success,cost="+(SystemClock.elapsedRealtime()-startTime));
    }

    @Override
    public void setCameraListener(CameraListener listener) {
        mCameraConfig.cameraListener = listener;
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

    @Override
    public void toggleFlash() {
        Camera.Parameters parameters = mCamera.getParameters();
        if (parameters.getFlashMode() == null) {
            mLogger.d("current camera is not support flash");
            return;
        }
        if (parameters.getFlashMode().equals(Camera.Parameters.FLASH_MODE_TORCH)) {
            parameters.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
        }else{
            parameters.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
        }
        mCamera.setParameters(parameters);
    }

    @Override
    public void release() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
        }
    }

    private int calculateOrientation() {
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
            return (360-(mCameraInfo.orientation+activityOrientation)%360)%360;
        }
    }

    private void initCameraParameters() {
        // 设置旋转角度
        mOrientation = calculateOrientation();
        mCamera.setDisplayOrientation(mOrientation);

        // 预览尺寸
        Camera.Parameters parameters = mCamera.getParameters();
        Camera.Size size = parameters.getSupportedPreviewSizes().get(0);
        mPreviewWidth = size.width;
        mPreviewHeight = size.height;
        parameters.setPreviewSize(mPreviewWidth,mPreviewHeight);

        // 对焦模式
        List<String> focusModes = parameters.getSupportedFocusModes();
        for (String mode : focusModes) {
            if (mode.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
        }

        mCamera.setParameters(parameters);

    }

}

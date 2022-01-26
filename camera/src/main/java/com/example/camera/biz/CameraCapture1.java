package com.example.camera.biz;

import android.content.Context;
import android.graphics.Picture;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.SystemClock;
import android.view.Surface;
import android.view.WindowManager;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.camera.api.CameraPictureListener;
import com.example.camera.api.CameraPreviewListener;
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
    private CameraState mState = CameraState.CAMERA_CREATE;

    private int mPreviewWidth;
    private int mPreviewHeight;
    private int mPictureWidth;
    private int mPictureHeight;
    private int mDisplayOrientation;
    private int mPictureRotation;

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
            mCameraConfig.cameraListener.onPreviewStart(new CameraPreviewListener.PreviewInfo(mPreviewWidth,mPreviewHeight,
                    mDisplayOrientation));
        }
        mCamera.startPreview();

        mLogger.d("camera start success,cost="+(SystemClock.elapsedRealtime()-startTime));
        mState = CameraState.CAMERA_PREVIEW;
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
    public void takePicture(CameraPictureListener listener) {
        if (mCamera == null || mState != CameraState.CAMERA_PREVIEW) {
            return ;
        }
        mCamera.takePicture(null, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                CameraPictureListener.PictureData pictureData =
                        new CameraPictureListener.PictureData(
                                data,
                                mPictureWidth,
                                mPictureHeight,
                                mPictureRotation);
                if (listener != null) {
                    listener.onPictureTaken(pictureData);
                    return;
                }
                if (mCameraConfig.cameraListener != null) {
                    mCameraConfig.cameraListener.onPictureTaken(pictureData);
                }
                if (mCamera != null && mState != CameraState.CAMERA_DESTROY) {
                    mCamera.cancelAutoFocus();
                    mCamera.startPreview();
                }
            }
        });
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
    public int getFacing() {
        return mCameraInfo.facing;
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
        if (mState == CameraState.CAMERA_DESTROY) {
            return;
        }
        if (mCamera != null) {
            mState = CameraState.CAMERA_DESTROY;
            mCamera.stopPreview();
            mCamera.release();
        }
    }

    private void calculateOrientation() {
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
        Camera.Parameters parameters = mCamera.getParameters();
        if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK){
            mPictureRotation = (mCameraInfo.orientation-activityOrientation+360)%360;
            mDisplayOrientation = mPictureRotation;
        }else {
            mPictureRotation = (mCameraInfo.orientation+activityOrientation)%360;
            mDisplayOrientation = (360-mPictureRotation)%360;
        }
        mCamera.setDisplayOrientation(mDisplayOrientation);
        parameters.setRotation(mPictureRotation);
        mCamera.setParameters(parameters);
    }

    private void initCameraParameters() {
        // 设置旋转角度
        calculateOrientation();

        // 预览尺寸
        Camera.Parameters parameters = mCamera.getParameters();

        Camera.Size size = parameters.getSupportedPreviewSizes().get(0);
        mPreviewWidth = size.width;
        mPreviewHeight = size.height;
        parameters.setPreviewSize(mPreviewWidth,mPreviewHeight);

        size = parameters.getSupportedPictureSizes().get(0);
        mPictureWidth = size.width;
        mPictureHeight = size.height;
        parameters.setPictureSize(mPictureWidth,mPictureHeight);

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

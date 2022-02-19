package com.example.frameword.framework;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.widget.Toast;

import com.example.camera.CameraService;
import com.example.camera.api.CameraCapture;
import com.example.camera.api.CameraConfig;
import com.example.camera.api.CameraListener;
import com.example.util.PictureCache;

import java.util.Deque;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Lock;

public class CameraFunctor extends Functor implements SurfaceTexture.OnFrameAvailableListener {
    private final Deque<NativeGLFrame> mBufferQueue = new ConcurrentLinkedDeque<>();
    private Context mContext;
    private int frameWidth;
    private int frameHeight;
    private int orientation;
    private int mTextureId;
    private Handler mInputTexHandler;
    private AtomicBoolean mIsExit = new AtomicBoolean(false);
    private Handler mMainHandler = new Handler(Looper.getMainLooper());
    private final CameraCapture mCameraCapture = CameraService.obtainCamera();

    @Override
    public boolean onInitialize() {
        HandlerThread thread = new HandlerThread("cameraFunctorFrameInput");
        thread.start();
        mInputTexHandler = new Handler(thread.getLooper());
        return false;
    }

    @Override
    public void onUnInitialize() {
        mIsExit.set(true);
        mCameraCapture.release();
        mInputTexHandler.removeCallbacksAndMessages(null);
        mBufferQueue.clear();
        mInputTexHandler.getLooper().quitSafely();
    }

    @Override
    public boolean onExecute() {
        if (mBufferQueue.isEmpty()) {
            return false;
        }
        NativeGLFrame frame = mBufferQueue.pollFirst();
        setOutputGLFrame(frame,"video",-1);
        return true;
    }

    @Override
    public void onSetOption(String key, Object value) {
        switch (key){
            case "open": {
                Context context = (Context)value;
                openCamera(context);
                break;
            }
            case "switch": {
                switchCamera();
                break;
            }
            case "takePicture": {
                takePicture();
                break;
            }
            case "toggleFlash": {
                mCameraCapture.toggleFlash();
                break;
            }
        }
    }

    private void openCamera(Context context) {
        mContext = context;
        int[] textureId = new int[1];
        runInGLThread(()->{
            GLES20.glGenTextures(1,textureId,0);
        });
        mTextureId = textureId[0];
        SurfaceTexture cameraTexture = new SurfaceTexture(mTextureId);
        cameraTexture.setOnFrameAvailableListener(this,mInputTexHandler);
        CameraConfig config = CameraConfig.createBuilder()
                .facing(CameraConfig.FACING_BACKGROUND)
                .setCameraListener(new CameraListener(){
                    @Override
                    public void onPreviewStart(PreviewInfo info) {
                        frameHeight = info.height;
                        frameWidth = info.width;
                        orientation = info.orientation;
                    }

                    @Override
                    public void onPictureTaken(PictureData result) {
                        Bitmap bitmap = BitmapFactory.decodeByteArray(result.data,0,result.data.length);
                        Matrix matrix = new Matrix();
                        matrix.setRotate(result.orientation);
                        Bitmap newBitmap = Bitmap.createBitmap(bitmap,0,0,bitmap.getWidth(),bitmap.getHeight(),matrix,false);
                        setOutputBitmap(newBitmap,"image",-1);
                    }
                })
                .setSurfaceTexture(cameraTexture)
                .build();
        mCameraCapture.openCamera(mContext,config);
    }

    private void switchCamera() {
        mCameraCapture.switchCamera();
    }

    private void takePicture() {
        mCameraCapture.takePicture(null);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (mIsExit.get()) {
            return;
        }
        mMainHandler.post(()->{
            runInGLThread(surfaceTexture::updateTexImage);
            NativeGLFrame frame = new NativeGLFrame();
            frame.orientation = orientation;
            frame.width = frameWidth;
            frame.height = frameHeight;
            frame.textureId = mTextureId;
            frame.isOES = true;
            surfaceTexture.getTransformMatrix(frame.matrix);
            mBufferQueue.offerLast(frame);
            executeSelf();
        });
    }
}

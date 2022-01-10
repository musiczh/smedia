package com.example.camera.api;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.view.Surface;
import android.view.SurfaceHolder;

/**
 * 相机配置类
 */
public class CameraConfig {
    public static int FACING_FRONT = Camera.CameraInfo.CAMERA_FACING_FRONT;
    public static int FACING_BACKGROUND = Camera.CameraInfo.CAMERA_FACING_BACK;

    public int facing = 0;
    public CameraListener cameraListener;

    // 仅生效一个，st优先
    public SurfaceTexture surfaceTexture;
    public SurfaceHolder surfaceHolder;

    public static Builder createBuilder() {
        return new Builder();
    }


    public static class Builder {
        private int facing = 0;
        private CameraListener cameraListener;
        private SurfaceTexture surfaceTexture;
        private SurfaceHolder surfaceHolder;

        public CameraConfig build() {
            CameraConfig config = new CameraConfig();
            config.facing = this.facing;
            config.cameraListener = this.cameraListener;
            // todo 后续需要强制设置此属性，目前兼容先过渡
            config.surfaceHolder = this.surfaceHolder;
            config.surfaceTexture = this.surfaceTexture;
            return config;
        }

        public Builder facing(int facing) {
            this.facing = facing;
            return this;
        }

        public Builder setCameraListener(CameraListener cameraListener) {
            this.cameraListener = cameraListener;
            return this;
        }

        public Builder setSurfaceHolder(SurfaceHolder surfaceHolder) {
            this.surfaceHolder = surfaceHolder;
            return this;
        }

        public Builder setSurfaceTexture(SurfaceTexture surfaceTexture) {
            this.surfaceTexture = surfaceTexture;
            return this;
        }
    }
}

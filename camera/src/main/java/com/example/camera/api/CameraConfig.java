package com.example.camera.api;

import android.hardware.Camera;

/**
 * 相机配置类
 */
public class CameraConfig {
    public static int FACING_FRONT = Camera.CameraInfo.CAMERA_FACING_FRONT;
    public static int FACING_BACKGROUND = Camera.CameraInfo.CAMERA_FACING_BACK;

    public int facing = 0;

    public static Builder createBuilder() {
        return new Builder();
    }


    static class Builder {
        private int facing = 0;

        public CameraConfig build() {
            CameraConfig config = new CameraConfig();
            config.facing = this.facing;
            return config;
        }

        Builder facing(int facing) {
            this.facing = facing;
            return this;
        }
    }
}

package com.example.camera;

import com.example.camera.api.CameraCapture;
import com.example.camera.biz.CameraCapture1;

public class CameraService {
    public static CameraCapture obtainCamera() {
        return new CameraCapture1();
    }
}

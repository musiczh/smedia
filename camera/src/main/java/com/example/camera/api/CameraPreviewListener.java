package com.example.camera.api;

public interface CameraPreviewListener {
    class PreviewInfo {
        public PreviewInfo(int width, int height, int orientation) {
            this.width = width;
            this.height = height;
            this.orientation = orientation;
        }

        public int width;
        public int height;
        public int orientation;
    }

    void onPreviewFrameAvailable(byte[] data,int width,int height);

    void onPreviewStart(PreviewInfo info) ;
}

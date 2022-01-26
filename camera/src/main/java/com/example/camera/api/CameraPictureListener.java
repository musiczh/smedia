package com.example.camera.api;

public interface CameraPictureListener {
     class PictureData {
        public PictureData(byte[] data,int width,int height,int orientation) {
            this.data = data;
            this.width = width;
            this.height = height;
            this.orientation = orientation;
        }
        public byte[] data;
        public int width;
        public int height;
        public int orientation;
    }
    void onPictureTaken(PictureData result);
}

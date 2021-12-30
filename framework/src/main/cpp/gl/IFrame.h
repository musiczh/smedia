//
// Created by wansu on 2021/12/30.
//

#ifndef SMEDIA_IFRAME_H
#define SMEDIA_IFRAME_H
/**
 * 渲染功能在图中传递帧的基类，子类有ImageFrame（cpu）和GLFrame（gpu）
 */
namespace smedia {
    enum FrameFormat {
        UNKNOWN = -1,
        NV12 = 0,
        RGB = 1,
        RGBA = 2,
        BGRA = 3,
        BGR = 4,
        NV21 = 5,
        GRAY = 6,
        I420 = 7,
        TEXTURE_2D = 100,
        TEXTURE_OES = 101
    };
    class IFrame {
    public:
        int width;
        int height;
        // 图形需要旋转的角度，来自相机的orientation
        int orientation;
        double timeStamp;
        FrameFormat format;

        // 渲染的时候需要使用的uv矩阵，让图形正常显示
        float UVMatrix[16];
        // 从原始的图形到目前为止给图像增加矩阵变换的累积，用于在对焦时取逆来还原初始坐标
        float calculateMatrix[16];

        // 镜像，暂时用不到，占位
        bool isMirrorY;
        bool isMirrorX;
    };
}



#endif //SMEDIA_IFRAME_H

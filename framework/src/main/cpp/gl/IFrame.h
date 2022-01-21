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
        FORMAT_UNKNOWN = -1,
        FORMAT_NV12 = 0,
        FORMAT_RGB = 1,
        FORMAT_RGBA = 2,
        FORMAT_BGRA = 3,
        FORMAT_BGR = 4,
        FORMAT_NV21 = 5,
        FORMAT_GRAY = 6,
        FORMAT_I420 = 7,
        FORMAT_TEXTURE_2D = 100,
        FORMAT_TEXTURE_OES = 101
    };
    class IFrame {
    public:
        int width;
        int height;
        // 图形需要旋转的角度，来自相机的orientation
        int orientation {0};
        double timeStamp {0};
        FrameFormat format;

        // 渲染的时候需要使用的uv矩阵，让图形正常显示
        float UVMatrix[16] {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };
        // 从原始的图形到目前为止给图像增加矩阵变换的累积，用于在对焦时取逆来还原初始坐标
        float calculateMatrix[16]{
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
        };

        // 镜像，暂时用不到，占位
        bool isMirrorY {false};
        bool isMirrorX {false};
    };
}



#endif //SMEDIA_IFRAME_H

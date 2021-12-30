//
// Created by wansu on 2021/12/11.
//

#ifndef SMEDIA_IMAGEFRAME_H
#define SMEDIA_IMAGEFRAME_H
/**
 * cpu数据帧
 */
namespace smedia {
    struct ImageFrame {
        int width;
        int height;
        float * uvMatrix;

    };
}



#endif //SMEDIA_IMAGEFRAME_H

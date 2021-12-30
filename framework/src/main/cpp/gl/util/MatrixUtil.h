//
// Created by wansu on 2021/12/27.
//

#ifndef SMEDIA_MATRIXUTIL_H
#define SMEDIA_MATRIXUTIL_H

namespace smedia {
    /**
    * 矩阵相乘
    * @param l 左矩阵
    * @param r 右矩阵
    * @param res 存储结果的矩阵
    */
    void MultiplyMM(const float *l,const float *r,float *res);
}



#endif //SMEDIA_MATRIXUTIL_H

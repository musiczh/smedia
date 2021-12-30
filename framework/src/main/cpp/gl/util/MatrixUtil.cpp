//
// Created by wansu on 2021/12/27.
//

#include "MatrixUtil.h"
namespace smedia {
    void MultiplyMM(const float *l,const float *r,float *res) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float value = 0;
                for (int k = 0; k < 4; ++k) {
                    value += l[i*4+k] * r[k*4+j];
                }
                res[i*4+j] = value;
            }
        }
    }
}

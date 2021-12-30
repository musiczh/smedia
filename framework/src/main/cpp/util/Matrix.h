//
// Created by wansu on 2021/12/30.
//

#ifndef SMEDIA_MATRIX_H
#define SMEDIA_MATRIX_H
#include "Eigen/Core"
namespace smedia {
    template<typename T>
    class Matrix {
    public:
        Matrix() = default;
        void setIdentify() {
        }
        void translate(T x,T y,T z,Matrix<T>&){
        }
        void scale(T x,T y,T z,Matrix<T>&){
        }
        void rotate(T x,T y,T z,Matrix<T>&){
        }
        T* getArray() {
        }
    private:
        Eigen::Matrix<T,4,4> matrix;
    };

    template<> class Matrix<float> {
    public:
        Matrix() = default;
        void setIdentify() {
            matrix.setIdentity();
        }
        void translate(float x,float y,float z,Matrix<float>&){

        }
        void scale(float x,float y,float z,Matrix<float>&){
        }
        void rotate(float x,float y,float z,Matrix<float>&){
        }
        float * getArray() {
            float * array = new float [16];
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    array[i*4+j] = matrix(i,j);
                }
            }
            return array;
        }
    private:
        Eigen::Matrix<float,4,4> matrix;
    };
}



#endif //SMEDIA_MATRIX_H

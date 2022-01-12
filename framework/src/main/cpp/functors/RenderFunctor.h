//
// Created by wansu on 2021/12/15.
//

#ifndef SMEDIA_RENDERFUNCTOR_H
#define SMEDIA_RENDERFUNCTOR_H
#include "IFunctor.h"
#include "Program.h"
#include "GLFrame.h"
#include "GLHelper.h"
#include "FunctorRegister.h"
#include "JNIBridge.h"
#include "android/native_window_jni.h"
#include "MatrixUtil.h"
#include <math.h>
/**
 * 接收2d纹理，并渲染到屏幕上，需要先设置
 * 1.NativeWindow:Surface
 * 2.窗口宽高：决定了渲染尺寸的正确
 */
namespace smedia {
    class RenderFunctor : public IFunctor {
    public:
        void initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(const std::string &key, Data value) override;

    private:
        std::unique_ptr<Program> mProgram;
        FunctorContext* mFunctorContext;

        GLContextRef mGLContext;

        int windowWidth;
        int windowHeight;

        volatile bool mInit;
    };
}



#endif //SMEDIA_RENDERFUNCTOR_H

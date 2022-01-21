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
 * android接收2d纹理，并渲染到屏幕上，需要先设置：
 * 1.NativeWindow:Surface
 * 2.窗口宽高：决定了渲染尺寸的正确
 */
namespace smedia {
    class Render;
    class GLBufferFrame;
    class RenderFunctor : public IFunctor {
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

    private:
        FunctorContext* mFunctorContext;
        GLContextRef mGLContext;
        std::unique_ptr<Render> mRender;

        int mWindowWidth;
        int mWindowHeight;
    };
}



#endif //SMEDIA_RENDERFUNCTOR_H

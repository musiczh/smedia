//
// Created by wansu on 2021/12/27.
//

#ifndef SMEDIA_IGLRENDERFUNCTOR_H
#define SMEDIA_IGLRENDERFUNCTOR_H
#include "IFunctor.h"
#include "GLContext.h"
#include "InputHandler.h"
#include "GLBufferFrame.h"
#include "GLFrame.h"
#include "Render.h"
#include "FunctorRegister.h"

// 帮助拼接字符串
#ifndef GL_DEFINE
#define GL_DEFINE

#define INPUT_CALLBACK [this](InputData inputData)->bool
#define CODE_STRING(code) #code
#define CODE_STRING_OES(code) "#version 300 es\n #extension GL_OES_EGL_image_external_essl3 : require\n"#code
#define CODE_STRING_SHADER(code) "#version 300 es\n"#code
#endif

/**
 * 所有的渲染functor基类，抽离公共操作
 */
namespace smedia {
    class IGLRenderFunctor : public IFunctor{
    private:
        bool initialize(FunctorContext *context) override;
        bool execute(FunctorContext *context) override;
        void setOption(FunctorContext *context, const std::string &key, Data value) override;

    public:
        // 初始化的时候调用，用于注册option以及其他的输入tag，video默认已经被注册，并会回调onDraw方法
        virtual bool onInit(InputHandler& inputHandler) = 0;
        // 进行绘制，此方法运行在渲染线程中。需要绑定bufferFrame，设置uniform，最后render和unbind。
        // 最后需要设置输出给functorContext
        virtual bool onDraw(GLBufferFrame* bufferFrame,Render* render,GLFrame& frame) = 0;
        virtual void unInitialize(FunctorContext *context) override;
        // 返回fragment着色器自动创建program,默认不创建program
        virtual std::string getFragmentCode();
        ~IGLRenderFunctor() override = default;

    protected:
        GLContextRef mGLContext;
        std::unique_ptr<GLBufferFrame> mGLBufferFrame;
        std::unique_ptr<Render> mRender;
        InputHandler mInputHandler;
        FunctorContext* mFunctorContext;
    };
}



#endif //SMEDIA_IGLRENDERFUNCTOR_H

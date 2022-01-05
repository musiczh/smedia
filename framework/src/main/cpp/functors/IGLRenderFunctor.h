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
        void initialize(FunctorContext *context) override;
        bool execute(FunctorContext *context) override;
        void setOption(const std::string &key, Data value) override;

    public:
        // 初始化的时候调用，用于注册option以及其他的输入tag，VIDEO已经被注册
        // 初始化program、input、输入参数
        virtual void onInit(InputHandler& inputHandler) = 0;
        // 进行绘制，需要创建渲染管线以及其他的操作,此方法执行在渲染线程
        virtual void onDraw(GLBufferFrame* bufferFrame,GLFrame& frame) = 0;
        virtual void unInitialize(FunctorContext *context) override = 0;
        // 返回fragment着色器自动创建program,默认不创建program
        virtual std::string getFragmentCode();
        ~IGLRenderFunctor() override = default;

    protected:
        GLContext mGLContext{};
        std::unique_ptr<GLBufferFrame> mGLBufferFrame;
        std::unique_ptr<Program> mRenderProgram;
        InputHandler mInputHandler;
        FunctorContext* mFunctorContext;

        // 是否自动输出纹理到VIDEO tag，默认输出
        bool ifSendFrame;
    };
}



#endif //SMEDIA_IGLRENDERFUNCTOR_H

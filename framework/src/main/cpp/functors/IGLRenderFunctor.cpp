//
// Created by wansu on 2021/12/27.
//

#include "IGLRenderFunctor.h"
namespace smedia {

    bool IGLRenderFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        if ((mGLContext = mFunctorContext->getService<GLContext>("GLContext")) == nullptr) {
            LOG_ERROR << "mGLContext in functorContext is null";
            return false;
        }
        // 初始化fbo
        mGLBufferFrame = GLBufferFrame::Create(mGLContext, nullptr);
        mRender = Render::CreateWithShaderCode(mGLContext,getFragmentCode());
        if (mRender == nullptr) {
            LOG_ERROR << "create render fail";
            return false;
        }

        mInputHandler.registerHandler("video",[this](const InputData& inputData)->bool{
            GLFrame *ptr = nullptr;
            Data data = inputData.data;
            if (data.isEmpty() || !data.isTypeOf<GLFrame>() || !(ptr = data.getData<GLFrame>())) {
                LOG_ERROR << "execute no input";
                return false;
            }
            GLFrame frame = *ptr;
            int viewPort[] = {0,0,frame.width,frame.height};

            mGLBufferFrame->setViewPort(viewPort);
            // 最后调用draw方法
            return mGLContext->runInRenderThread([this,&frame]()->bool{
                return onDraw(mGLBufferFrame.get(),mRender.get(),frame);
            });
        });

        return onInit(mInputHandler);
    }

    bool IGLRenderFunctor::execute(FunctorContext *context) {
        // 渲染需要放到渲染线程中去执行
        if (mGLContext == nullptr) {
            return false;
        }
        return mGLContext->runInRenderThread([this,context]()->bool {
            return mInputHandler.runExecuteHandler(context);
        });
    }

    void IGLRenderFunctor::setOption(FunctorContext *context,const std::string &key, Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }

    std::string IGLRenderFunctor::getFragmentCode() {
        return "";
    }

    void IGLRenderFunctor::unInitialize(FunctorContext *context) {
        //
    }
}
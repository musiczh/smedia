//
// Created by wansu on 2021/12/27.
//

#include "IGLRenderFunctor.h"
namespace smedia {

    void IGLRenderFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        ifSendFrame = true;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGLContext)) {
            LOG_ERROR << "glContext in functorContext is null";
            return;
        }
        // 初始化fbo
        mGLContext->runInRenderThread([this]()->bool{
            if (mGLBufferFrame == nullptr) {
                mGLBufferFrame = std::unique_ptr<GLBufferFrame>(new GLBufferFrame(mGLContext->getRenderCore()));
            }
            if (mRenderProgram == nullptr) {
                std::string fv = getFragmentCode();
                if (fv.length() != 0) {
                    mRenderProgram = std::unique_ptr<Program>(
                            mGLContext->getRenderCore()->createProgram(fv));
                }

            }
            return true;
        } );

        mInputHandler.registerHandler("VIDEO",[this](const InputData& inputData)->bool{
            GLFrame *ptr = nullptr;
            Data data = inputData.data;
            if (data.isEmpty() || !data.isTypeOf<GLFrame>() || !(ptr = data.getData<GLFrame>())) {
                LOG_ERROR << "execute no input";
                return false;
            }
            GLFrame frame = *ptr;
            // 创建fbo的颜色附着，若已经创建了则为无操作，否则会创建对应的新的纹理附着
            mGLBufferFrame->createTextureAttach(frame.width,frame.height);
            glViewport(0,0,frame.width,frame.height);
            // 最后调用draw方法
            onDraw(mGLBufferFrame.get(),frame);
            if (ifSendFrame) {
                auto* newFrame = new GLFrame(frame);
                auto* glTexture = new GLTexture(mGLContext,mGLBufferFrame->getTextureId(),frame.width,frame.height);
                newFrame->glTextureRef = std::shared_ptr<GLTexture>(glTexture);
                mFunctorContext->setOutput(Data::create(newFrame),"VIDEO");
            }
            mGLContext->runInRenderThread([this]()->bool{
                unsigned int tex = mGLBufferFrame->getTextureId();
                if (!ifSendFrame) {
                    // todo 如果没有存储到glFrame中，则需要释放纹理
                    mGLContext->getRenderCore()->deleteTexture(tex);
                }
                return true;
            });


            return true;
        });
        onInit(mInputHandler);
    }

    bool IGLRenderFunctor::execute(FunctorContext *context) {
        // 渲染需要放到渲染线程中去执行
        return mGLContext->runInRenderThread([this,context]()->bool {
            return mInputHandler.runHandler(context);
        });
    }

    void IGLRenderFunctor::setOption(const std::string &key, Data value) {
        mInputHandler.runHandler(mFunctorContext,key,value);
    }

    std::string IGLRenderFunctor::getFragmentCode() {
        return "";
    }
}
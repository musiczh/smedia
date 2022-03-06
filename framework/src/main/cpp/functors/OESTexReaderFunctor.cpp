//
// Created by wansu on 2021/12/14.
//

#include "OESTexReaderFunctor.h"
namespace smedia {

    const static std::string fragmentShader = "#version 300 es\n"
                                              "#extension GL_OES_EGL_image_external_essl3 : require\n"
                                 "out vec4 FragColor;\n"
                                 "// 顶点着色传进来的纹理坐标\n"
                                 "in vec2 otPos;\n"
                                 "uniform samplerExternalOES boxTexture;\n"
                                 "void main(){\n"
                                 "    FragColor = texture(boxTexture,vec2(otPos.xy));\n"
                                 "}";


    bool OESTexReaderFunctor::initialize(smedia::FunctorContext *context) {
        mFunctorContext = context;
        if ((mGLContext = mFunctorContext->getService<GLContext>("GLContext")) == nullptr) {
            LOG_ERROR << "mGLContext in functorContext is null";
            return false;
        }
        mGLBufferFrame = GLBufferFrame::Create(mGLContext, 0,0);
        mRender = Render::CreateWithShaderCode(mGLContext,fragmentShader);
        if (mRender == nullptr) {
            LOG_ERROR << "create render error";
            return false;
        }
        LOG_DEBUG << "initial OESTexReaderFunctor success";
        return true;
    }

    void OESTexReaderFunctor::unInitialize(smedia::FunctorContext *context) {
        // 释放纹理和fbo对象
        LOG_DEBUG << "unInitialize OESTexReaderFunctor";
    }

    bool OESTexReaderFunctor::execute(smedia::FunctorContext *context) {
        if (mGLContext == nullptr) {
            // 这里不打印错误日志，原因是上面会打印可以定位错误；且execute是每帧调用会大量产生日志
            return false;
        }
        Data data = getInputGLFrame();
        if (data.isEmpty()) {
            return false;
        }
        auto frame = *data.getData<GLFrame>();
        frame.glTextureRef = GLTexture::Create(mGLContext,frame.width,frame.height,
                                                frame.format == FORMAT_TEXTURE_2D? TEXTURE_TYPE_2D : TEXTURE_TYPE_OES,
                                                frame.textureId);
        // 外部oes纹理不能回收和复用
        frame.glTextureRef->setAutoOption(false, false);

        int viewPort[4] = {0,0,frame.width,frame.height};
        mGLBufferFrame->setSize(frame.width,frame.height);
        mGLBufferFrame->bind();
        mRender->setViewPort(viewPort);
        mRender->getProgram()->setTexture("boxTexture",frame.glTextureRef);
        mRender->draw();
        GLTextureRef glTexture = mGLBufferFrame->unBind();

        auto* newFrame = new GLFrame(frame);
        newFrame->glTextureRef = glTexture;
        Data newData = Data::create(newFrame);
        mFunctorContext->setOutput(newData,"video");
        return true;
    }


    void OESTexReaderFunctor::setOption(FunctorContext *context,const std::string &key, smedia::Data value) {
        if (key == "DATA") {
            if (mGLContext == nullptr) {
                return;
            }
            std::unique_lock<std::mutex> lock(mQueueLock);
            mOptionQueue.push_back(value);
            mFunctorContext->executeSelf();
        }
    }

    Data OESTexReaderFunctor::getInputGLFrame() {
        Data data;
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            if (!mOptionQueue.empty()) {
                data = mOptionQueue.front();
                mOptionQueue.pop_front();
            }
        }
        if (!data.isEmpty()) {
            return data;
        }

        data = mFunctorContext->popInput("video");
        return data;
    }

    REGISTER_CLASS(OESTexReaderFunctor)
}


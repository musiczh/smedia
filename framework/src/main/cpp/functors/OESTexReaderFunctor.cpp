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
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGLContext)) {
            LOG_ERROR << "mGLContext in functorContext is null";
            return false;
        }
        mGLBufferFrame = GLBufferFrame::Create(mGLContext, nullptr);
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
        Data data;
        GLFrame frame{};
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            if (!mOptionQueue.empty()) {
                data = mOptionQueue.front();
                mOptionQueue.pop_front();
            }
        }
        if (data.isEmpty() || !data.getData(frame)) {
            LOG_ERROR << "getData empty";
            return false;
        }

        int viewPort[4] = {0,0,frame.width,frame.height};
        mGLBufferFrame->setViewPort(viewPort);
        mGLBufferFrame->bind();
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
                LOG_ERROR << "mGLContext is null";
                return;
            }

            GLFrame* frame;
            if (!value.isTypeOf<GLFrame>() || value.isEmpty() || !(frame = value.getData<GLFrame>())) {
                LOG_ERROR << "get data fail";
                return;
            }
            frame->glTextureRef = GLTexture::Create(mGLContext,frame->width,frame->height,
                                                    frame->format == FORMAT_TEXTURE_2D? TEXTURE_TYPE_2D : TEXTURE_TYPE_OES,
                                                    frame->textureId);
            // 外部oes纹理不能回收和复用
            frame->glTextureRef->setAutoOption(false, false);
            std::unique_lock<std::mutex> lock(mQueueLock);
            mOptionQueue.push_back(value);
            mFunctorContext->executeSelf();
        }
    }

    REGISTER_FUNCTOR(OESTexReaderFunctor)
}


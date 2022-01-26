//
// Created by wansu on 2021/12/15.
//

#include "RenderFunctor.h"
#include "Render.h"
#include "GLBufferFrame.h"

namespace smedia {
    const static std::string fragmentShade = "#version 300 es\n"
                                              "out vec4 FragColor;\n"
                                              "in vec2 otPos;\n"
                                              "uniform mat4 model;\n"
                                              "uniform sampler2D tex;\n"
                                              "uniform int mirrorY;\n"
                                              "void main(){\n"
                                              "    vec4 pos = model * vec4(otPos.xy,0.0,1.0);\n"
                                              "    if (mirrorY == 1) {\n"
                                              "        pos.y = 1.0-pos.y;\n"
                                              "    }\n"
                                              "    FragColor = texture(tex,vec2(pos.x,pos.y));\n"
                                              "}";

    bool RenderFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGLContext)) {
            LOG_ERROR << "mGLContext in functorContext is null";
            return false;
        }
        mRender = Render::CreateWithShaderCode(mGLContext,fragmentShade);
        LOG_DEBUG << "initial RenderFunctor success";
        return true;
    }

    void RenderFunctor::unInitialize(FunctorContext *context) {
    }

    bool RenderFunctor::execute(FunctorContext *context) {
        Data inputData = mFunctorContext->popInput("video");
        GLFrame frame{};
        if (inputData.isEmpty() || !inputData.getData(frame)) {
            LOG_ERROR << "execute no input";
            return false;
        }

        // 帧数据的比例和屏幕的比例不同，需要进行缩放裁剪
        // todo 后面需要支持不同的缩放裁剪逻辑
        float windowRatio = mWindowWidth / (mWindowHeight * 1.0f);
        float frameRatio;
        if (frame.orientation == 90 || frame.orientation == 270) {
            frameRatio = frame.height/(frame.width*1.0f);
        }else{
            frameRatio = frame.width/(frame.height*1.0f);
        }
        float ratio = frameRatio/windowRatio;
        float mCropMatrix[16] = {1,0,0,0,
                                 0,1,0,0,
                                 0,0,1,0,
                                 0,0,0,1};
        auto *res = new float [16]{0};
        if (ratio > 1.0f) {
            mCropMatrix[0] = 1.0f / ratio;
            mCropMatrix[12] = 0.5f * (1.0f - 1.0f / ratio);
        } else if (ratio < 1.0f && ratio > 0) {
            mCropMatrix[5] = ratio;
            mCropMatrix[13] = 0.5f * (1.0f - ratio);
        }
        MultiplyMM(mCropMatrix,frame.UVMatrix,res);

        mGLContext->runInRenderThreadV([this,frame](){
            glViewport(0,0,mWindowWidth,mWindowHeight);
        });
        mRender->getProgram()->setTexture("tex",frame.glTextureRef);
        mRender->getProgram()->setMat4("model",res);
        // todo 系统相机出来的UVMatrix对openGL坐标和纹理坐标的Y轴倒置问题已经解决，而普通的照片显示需要进行Y轴倒置
        //  最好的解决思路是去掉系统的倒置，然后统一添加倒置矩阵。但是这里还不会使用矩阵
        //  暂时的解决方式是通过orientation判断是照片还是相机纹理，但是这个做法并不好，后续进行优化
        mRender->getProgram()->setInt("mirrorY",frame.orientation == -1 ? 1 : 0);
        mRender->draw();
        mGLContext->runInRenderThreadV([this](){
            mGLContext->getEglCore()->swapBuffer();
        });
        return true;
    }

    void RenderFunctor::setOption(FunctorContext *context, const std::string &key, Data value) {
        GLContextRef glContext;
        Data contextData = mFunctorContext->getGlobalService(GL_CONTEXT);
        if (!contextData.getData(glContext)) {
            LOG_ERROR << "mGLContext is not init";
            return;
        }
        if (key == NATIVE_WINDOW) {
            // 设置系统窗口
            JNIObject object;
            if (!value.getData(object)) {
                LOG_ERROR << "native window getData error";
                return;
            }
            ANativeWindow* nativeWindow = ANativeWindow_fromSurface(JNIService::getEnv(),object.getJObject());
            auto nativeWindowType = reinterpret_cast<EGLNativeWindowType>(nativeWindow);
            EGLCore* eglCore = glContext->getEglCore();
            glContext->runInRenderThread([eglCore,nativeWindowType]()->bool{
                EGLSurface surface = eglCore->createWindowSurface(nativeWindowType);
                eglCore->makeCurrentContext(surface);
                return true;
            });
            LOG_DEBUG << "create window context success";
            return;
        }
        if (key == "mWindowHeight") {
            value.getData(mWindowHeight);
        }
        if (key == "mWindowWidth") {
            value.getData(mWindowWidth);
        }
    }


    REGISTER_FUNCTOR(RenderFunctor)
}
//
// Created by wansu on 2021/12/15.
//

#include "RenderFunctor.h"
namespace smedia {
    const static std::string fragmentShade = "#version 300 es\n"
                                              "out vec4 FragColor;\n"
                                              "in vec2 otPos;\n"
                                             "uniform mat4 model;\n"
                                              "\n"
                                              "uniform sampler2D tex;\n"
                                              "\n"
                                              "void main(){\n"
                                              "    vec4 pos = model * vec4(otPos.xy,0.0,1.0);\n"
                                              "    FragColor = texture(tex,vec2(pos.x,pos.y));\n"
                                              "}";

    void RenderFunctor::initialize(FunctorContext *context) {
        if (mInit) {
            LOG_DEBUG << "RenderFunctor has init";
            return;
        }
        mFunctorContext = context;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGlContext)) {
            LOG_ERROR << "glContext in functorContext is null";
            return;
        }
        LOG_DEBUG << "initial RenderFunctor success" ;
        mInit = true;
    }

    void RenderFunctor::unInitialize(FunctorContext *context) {

    }

    bool RenderFunctor::execute(FunctorContext *context) {
        if (!mInit) {
            LOG_ERROR << "RenderFunctor has not init";
            return false;
        }
        Data inputData = mFunctorContext->popInput("VIDEO");
        GLFrame frame{};
        if (inputData.isEmpty() || !inputData.getData(frame)) {
            LOG_ERROR << "execute no input";
            return false;
        }

        RenderCore* renderCore = mGlContext.getRenderCore();
        EGLCore* eglCore = mGlContext.getEglCore();

        // 帧数据的比例和屏幕的比例不同，需要进行缩放裁剪
        float windowRatio = windowWidth/(windowHeight*1.0f);
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
        float res[16] = {0};

        if (ratio > 1.0f) {
            mCropMatrix[0] = 1.0f / ratio;
            mCropMatrix[12] = 0.5f * (1.0f - 1.0f / ratio);
        } else if (ratio < 1.0f && ratio > 0) {
            mCropMatrix[5] = ratio;
            mCropMatrix[13] = 0.5f * (1.0f - ratio);
        }
        MultiplyMM(mCropMatrix,frame.UVMatrix,res);

        float *uvm = res;

        mGlContext.runInRenderThread([this,renderCore,eglCore,uvm,frame]()->bool{
            glViewport(0,0,windowWidth,windowHeight);
            if (mProgram == nullptr) {
                mProgram = std::unique_ptr<Program>(renderCore->createProgram(fragmentShade));
            }
            mProgram->use();
            mProgram->setMat4("model",uvm);
            renderCore->draw(GL_TEXTURE_2D,frame.textureId,mProgram.get(),0);
            eglCore->swapBuffer();
            return true;
        });
        return true;
    }

    void RenderFunctor::setOption(const std::string &key, Data value) {
        long nativeWindowHandle = 0;
        GLContext glContext;
        Data contextData = mFunctorContext->getGlobalService(GL_CONTEXT);
        if (!contextData.getData(glContext)) {
            LOG_ERROR << "glContext is not init";
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
            EGLCore* eglCore = glContext.getEglCore();
            glContext.runInRenderThread([eglCore,nativeWindowType]()->bool{
                EGLSurface surface = eglCore->createWindowSurface(nativeWindowType);
                eglCore->makeCurrentContext(surface);
                return true;
            });
            LOG_DEBUG << "create window context success";
            return;
        }
        if (key == "windowHeight") {
            value.getData(windowHeight);
        }
        if (key == "windowWidth") {
            value.getData(windowWidth);
        }

    }

    REGISTER_FUNCTOR(RenderFunctor)
}
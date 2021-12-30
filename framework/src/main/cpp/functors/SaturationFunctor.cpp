//
// Created by wansu on 2021/12/28.
//

#include "SaturationFunctor.h"
namespace smedia {
    const static std::string fragmentShader = "#version 300 es\n"
                                              "out vec4 FragColor;\n"
                                              "// 顶点着色传进来的纹理坐标\n"
                                              "in vec2 otPos;\n"
                                              "uniform float saturation;\n"
                                              "\n"
                                              "uniform sampler2D tex;\n"
                                              "\n"
                                              "void main(){\n"
                                              "    vec4 color = texture(tex,otPos);\n"
                                              "vec2 test = color.xy / otPos.xy;\n"
                                              "float avg = (color.x+color.y+color.z) / 3.0;\n"
                                              "    FragColor = color + vec4((color.xyz-avg)*saturation,0);\n"
                                              "}";

    void SaturationFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mSaturation = 0.0f;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGLContext)) {
            LOG_ERROR << "glContext in functorContext is null";
            return;
        }
        RenderCore* renderCore = mGLContext.getRenderCore();
        mGLContext.runInRenderThread([this,renderCore]()->bool{
            mProgram = std::unique_ptr<Program>(renderCore->createProgram(fragmentShader));
            mFBO = renderCore->createFrameBuffer();
            return true;
        });
        LOG_DEBUG << "initial SaturationFunctor success , textureId = " <<mTextureId<< " fbo = " << mFBO;
    }

    void SaturationFunctor::unInitialize(FunctorContext *context) {
        // 释放纹理和fbo对象
        glDeleteTextures(1,&mTextureId);
        glDeleteFramebuffers(1,&mFBO);
    }

    bool SaturationFunctor::execute(FunctorContext *context) {
        Data inputData = mFunctorContext->popInput("VIDEO");
        GLFrame frame{};
        if (inputData.isEmpty() || !inputData.getData(frame)) {
            LOG_ERROR << "execute no input";
            return false;
        }

        RenderCore* renderCore = mGLContext.getRenderCore();
        unsigned int textureId = frame.textureId;

        int w = frame.width;
        int h = frame.height;

        mGLContext.runInRenderThread([this,renderCore,textureId,w,h]()->bool{
            if (mTextureId == 0) {
                mTextureId = renderCore->create2DTexture(w,h);
            }
            mProgram->use();
            mProgram->setFloat("saturation",mSaturation);
            renderCore->bindTextureInFrameBuffer(mFBO,mTextureId);
            renderCore->draw(GL_TEXTURE_2D,textureId,mProgram.get(),mFBO);
            return true;
        });

        auto* newFrame = new GLFrame(frame);
        newFrame->textureId = mTextureId;
        Data newData = Data::create(newFrame);
        mFunctorContext->setOutput(newData,"VIDEO");
        return true;
    }

    void SaturationFunctor::setOption(const std::string &key, Data value) {
        float saturationValue = 0;
        if (mProgram != nullptr && value.getData(saturationValue)) {
            mSaturation = saturationValue;
            return;
        }
        LOG_ERROR << "program = " << mProgram << ",contrastValue = " << saturationValue;
    }

    REGISTER_FUNCTOR(SaturationFunctor)

}
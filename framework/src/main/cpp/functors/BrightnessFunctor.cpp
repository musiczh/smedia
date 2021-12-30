//
// Created by wansu on 2021/12/15.
//

#include <GLFrame.h>
#include "BrightnessFunctor.h"
const static std::string fragmentShader = "#version 300 es\n"
                                          "out vec4 FragColor;\n"
                                          "// 顶点着色传进来的纹理坐标\n"
                                          "in vec2 otPos;\n"
                                          "uniform float brightness;\n"
                                          "\n"
                                          "uniform sampler2D tex;\n"
                                          "\n"
                                          "void main(){\n"
                                          "    // 传入坐标和采样器，获取颜色并返回\n"
                                          "    vec4 color = texture(tex,otPos);\n"
                                          "    FragColor = vec4(color.xyz+vec3(brightness),color.w);\n"
                                          "}";

namespace smedia{



    void BrightnessFunctor::onInit(InputHandler &inputHandler) {
        mBrightness = 0;
        mGLContext.runInRenderThread([this]()->bool {
            if (mProgram == nullptr) {
                mProgram = std::unique_ptr<Program>(mGLContext.getRenderCore()->createProgram(fragmentShader));
            }
            return true;
        });
        inputHandler.registerHandler("brightness",[this](InputData inputData)->bool{
            float value = 0;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "brightness get data fail";
                return false;
            }
            mBrightness = value;
            return true;
        });
    }

    void BrightnessFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {
        mProgram->use();
        mProgram->setFloat("brightness",mBrightness);
        mGLContext.getRenderCore()->draw(GL_TEXTURE_2D,frame.textureId,mProgram.get(),bufferFrame->getFBOId());
    }

    void BrightnessFunctor::unInitialize(FunctorContext *context) {

    }

    REGISTER_FUNCTOR(BrightnessFunctor)
}

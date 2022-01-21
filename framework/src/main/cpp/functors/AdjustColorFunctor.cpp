//
// Created by wansu on 2021/12/28.
//

#include "AdjustColorFunctor.h"
#include "FunctorContext.h"

namespace smedia {

    const static std::string fragmentShader =
            "#version 300 es\n"
            "out vec4 FragColor;\n"
            "// 顶点着色传进来的纹理坐标\n"
            "in vec2 otPos;\n"
            "uniform float brightness;\n"
            "uniform float contrast;\n"
            "uniform float saturation;\n"
            "\n"
            "uniform sampler2D tex;\n"
            "mat4 brightnessMatrix( float brightness )\n"
            "{\n"
            "    return mat4( 1, 0, 0, 0,\n"
            "                 0, 1, 0, 0,\n"
            "                 0, 0, 1, 0,\n"
            "                 brightness, brightness, brightness, 1 );\n"
            "}\n"
            "\n"
            "mat4 contrastMatrix( float contrast )\n"
            "{\n"
            "\tfloat t = ( 1.0 - contrast ) / 2.0;\n"
            "    \n"
            "    return mat4( contrast, 0, 0, 0,\n"
            "                 0, contrast, 0, 0,\n"
            "                 0, 0, contrast, 0,\n"
            "                 t, t, t, 1 );\n"
            "\n"
            "}\n"
            "\n"
            "mat4 saturationMatrix( float saturation )\n"
            "{\n"
            "    vec3 luminance = vec3( 0.3086, 0.6094, 0.0820 );\n"
            "    \n"
            "    float oneMinusSat = 1.0 - saturation;\n"
            "    \n"
            "    vec3 red = vec3( luminance.x * oneMinusSat );\n"
            "    red+= vec3( saturation, 0, 0 );\n"
            "    \n"
            "    vec3 green = vec3( luminance.y * oneMinusSat );\n"
            "    green += vec3( 0, saturation, 0 );\n"
            "    \n"
            "    vec3 blue = vec3( luminance.z * oneMinusSat );\n"
            "    blue += vec3( 0, 0, saturation );\n"
            "    \n"
            "    return mat4( red,     0,\n"
            "                 green,   0,\n"
            "                 blue,    0,\n"
            "                 0, 0, 0, 1 );\n"
            "}"
            "\n"
            "void main(){\n"
            "    // 传入坐标和采样器，获取颜色并返回\n"
            "    vec4 color = texture(tex,otPos);\n"
            "    FragColor = brightnessMatrix( brightness ) * contrastMatrix( contrast ) * \n"
            "        saturationMatrix( saturation ) * color;\n"
            "}";

    bool AdjustColorFunctor::onInit(InputHandler &inputHandler) {
        mContrast = 1;
        mBrightness = 0;
        mSaturation = 1;

        mInputHandler.registerHandler("contrast",[this](InputData inputData)->bool {
            float value = 0;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "AdjustColorFunctor set option getData fail";
                return false;
            }
            mContrast = value;
            return true;
        });
        mInputHandler.registerHandler("saturation",[this](InputData inputData)->bool {
            float value = 0;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "AdjustColorFunctor set option getData fail";
                return false;
            }
            mSaturation = value;
            return true;
        });
        mInputHandler.registerHandler("brightness",[this](InputData inputData)->bool {
            float value = 0;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "AdjustColorFunctor set option getData fail";
                return false;
            }
            mBrightness = value;
            return true;
        });
        LOG_DEBUG << "AdjustColorFunctor init finish";
        return true;
    }

    void AdjustColorFunctor::unInitialize(FunctorContext *context) {
        // unInitialize
    }


    bool AdjustColorFunctor::onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) {
        render->getProgram()->setTexture("tex",frame.glTextureRef);
        render->getProgram()->setFloat("brightness",mBrightness);
        render->getProgram()->setFloat("contrast",mContrast);
        render->getProgram()->setFloat("saturation",mSaturation);
        bufferFrame->bind();
        render->draw();
        GLTextureRef glTexture = bufferFrame->unBind();
        auto *newFrame = new GLFrame(frame);
        newFrame->glTextureRef = glTexture;
        mFunctorContext->setOutput(Data::create(newFrame),"video");
        return true;
    }

    std::string AdjustColorFunctor::getFragmentCode() {
        return fragmentShader;
    }

    REGISTER_FUNCTOR(AdjustColorFunctor)
}
//
// Created by wansu on 2021/12/29.
//

#include "SharpenFunctor.h"
namespace smedia {
    const static std::string fv = "#version 300 es\n"
                                  "out vec4 FragColor;\n"
                                  "in vec2 otPos;\n"
                                  "uniform float sharpen;\n"
                                  "uniform float mWidth;\n"
                                  "uniform float mHeight;\n"
                                  "uniform sampler2D tex;\n"
                                  "\n"
                                  "void main(){\n"
                                  "vec2 uv = otPos;\n"
                                  "\n"
                                  "//uv.x = mod( uv.x * 2.0, 1.0 );\n"
                                  "\n"
                                  "\n"
                                  "vec2 step = vec2(1.0/mWidth,1.0/mHeight);\n"
                                  "\n"
                                  "vec3 texA = texture( tex, uv + vec2(-step.x, -step.y) * 1.5 ).rgb;\n"
                                  "vec3 texB = texture( tex, uv + vec2( step.x, -step.y) * 1.5 ).rgb;\n"
                                  "vec3 texC = texture( tex, uv + vec2(-step.x,  step.y) * 1.5 ).rgb;\n"
                                  "vec3 texD = texture( tex, uv + vec2( step.x,  step.y) * 1.5 ).rgb;\n"
                                  "vec3 texE = texture( tex, uv + vec2( 0.0,  step.y) * 1.5 ).rgb;\n"
                                  "vec3 texF = texture( tex, uv + vec2( step.x,  0.0) * 1.5 ).rgb;\n"
                                  "vec3 texG = texture( tex, uv + vec2( 0.0,  -step.y) * 1.5 ).rgb;\n"
                                  "vec3 texH = texture( tex, uv + vec2( -step.x,  0.0) * 1.5 ).rgb;\n"
                                  "\n"
                                  "vec3 around = 0.125 * (texA + texB + texC + texD + texE + texF + texG + texH);\n"
                                  "\n"
                                  "vec3 center  = texture( tex, uv ).rgb;\n"
                                  "\n"
                                  "vec3 col = center + sharpen*(center - around);\n"
                                  "\n"
                                  "FragColor = vec4(col,1.0);"
                                  "}";

    bool SharpenFunctor::onInit(InputHandler &inputHandler) {
        mSharpen = 0;
        inputHandler.registerHandler("sharpen",INPUT_CALLBACK {
            float value;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "SharpenFunctor get sharpen fail";
                return false;
            }
            mSharpen = value;
            return true;
        });
        return true;
    }

    bool SharpenFunctor::onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) {
        render->getProgram()->setTexture("tex",frame.glTextureRef);
        render->getProgram()->setFloat("sharpen",mSharpen);
        render->getProgram()->setFloat("mWidth",frame.width);
        render->getProgram()->setFloat("mHeight",frame.height);
        bufferFrame->bind();
        render->draw();
        auto texture = bufferFrame->unBind();
        auto *newFrame = new GLFrame(frame);
        newFrame->glTextureRef = texture;
        mFunctorContext->setOutput(Data::create(newFrame),"video");
        return true;
    }

    void SharpenFunctor::unInitialize(FunctorContext *context) {
    }


    std::string SharpenFunctor::getFragmentCode() {
        return fv;
    }

    REGISTER_CLASS(SharpenFunctor)
}
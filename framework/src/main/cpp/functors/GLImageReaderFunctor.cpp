//
// Created by wansu on 2021/12/29.
//

#include "GLImageReaderFunctor.h"
#include "FunctorRegister.h"
#include "ImageFrame.h"
#include "FunctorRegister.h"
namespace smedia {

    const static std::string fv = CODE_STRING_SHADER(
        in vec2 otPos;
        out vec4 fragColor;
        uniform sampler2D tex;

        void main() {
            fragColor = texture(tex,otPos);
        }
    );

    void GLImageReaderFunctor::unInitialize(FunctorContext *context) {

    }

    std::string GLImageReaderFunctor::getFragmentCode() {
        return fv;
    }

    bool GLImageReaderFunctor::onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) {
        if (mRatio != -1) {
            // todo 处理图片比例问题，进行裁剪
        }
        bufferFrame->bind();
        render->getProgram()->setTexture("tex",frame.glTextureRef);
        render->draw();
        uint8_t * buffer = bufferFrame->readRGBAPixelData();
        bufferFrame->unBind();
        auto* imageFrame = new ImageFrame(frame.width, frame.height, buffer, FORMAT_RGBA);
        mFunctorContext->setOutput(Data::create(imageFrame),"buffer");
        return true;
    }

    bool GLImageReaderFunctor::onInit(InputHandler &inputHandler) {
        mEnableRGBA = true;
        mRatio = -1;
        inputHandler.registerHandler("enableRGBA",INPUT_CALLBACK{
            bool value;
            if (inputData.data.getData(value)) {
                mEnableRGBA = value;
                return true;
            }
            LOG_ERROR << "GLImageReaderFunctor get enableRGBA fail ";
            return false;
        });
        inputHandler.registerHandler("ratio",INPUT_CALLBACK{
            float value;
            if (inputData.data.getData(value)) {
                mRatio = value;
                return true;
            }
            LOG_ERROR << "GLImageReaderFunctor get ratio fail ";
            return false;
        });
        return true;
    }

    REGISTER_FUNCTOR(GLImageReaderFunctor)
}
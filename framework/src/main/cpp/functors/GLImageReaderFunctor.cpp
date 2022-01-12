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

    void GLImageReaderFunctor::onInit(InputHandler &inputHandler) {
        mEnableRGBA = true;
        mRatio = -1;
        // 不自动发生数据给下面节点
        ifSendFrame = false;
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
    }

    void GLImageReaderFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {
        if (mRatio != -1) {
            // todo 处理图片比例问题，进行裁剪
        }
        mGLContext.getRenderCore()->bindFrameBuffer(bufferFrame->getFBOId());
        mRenderProgram->use();
        mGLContext.getRenderCore()->draw(GL_TEXTURE_2D,frame.glTextureRef->textureId,mRenderProgram.get(),0);
        // RGBA一个通道一个字节
        uint8_t * buffer = mGLContext.getRenderCore()->getCurrentRGBAData(frame.width,frame.height);
        mGLContext.getRenderCore()->bindFrameBuffer(0);
        auto* imageFrame = new ImageFrame(frame.width,frame.height,buffer,RGBA);
        mFunctorContext->setOutput(Data::create(imageFrame),"Data");
    }

    void GLImageReaderFunctor::unInitialize(FunctorContext *context) {

    }

    std::string GLImageReaderFunctor::getFragmentCode() {
        return fv;
    }

    REGISTER_FUNCTOR(GLImageReaderFunctor)
}
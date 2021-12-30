//
// Created by wansu on 2021/12/29.
//

#include "GLImageReaderFunctor.h"
#include "FunctorRegister.h"
namespace smedia {

    const static std::string fv = CODE_STRING_SHADER(
        in vec2 otPos;
        out vec4 fragColor;
        uniform sampler2D tex;

        void main {
            fragColor = texture(tex,otPos);
        }
    );

    void GLImageReaderFunctor::onInit(InputHandler &inputHandler) {
        mEnableRGBA = true;
    }

    void GLImageReaderFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {

    }

    void GLImageReaderFunctor::unInitialize(FunctorContext *context) {

    }

    std::string GLImageReaderFunctor::getFragmentCode() {
        return fv;
    }
}
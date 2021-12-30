//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_GLIMAGEREADERFUNCTOR_H
#define SMEDIA_GLIMAGEREADERFUNCTOR_H

#include <Program.h>
#include "IFunctor.h"
#include "InputHandler.h"
#include "IGLRenderFunctor.h"
namespace smedia {
    class GLImageReaderFunctor : public IGLRenderFunctor{
    public:
        void onInit(InputHandler &inputHandler) override;

        void onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        std::string getFragmentCode() override;

        ~GLImageReaderFunctor() override = default;

    private:
        bool mEnableRGBA;
    };
}



#endif //SMEDIA_GLIMAGEREADERFUNCTOR_H

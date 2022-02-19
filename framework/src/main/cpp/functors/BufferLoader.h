//
// Created by wansu on 2022/2/18.
//

#ifndef SMEDIA_BUFFERLOADER_H
#define SMEDIA_BUFFERLOADER_H
#include "IFunctor.h"
#include "InputHandler.h"
#include "ImageFrame.h"
#include "GLTexture.h"
#include "GLFrame.h"
#include "ObjectRegister.h"
#include <memory>
namespace smedia {
    class BufferLoader : public IFunctor {
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~BufferLoader() override = default;

    private:
        FunctorContext* mFunctorContext;
        InputHandler mInputHandler;
    };



}



#endif //SMEDIA_BUFFERLOADER_H

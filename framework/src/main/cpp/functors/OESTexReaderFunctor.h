//
// Created by wansu on 2021/12/14.
//

#ifndef SMEDIA_OESTEXREADERFUNCTOR_H
#define SMEDIA_OESTEXREADERFUNCTOR_H
#include "ObjectRegister.h"
#include "IFunctor.h"
#include "Program.h"
#include "GLFrame.h"
#include "Render.h"
#include "GLTexture.h"
#include "GLBufferFrame.h"
#include <deque>
#include "JNIBridge.h"
#include <android/native_window_jni.h>

/**
 * 读取OES纹理，并渲染到2D纹理上
 */
namespace smedia {
    class OESTexReaderFunctor : public IFunctor {

    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context,const std::string &key, Data value) override;

    private:
        FunctorContext* mFunctorContext;
        std::deque<Data> mOptionQueue;
        std::mutex mQueueLock;
        GLContext* mGLContext;
        std::unique_ptr<Render> mRender;
        std::unique_ptr<GLBufferFrame> mGLBufferFrame;

    };
}



#endif //SMEDIA_OESTEXREADERFUNCTOR_H

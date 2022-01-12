//
// Created by wansu on 2021/12/14.
//

#ifndef SMEDIA_OESTEXREADERFUNCTOR_H
#define SMEDIA_OESTEXREADERFUNCTOR_H
#include "FunctorRegister.h"
#include "IFunctor.h"
#include "Program.h"
#include "GLFrame.h"
#include "GLHelper.h"
#include <deque>
#include "JNIBridge.h"
#include <android/native_window_jni.h>

/**
 * 读取OES纹理，并渲染到2D纹理上
 */
namespace smedia {
    class OESTexReaderFunctor : public IFunctor {

    public:
        void initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(const std::string &key, Data value) override;

    private:
        FunctorContext* mFunctorContext;
        std::deque<Data> mOptionQueue;

        std::mutex mQueueLock;

        int width;
        int height;

        GLContextRef mGLContext;
        int windowWidth;
        int windowHeight;

        unsigned int mTextureId{0};
        unsigned int mFBO{0};
        std::unique_ptr<Program> mProgram;

        volatile bool mInit;
    };
}



#endif //SMEDIA_OESTEXREADERFUNCTOR_H

//
// Created by wansu on 2022/2/18.
//

#include "BufferLoader.h"
namespace smedia {
    bool BufferLoader::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mInputHandler.registerHandler("buffer",[this](InputData inputData)->bool{
            auto& imageFrame = *inputData.data.getData<ImageFrame>();
            auto* glContext = mFunctorContext->getService<GLContext>("GLContext");
            GLTextureRef texture = GLTexture::Create(glContext,imageFrame.width,imageFrame.height,TEXTURE_TYPE_2D);
            texture->setPixelData(PIXEL_RGBA,imageFrame.pixelBuffer.get());
            auto *glFrame = new GLFrame;
            glFrame->width = imageFrame.width;
            glFrame->height = imageFrame.height;
            glFrame->glTextureRef = texture;
            glFrame->format = FORMAT_TEXTURE_2D;
            mFunctorContext->setOutput(Data::create(glFrame),"video");
            return true;
        });
        return true;
    }

    void BufferLoader::unInitialize(FunctorContext *context) {

    }

    bool BufferLoader::execute(FunctorContext *context) {
        return mInputHandler.runExecuteHandler(context);
    }

    void BufferLoader::setOption(FunctorContext *context, const std::string &key,
                                 Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }

    REGISTER_CLASS(BufferLoader)

}

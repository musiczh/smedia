//
// Created by wansu on 2021/12/31.
//

#include "ImageLoader.h"
#include "ImageFrame.h"
#include "FunctorRegister.h"
namespace smedia {

    void ImageLoader::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mInputHandler.registerHandler("Data",[this](InputData inputData) ->bool {
            ImageFrame* imageFramePtr;
            Data data = inputData.data;
            if (data.isTypeOf<ImageFrame>() && (imageFramePtr = data.getData<ImageFrame>())) {
                JNIObject object = readPixelBuffer(*imageFramePtr);
                if (object.getJObject() == nullptr) {
                    LOG_ERROR << "ImageLoader load pixel error";
                    return false;
                }
                Data res = Data::create(new JNIObject(object));
                mFunctorContext->setOutput(res,"Data");
                return true;
            }
            LOG_ERROR << "ImageLoader read input data error";
            return false;
        });
    }

    void ImageLoader::unInitialize(FunctorContext *context) {

    }

    bool ImageLoader::execute(FunctorContext *context) {
        return mInputHandler.runHandler(context);
    }

    void ImageLoader::setOption(const std::string &key, Data value) {
        mInputHandler.runHandler(mFunctorContext,key,value);
    }

    JNIObject ImageLoader::readPixelBuffer(ImageFrame &imageFrame) {
        JNIObject res;
        switch (imageFrame.format) {
            case RGBA:{
                res = readAsBitmap(imageFrame);
                break;
            }
            default:{
                LOG_ERROR << "not support format";
            }
        }
        return res;
    }

    JNIObject ImageLoader::readAsBitmap(ImageFrame &imageFrame) {
        int bufferLength = imageFrame.width * imageFrame.height * 4;
        jobject byteBufferObject = JNIService::getEnv()->NewDirectByteBuffer(
                imageFrame.pixelBuffer.get(),bufferLength);
        JNIObject bitmapObject = JNIInvoker<JNIObject,JNIObject,int,int>::InvokeStaticMethod("com/example/frameword/framework/JNIUtil",
                                                                                  "onNativeCreateBitmap",JNIObject(byteBufferObject),
                                                                                  imageFrame.width,imageFrame.height);
        JNIService::getEnv()->DeleteLocalRef(byteBufferObject);

        return bitmapObject;
    }

    REGISTER_FUNCTOR(ImageLoader)
}
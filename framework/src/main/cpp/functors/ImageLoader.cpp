//
// Created by wansu on 2021/12/31.
//

#include "ImageLoader.h"
#include "ImageFrame.h"
#include "FunctorRegister.h"
namespace smedia {

    bool ImageLoader::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mInputHandler.registerHandler("data",[this](InputData inputData) ->bool {
            ImageFrame* imageFramePtr;
            Data data = inputData.data;
            if (data.isTypeOf<ImageFrame>() && (imageFramePtr = data.getData<ImageFrame>())) {
                JNIObject object = readPixelBuffer(*imageFramePtr);
                if (object.getJObject() == nullptr) {
                    LOG_ERROR << "ImageLoader load pixel error";
                    return false;
                }
                Data res = Data::create(new JNIObject(object));
                mFunctorContext->setOutput(res,"data");
                return true;
            }
            LOG_ERROR << "ImageLoader read input data error";
            return false;
        });
        return true;
    }

    void ImageLoader::unInitialize(FunctorContext *context) {

    }

    bool ImageLoader::execute(FunctorContext *context) {
        return mInputHandler.runExecuteHandler(context);
    }


    JNIObject ImageLoader::readPixelBuffer(ImageFrame &imageFrame) {
        JNIObject res;
        switch (imageFrame.format) {
            case FORMAT_RGBA:{
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

    void ImageLoader::setOption(FunctorContext *context, const std::string &key, Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }

    REGISTER_FUNCTOR(ImageLoader)
}
//
// Created by wansu on 2022/1/6.
//

#include "ImageSourceFunctor.h"
#include "JNIBridge.h"
#include "android/bitmap.h"
#include "ImageFrame.h"
#include "GLFrame.h"
#include "FunctorRegister.h"
namespace smedia {

    void ImageSourceFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGLContext)) {
            LOG_ERROR << "glContext in functorContext is null";
            return;
        }
        mImageEnable = false;
        mGlEnable = false;
        mInputHandler.registerHandler("imageEnable",[this](InputData inputData)->bool {
            bool value = false;
            if (inputData.data.getData(value)) {
                mImageEnable = value;
                return true;
            }
            return false;
        });
        mInputHandler.registerHandler("glEnable",[this](InputData inputData)->bool {
            bool value = false;
            if (inputData.data.getData(value)) {
                mGlEnable = value;
                return true;
            }
            return false;
        });
        mInputHandler.registerHandler("Data",[this](InputData inputData_)->bool {
            inputData = inputData_.data;
            mFunctorContext->executeSelf();
            return true;
        });
    }

    void ImageSourceFunctor::unInitialize(FunctorContext *context) {

    }

    bool ImageSourceFunctor::execute(FunctorContext *context) {
        JNIObject object;
        if (!inputData.getData(object)) {
            LOG_ERROR << "ImageSourceFunctor get data error";
            return false;
        }
        jobject bitmapObject = object.getJObject();
        AndroidBitmapInfo info;
        int status = AndroidBitmap_getInfo(JNIService::getEnv(),bitmapObject,&info);
        if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOG_ERROR << "get bitmap info error";
            return false;
        }
        // 仅支持RGBA的bitmap
        if (info.format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOG_ERROR << "bitmap format is not rgba888";
            return false;
        }

        unsigned char *bitmapBuffer;
        status = AndroidBitmap_lockPixels(JNIService::getEnv(),
                                          bitmapObject,
                                          reinterpret_cast<void **>(&bitmapBuffer));
        if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOG_ERROR << "lock pixel error";
            return false;
        }
        size_t bitmapSize = info.width * info.height * 4;
        auto* pixelBuffer = new unsigned char[bitmapSize];
        memcpy(pixelBuffer, bitmapBuffer, bitmapSize);

        status = AndroidBitmap_unlockPixels(JNIService::getEnv(),
                                            bitmapObject);
        if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOG_ERROR << "unlock pixel error";
            return false;
        }
        if (mGlEnable) {
            // 转化为GLFrame
            unsigned int textureId = 0;
            mGLContext.runInRenderThread([&textureId,this,info,pixelBuffer]()->bool {
                textureId = mGLContext.getRenderCore()
                        ->create2DTexture(info.width, info.height, pixelBuffer);
                return true;
            });

            auto *glFrame = new GLFrame;
            glFrame->width = info.width;
            glFrame->height = info.height;
            glFrame->textureId = textureId;
            glFrame->format = TEXTURE_2D;
            mFunctorContext->setOutput(Data::create(glFrame),"video");
        }
        if (mImageEnable) {
            // 转化为ImageFrame
            auto* frame = new ImageFrame(info.width, info.height, pixelBuffer, RGBA);
            mFunctorContext->setOutput(Data::create(frame),"image");
        }
        return true;
    }

    void ImageSourceFunctor::setOption(const std::string &key, Data value) {
        mInputHandler.runHandler(mFunctorContext,key,value);
    }

    REGISTER_FUNCTOR(ImageSourceFunctor)
}
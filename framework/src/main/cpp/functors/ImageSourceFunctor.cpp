//
// Created by wansu on 2022/1/6.
//

#include "ImageSourceFunctor.h"
#include "JNIBridge.h"
#include "android/bitmap.h"
#include "ImageFrame.h"
#include "GLFrame.h"
#include "ObjectRegister.h"

namespace smedia {

    bool ImageSourceFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        if ((mGLContext = mFunctorContext->getService<GLContext>("GLContext")) == nullptr) {
            LOG_ERROR << "mGLContext in functorContext is null";
            return false;
        }
        // 默认都不输出
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
        mInputHandler.registerHandler("data",[this](InputData inputData_)->bool {
            inputData = inputData_.data;
            mFunctorContext->executeSelf();
            return true;
        });
        return true;
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
        // 使用智能指针，防止忘记delete导致泄露
        auto pixelBuffer = std::unique_ptr<unsigned char>(new unsigned char[bitmapSize]);
        memcpy(pixelBuffer.get(), bitmapBuffer, bitmapSize);

        status = AndroidBitmap_unlockPixels(JNIService::getEnv(),
                                            bitmapObject);
        if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOG_ERROR << "unlock pixel error";
            return false;
        }
        // 按照配置进行输出
        if (mGlEnable) {
            // 转化为GLFrame
            auto* pb = pixelBuffer.get();
            GLTextureRef texture = GLTexture::Create(mGLContext,info.width,info.height,TEXTURE_TYPE_2D);
            texture->setPixelData(PIXEL_RGBA,pb);

            auto *glFrame = new GLFrame;
            glFrame->width = info.width;
            glFrame->height = info.height;
            glFrame->glTextureRef = texture;
            glFrame->format = FORMAT_TEXTURE_2D;
            mFunctorContext->setOutput(Data::create(glFrame),"video");
        }
        if (mImageEnable) {
            // 转化为ImageFrame
            auto* frame = new ImageFrame(info.width, info.height, pixelBuffer.release(), FORMAT_RGBA);
            mFunctorContext->setOutput(Data::create(frame),"buffer");
        }
        // 如果没有输出不需要调度后面的节点
        if (mGlEnable || mImageEnable) {
            return true;
        }
        return false;
    }

    void
    ImageSourceFunctor::setOption(FunctorContext *context, const std::string &key, Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }

    REGISTER_CLASS(ImageSourceFunctor)
}
#include <jni.h>
#include <android/bitmap.h>
#include "FunctorContext.h"
#include "JNIBridge.h"
#include "JNIObject.h"
#include "ImageFrame.h"
#include "GLFrame.h"
//
// Created by wansu on 2022/2/15.
//
using namespace smedia;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_FunctorContext_nativeRunInGLThread(JNIEnv *env, jobject thiz,
                                                                        jlong native_handle,
                                                                        jobject task) {
    auto* context = reinterpret_cast<FunctorContext*>(native_handle);
    JNIObject runTask = JNIObject(task);
    auto* glContext = context->getService<GLContext>("GLContext");
    if (glContext != nullptr) {
        glContext->runInRenderThreadV([runTask](){
            JNIInvoker<void>::InvokeObjectMethod(runTask.getJObject(),"run");
        });
    }

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_frameword_framework_FunctorContext_nativeGetInputGLFrame(JNIEnv *env, jobject thiz,
                                                                          jlong native_handle,
                                                                          jstring tag, jint index) {
    auto* context = reinterpret_cast<FunctorContext*>(native_handle);
    std::string inputTag = JNIDataUtil<std::string>::convertToC(tag);
    Data data = context->getInput(inputTag,index);
    if (data.getTypeId() != typeid(GLFrame).hash_code()) {
        return nullptr;
    }
    data = context->popInput(inputTag,index);
    GLFrame& frame = *data.getData<GLFrame>();
    // todo 这里不能自动释放和回收纹理，交给java层去处理
    frame.glTextureRef->setAutoOption(false, false);
    JNIObjectRef jniObject = JNICreateInstance<int,int,int>(
            "com/example/frameword/framework/NativeGLFrame",frame.width,frame.height,(int)frame.glTextureRef->getTextureId());
    JNIInvoker<void,int>::InvokeObjectMethod(jniObject->getJObject(),"setOrientation",frame.orientation);
    JNIInvoker<void,bool>::InvokeObjectMethod(jniObject->getJObject(),"setMirrorY",frame.isMirrorY);
    JNIInvoker<void,double>::InvokeObjectMethod(jniObject->getJObject(),"setTimeStamp",frame.timeStamp);
    std::vector<float> v(16);
    memcpy(v.data(),frame.UVMatrix,16*sizeof(float));
    JNIInvoker<void,std::vector<float>>::InvokeObjectMethod(jniObject->getJObject(),"setMatrix",v);
    return jniObject->obtainNewLocalRef();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_FunctorContext_nativeExecuteSelf(JNIEnv *env, jobject thiz,
                                                                      jlong native_handle) {
    auto* context = reinterpret_cast<FunctorContext*>(native_handle);
    context->executeSelf();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_FunctorContext_nativeSetOutputGLFrame(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jlong native_handle,
                                                                           jobject gl_frame,
                                                                           jstring tag,
                                                                           jint index) {
    Data data = JNIData::jObjectToData(gl_frame);
    if (data.isEmpty()) {
        LOG_ERROR << "nativeSetOutputGLFrame cast GLFrame fail";
        return;
    }
    auto *frame = data.getData<GLFrame>();
    if (JNIInvoker<bool>::GetObjectFiled(gl_frame,"isOES")) {
        frame->format = FORMAT_TEXTURE_OES;
    } else {
        frame->format = FORMAT_TEXTURE_2D;
    }
    auto* context = reinterpret_cast<FunctorContext*>(native_handle);
    context->setOutput(data,JNIDataUtil<std::string>::convertToC(tag),index);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_FunctorContext_nativeSetOutputBitmap(JNIEnv *env,
                                                                          jobject thiz,
                                                                          jlong native_handle,
                                                                          jobject bitmapObject,
                                                                          jstring tag,
                                                                          jint index) {
    auto* context = reinterpret_cast<FunctorContext*>(native_handle);
    AndroidBitmapInfo info;
    int status = AndroidBitmap_getInfo(JNIService::getEnv(),bitmapObject,&info);
    if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_ERROR << "get bitmap info error";
        return;
    }
    // 仅支持RGBA的bitmap
    if (info.format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOG_ERROR << "bitmap format is not rgba888";
        return;
    }

    unsigned char *bitmapBuffer;
    status = AndroidBitmap_lockPixels(JNIService::getEnv(),
                                      bitmapObject,
                                      reinterpret_cast<void **>(&bitmapBuffer));
    if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_ERROR << "lock pixel error";
        return;
    }
    size_t bitmapSize = info.width * info.height * 4;
    // 使用智能指针，防止忘记delete导致泄露
    auto pixelBuffer = std::unique_ptr<unsigned char>(new unsigned char[bitmapSize]);
    memcpy(pixelBuffer.get(), bitmapBuffer, bitmapSize);

    status = AndroidBitmap_unlockPixels(JNIService::getEnv(),
                                        bitmapObject);
    if (status != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_ERROR << "unlock pixel error";
        return ;
    }

    // 转化为ImageFrame
    auto* frame = new ImageFrame(info.width, info.height, pixelBuffer.release(), FORMAT_RGBA);
    context->setOutput(Data::create(frame),JNIDataUtil<std::string>::convertToC(tag),index);
}
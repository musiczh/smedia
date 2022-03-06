#include <jni.h>
#include "android/native_window_jni.h"
#include "JNIBridge.h"
#include "FunctorContext.h"
#include "GLContext.h"
#include "Render.h"
#include "GLTexture.h"
//
// Created by wansu on 2022/2/16.
//
using namespace smedia;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_frameword_framework_RenderCore_nativeInitRender(JNIEnv *env, jobject thiz,
                                                                 jlong native_handle,
                                                                 jstring vertex_code,
                                                                 jstring fragment_code) {
    auto* functorContext = reinterpret_cast<FunctorContext*>(native_handle);
    auto* glContext = functorContext->getService<GLContext>("GLContext");
    auto render = Render::CreateWithShaderCode(glContext,
                                               JNIDataUtil<std::string>::convertToC(fragment_code));
    return reinterpret_cast<long>(render.release());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeRender(JNIEnv *env, jobject thiz,
                                                             jlong native_render_handle) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    render->draw();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSetTexture(JNIEnv *env, jobject thiz,
                                                                 jlong native_render_handle,
                                                                 jlong nativeFunctorContextHandle,
                                                                 jstring name, jint texture_id) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    auto* glContext = reinterpret_cast<FunctorContext*>(nativeFunctorContextHandle)->getService<GLContext>("GLContext");
    auto glTexture = GLTexture::Create(glContext,0,0,TEXTURE_TYPE_2D,texture_id);
    glTexture->setAutoOption(false, false);
    render->getProgram()->setTexture(JNIDataUtil<std::string>::convertToC(name),glTexture);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSetMat4(JNIEnv *env, jobject thiz,
                                                              jlong native_render_handle,
                                                              jstring name, jfloatArray matrix) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    std::vector<float> v = JNIDataUtil<std::vector<float>>::convertToC(matrix);
    auto *res = new float [16]{0};
    memcpy(res,v.data(),v.size()*sizeof (float ));
    render->getProgram()->setMat4(JNIDataUtil<std::string>::convertToC(name),res);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSetInt(JNIEnv *env, jobject thiz,
                                                             jlong native_render_handle,
                                                             jstring name, jint value) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    render->getProgram()->setInt(JNIDataUtil<std::string>::convertToC(name),value);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSetWindowSurface(JNIEnv *env, jclass clazz,
                                                                       jlong native_handle,
                                                                       jobject surface, jint width,
                                                                       jint height) {
    auto* functorContext = reinterpret_cast<FunctorContext*>(native_handle);
    auto* glContext = functorContext->getService<GLContext>("GLContext");
    EGLNativeWindowType nativeWindow = ANativeWindow_fromSurface(env,surface);
    glContext->runInRenderThreadV([glContext,nativeWindow](){
        glContext->getEglCore()->makeCurrentWindowContext(nativeWindow);
    });
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSetViewPort(JNIEnv *env, jobject thiz,
                                                                  jlong native_render_handle,
                                                                  jintArray view_port) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    render->setViewPort(JNIDataUtil<std::vector<int>>::convertToC(view_port).data());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeReleaseRender(JNIEnv *env, jobject thiz,
                                                                    jlong native_render_handle) {
    auto* render = reinterpret_cast<Render*>(native_render_handle);
    delete render;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeSwapEGLBuffer(JNIEnv *env, jobject thiz,
                                                                    jlong native_functor_context) {
    auto* context = reinterpret_cast<FunctorContext*>(native_functor_context);
    context->getService<GLContext>("GLContext")->getEglCore()->swapBuffer();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_RenderCore_nativeReleaseTexture(JNIEnv *env, jobject thiz,
                                                                     jlong native_functor_context_handle,
                                                                     jint width, jint height,
                                                                     jint texture_id,
                                                                     jboolean reuse) {
    auto* context = reinterpret_cast<FunctorContext*>(native_functor_context_handle);
    auto* glContext = context->getService<GLContext>("GLContext");
    auto glTexture = GLTexture::Create(glContext,width,height,TEXTURE_TYPE_2D,texture_id);
    glTexture->setAutoOption(true,JNIDataUtil<bool>::convertToC(reuse));
}
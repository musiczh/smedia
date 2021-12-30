#include <jni.h>
#include <android/native_window_jni.h>
#include "EGLCore.h"
#include "Logger.h"
#include "RenderCore.h"


//
// Created by wansu on 2021/12/6.
//
std::string fragmentShader_ = "#version 300 es\n"
                             "#extension GL_OES_EGL_image_external_essl3 : require\n"
                             "precision mediump float;\n"
                             "out vec4 FragColor;\n"
                             "// 顶点着色传进来的纹理坐标\n"
                             "in vec2 otPos;\n"
                             "\n"
                             "// 采样器，外部可以把纹理赋值给此\n"
                             "uniform samplerExternalOES boxTexture;\n"
                             "\n"
                             "void main(){\n"
                             "    // 传入坐标和采样器，获取颜色并返回\n"
                             "    FragColor = texture(boxTexture,otPos);\n"
                             "}";

JNIEnv* jniEnv;
void logInJava(LoggerLevel level,const char * s){
    jclass claz = jniEnv->FindClass("com/example/egldemo/util/LogUtil");
    jmethodID jmethod = jniEnv->GetStaticMethodID(
            claz,"Log","(Ljava/lang/String;Ljava/lang/String;I)V");
    jstring tag = jniEnv->NewStringUTF("[native] huan");
    jstring msg = jniEnv->NewStringUTF(s);
    int level_ = 0;
    switch (level) {

        case ERROR:level_ = 3;
            break;
        case WARN:level_ = 2;
            break;
        case INFO:level_ = 1;
            break;
        case DEBUG:level_ = 0;
            break;
    }
    jniEnv->CallStaticVoidMethod(claz,jmethod,tag,msg,level_);

    jniEnv->DeleteLocalRef(tag);
    jniEnv->DeleteLocalRef(msg);
    jniEnv->DeleteLocalRef(claz);
}



extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeInitEGL(JNIEnv *env, jobject thiz) {
    jniEnv = env;
    Logger::setLogFunction(&logInJava);
    LOG_DEBUG << "init logger finish";
    auto *eglCore = new EGLCore();
    eglCore->initEGL(nullptr);
    return reinterpret_cast<int64_t>(eglCore);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeInitRender(JNIEnv *env, jobject thiz) {
    auto* render = new RenderCore();
    render->initGLES();
    return reinterpret_cast<int64_t>(render);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeCreateOESTexture(JNIEnv *env, jobject thiz,
                                                                  jlong native_render_handle) {
    auto* render = reinterpret_cast<RenderCore*>(native_render_handle);
    auto textureId = render->createOESTexture();
    return textureId;
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeCreateWindowSurface(JNIEnv *env, jobject thiz,
                                                                     jlong native_egl_handle,jobject surface) {
    auto* eglCore = reinterpret_cast<EGLCore*>(native_egl_handle);
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);
    EGLSurface eglSurface = eglCore->createWindowSurface(nativeWindow);
    return reinterpret_cast<int64_t>(eglSurface);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeMakeCurrentSurface(JNIEnv *env, jobject thiz,
                                                                    jlong native_egl_handle,
                                                                    jlong egl_surface) {
    auto* eglCore = reinterpret_cast<EGLCore*>(native_egl_handle);
    auto eglSurface = reinterpret_cast<EGLSurface>(egl_surface);
    eglCore->makeCurrentContext(eglSurface);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeCreateProgram(JNIEnv *env, jobject thiz,
                                                               jlong native_render_handle) {
    auto* render = reinterpret_cast<RenderCore*>(native_render_handle);
    auto* program = render->createProgram(fragmentShader_);
    return reinterpret_cast<int64_t>(program);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_egldemo_gles_NativeRender_nativeDraw(JNIEnv *env, jobject thiz,
                                                      jlong native_egl_core,
                                                      jlong native_render_core, jint texture_id,
                                                      jlong program) {
    auto* render = reinterpret_cast<RenderCore*>(native_render_core);
    auto* eglCore = reinterpret_cast<EGLCore*>(native_egl_core);
    auto* program_ = reinterpret_cast<Program*>(program);

    render->draw(GL_TEXTURE_EXTERNAL_OES,texture_id,program_);
    eglCore->swapBuffer();
}
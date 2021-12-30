//
// Created by wansu on 2021/12/14.
//

#include "OESTexReaderFunctor.h"
namespace smedia {

    const static std::string fragmentShader = "#version 300 es\n"
                                              "#extension GL_OES_EGL_image_external_essl3 : require\n"
                                 "out vec4 FragColor;\n"
                                 "// 顶点着色传进来的纹理坐标\n"
                                 "in vec2 otPos;\n"
                                 "\n"
                                 "// 采样器，外部可以把纹理赋值给此\n"
                                 "uniform samplerExternalOES boxTexture;\n"
                                 "\n"
                                 "void main(){\n"
                                 "    // 传入坐标和采样器，获取颜色并返回\n"
                                 "    FragColor = texture(boxTexture,vec2(otPos.xy));\n"
                                 "}";


    void OESTexReaderFunctor::initialize(smedia::FunctorContext *context) {
        if (mInit) {
            LOG_DEBUG << "OESTexReaderFunctor has init";
            return;
        }
        mFunctorContext = context;
        if (!mFunctorContext->getGlobalService("GLContext").getData(mGlContext)) {
            LOG_ERROR << "glContext in functorContext is null";
            return;
        }
        RenderCore* renderCore = mGlContext.getRenderCore();
        mGlContext.runInRenderThread([this,renderCore]()->bool{
            mProgram = std::unique_ptr<Program>(renderCore->createProgram(fragmentShader));
            mFBO = renderCore->createFrameBuffer();
            return true;
        });
        LOG_DEBUG << "initial OESTexReaderFunctor success , textureId = " <<mTextureId << " fbo = " << mFBO;
        mInit = true;
    }

    void OESTexReaderFunctor::unInitialize(smedia::FunctorContext *context) {
        // 释放纹理和fbo对象
        glDeleteTextures(1,&mTextureId);
        glDeleteFramebuffers(1,&mFBO);
    }

    bool OESTexReaderFunctor::execute(smedia::FunctorContext *context) {
        if (!mInit) {
            LOG_ERROR << "OESTexReaderFunctor has not init";
            return false;
        }

        Data data;
        GLFrame frame{};
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            if (!mOptionQueue.empty()) {
                data = mOptionQueue.front();
                mOptionQueue.pop_front();
            }
        }
        if (data.isEmpty() || !data.getData(frame)) {
            LOG_ERROR << "getData empty";
            return false;
        }

        mGlContext.runInRenderThread([this,frame]()->bool{
            if (mTextureId == 0) {
                mTextureId = mGlContext.getRenderCore()->create2DTexture(frame.width,frame.height);
            }
            mGlContext.getRenderCore()->bindTextureInFrameBuffer(mFBO,mTextureId);
            glViewport(0,0,frame.width,frame.height);
            mProgram->use();
            mGlContext.getRenderCore()->draw(GL_TEXTURE_EXTERNAL_OES,frame.textureId,mProgram.get(),mFBO);
            return true;
        });

        auto* newFrame = new GLFrame(frame);
        newFrame->textureId = mTextureId;
        Data newData = Data::create(newFrame);
        mFunctorContext->setOutput(newData,"VIDEO");
        return true;
    }


    void OESTexReaderFunctor::setOption(const std::string &key, smedia::Data value) {
        if (key == "DATA") {
            GLContext glContext;
            Data contextData = mFunctorContext->getGlobalService("GLContext");
            if (contextData.isEmpty() || !contextData.getData(glContext)) {
                LOG_ERROR << "glContext is null";
                return;
            }

            JNIObject object;
            if (!value.getData(object)) {
                LOG_ERROR << "get data fail";
                return;
            }

            auto* glFrame = new GLFrame{};
            jclass clazz = JClassManager::getJavaClass("com/example/frameword/framework/NativeGLFrame");
            glFrame->width = JNICaller<int>::getObjectField(object.getJObject(),JNISignature::getObjectFiledId<int>(clazz,"width"));
            glFrame->height = JNICaller<int>::getObjectField(object.getJObject(),JNISignature::getObjectFiledId<int>(clazz,"height"));
            glFrame->textureId = JNICaller<int>::getObjectField(object.getJObject(),JNISignature::getObjectFiledId<int>(clazz,"textureId"));
            JNIObject object1 = JNICaller<JNIObject>::callObjectMethod(object.getJObject(),JNISignature::getMethodId<JNIObject>(clazz,"onNativeGetUVMatrix"));
            glFrame->orientation = JNICaller<int>::getObjectField(object.getJObject(),JNISignature::getObjectFiledId<int>(clazz,"orientation"));
            std::vector<float> v = JNIData::jFloatArrayToVector(reinterpret_cast<jfloatArray>(object1.getJObject()));
            for (int i=0;i<16;i++) {
                glFrame->UVMatrix[i] = v[i];
            }
            std::unique_lock<std::mutex> lock(mQueueLock);
            mOptionQueue.push_back(Data::create(glFrame));
            mFunctorContext->executeSelf();
        }
    }

    REGISTER_FUNCTOR(OESTexReaderFunctor)
}


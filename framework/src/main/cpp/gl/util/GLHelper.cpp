//
// Created by wansu on 2021/12/16.
//

#include "GLHelper.h"
namespace smedia {
    const std::string EGL_SHARED_CONTEXT = "EGLSharedContext";
    const std::string GL_CONTEXT = "GLContext";
    const std::string NATIVE_WINDOW = "nativeWindow";

    bool getGLContext(FunctorContext* functorContext,GLContext& result){
        Data contextData = functorContext->getGlobalService(GL_CONTEXT);
        if (!contextData.isEmpty() && contextData.getData(result)) {
           return true;
        }
        return false;
    }


}
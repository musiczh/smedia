//
// Created by wansu on 2022/1/11.
//

#include "GLTexture.h"
#include "GLContext.h"
namespace smedia {

    GLTexture::GLTexture(GLContext* glContext,unsigned int textureId, int width, int height)
    :glContext(glContext),textureId(textureId),width(width),height(height) {
        // 构建GLTexture
    }

    GLTexture::~GLTexture() {
        glContext->runInRenderThread([this]()->bool{
            glContext->getRenderCore()->deleteTexture(textureId);
            LOG_ERROR << "delete 纹理" << textureId;
            return true;
        });
    }


}
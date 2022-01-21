//
// Created by wansu on 2022/1/12.
//

#include "GLTexturePool.h"
#include "GLContext.h"
#include <sstream>
namespace smedia {

    GLTexturePool::GLTexturePool(GLContextRef glContext):mGLContext(glContext) {
        LOG_DEBUG << "create glTexturePool";
    }

    std::string GLTexturePool::hashTexture(int width, int height, int textureType) {
        std::stringstream stringStream;
        stringStream << width << "x" << height << ":" << textureType;
        return stringStream.str();
    }

    std::shared_ptr<Texture> GLTexturePool::obtainTexture(int width, int height,
                                                          TextureType type, int textureId) {
        if (textureId != 0) {
            // 如果纹理已经存在，直接构建一个对象并返回
            return std::make_shared<Texture>(mGLContext, width, height, type, textureId);
        }

        if (width < 0 || height < 0) {
            LOG_ERROR << "obtain2DTexture params illegal,mWidth=" << width << " mHeight=" << height;
            return std::shared_ptr<Texture>(nullptr);
        }
        std::string key = hashTexture(width, height, type);
        // 查找map中是否有缓存，有的话则弹出并返回
        // 使用iterator减少拷贝
        auto ptr = mTexMap.find(key);
        if (ptr != mTexMap.end()) {
            if (!ptr->second.empty()) {
                auto res = ptr->second.back();
                ptr->second.pop_back();
                return res;
            }
        }

        // 若纹理池不存在则新建一个新的纹理并返回
        return std::make_shared<Texture>(mGLContext, width, height, type);
    }

    void GLTexturePool::recycleTexture(std::shared_ptr<Texture> glTextureRef) {
        if (glTextureRef == nullptr) {
            LOG_ERROR << "can not recycle null texture";
            return;
        }
        auto key = hashTexture(glTextureRef->mWidth, glTextureRef->mHeight,
                               glTextureRef->mTextureType);
        if (mTexMap.find(key) == mTexMap.end()) {
            std::vector<TextureRef> newVector{};
            mTexMap[key] = newVector;
        }

        auto ptr = mTexMap.find(key);
        if (ptr->second.size() < 3) {
            ptr->second.push_back(std::move(glTextureRef));
        }
    }

    void GLTexturePool::release() {
        for (auto& item : mTexMap) {
            item.second.clear();
        }
        mTexMap.clear();
    }

    GLTexturePool::~GLTexturePool() {
        LOG_DEBUG << "destroy glTexturePool";
    }
}
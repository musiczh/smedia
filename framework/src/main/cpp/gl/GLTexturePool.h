//
// Created by wansu on 2022/1/12.
//

#ifndef SMEDIA_GLTEXTUREPOOL_H
#define SMEDIA_GLTEXTUREPOOL_H
#include <memory>
#include <map>
#include <vector>
#include "Texture.h"
/**
 * 纹理池，进行纹理复用。
 * 设计纹理池的主要作用是：避免频繁创建、释放纹理，导致内存抖动，响应慢；
 * 同时可以控制纹理内存的上限，避免内存占用过高。仅支持2D纹理
 * todo 暂时把实例放在glContext中，如果作为单例存在也许GLContext去进行释放，不如直接放在GLContext中，后续看看有没有更好的设计方式
 */
namespace smedia {
    class GLContext;

    class GLTexturePool {
    public:
        /**
         * 需要传入GLContext对象，因为所有纹理相关的操作，都必须有egl上下文
         * @param glContext
         */
        GLTexturePool(GLContextRef glContext);
        ~GLTexturePool();

        /**
         * 根据尺寸获取纹理对象
         * @param width 纹理宽度
         * @param height 纹理长度
         * @param type 纹理类型
         * @param textureId 是否使用已存在纹理构建
         * @return 返回纹理对象指针
         */
        std::shared_ptr<Texture> obtainTexture(int width, int height, TextureType type, int textureId = 0);

        /**
         * 回收纹理
         * @param glTextureRef 纹理对象
         */
        void recycleTexture(std::shared_ptr<Texture> glTextureRef);

        /**
         * 释放纹理池中所有的纹理
         */
        void release();

    private:
        static std::string hashTexture(int width, int height, int textureType);

    private:
        GLContextRef mGLContext;
        std::map<std::string,std::vector<std::shared_ptr<Texture>>> mTexMap;
    };
}



#endif //SMEDIA_GLTEXTUREPOOL_H

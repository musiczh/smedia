//
// Created by wansu on 2021/12/16.
//

#ifndef SMEDIA_GLHELPER_H
#define SMEDIA_GLHELPER_H
#include <string>
/**
 * 统一GL的静态辅助类方法，供全局调用
 */
namespace smedia {
    extern const std::string EGL_SHARED_CONTEXT;
    extern const std::string GL_CONTEXT;
    extern const std::string NATIVE_WINDOW ;


    // 检查gl和egl操作是否出现问题
#ifndef CHECK_GL_ERROR
#define CHECK_GL_ERROR(operation) \
    checkEGLError(#operation);\
    checkGLError(#operation);
#endif

    // 自动检查代码是否出现问题
#ifndef CHECK_GL_CODE
#define CHECK_GL_CODE(code) \
    code                    \
    CHECK_GL_ERROR(code)
#endif

// 这里主要是加上了分号，CHECK_GL_CODE每次都需要加分号比较麻烦
// 如果只是一行代码，使用GL_CODE更加方便
#ifndef GL_CODE
    #define GL_CODE(code) \
    code;                    \
    CHECK_GL_ERROR(code)
#endif

    bool checkEGLError(const std::string &operation);
    bool checkGLError(const std::string &operation);

    unsigned int Create2DGLTexture(int width, int height, const unsigned char* pixel = nullptr);
    unsigned int CreateOESTexture(int width, int height, const unsigned char* pixel = nullptr);
    void DeleteGLTexture(unsigned int textureId);
    inline void DeleteGLFBO(unsigned int fboId);
    inline unsigned int CreateFrameBuffer();
    inline void BindTextureInFrameBuffer(unsigned int frameBuffer, unsigned int textureId);
    inline void BindFrameBuffer(unsigned int fboId);
}



#endif //SMEDIA_GLHELPER_H

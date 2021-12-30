//
// Created by wansu on 2021/12/10.
//

#include "RenderCore.h"
namespace smedia {

    const static float vertices[] = {
            1.0f, -1.0f, 0, 1.0f, 0,// 右下角
            -1.0f, -1.0f, 0, 0, 0,  // 左下角
            1.0f, 1.0f, 0, 1.0f, 1.0f,// 右上角
            -1.0f, 1.0f, 0, 0, 1.0f// 左上角
    };
    const static std::string fv = "#version 300 es\n"
                                  "out vec4 FragColor;\n"
                                  "in vec2 otPos;\n"
                                  "uniform float sharpen;\n"
                                  "uniform sampler2D tex;\n"
                                  "\n"
                                  "void main(){\n"
                                  "    vec4 color = texture(tex,otPos);\n"
                                  "    FragColor = vec4(color.xyz+vec3(brightness),color.w);\n"
                                  "}";

    const static std::string __vertexShaderSource = "#version 300 es\n"
                                           "layout (location=0) in vec3 aPos;\n"
                                           "layout (location=1) in vec2 tPos;\n"

                                           "\n"
                                           "out vec2 otPos;\n"
                                           "void main(){\n"
                                           "    gl_Position = vec4(aPos,1.0);\n"
                                           "    otPos = tPos;\n"
                                           "}";

    RenderCore::RenderCore() {
        LOG_INFO << "create render core";
    }

    Program *RenderCore::createProgram(const std::string &fragmentShaderSource) {
        auto *program = new Program;
        program->addShader(GL_VERTEX_SHADER, __vertexShaderSource);
        program->addShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        if (!program->link()) {
            return nullptr;
        }
        program->use();
        return program;
    }

    bool RenderCore::initGLES() {
        // 初始化主要是把添加顶点数据，视屏渲染都是矩形，所以这里对顶点数据进行固定
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 这里需要对纹理坐标和顶点坐标进行解析
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        LOG_DEBUG << "init program success";
        return true;
    }


    unsigned int RenderCore::createOESTexture() {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        if (checkGLError("createOESTexture")) {
            return -1;
        }
        LOG_DEBUG << "create texture success";
        return texture;
    }

    unsigned int RenderCore::create2DTexture() {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (checkGLError("create2DTexture")) {
            return -1;
        }
        LOG_DEBUG << "create texture success,id = " << texture;
        return texture;
    }

    unsigned int RenderCore::create2DTexture(int w,int h) {
        LOG_DEBUG<< "w=" << w << " h=" << h;
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (checkGLError("create2DTexture")) {
            return -1;
        }
        LOG_DEBUG << "create texture success,id = " << texture;
        return texture;
    }

    void RenderCore::draw(int textureType, unsigned int textureId, Program *program,unsigned int fbo) {
        if (fbo != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER,fbo);
        }
        if (program == nullptr) {
            LOG_ERROR << "program is null onDraw";
            return;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.7, 0.8, 0.6, 1.0);
        program->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(textureType, textureId);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(textureType,0);
        if (fbo != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER,0);
        }
        checkGLError("draw finish");
        checkEGLError("draw finish egl");
    }

    unsigned int RenderCore::createFrameBuffer() {
        unsigned int frameBufferId;
        glGenFramebuffers(1,&frameBufferId);
        checkGLError("createFrameBuffer");
        return frameBufferId;
    }

    void RenderCore::bindTextureInFrameBuffer(unsigned int frameBuffer, unsigned int textureId) {
        checkGLError("bind fbo 111");
        glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
        checkGLError("bind fbo ");
        glBindTexture(GL_TEXTURE_2D,textureId);
        checkGLError("bind 2DTexture");
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureId,0);
        checkGLError("bind texture to fbo");
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glBindTexture(GL_TEXTURE_2D,0);
    }

    void RenderCore::deleteTexture(unsigned int textureId) {
        GLuint array[] = {textureId};
        glDeleteTextures(1,array);
    }

    void RenderCore::deleteFBO(unsigned int fboId) {
        GLuint array[] = {fboId};
        glDeleteFramebuffers(1,array);
    }

    void RenderCore::bindFrameBuffer(unsigned int fboId) {
        glBindFramebuffer(GL_FRAMEBUFFER,fboId);
        checkGLError("bind fbo");
    }
}




//
// Created by wansu on 2021/12/10.
//

#include "RenderCore.h"

float vertices[] = {
        1.0f,    -1.0f,   0,    1.0f,0,// 右下角
        -1.0f,   -1.0f,   0,    0,0,  // 左下角
        1.0f,    1.0f,    0,    1.0f,1.0f,// 右上角
        -1.0f,   1.0f,    0,    0,1.0f// 左上角
};

const std::string vertexShaderSource = "#version 300 es\n"
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

Program* RenderCore::createProgram(const std::string &fragmentShaderSource) {
    auto* program = new Program;
    program->addShader(GL_VERTEX_SHADER,vertexShaderSource);
    program->addShader(GL_FRAGMENT_SHADER,fragmentShaderSource);
    if (!program->link()) {
        return nullptr;
    }
    program->use();
    return program;
}

bool RenderCore::initGLES() {
    // 初始化主要是把添加顶点数据，视屏渲染都是矩形，所以这里对顶点数据进行固定
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof (vertices),vertices,GL_STATIC_DRAW);
    // 这里需要对纹理坐标和顶点坐标进行解析
    glVertexAttribPointer(0,3, GL_FLOAT,GL_FALSE,5*sizeof (float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2, GL_FLOAT,GL_FALSE,5*sizeof (float),(void*)(3*sizeof (float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(VAO);
    LOG_DEBUG << "init program success";
    return true;
}

unsigned int RenderCore::createTexFBO(int width, int height) {
    return 0;
}

unsigned int RenderCore::createOESTexture() {
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,texture);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,0);
    if (checkGLError("createOESTexture")) {
        return -1;
    }
    LOG_DEBUG << "create texture success";
    return texture;
}

unsigned int RenderCore::create2DTexture() {
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);
    if (checkGLError("create2DTexture")) {
        return -1;
    }
    LOG_DEBUG << "create texture success";
    return texture;
}

void RenderCore::draw(int textureType, unsigned int textureId, Program *program) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.4,0.6,0.7,1.0);
    glBindVertexArray(VAO);
    glBindTexture(textureType,textureId);
    program->use();
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}




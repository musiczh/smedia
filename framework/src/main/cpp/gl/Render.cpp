//
// Created by wansu on 2022/1/13.
//

#include "Render.h"

#include <utility>
namespace smedia {
    const static float verticesQuad[] = {
            1.0f, -1.0f, 0, 1.0f, 0,// 右下角
            -1.0f, -1.0f, 0, 0, 0,  // 左下角
            1.0f, 1.0f, 0, 1.0f, 1.0f,// 右上角
            -1.0f, 1.0f, 0, 0, 1.0f// 左上角
    };

    // 默认的碎片着色器，直接渲染纹理
    const static std::string defaultFShaderCode = "#version 300 es\n"
                                                  "in vec2 otPos;\n"
                                                  "out vec4 fragColor;\n"
                                                  "uniform sampler2D tex;\n"
                                                  "void main() {\n"
                                                  "    fragColor = texture(tex,otPos);\n"
                                                  "}";

    // 默认的顶点着色器
    const static std::string defaultVShaderCode = "#version 300 es\n"
                                                  "layout (location=0) in vec3 aPos;\n"
                                                  "layout (location=1) in vec2 tPos;\n"
                                                  "out vec2 otPos;\n"
                                                  "void main(){\n"
                                                  "    gl_Position = vec4(aPos,1.0);\n"
                                                  "    otPos = tPos;\n"
                                                  "}";


    Render::Render(GLContextRef glContext):mGLContext(std::move(glContext)) {
        // 构建render
        mState = RENDER_CREATE;
    }

    Render::~Render() {
        // 析构render
        mGLContext->runInRenderThreadV([this](){
            GL_CODE(glDeleteVertexArrays(1,&mVAO))
        });
        mState = RENDER_DESTROY;
        LOG_DEBUG << "destroy render";
    }

    std::unique_ptr<Render>
    Render::CreateWithShaderCode(GLContextRef glContext, const std::string &fShaderCode,
                                 const std::string &vShaderCode) {
        auto render = std::unique_ptr<Render>(new Render(std::move(glContext)));
        if (render->initWithShaderCode(fShaderCode,vShaderCode)) {
            return render;
        }
        return nullptr;
    }

    bool Render::initWithShaderCode(const std::string &fShaderCode, const std::string &vShaderCode) {
        if (mState > RENDER_INIT) {
            LOG_ERROR << "illegal render state,can not init,state" << mState;
            return false;
        }
        std::string fSCode = fShaderCode;
        std::string vSCode = vShaderCode;
        // 默认着色器
        if (fSCode.empty()) {
            fSCode = defaultFShaderCode;
        }
        if (vSCode.empty()) {
            vSCode = defaultVShaderCode;
        }

        auto program = Program::CreateWithShaderSource(mGLContext,vSCode,fSCode);
        if (!program->link()) {
            LOG_ERROR << "init render with program error";
            return false;
        }
        // 重新设置渲染管线
        mProgram = std::move(program);
        LOG_DEBUG << "init render success";
        mState = RENDER_INIT;
        return true;
    }

    void Render::bind() {
        if (mState != RENDER_INIT) {
            LOG_ERROR << "illegal render state,can not bind,state=" << mState;
            return;
        }
        // 第一次需要绑定顶点对象
        if (mVAO == 0) {
            GL_CODE(glGenVertexArrays(1, &mVAO))
            GL_CODE(glBindVertexArray(mVAO);)
            unsigned int VBO;
            GL_CODE(glGenBuffers(1, &VBO))
            GL_CODE(glBindBuffer(GL_ARRAY_BUFFER, VBO))
            GL_CODE(glBufferData(GL_ARRAY_BUFFER, sizeof(verticesQuad), verticesQuad, GL_STATIC_DRAW))
            GL_CODE(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr))
            GL_CODE(glEnableVertexAttribArray(0))
            GL_CODE(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                          (void *) (3 * sizeof(float))))
            GL_CODE(glEnableVertexAttribArray(1))
            LOG_DEBUG << "init vertex array success";
        }
        GL_CODE(glBindVertexArray(mVAO))
        mProgram->bind();
        mState = RENDER_BIND;
    }


    void Render::unBind() {
        if (mState != RENDER_BIND) {
            LOG_ERROR << "illegal render state,can not unbind ,state=" << mState;
            return;
        }
        CHECK_GL_CODE(glBindVertexArray(0);)
        mState = RENDER_INIT;
    }

    void Render::draw() {
        mGLContext->runInRenderThreadV([this](){
            bind();
            GL_CODE(glClear(GL_COLOR_BUFFER_BIT))
            GL_CODE(glClearColor(0.7, 0.8, 0.6, 1.0))
            GL_CODE(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4))
            unBind();
            CHECK_GL_ERROR(draw finish)
        });
    }



    Program *Render::getProgram() {
        return mProgram.get();
    }


}













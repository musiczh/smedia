//
// Created by wansu on 2022/1/13.
//

#ifndef SMEDIA_RENDER_H
#define SMEDIA_RENDER_H
#include <memory>
#include "Program.h"
#include "GLContext.h"
/**
 * 单独一个渲染对象，管理：渲染管线program，渲染顶点数据，渲染上下文
 * 不同的渲染管线有不同的Render对象
 */
namespace smedia {
    enum RenderState {
        RENDER_DEFAULT,RENDER_CREATE,RENDER_INIT,RENDER_BIND,RENDER_DESTROY,
    };


    class Render {
    public:
        static std::unique_ptr<Render> CreateWithShaderCode(GLContext* glContext,
                                              const std::string& fShaderCode = "",
                                              const std::string& vShaderCode = "");
    public:
        ~Render();

    public:
        Program* getProgram();
        void setViewPort(const int* viewPort);
        void draw();

    private:
        // 通过静态方法来创建
        Render(GLContext* glContext);
        /**
         * 绑定渲染管线和顶点数据
         */
        void bind();
        // 解绑顶点数据
        void unBind();
        // 通过顶点着色器代码来初始化
        bool initWithShaderCode(const std::string& fShaderCode = "",const std::string& vShaderCode = "");

    private:
        std::unique_ptr<Program> mProgram;
        GLContext* mGLContext;
        // todo 后续有需求可以把顶点数据操作抽离到一个单独类去处理，（顶点类型、解析等）这里纹理渲染只需要矩形，暂时不需要
        unsigned int mVAO{0};
        int mViewPort[4]{};

        volatile RenderState mState{RENDER_DEFAULT};
    };
}



#endif //SMEDIA_RENDER_H

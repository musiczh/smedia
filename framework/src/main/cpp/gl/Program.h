//
// Created by wansu on 2021/11/29.
//

#ifndef LEARNOPENGL_PROGRAM_H
#define LEARNOPENGL_PROGRAM_H
#include "Logger.h"
#include <GLES3/gl3.h>
#include <map>
#include <vector>
#include <mutex>

/**
 * 负责渲染管线相关的内容，包括创建渲染管线、设置uniform数据
 *
 * todo 暂时没有把这些操作放在gl线程，看看能不能运行，后续再把它们放在gl线程
 */
namespace smedia {
    class Shader;
    class UniformParameter;
    class GLParameter;
    class GLContext;
    class GLTexture;

    class Program {
        // 使用友元类来屏蔽一些对外接口
        friend class Render;
    public:
        Program(GLContext* glContext);
        ~Program();

    protected:
        static std::unique_ptr<Program> CreateWithShaderSource(GLContext* glContext,
                                                               const std::string& vShader,
                                                               const std::string& fShader);

    public:
        // 设置uniform数据，会保存到缓存中，在调用bind的时候统一设置给渲染管线
        // 目前仅支持此三种类型
        // todo 拓展更多的类型需要同时修改三处地方：GLParameter的枚举、setUniform方法、这里增加接口
        void setInt(const std::string &name, int value);
        void setFloat(const std::string &name, float value);
        void setMat4(const std::string &name,const float* value);
        void setTexture(const std::string &name, std::shared_ptr<GLTexture> glTexture);

    protected:
        // 获取programId
        unsigned int get();
        // 提交所有的属性设置，并使用渲染管线
        void bind();

    private:
        /**
         * 把渲染管线中的uniform变量都保存起来，方便后续变量的配置
         */
        void initUniform();
        // 内部处理日志的打印
        // 添加shader
        bool addShader(unsigned int shaderType, const std::string &code);
        // 链接着色器
        bool link();
        void addGLParameter(GLParameter *value);
        void setUniform(std::shared_ptr<GLParameter> glParameter,
                        std::shared_ptr<UniformParameter> uniformParameter);

    private:
        // 保存名称和uniform的映射
        std::map<std::string,std::shared_ptr<UniformParameter>> mUniformMap;
        // 缓存属性设置，在bind的时候一次性设置给管线
        std::vector<std::shared_ptr<GLParameter>> mUniformCache;
        std::mutex mUniformCacheLock;
        GLContext* mGLContext;
        unsigned int mProgram;
        char info[512]{};
    };


    /**
     * 负责编译Shader代码，并自动释放顶点着色器
     */
    class Shader {
    public:
        explicit Shader(const std::string &code, unsigned int shaderType);

        ~Shader();

        bool compile();

        unsigned int get();

    private:
        std::string mShaderCode;
        unsigned int type;
        char info[512];
        unsigned int mShader;
    };

    /**
     * 一个UniformParameter代表渲染管线中的一个uniform变量。在Program被初始化时候会创建初始化Uniform
     * 初始化之后会保存在Program中
     */
    class UniformParameter {
        // 数据对外仅为部分可读，只有Program类可以做修改
        friend class Program;
    public:
        std::string getName();
        GLenum getValueType();

    private:
        // 变量名称
        std::string mName;
        // 变量位置
        int mLocation;
        // 针对于纹理类型需要有一个纹理位置
        unsigned int mSamplerIndex;
        // 该变量的类型，如GL_FLOAT,GL_TEXTURE_2D等
        GLenum mValueType;
    };

    class GLParameter {
    public:
        // value表示设置的参数是什么类型，value_type是对应要设置给渲染管线的类型
        // 所以value_type的类型要比value多
        union {
            float floatValue;
            int intValue;
            unsigned int unsignedIntValue;
            int* intPtrValue;
            float* floatPtrValue;
            std::shared_ptr<GLTexture>* glTextureValue;
        } value;

        enum {
            NONE,
            FLOAT,FLOAT_ARRAY,
            INT,INT_ARRAY,
            TEXTURE,MATRIX4
        } value_type;

        std::string name{};
        // 是否需要被释放
        bool needRelease{false};
        // 如果是数组的话，标记数据的长度
        unsigned int arrayCount{0};

    public:
        ~GLParameter();
        void cleanValue();
    };
}


#endif //LEARNOPENGL_PROGRAM_H

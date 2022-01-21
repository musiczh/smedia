//
// Created by wansu on 2021/11/29.
//

#include "Program.h"
#include "GLHelper.h"
#include "GLContext.h"
#include "GLTexture.h"
#include <GLES2/gl2ext.h>
#include <utility>

namespace smedia {

    Program::Program(std::shared_ptr<GLContext> glContext) {
        mGLContext = std::move(glContext);
        mGLContext->runInRenderThreadV([this](){
            mProgram = glCreateProgram();
        });
    }

    bool Program::addShader(unsigned int shaderType, const std::string &code) {
        return mGLContext->runInRenderThread([this,shaderType,&code]()->bool{
            Shader shader(code, shaderType);
            if (!shader.compile()) {
                return false;
            }
            glAttachShader(mProgram, shader.get());
            return true;
        });
    }

    bool Program::link() {
        return mGLContext->runInRenderThread([this]()->bool {
            int success;
            char msg[512];
            glLinkProgram(mProgram);
            glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
            if (!success || checkGLError("link program")) {
                glGetProgramInfoLog(mProgram, 512, nullptr, msg);
                const std::string s(msg);
                LOG_ERROR << "error:program link error. msg: " ;
                LOG_ERROR << s;
                return false;
            }
            LOG_DEBUG << "link program success";
            return true;
        });
    }

    unsigned int Program::get() {
        return mProgram;
    }

    void Program::setInt(const std::string &name, int value) {
        auto* parameter = new GLParameter;
        parameter->value_type = GLParameter::INT;
        parameter->value.intValue = value;
        parameter->name = name;
        addGLParameter(parameter);
    }

    void Program::setFloat(const std::string &name, float value) {
        auto* parameter = new GLParameter;
        parameter->value_type = GLParameter::FLOAT;
        parameter->value.floatValue = value;
        parameter->name = name;
        addGLParameter(parameter);
    }

    void Program::setMat4(const std::string &name,const float *value) {
        auto* parameter = new GLParameter;
        parameter->value_type = GLParameter::MATRIX4;
        parameter->value.floatPtrValue = const_cast<float*>(value);
        parameter->name = name;
        parameter->needRelease = true;
        // 在矩阵中，这里的count表示矩阵的数目
        parameter->arrayCount = 1;
        addGLParameter(parameter);
    }

    Program::~Program() {
        mGLContext->runInRenderThreadV([this](){
            CHECK_GL_CODE(glDeleteProgram(mProgram);)
        });
    }

    std::unique_ptr<Program>
    Program::CreateWithShaderSource(std::shared_ptr<GLContext> glContext,
                                    const std::string &vShader,
                                    const std::string &fShader) {
        auto* program = new Program(std::move(glContext));
        program->addShader(GL_VERTEX_SHADER,vShader);
        program->addShader(GL_FRAGMENT_SHADER,fShader);
        if (!program->link()) {
            LOG_ERROR << "link program error";
            return nullptr;
        }
        program->initUniform();
        return std::unique_ptr<Program>(program);
    }

    void Program::initUniform() {
        mGLContext->runInRenderThreadV([this](){
            GL_CODE(glUseProgram(mProgram))
            GLint uniformCounts;
            GLint uniformMaxLength;
            GL_CODE(glGetProgramiv(mProgram,GL_ACTIVE_UNIFORMS,&uniformCounts))
            if (uniformCounts <= 0) {
                LOG_DEBUG << "no active uniform";
                return ;
            }
            GL_CODE(glGetProgramiv(mProgram,GL_ACTIVE_UNIFORM_MAX_LENGTH,&uniformMaxLength))
            if (uniformMaxLength == 0) {
                LOG_DEBUG << "uniform length = " << uniformMaxLength;
                return ;
            }

            auto* uniformName = new GLchar[uniformMaxLength+1];
            GLint uniformSize;
            GLint uniformNameLength;
            GLint uniformLocation;
            GLenum uniformType;
            unsigned int samplerIndex = GL_TEXTURE0;
            for (int i = 0; i < uniformCounts; ++i) {
                GL_CODE(glGetActiveUniform(mProgram, i, uniformMaxLength, &uniformNameLength,
                                           &uniformSize, &uniformType, uniformName))
                if (uniformNameLength > 3) {
                    // 有些设备的数组名字是 array[3]，有些是array，不带中括号和长度，这里统一为前一种
                    // 否则外部在设置属性的时候可能会出现找不到uniform的情况
                    char* p = strrchr(uniformName,'[');
                    if (p) {
                        *p = '\0';
                    }
                }
                GL_CODE(uniformLocation = glGetUniformLocation(mProgram,uniformName))
                auto newUniform = std::make_shared<UniformParameter>();
                newUniform->mName = uniformName;
                newUniform->mValueType = uniformType;
                newUniform->mLocation = uniformLocation;
                // 2d或者oes纹理
                if (uniformType == GL_SAMPLER_2D || uniformType == GL_SAMPLER_EXTERNAL_OES) {
                    newUniform->mSamplerIndex = samplerIndex;
                    samplerIndex++;
                }
                mUniformMap[uniformName] = newUniform;
            }
        });

    }

    void Program::bind() {
        if (mProgram == 0) {
            LOG_ERROR << "program is validate";
            return;
        }
        mGLContext->runInRenderThreadV([this](){
            GL_CODE(glUseProgram(mProgram))
            std::unique_lock<std::mutex> lock(mUniformCacheLock);
            // 遍历所有设置的uniform属性并设置
            for (auto& item : mUniformCache) {
                if (mUniformMap.find(item->name) == mUniformMap.end()) {
                    LOG_ERROR << "can not find uniform : " << item->name;
                    continue;
                }
                setUniform(item,mUniformMap[item->name]);
            }
            mUniformCache.clear();
        });
    }

    void Program::addGLParameter(GLParameter* value) {
        std::unique_lock<std::mutex> lock(mUniformCacheLock);
        mUniformCache.push_back(std::shared_ptr<GLParameter>(value));
    }

    void Program::setUniform(std::shared_ptr<GLParameter> glParameter,
                             std::shared_ptr<UniformParameter> uniformParameter) {
        switch (glParameter->value_type) {
            case GLParameter::FLOAT:
                GL_CODE(glUniform1f(uniformParameter->mLocation,
                                    glParameter->value.floatValue))
                break;
            case GLParameter::FLOAT_ARRAY:
                GL_CODE(glUniform1fv(uniformParameter->mLocation,
                                     glParameter->arrayCount,
                                     glParameter->value.floatPtrValue))
                break;
            case GLParameter::INT:
                GL_CODE(glUniform1i(uniformParameter->mLocation,
                                    glParameter->value.intValue))
                break;
            case GLParameter::INT_ARRAY:
                GL_CODE(glUniform1iv(uniformParameter->mLocation,
                                     glParameter->arrayCount,
                                     glParameter->value.intPtrValue))
                break;
            case GLParameter::TEXTURE:
                GL_CODE(glActiveTexture(uniformParameter->mSamplerIndex))
                GL_CODE(glUniform1i(uniformParameter->mLocation,uniformParameter->mSamplerIndex))
                (*(glParameter->value.glTextureValue))->bind();
                break;
            case GLParameter::MATRIX4:
                GL_CODE(glUniformMatrix4fv(uniformParameter->mLocation,
                                           glParameter->arrayCount,false,
                                           glParameter->value.floatPtrValue))
                break;
            default:
                LOG_ERROR << "setUniform with validate valueType = "
                        << glParameter->value_type;
                break;
        }
    }

    void Program::setTexture(const std::string &name, std::shared_ptr<GLTexture> glTexture) {
        auto * glParameter = new GLParameter;
        glParameter->value_type = GLParameter::TEXTURE;
        glParameter->value.glTextureValue = new std::shared_ptr<GLTexture>(std::move(glTexture));
        glParameter->needRelease = true;
        glParameter->name = name;
        addGLParameter(glParameter);
    }


    Shader::Shader(const std::string &code,unsigned int shaderType) {
        mShaderCode = code;
        type = shaderType;
        mShader = glCreateShader(shaderType);
        const char* shaderCode = mShaderCode.c_str();
        CHECK_GL_CODE(glShaderSource(mShader,1,&shaderCode, nullptr);)
    }

    bool Shader::compile() {
        glCompileShader(mShader);
        int success;
        glGetShaderiv(mShader,GL_COMPILE_STATUS,&success);
        if (!success || checkGLError("glCompileShader")) {
            glGetShaderInfoLog(mShader,512, nullptr,info);
            LOG_ERROR << "compile " << type << " shader error. msg: " << info;
            return false;
        }
        LOG_DEBUG << "compile shader success";
        return true;
    }


    Shader::~Shader() {
        CHECK_GL_CODE(glDeleteShader(mShader);)
    }

    unsigned int Shader::get() {
        return mShader;
    }

    std::string UniformParameter::getName() {
        return mName;
    }

    GLenum UniformParameter::getValueType() {
        return mValueType;
    }


    GLParameter::~GLParameter() {
        cleanValue();
    }

    void GLParameter::cleanValue() {
        if (needRelease) {
            switch (value_type) {
                case FLOAT_ARRAY:
                    delete[] value.floatPtrValue; break;
                case INT_ARRAY:
                    delete[] value.intPtrValue; break;
                case TEXTURE:
                    delete value.glTextureValue; break;
                case MATRIX4:
                    delete[] value.floatPtrValue; break;
                default:
                    LOG_DEBUG << "no need to release";
                    break;
            }
        }
        needRelease = false;
        name = "";
        arrayCount = 0;
        value_type = NONE;
    }
}


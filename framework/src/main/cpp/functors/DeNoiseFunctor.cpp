//
// Created by wansu on 2021/12/29.
//

#include "DeNoiseFunctor.h"
namespace smedia {

    const static std::string fv = "#version 300 es\n"
                                  "out vec4 FragColor;\n"
                                  "// 顶点着色传进来的纹理坐标\n"
                                  "in vec2 otPos;\n"
                                  "uniform float sigma;\n"
                                  "uniform float kSigma;\n"
                                  "uniform float threshold;\n"
                                  "\n"
                                  "uniform sampler2D tex;\n"
                                  "\n"
                                  "#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI\n"
                                  "#define INV_PI 0.31830988618379067153776752674503\n"
                                  "\n"
                                  "\n"
                                  "vec4 smartDeNoise(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)\n"
                                  "{\n"
                                  "    float radius = round(kSigma*sigma);\n"
                                  "    float radQ = radius * radius;\n"
                                  "    \n"
                                  "    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)\n"
                                  "    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)\n"
                                  "    \n"
                                  "    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)\n"
                                  "    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)\n"
                                  "    \n"
                                  "    vec4 centrPx = texture(tex,uv);\n"
                                  "    \n"
                                  "    float zBuff = 0.0;\n"
                                  "    vec4 aBuff = vec4(0.0);\n"
                                  "    vec2 size = vec2(textureSize(tex, 0));\n"
                                  "    \n"
                                  "    for(float x=-radius; x <= radius; x++) {\n"
                                  "        float pt = sqrt(radQ-x*x);  // pt = yRadius: have circular trend\n"
                                  "        for(float y=-pt; y <= pt; y++) {\n"
                                  "            vec2 d = vec2(x,y);\n"
                                  "\n"
                                  "            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI; \n"
                                  "            \n"
                                  "            vec4 walkPx =  texture(tex,uv+d/size);\n"
                                  "\n"
                                  "            vec4 dC = walkPx-centrPx;\n"
                                  "            float deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;\n"
                                  "                                 \n"
                                  "            zBuff += deltaFactor;\n"
                                  "            aBuff += deltaFactor*walkPx;\n"
                                  "        }\n"
                                  "    }\n"
                                  "    return aBuff/zBuff;\n"
                                  "}"
                                  "void main(){\n"
                                  "    // 传入坐标和采样器，获取颜色并返回\n"
                                  "    FragColor = smartDeNoise(tex,otPos,sigma,kSigma,threshold);\n"
                                  "}";

    void DeNoiseFunctor::onInit(InputHandler &inputHandler) {
        mSigma = 3;
        mKSigma = 0;
        mThreshold = 0.1;
        if (mProgram == nullptr) {
            mGLContext.runInRenderThread([this]()->bool {
                mProgram = std::unique_ptr<Program>(mGLContext.getRenderCore()->createProgram(fv));
                return true;
            });
        }
        inputHandler.registerHandler("sigma",[this](InputData inputData)->bool {
            float value;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "DeNoiseFunctor get sigma fail";
                return false;
            }
            mSigma = value;
            return true;
        });
        inputHandler.registerHandler("kSigma",[this](InputData inputData)->bool {
            float value;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "DeNoiseFunctor get kSigma fail";
                return false;
            }
            mKSigma = value;
            return true;
        });
        inputHandler.registerHandler("threshold",[this](InputData inputData)->bool {
            float value;
            if (!inputData.data.getData(value)) {
                LOG_ERROR << "DeNoiseFunctor get threshold fail";
                return false;
            }
            mThreshold = value;
            return true;
        });
    }

    void DeNoiseFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {
        mProgram->use();
        mProgram->setFloat("sigma",mSigma);
        mProgram->setFloat("kSigma",mKSigma);
        mProgram->setFloat("threshold",mThreshold);
        mGLContext.getRenderCore()->draw(GL_TEXTURE_2D,frame.glTextureRef->textureId,mProgram.get(),bufferFrame->getFBOId());
    }

    void DeNoiseFunctor::unInitialize(FunctorContext *context) {

    }

    REGISTER_FUNCTOR(DeNoiseFunctor)
}
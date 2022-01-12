//
// Created by wansu on 2021/12/29.
//

#include "HighLightShadowFunctor.h"
namespace smedia {
    const static std::string fv = CODE_STRING_SHADER(
           uniform sampler2D inputImageTexture;
           in vec2 otPos;
           out vec4 gl_FragColor;

           uniform float shadows;
           uniform float highlights;

           const vec3 luminanceWeighting = vec3(0.3, 0.3, 0.3);
           void main()
           {vec4 source = texture(inputImageTexture, otPos);
           float luminance = dot(source.rgb, luminanceWeighting);

           float shadow = clamp((pow(luminance, 1.0/(shadows+1.0)) + (-0.76)*pow(luminance, 2.0/(shadows+1.0))) - luminance, 0.0, 1.0);
           float highlight = clamp((1.0 - (pow(1.0-luminance, 1.0/(2.0-highlights)) + (-0.8)*pow(1.0-luminance, 2.0/(2.0-highlights)))) - luminance, -1.0, 0.0);
           	vec3 result = vec3(0.0, 0.0, 0.0) + ((luminance + shadow + highlight) - 0.0) * ((source.rgb - vec3(0.0, 0.0, 0.0))/(luminance - 0.0));

           	gl_FragColor = vec4(result.rgb, source.a);
           }
        );

    void HighLightShadowFunctor::onInit(InputHandler &inputHandler) {
        inputHandler.registerHandler("shadow",INPUT_CALLBACK{
            float value;
            if (inputData.data.getData(value)) {
                mShadow = value;
                return true;
            }
            LOG_ERROR << "HighLightShadowFunctor get shadow fail";
            return false;
        });
        inputHandler.registerHandler("highLight",INPUT_CALLBACK{
            float value;
            if (inputData.data.getData(value)) {
                mHighLight = value;
                return true;
            }
            LOG_ERROR << "HighLightShadowFunctor get highLight fail";
            return false;
        });
    }

    void HighLightShadowFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {
        mRenderProgram->use();
        mRenderProgram->setFloat("shadows",mShadow);
        mRenderProgram->setFloat("highlights",mHighLight);
        mGLContext->getRenderCore()->draw(GL_TEXTURE_2D,frame.glTextureRef->textureId,mRenderProgram.get(),bufferFrame->getFBOId());
    }

    void HighLightShadowFunctor::unInitialize(FunctorContext *context) {

    }

    std::string HighLightShadowFunctor::getFragmentCode() {
        return fv;
    }

    REGISTER_FUNCTOR(HighLightShadowFunctor)
}
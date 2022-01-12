//
// Created by wansu on 2021/12/29.
//

#include "WhiteBalanceFunctor.h"
namespace smedia {
    const static std::string fragmentCode = CODE_STRING_SHADER(
            uniform sampler2D inputImageTexture;
            in vec2 otPos;
            out vec4 gl_FragColor;
            uniform float temperature;
            uniform float tint;
            vec3 warmFilter = vec3(0.93, 0.54, 0.0);
            mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
            mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);

            void main()
            {
				vec2 textureCoordinate = otPos;
				vec4 source = texture(inputImageTexture, textureCoordinate);

				vec3 yiq = RGBtoYIQ * source.rgb;
            	yiq.b = clamp(yiq.b + tint*0.5226*0.1, -0.5226, 0.5226);
            	vec3 rgb = YIQtoRGB * yiq;

            	vec3 processed = vec3(
            		(rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))),
            		(rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
            		(rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));

            	gl_FragColor = vec4(mix(rgb, processed, temperature), source.a);
            }
            );

    void WhiteBalanceFunctor::onInit(InputHandler &inputHandler) {
		inputHandler.registerHandler("temperature",INPUT_CALLBACK{
			float value;
			if (inputData.data.getData(value)){
				mTemperature = value;
				return true;
			}
			LOG_DEBUG << "WhiteBalanceFunctor get temperature fail";
			return false;
		});
		inputHandler.registerHandler("tint",INPUT_CALLBACK{
			float value;
			if (inputData.data.getData(value)){
				mTint = value;
				return true;
			}
			LOG_DEBUG << "WhiteBalanceFunctor get tint fail";
			return false;
		});
    }

    void WhiteBalanceFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) {
		mRenderProgram->use();
		mRenderProgram->setFloat("temperature",mTemperature);
		mRenderProgram->setFloat("tint",mTint);
		mGLContext.getRenderCore()->draw(GL_TEXTURE_2D,frame.glTextureRef->textureId,mRenderProgram.get(),bufferFrame->getFBOId());
    }

    void WhiteBalanceFunctor::unInitialize(FunctorContext *context) {

    }

    std::string WhiteBalanceFunctor::getFragmentCode() {
        return fragmentCode;
    }

    REGISTER_FUNCTOR(WhiteBalanceFunctor)
}
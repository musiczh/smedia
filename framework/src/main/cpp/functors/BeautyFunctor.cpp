//
// Created by wansu on 2021/12/28.
//

#include "BeautyFunctor.h"
namespace smedia {
    const static std::string fragmentShader = "#version 300 es\n"
                                              "out vec4 gl_FragColor;\n"
                                              "uniform sampler2D inputImageTexture;\n"
                                              "in vec2 otPos;\n"
                                              "uniform float mulW;\n"
                                              "uniform float mulH;\n"
                                              "uniform float beautyStrength;"
                                              "\n"
                                              "void main(){\n"
                                              "    vec2 textureCoordinate = vec2(otPos.x,otPos.y);"
                                              "    vec4 textureColor;\n"
                                              "    vec3 centralColor;\n"
                                              "    float sampleColor;\n"
                                              "    vec2 blurCoordinates[20];\n"
                                              "    float mul = 2.0;\n"
                                              "    float mul_x = mul / mulW;\n"
                                              "    float mul_y = mul / mulH;\n"
                                              "    blurCoordinates[0] = textureCoordinate + vec2(0.0 * mul_x,-10.0 * mul_y);\n"
                                              "    blurCoordinates[1] = textureCoordinate + vec2(5.0 * mul_x,-8.0 * mul_y);\n"
                                              "    blurCoordinates[2] = textureCoordinate + vec2(8.0 * mul_x,-5.0 * mul_y);\n"
                                              "    blurCoordinates[3] = textureCoordinate + vec2(10.0 * mul_x,0.0 * mul_y);\n"
                                              "    blurCoordinates[4] = textureCoordinate + vec2(8.0 * mul_x,5.0 * mul_y);\n"
                                              "    blurCoordinates[5] = textureCoordinate + vec2(5.0 * mul_x,8.0 * mul_y);\n"
                                              "    blurCoordinates[6] = textureCoordinate + vec2(0.0 * mul_x,10.0 * mul_y);\n"
                                              "    blurCoordinates[7] = textureCoordinate + vec2(-5.0 * mul_x,8.0 * mul_y);\n"
                                              "    blurCoordinates[8] = textureCoordinate + vec2(-8.0 * mul_x,5.0 * mul_y);\n"
                                              "    blurCoordinates[9] = textureCoordinate + vec2(-10.0 * mul_x,0.0 * mul_y);\n"
                                              "    blurCoordinates[10] = textureCoordinate + vec2(-8.0 * mul_x,-5.0 * mul_y);\n"
                                              "    blurCoordinates[11] = textureCoordinate + vec2(-5.0 * mul_x,-8.0 * mul_y);\n"
                                              "    blurCoordinates[12] = textureCoordinate + vec2(0.0 * mul_x,-6.0 * mul_y);\n"
                                              "    blurCoordinates[13] = textureCoordinate + vec2(-4.0 * mul_x,-4.0 * mul_y);\n"
                                              "    blurCoordinates[14] = textureCoordinate + vec2(-6.0 * mul_x,0.0 * mul_y);\n"
                                              "    blurCoordinates[15] = textureCoordinate + vec2(-4.0 * mul_x,4.0 * mul_y);\n"
                                              "    blurCoordinates[16] = textureCoordinate + vec2(0.0 * mul_x,6.0 * mul_y);\n"
                                              "    blurCoordinates[17] = textureCoordinate + vec2(4.0 * mul_x,4.0 * mul_y);\n"
                                              "    blurCoordinates[18] = textureCoordinate + vec2(6.0 * mul_x,0.0 * mul_y);\n"
                                              "    blurCoordinates[19] = textureCoordinate + vec2(4.0 * mul_x,-4.0 * mul_y);\n"
                                              "    sampleColor = texture(inputImageTexture, textureCoordinate).g * 22.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[0]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[1]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[2]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[3]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[4]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[5]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[6]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[7]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[8]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[9]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[10]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[11]).g;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[12]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[13]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[14]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[15]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[16]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[17]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[18]).g * 2.0;\n"
                                              "    sampleColor += texture(inputImageTexture, blurCoordinates[19]).g * 2.0;\n"
                                              "    sampleColor = sampleColor/50.0;\n"
                                              "    textureColor = texture(inputImageTexture, textureCoordinate);\n"
                                              "    centralColor = textureColor.rgb;\n"
                                              "    float dis = centralColor.g - sampleColor + 0.5;\n"
                                              "    if(dis <= 0.5)\n"
                                              "    {\n"
                                              "        dis = dis * dis * 2.0;\n"
                                              "    }\n"
                                              "    else\n"
                                              "    {\n"
                                              "        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);\n"
                                              "    }\n"
                                              "\n"
                                              "    if(dis <= 0.5)\n"
                                              "    {\n"
                                              "        dis = dis * dis * 2.0;\n"
                                              "    }\n"
                                              "    else\n"
                                              "    {\n"
                                              "        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);\n"
                                              "    }\n"
                                              "\n"
                                              "    if(dis <= 0.5)\n"
                                              "    {\n"
                                              "        dis = dis * dis * 2.0;\n"
                                              "    }\n"
                                              "    else\n"
                                              "    {\n"
                                              "        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);\n"
                                              "    }\n"
                                              "\n"
                                              "    if(dis <= 0.5)\n"
                                              "    {\n"
                                              "        dis = dis * dis * 2.0;\n"
                                              "    }\n"
                                              "    else\n"
                                              "    {\n"
                                              "        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);\n"
                                              "    }\n"
                                              "\n"
                                              "    if(dis <= 0.5)\n"
                                              "    {\n"
                                              "        dis = dis * dis * 2.0;\n"
                                              "    }\n"
                                              "    else\n"
                                              "    {\n"
                                              "    dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);\n"
                                              "    }\n"
                                              "    float aa= 1.03;\n"
                                              "    vec3 smoothColor = centralColor*aa - vec3(dis)*(aa-1.0);\n"
                                              "    float hue = dot(smoothColor, vec3(0.299,0.587,0.114));\n"
                                              "    aa = 1.0 + pow(hue, 0.6)*0.1;\n"
                                              "    smoothColor = centralColor*aa - vec3(dis)*(aa-1.0);\n"
                                              "    smoothColor.r = clamp(pow(smoothColor.r, 0.8),0.0,1.0);\n"
                                              "    smoothColor.g = clamp(pow(smoothColor.g, 0.8),0.0,1.0);\n"
                                              "    smoothColor.b = clamp(pow(smoothColor.b, 0.8),0.0,1.0);\n"
                                              "    vec3 lvse = vec3(1.0)-(vec3(1.0)-smoothColor)*(vec3(1.0)-centralColor);\n"
                                              "    vec3 bianliang = max(smoothColor, centralColor);\n"
                                              "    vec3 rouguang = 2.0*centralColor*smoothColor + centralColor*centralColor - 2.0*centralColor*centralColor*smoothColor;\n"
                                              "    gl_FragColor = vec4(mix(centralColor, lvse, pow(hue, 0.6)), 1.0);\n"
                                              "    gl_FragColor.rgb = mix(gl_FragColor.rgb, bianliang, pow(hue, 0.6));\n"
                                              "    gl_FragColor.rgb = mix(gl_FragColor.rgb, rouguang, 0.25);\n"
                                              "    mat3 saturateMatrix = mat3(1.1102,-0.0598, -0.061, -0.0774, 1.0826, -0.1186, -0.0228, -0.0228, 1.1772);\n"
                                              "    vec3 satcolor = gl_FragColor.rgb * saturateMatrix;\n"
                                              "    gl_FragColor.rgb = mix(gl_FragColor.rgb, satcolor, 0.18);\n"
                                              "    gl_FragColor.rgb = mix(centralColor, gl_FragColor.rgb, beautyStrength);\n"
                                              "    gl_FragColor.a = textureColor.a;"
                                              "}";

    void BeautyFunctor::onInit(InputHandler &inputHandler) {
        mLevel = 0;
        inputHandler.registerHandler("level",[this](InputData data)->bool{
            float value = 0;
            if (data.data.getData(value)) {
                mLevel = value;
            }
            return true;
        });
    }

    void BeautyFunctor::onDraw(GLBufferFrame *bufferFrame, GLFrame& frame) {
        if (mProgram == nullptr) {
            mProgram = std::unique_ptr<Program>(mGLContext.getRenderCore()->createProgram(fragmentShader));
        }
        mProgram->use();
        mProgram->setFloat("mulW",frame.width);
        mProgram->setFloat("mulH",frame.height);
        mProgram->setFloat("beautyStrength",mLevel);
        mGLContext.getRenderCore()->draw(GL_TEXTURE_2D,frame.glTextureRef->textureId,mProgram.get(),bufferFrame->getFBOId());
    }

    void BeautyFunctor::unInitialize(FunctorContext *context) {

    }

    REGISTER_FUNCTOR(BeautyFunctor)
}
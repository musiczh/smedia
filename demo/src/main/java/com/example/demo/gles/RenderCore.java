package com.example.demo.gles;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLES30;


import com.example.demo.util.LogUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class RenderCore {
    float[] mTriangleVerticesData = {
            // X, Y, Z, U, V
            -1.0f, -1.0f, 0, 0.f, 0.f,
            1.0f, -1.0f, 0, 1.f, 0.f,
            -1.0f,  1.0f, 0, 0.f, 1.f,
            1.0f,  1.0f, 0, 1.f, 1.f,
    };
    String vertexShaderCode = "#version 300 es\n"+
                                "layout (location=0) in vec3 aPos;\n"+
                                   "layout (location=1) in vec2 tPos;\n"+
                                   "\n"+
                                   "out vec2 otPos;\n"+
                                   "void main(){\n"+
                                   "    gl_Position = vec4(aPos,1.0);\n"+
                                   "    otPos = tPos;\n"+
                                   "}";
    String fragmentShaderCode = "#version 300 es\n"+
                             "#extension GL_OES_EGL_image_external_essl3 : require\n"+
                                     "out vec4 FragColor;\n"+
                                     "// 顶点着色传进来的纹理坐标\n"+
                                     "in vec2 otPos;\n"+
                                     "\n"+
                                     "// 采样器，外部可以把纹理赋值给此\n"+
                                     "uniform samplerExternalOES boxTexture;\n"+
                                     "\n"+
                                     "void main(){\n"+
                                     "    // 传入坐标和采样器，获取颜色并返回\n"+
                                     "    FragColor = texture(boxTexture,vec2(otPos.x,1.0-otPos.y));\n"+
                                     "}";
    String fragmentShaderCode2 = "#version 300 es\n"+
            "#extension GL_OES_EGL_image_external_essl3 : require\n"+
            "out vec4 FragColor;\n"+
            "// 顶点着色传进来的纹理坐标\n"+
            "in vec2 otPos;\n"+
            "\n"+
            "// 采样器，外部可以把纹理赋值给此\n"+
            "uniform sampler2D boxTexture;\n"+
            "\n"+
            "void main(){\n"+
            "    // 传入坐标和采样器，获取颜色并返回\n"+
            "    vec4 color = texture(boxTexture,otPos);\n"+
            "FragColor = vec4(color.x+0.2,color.y+0.2,color.z+0.2,color.w+0.2);\n"+
            "}";
    private int mInputTextureId;
    private int mProgram;
    private int mProgram2;
    private FloatBuffer mVertexBuffer;

    private int frameBufferId;
    private int frameBufferTextureId;

    private int VAO;

    public RenderCore() {
        mVertexBuffer = ByteBuffer.allocateDirect(
                mTriangleVerticesData.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertexBuffer.put(mTriangleVerticesData).position(0);
    }
    public int createOESTexture() {
        int[] textureId = new int[1];
        GLES30.glGenTextures(1,textureId,0);
        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,textureId[0]);
        // 绑定到外部纹理
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_NEAREST);
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR);
        GLES30.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE);
        mInputTextureId = textureId[0];
        checkGlError("create oes texture");

        LogUtil.log("create oes texture success");
        return textureId[0];
    }

    public void createProgram(int width,int height) {
        int vertexShader = loadShader(GLES30.GL_VERTEX_SHADER,vertexShaderCode);
        int fragmentShader = loadShader(GLES30.GL_FRAGMENT_SHADER,fragmentShaderCode);
        mProgram = GLES30.glCreateProgram();
        GLES30.glAttachShader(mProgram,vertexShader);
        GLES30.glAttachShader(mProgram,fragmentShader);
        GLES30.glLinkProgram(mProgram);
        int[] linkStatus = new int[1];
        GLES30.glGetProgramiv(mProgram, GLES30.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES20.GL_TRUE) {
            LogUtil.log("link program error");
            LogUtil.log(GLES30.glGetProgramInfoLog(mProgram));
            GLES20.glDeleteProgram(mProgram);
        }
        checkGlError("create program");
        LogUtil.log("create Program success");

        createProgram2(width, height);
    }

    public void createProgram2(int width,int height) {
        int vertexShader = loadShader(GLES30.GL_VERTEX_SHADER,vertexShaderCode);
        int fragmentShader = loadShader(GLES30.GL_FRAGMENT_SHADER,fragmentShaderCode2);
        mProgram2 = GLES30.glCreateProgram();
        GLES30.glAttachShader(mProgram2,vertexShader);
        GLES30.glAttachShader(mProgram2,fragmentShader);
        GLES30.glLinkProgram(mProgram2);
        int[] linkStatus = new int[1];
        GLES30.glGetProgramiv(mProgram2, GLES30.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES20.GL_TRUE) {
            LogUtil.log("link program error");
            LogUtil.log(GLES30.glGetProgramInfoLog(mProgram2));
            GLES20.glDeleteProgram(mProgram2);
        }
        checkGlError("create program");
        LogUtil.log("create Program success");

        int[] a = new int[1];
        GLES30.glGenFramebuffers(1,a,0);
        frameBufferId = a[0];

        GLES30.glGenTextures(1,a,0);
        frameBufferTextureId = a[0];
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D,frameBufferTextureId);
        GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_NEAREST);
        GLES30.glTexParameterf(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR);
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D,0,GLES30.GL_RGBA,width,height,0,
                GLES30.GL_RGBA,GLES30.GL_UNSIGNED_BYTE,null);

        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER,frameBufferId);
        GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER,GLES30.GL_COLOR_ATTACHMENT0,
                GLES30.GL_TEXTURE_2D,frameBufferTextureId,0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D,0);
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER,0);



    }

    private void bindVertexData() {
        mVertexBuffer.position(0);
        GLES30.glVertexAttribPointer(0,3,GLES30.GL_FLOAT,false,5*4,mVertexBuffer);
        checkGlError("glVertexAttribPointer1");
        GLES30.glEnableVertexAttribArray(0);
        checkGlError("glEnableVertexAttribArray1");

        mVertexBuffer.position(3);
        GLES30.glVertexAttribPointer(1,2,GLES30.GL_FLOAT,false,5*4,mVertexBuffer);
        checkGlError("glVertexAttribPointer2");
        GLES30.glEnableVertexAttribArray(1);
        checkGlError("glEnableVertexAttribArray2");
    }

    private int loadShader(int shaderType,String code){
        int shader = GLES30.glCreateShader(shaderType);
        GLES30.glShaderSource(shader,code);
        GLES30.glCompileShader(shader);
        int[] compiled = new int[1];
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            LogUtil.log("Could not compile shader " + shaderType + ":");
            LogUtil.log(GLES30.glGetShaderInfoLog(shader));
            GLES20.glDeleteShader(shader);
            shader = 0;
        }
        return shader;
    }

    public void draw(int width,int height) {
        GLES30.glViewport(0,0,width,height);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
        GLES30.glClearColor(0.3f,0.6f,0.4f,1.0f);

        bindVertexData();

        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER,frameBufferId);

        GLES30.glUseProgram(mProgram);
        checkGlError("glUseProgram");

        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        checkGlError("glActiveTexture");
        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,mInputTextureId);
        checkGlError("glBindTexture");

        int textureLocation = GLES30.glGetUniformLocation(mProgram,"boxTexture");
        GLES30.glUniform1i(textureLocation,0);
        checkGlError("glUniform1i");

        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_STRIP,0,4);
        checkGlError("glDrawArrays");
//------------
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER,0);
        bindVertexData();
        GLES30.glUseProgram(mProgram2);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D,frameBufferTextureId);
        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        textureLocation = GLES30.glGetUniformLocation(mProgram2,"boxTexture");
        GLES30.glUniform1i(textureLocation,0);
        checkGlError("glUniform1i");
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_STRIP,0,4);
        checkGlError("glDrawArrays");
    }

    public void checkGlError(String op) {
        int error;
        if ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            LogUtil.log(op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }

}

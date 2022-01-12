//
// Created by wansu on 2021/12/28.
//

#include "GLBufferFrame.h"
namespace smedia {

    GLBufferFrame::GLBufferFrame(RenderCore *renderCore):mTextureId(0) {
        mRenderCore = renderCore;
        mFBO = renderCore->createFrameBuffer();
        mWidth = -1;
        mHeight = -1;
    }

    void GLBufferFrame::bindTexture(unsigned int textureId) {
        mRenderCore->bindTextureInFrameBuffer(mFBO,textureId);
        if (mTextureId != 0) {
            mRenderCore->deleteTexture(mTextureId);
        }
        mTextureId = textureId;
    }

    void GLBufferFrame::createTextureAttach(int width, int height) {
        mWidth = width;
        mHeight = height;
        LOG_DEBUG << "create texture attach width=" << width << " height=" << height;
        if (mTextureId != 0) {
            mRenderCore->deleteTexture(mTextureId);
        }
        mTextureId = mRenderCore->create2DTexture(width,height);
        mRenderCore->bindTextureInFrameBuffer(mFBO,mTextureId);
    }

    int GLBufferFrame::getWidth() {
        return mWidth;
    }

    int GLBufferFrame::getHeight() {
        return mHeight;
    }

    GLBufferFrame::~GLBufferFrame() {
        mRenderCore->deleteTexture(mTextureId);
        mRenderCore->deleteFBO(mFBO);
    }

    unsigned int GLBufferFrame::getTextureId() {
        return mTextureId;
    }

    unsigned int GLBufferFrame::getFBOId() {
        return mFBO;
    }

    void GLBufferFrame::detachTexture() {
        mRenderCore->bindTextureInFrameBuffer(mFBO,0);
    }
}


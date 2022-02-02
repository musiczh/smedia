//
// Created by wansu on 2022/1/6.
//

#ifndef SMEDIA_IMAGESOURCEFUNCTOR_H
#define SMEDIA_IMAGESOURCEFUNCTOR_H
#include "IFunctor.h"
#include "InputHandler.h"
#include "GLContext.h"

/**
 * 接收平台层的图片数据，进行转化为内部传递数据结构：GLFrame or ImageFrame
 * - options
 *  - data: 上层输入bitmap对象即可
 *  - glEnable: 是否输出gl纹理对象
 *  - imageEnable：是否输出image对象
 * - outputs
 *  - video: glFrame
 *  - buffer: imageFrame
 * todo 目前仅接收RGBA格式的bitmap
 */
namespace smedia {
    class ImageSourceFunctor : public IFunctor {
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~ImageSourceFunctor() override = default;

    private:
        InputHandler mInputHandler;
        FunctorContext* mFunctorContext;
        GLContext* mGLContext;

        Data inputData{};

        volatile bool mImageEnable;
        volatile bool mGlEnable;
    };
}



#endif //SMEDIA_IMAGESOURCEFUNCTOR_H

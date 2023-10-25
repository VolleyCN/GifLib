
#ifndef RASTERMILL_GIFFRAMEHANDLER_H
#define RASTERMILL_GIFFRAMEHANDLER_H

#include "giflib/config.h"
#include "giflib/gif_lib.h"

#include "GifStream.h"
#include "GifColor.h"
#include "GifFrame.h"

class GifFrameHandler : public GifFrame {
public:
    GifFrameHandler(GifStream* stream);
    virtual ~GifFrameHandler();

    virtual int getWidth() const {
        return mGif ? mGif->SWidth : 0;
    }

    virtual int getHeight() const {
        return mGif ? mGif->SHeight : 0;
    }

    virtual bool isOpaque() const {
        return (mBgColor & COLOR_8888_ALPHA_MASK) == COLOR_8888_ALPHA_MASK;
    }

    virtual int getFrameCount() const {
        return mGif ? mGif->ImageCount : 0;
    }

    virtual int getDefaultLoopCount() const {
        return mLoopCount;
    }

    virtual jobject getRawByteBuffer() const {
        return NULL;
    }

    virtual GifFrameState* createState() const;

    GifFileType* getGif() const { return mGif; }
    Color8888 getBackgroundColor() const { return mBgColor; }
    bool getPreservedFrame(int frameIndex) const { return mPreservedFrames[frameIndex]; }
    int getRestoringFrame(int frameIndex) const { return mRestoringFrames[frameIndex]; }

private:
    GifFileType* mGif;
    int mLoopCount;
    Color8888 mBgColor;

    // array of bool per frame - if true, frame data is used by a later DISPOSE_PREVIOUS frame
    bool* mPreservedFrames;

    // array of ints per frame - if >= 0, points to the index of the preserve that frame needs
    int* mRestoringFrames;
};

class GifFrameState_gif : public GifFrameState {
public:
    GifFrameState_gif(const GifFrameHandler& gifFrameGif);
    virtual ~GifFrameState_gif();

    // returns frame's delay time in ms
    virtual long drawFrame(int frameNr,
            Color8888* outputPtr, int outputPixelStride, int previousFrameNr);

private:
    void savePreserveBuffer(Color8888* outputPtr, int outputPixelStride, int frameNr);
    void restorePreserveBuffer(Color8888* outputPtr, int outputPixelStride);

    const GifFrameHandler& mFrameSequence;
    Color8888* mPreserveBuffer;
    int mPreserveBufferFrame;
};

#endif //RASTERMILL_GIFFRAMEHANDLER_H

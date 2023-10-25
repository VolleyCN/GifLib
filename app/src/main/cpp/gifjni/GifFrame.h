#ifndef RASTERMILL_GIFFRAME_H
#define RASTERMILL_GIFFRAME_H
#include "GifStream.h"
#include "GifColor.h"

class GifFrameState {
public:
    virtual long drawFrame(int frameNr,
            Color8888* outputPtr, int outputPixelStride, int previousFrameNr) = 0;
    virtual ~GifFrameState() {}
};

class GifFrame {
public:
    static GifFrame* create(GifStream* stream);
    virtual ~GifFrame() {}
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual bool isOpaque() const = 0;
    virtual int getFrameCount() const = 0;
    virtual int getDefaultLoopCount() const = 0;
    virtual jobject getRawByteBuffer() const = 0;

    virtual GifFrameState* createState() const = 0;
};

#endif //RASTERMILL_GIFFRAME_H

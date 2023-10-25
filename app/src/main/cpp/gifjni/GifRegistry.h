#ifndef RASTERMILL_GIFREGISTRY_H
#define RASTERMILL_GIFREGISTRY_H

#include "jni.h"
#include <stdint.h>

class GifFrame;
class Decoder;
class GifStream;

struct RegistryEntry {
    int requiredHeaderBytes;
    bool (*checkHeader)(void* header, int header_size);
    GifFrame* (*createGifFrame)(GifStream* stream);
    Decoder* (*createDecoder)(GifStream* stream);
    bool (*acceptsBuffer)();
};

/**
 * Template class for registering subclasses that can produce instances of themselves given a
 * DataStream pointer.
 *
 * The super class / root constructable type only needs to define a single static construction
 * meathod that creates an instance by iterating through all factory methods.
 */
class GifRegistry {
public:
    GifRegistry(const RegistryEntry& entry);

    static const RegistryEntry* Find(GifStream* stream);

private:
    RegistryEntry mImpl;
    GifRegistry* mNext;
};

#endif // RASTERMILL_GIFREGISTRY_H

#include "GifRegistry.h"

#include "GifStream.h"

static GifRegistry* gHead = 0;
static int gHeaderBytesRequired = 0;

GifRegistry::GifRegistry(const RegistryEntry& entry) {
    mImpl = entry;

    mNext = gHead;
    gHead = this;

    if (gHeaderBytesRequired < entry.requiredHeaderBytes) {
        gHeaderBytesRequired = entry.requiredHeaderBytes;
    }
}

const RegistryEntry* GifRegistry::Find(GifStream* stream) {
    GifRegistry* registry = gHead;

    if (stream->getRawBuffer() != NULL) {
        while (registry) {
            if (registry->mImpl.acceptsBuffer()) {
                return &(registry->mImpl);
            }
            registry = registry->mNext;
        }
    } else {
        int headerSize = gHeaderBytesRequired;
        char header[headerSize];
        headerSize = stream->peek(header, headerSize);
        while (registry) {
            if (headerSize >= registry->mImpl.requiredHeaderBytes
                    && registry->mImpl.checkHeader(header, headerSize)) {
                return &(registry->mImpl);
            }
            registry = registry->mNext;
        }
    }
    return 0;
}


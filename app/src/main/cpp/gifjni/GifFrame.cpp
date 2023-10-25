#include "GifFrame.h"
#include "GifRegistry.h"

GifFrame* GifFrame::create(GifStream* stream) {
    const RegistryEntry* entry = GifRegistry::Find(stream);

    if (!entry) return NULL;

    GifFrame* gifFrame = entry->createGifFrame(stream);
    if (!gifFrame->getFrameCount() ||
        !gifFrame->getWidth() || !gifFrame->getHeight()) {
        delete gifFrame;
        return NULL;
    }

    return gifFrame;
}

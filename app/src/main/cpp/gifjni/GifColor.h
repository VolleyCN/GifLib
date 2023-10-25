#ifndef RASTERMILL_GIFCOLOR_H
#define RASTERMILL_GIFCOLOR_H

#include <sys/types.h>

typedef uint32_t Color8888;

static const Color8888 COLOR_8888_ALPHA_MASK = 0xff000000;
static const Color8888 TRANSPARENT = 0x0;
#define ARGB_TO_COLOR8888(a, r, g, b) \
    ((a) << 24 | (b) << 16 | (g) << 8 | (r))
#endif // RASTERMILL_GIFCOLOR_H

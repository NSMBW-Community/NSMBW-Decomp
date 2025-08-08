#pragma once

#include <types.h>

#include <revolution/GX.h>

namespace EGG {

namespace StateGX {
    void initialize(unsigned short, unsigned short, GXColor, GXPixelFmt);
    void resetGXCache();
}

} // namespace EGG

#pragma once

#include <types.h>

#include <lib/rvl/gx/GX.h>

namespace EGG {

namespace StateGX {
    void initialize(unsigned short, unsigned short, GXColor, GXPixelFmt);
    void resetGXCache();
}

} // namespace EGG

#pragma once

#include <types.h>
#include <revolution/GX.h>

namespace EGG {

namespace GfxEngine {
    class Configuration {
    public:
        Configuration();

        u16 m_00;
        u16 m_02;
        GXColor mColor;
        GXPixelFmt mPixelFmt;
        u8 mPad1[0x8];
        u16 m_14;
        u16 m_16;
        u8 mPad2[0x8];
    };
}

} // namespace EGG

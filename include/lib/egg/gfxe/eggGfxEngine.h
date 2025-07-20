#pragma once

#include <types.h>

namespace EGG {

namespace GfxEngine {
    class Configuration {
    public:
        u16 x0;
        u16 x2;
        char pad2[0x10];
        u16 x14;
        u16 x16;
        Configuration();
    };
} 

} // namespace EGG

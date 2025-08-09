#pragma once

#include <lib/egg/gfxe/eggScreen.h>
#include <nw4r/math.h>

namespace EGG {

namespace GlobalDrawState {
    void beginDrawView(unsigned short, const nw4r::math::MTX34 &, const EGG::Screen &);

    extern u32 sDrawFlag;
};

} // namespace EGG

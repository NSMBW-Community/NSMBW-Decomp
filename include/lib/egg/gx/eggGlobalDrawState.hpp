#pragma once

#include <lib/egg/gx/eggScreen.hpp>
#include <lib/nw4r/math/mtx.hpp>

namespace EGG {

namespace GlobalDrawState {
    void beginDrawView(unsigned short, const nw4r::math::MTX34 &, const EGG::Screen &);
    
    extern u32 sDrawFlag;
};

} // namespace EGG

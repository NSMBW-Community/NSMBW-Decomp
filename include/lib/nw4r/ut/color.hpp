#pragma once
#include <lib/rvl/gx/GX.h>

namespace nw4r {
namespace ut {

/// @brief A 32-bit RGBA color.
struct Color : public GXColor {
    Color(u32 rgba) {
        this->rgba = rgba;
    }
};

} // namespace ut
} // namespace nw4r

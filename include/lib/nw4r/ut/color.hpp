#pragma once
#include <lib/rvl/gx/GX.h>

namespace nw4r {
namespace ut {

/// @brief A 32-bit RGBA color.
struct Color : public GXColor {

    /// @brief Constructs an empty color (defaults to white).
    mColor() { rgba = 0xFFFFFFFF; }

    /// @brief Constructs a new color, given the 4 individual components.
    mColor(u8 red, u8 green, u8 blue, u8 alpha) { r = red; g = green; b = blue; a = alpha; }

    /// @brief Constructs a new color, given the 4 components.
    mColor(u32 color) { rgba = color; }
};

} // namespace ut
} // namespace nw4r

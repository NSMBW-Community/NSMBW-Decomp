#pragma once
#include <lib/rvl/gx/GXStruct.h>

/// @brief A 32-bit RGBA color.
/// @details Game-specific implementation.
struct mColor : public GXColor {

    /// @brief Constructs an empty color (defaults to white).
    mColor() { rgba = 0xFFFFFFFF; }

    /// @brief Constructs a new color, given the 4 individual components.
    mColor(u8 red, u8 green, u8 blue, u8 alpha) { r = red; g = green; b = blue; a = alpha; }

    /// @brief Constructs a new color, given the 4 components.
    mColor(u32 color) { rgba = color; }
};

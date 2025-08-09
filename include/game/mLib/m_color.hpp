#pragma once
#include <nw4r/ut.h>

/// @brief A 32-bit RGBA color.
/// @ingroup mlib
struct mColor : public nw4r::ut::Color {
public:
    mColor() : nw4r::ut::Color() {}
    mColor(nw4r::ut::Color color) : nw4r::ut::Color(color) {}
};

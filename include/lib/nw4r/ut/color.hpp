#pragma once

namespace nw4r {
namespace ut {

/// @brief A 32-bit color.
struct Color {
    union {
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        };
        u32 rgba;
    };
};

} // namespace ut
} // namespace nw4r

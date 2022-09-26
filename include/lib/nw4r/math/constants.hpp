#pragma once
#include <types.h>

namespace nw4r {
namespace math {

namespace detail {
    const u32 I_ULP = 0x34000000; // sign=+ exp=-23 frac=0
} // namespace detail

const f32 F_ULP = *(const f32*)(&detail::I_ULP); // Minimum positive value that satisfies 1.0f + x != 1.0f

} // namespace math
} // namespace nw4r

#pragma once
#include <lib/rvl/mtx/mtx.h>

namespace nw4r {
namespace math {

struct _MTX34 {
    union {
        f32 mData[3][4];
        Mtx mtx;
    };
};

/// @brief A 3x4 matrix.
struct MTX34 : public _MTX34 {};

} // namespace math
} // namespace nw4r

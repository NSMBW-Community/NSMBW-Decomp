#pragma once

#include <lib/nw4r/math/vec.hpp>

namespace nw4r {
namespace math {

/// @brief An axis-aligned bounding box.
struct AABB {
    VEC3 minCorner;
    VEC3 maxCorner;
};

} // namespace math
} // namespace nw4r

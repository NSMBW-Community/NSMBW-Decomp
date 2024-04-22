#pragma once

namespace nw4r {
namespace math {

/// @brief A two-dimensional floating point vector.
struct VEC2 {
    float x;
    float y;
};

struct _VEC3 {
    float x;
    float y;
    float z;
};

/// @brief A three-dimensional floating point vector.
struct VEC3 : public _VEC3 {
    VEC3() {}
    VEC3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }
};

} // namespace math
} // namespace nw4r

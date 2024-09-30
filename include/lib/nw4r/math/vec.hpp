#pragma once

namespace nw4r {
namespace math {

/// @brief A two-dimensional floating point vector.
struct _VEC2 {
    float x, y;
};

struct VEC2 : public _VEC2 {
    VEC2() {}
    VEC2(float fx, float fy) { x = fx; y = fy; }
    VEC2(const VEC2 &v) { x = v.x; y = v.y; }
    // VEC2(const VEC2 &v) { set(x, y); }

    void set(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

struct _VEC3 {
    float x, y, z;
};

/// @brief A three-dimensional floating point vector.
struct VEC3 : public _VEC3 {
    VEC3() {}
    VEC3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }
};

} // namespace math
} // namespace nw4r

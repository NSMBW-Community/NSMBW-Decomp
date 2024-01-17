#pragma once

namespace nw4r {
namespace math {

/// @brief A two-dimensional floating point vector.
struct VEC2 {
    float x;
    float y;
};

/// @brief A three-dimensional floating point vector.
struct VEC3 {
    float x;
    float y;
    float z;

    VEC3() {}
    VEC3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

    void set(const VEC3 &v) { x = v.x; y = v.y; z = v.z; }
    void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

    void reset() { set(0, 0, 0); }
};

} // namespace math
} // namespace nw4r

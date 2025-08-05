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
};

struct _VEC3 {
    float x, y, z;
};

struct VEC3;
VEC3* VEC3Add(VEC3* pOut, const VEC3* pA, const VEC3* pB);

/// @brief A three-dimensional floating point vector.
struct VEC3 : public _VEC3 {
    VEC3() {}
    VEC3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }

    VEC3 operator+(const VEC3 &rRhs) const {
        VEC3 out;
        VEC3Add(&out, this, &rRhs);
        return out;
    }
};

inline VEC3* VEC3Add(register VEC3* pOut, register const VEC3* pA,
                     register const VEC3* pB) {
    register float work0, work1, work2;

    // clang-format off
    asm {
        // Add XY
        psq_l  work0, VEC3.x(pA),   0, 0
        psq_l  work1, VEC3.x(pB),   0, 0
        ps_add work2, work0, work1
        psq_st work2, VEC3.x(pOut), 0, 0

        // Add Z
        psq_l  work0, VEC3.z(pA),   1, 0
        psq_l  work1, VEC3.z(pB),   1, 0
        ps_add work2, work0, work1
        psq_st work2, VEC3.z(pOut), 1, 0
    }
    // clang-format on

    return pOut;
}

} // namespace math
} // namespace nw4r

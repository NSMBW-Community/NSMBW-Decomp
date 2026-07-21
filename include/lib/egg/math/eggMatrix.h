#pragma once

#include <lib/egg/math/eggMath.h>
#include <nw4r/math.h>
#include <types.h>

namespace EGG {

struct Matrix34f : public nw4r::math::MTX34 {
    Matrix34f() {}

    Matrix34f(const nw4r::math::MTX34 &mtx) : MTX34(mtx) {}

    f32 operator()(int i, int j) const {
        return m[i][j];
    }
    f32 &operator()(int i, int j) {
        return m[i][j];
    }
    f32 operator()(int i) const {
        return a[i];
    }
    f32 &operator()(int i) {
        return a[i];
    }

    Matrix34f &operator=(const Matrix34f &other) {
        copyFrom(other);
        return *this;
    }

    void copyFrom(const Matrix34f &other) {
        for (int i = 0; i < 12; i++) {
            (*this)(i) = other(i);
        }
    }
};

} // namespace EGG

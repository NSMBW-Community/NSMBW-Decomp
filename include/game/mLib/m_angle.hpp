#pragma once
#include <types.h>
#include <game/sLib/s_math.hpp>
#include <nw4r/math.h>

extern "C" {
    int abs(int);
}

/// @brief A one-dimensional short angle vector.
/// @ingroup mlib
struct mAng {

    /// @brief Constructs an empty vector.
    mAng() {}

    /// @brief Constructs a vector from a short value.
    mAng(s16 x) : mAngle(x) {}

    operator s16() { return mAngle; }

    bool chase(short target, short step) {
        return sLib::chase(&mAngle, target, step);
    }

    mAng abs() const {
        return mAng(::abs(mAngle));
    }

    /// @brief Augmented addition operator.
    mAng &operator+=(const mAng &v) { mAngle += v.mAngle; return *this; }

    /// @brief Augmented subtraction operator.
    mAng &operator-=(const mAng &v) { mAngle -= v.mAngle; return *this; }

    /// @brief Positive operator.
    mAng operator+() const { return *this; }

    /// @brief Negative operator.
    mAng operator-() const { return mAng(-mAngle); }

    /// @brief Addition operator.
    mAng operator+(const mAng &v) const { return mAng(mAngle + v.mAngle); }

    /// @brief Subtraction operator.
    mAng operator-(const mAng &v) const { return mAng(mAngle - v.mAngle); }

    /// @brief Computes the sine of the angle.
    float sin() const { return nw4r::math::SinIdx(mAngle); }

    /// @brief Computes the cosine of the angle.
    float cos() const { return nw4r::math::CosIdx(mAngle); }

    s16 mAngle; ///< The rotation.
};

/// @brief A three-dimensional short angle vector.
/// @ingroup mlib
class mAng3_c {
public:

    /// @brief Constructs an empty vector.
    mAng3_c() {}

    /// @brief Constructs a vector from a short array.
    mAng3_c(const s16 *p) { x = p[0]; y = p[1]; z = p[2]; }

    /// @brief Constructs a vector from three short values.
    mAng3_c(s16 fx, s16 fy, s16 fz) { x = fx; y = fy; z = fz; }

    static mAng3_c onlyY(s16 fy) {
        mAng3_c tmp;
        tmp.y = fy; tmp.x = tmp.z = mAng(0);
        return tmp;
    }

    /// @brief Constructs a vector from three mAng values.
    mAng3_c(mAng fx, mAng fy, mAng fz) : x(fx), y(fy), z(fz) {}

    /// @brief Copy constructor.
    mAng3_c(const mAng3_c &v) : x(v.x), y(v.y), z(v.z) {}

    mAng3_c *operator=(const mAng3_c &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return this;
    }

    /// @brief Augmented addition operator.
    mAng3_c &operator+=(const mAng3_c &v) { x += v.x; y += v.y; z += v.z; return *this; }

    /// @brief Augmented subtraction operator.
    mAng3_c &operator-=(const mAng3_c &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

    /// @brief Positive operator.
    mAng3_c operator+() const { return *this; }

    /// @brief Negative operator.
    mAng3_c operator-() const { return mAng3_c(-x, -y, -z); }

    /// @brief Addition operator.
    mAng3_c operator+(const mAng3_c &v) const { return mAng3_c(x + v.x, y + v.y, z + v.z); }

    /// @brief Subtraction operator.
    mAng3_c operator-(const mAng3_c &v) const { return mAng3_c(x - v.x, y - v.y, z - v.z); }

    mAng x; ///< The rotation on the X axis.
    mAng y; ///< The rotation on the Y axis.
    mAng z; ///< The rotation on the Z axis.

    static mAng3_c Zero; ///< The null rotation vector.
    static mAng3_c Ex; ///< The unit rotation vector for the X axis.
    static mAng3_c Ey; ///< The unit rotation vector for the Y axis.
    static mAng3_c Ez; ///< The unit rotation vector for the Z axis.
};

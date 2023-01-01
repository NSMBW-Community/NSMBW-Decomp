#pragma once
#include <types.h>
#include <lib/rvl/mtx/vec.h>

/// @brief A one-dimensional short angle vector.
/// @details [Used by game-specific code].
struct mAng {

    /// @brief Constructs an empty vector.
    mAng() {}

    /// @brief Constructs a vector from a short pointer.
    mAng(const s16 *p) { mAngle = *p; }

    /// @brief Constructs a vector from a short value.
    mAng(s16 x) { mAngle = x; }

    /// @brief Short cast operator.
    operator s16*() { return &mAngle; }

    /// @brief Const short cast operator.
    operator const s16*() const { return &mAngle; }

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

    /// @brief Equality operator.
    bool operator==(const mAng &v) const { return mAngle == v.mAngle; }

    /// @brief Inequality operator.
    bool operator!=(const mAng &v) const { return mAngle != v.mAngle; }

    s16 mAngle; ///< The rotation.
};

/// @brief A three-dimensional short angle vector.
/// @details [Used by game-specific code].
class mAng3_c {
public:

    /// @brief Constructs an empty vector.
    mAng3_c() {}

    /// @brief Constructs a vector from a short array.
    mAng3_c(const s16 *p) { x = p[0]; y = p[1]; z = p[2]; }

    /// @brief Constructs a vector from three short values.
    mAng3_c(s16 fx, s16 fy, s16 fz) { x = fx; y = fy; z = fz; }

    /// @brief Constructs a new vector from an existing vector from the MTX library.
    mAng3_c(const S16Vec &v) { x = v.x; y = v.y; z = v.z; }

    /// @brief Short cast operator.
    operator s16*() { return &x; }

    /// @brief Const short cast operator.
    operator const s16*() const { return &x; }

    /// @brief S16Vec cast operator.
    operator S16Vec*() { return (S16Vec*)&x; }

    /// @brief Const S16Vec cast operator.
    operator const S16Vec*() const { return (const S16Vec*)&x; }

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

    /// @brief Equality operator.
    bool operator==(const mAng3_c &v) const { return x == v.x && y == v.y && z == v.z; }

    /// @brief Inequality operator.
    bool operator!=(const mAng3_c &v) const { return x != v.x || y != v.y || z != v.z; }

    s16 x; ///< The rotation on the X axis.
    s16 y; ///< The rotation on the Y axis.
    s16 z; ///< The rotation on the Z axis.

    static mAng3_c Zero; ///< The null rotation vector.
    static mAng3_c Ex; ///< The unit rotation vector for the X axis.
    static mAng3_c Ey; ///< The unit rotation vector for the Y axis.
    static mAng3_c Ez; ///< The unit rotation vector for the Z axis.
};

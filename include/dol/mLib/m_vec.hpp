#pragma once
#include <lib/nw4r/math/vec.hpp>
#include <lib/rvl/mtx/vec.h>
#include <dol/mLib/m_angle.hpp>

/// @brief A two-dimensional floating point vector.
/// @details [Used by game-specific code].
/// @todo Add EGG::vector2f operators.
class mVec2_c {
public:

    /// @brief Constructs an empty vector.
    mVec2_c() {}

    /// @brief Constructs a vector from a float array.
    mVec2_c(const f32 *p) { x = p[0]; y = p[1]; }

    /// @brief Constructs a vector from two floating point values.
    mVec2_c(f32 fx, f32 fy) { x = fx; y = fy; }

    /// @brief Constructs a new vector from an existing vector from the MTX library.
    mVec2_c(const Vec2 &v) { x = v.x; y = v.y; }

    /// @brief Constructs a new vector from an existing vector from the nw4r::math library.
    mVec2_c(const nw4r::math::VEC2 &v) { x = v.x; y = v.y; }

    /// @brief Destroys the vector.
    ~mVec2_c() {}

    /// @brief Float cast operator.
    operator f32*() { return &x; }

    /// @brief Const float cast operator.
    operator const f32*() const { return &x; }

    /// @brief Vec2 cast operator.
    operator Vec2*() { return (Vec2*)&x; }

    /// @brief Const Vec2 cast operator.
    operator const Vec2*() const { return (const Vec2*)&x; }

    /// @brief nw4r::math::VEC2 cast operator.
    operator nw4r::math::VEC2*() { return (nw4r::math::VEC2*)&x; }

    /// @brief Const nw4r::math::VEC2 cast operator.
    operator const nw4r::math::VEC2*() const { return (const nw4r::math::VEC2*)&x; }

    /// @brief Augmented addition operator.
    mVec2_c &operator+=(const mVec2_c &v) { x += v.x; y += v.y; return *this; }

    /// @brief Augmented subtraction operator.
    mVec2_c &operator-=(const mVec2_c &v) { x -= v.x; y -= v.y; return *this; }

    /// @brief Augmented scalar product operator.
    mVec2_c &operator*=(f32 f) { x *= f; y *= f; return *this; }

    /// @brief Augmented scalar division operator.
    mVec2_c &operator/=(f32 f) { return operator*=(1.0f / f); }

    /// @brief Positive operator.
    mVec2_c operator+() const { return *this; }

    /// @brief Negative operator.
    mVec2_c operator-() const { return mVec2_c(-x, -y); }

    /// @brief Addition operator.
    mVec2_c operator+(const mVec2_c &v) const { return mVec2_c(x + v.x, y + v.y); }

    /// @brief Subtraction operator.
    mVec2_c operator-(const mVec2_c &v) const { return mVec2_c(x - v.x, y - v.y); }

    /// @brief Scalar product operator.
    mVec2_c operator*(f32 f) const { return mVec2_c(f * x, f * y); }

    /// @brief Scalar division operator.
    mVec2_c operator/(f32 f) const { f32 r = 1.0f / f; return operator*(r); }

    /// @brief Equality operator.
    bool operator==(const mVec2_c &v) const { return x == v.x && y == v.y; }

    /// @brief Inequality operator.
    bool operator!=(const mVec2_c &v) const { return x != v.x || y != v.y; }

    float x; ///< The coordinates on the X axis.
    float y; ///< The coordinates on the Y axis.
};

/// @brief A three-dimensional floating point vector.
/// @details [Used by game-specific code].
/// @todo Add EGG::vector3f operators.
class mVec3_c {
public:

    /// @brief Constructs an empty vector.
    mVec3_c() {}

    /// @brief Constructs a vector from a float array.
    mVec3_c(const f32 *p) { x = p[0]; y = p[1]; z = p[2]; }

    /// @brief Constructs a vector from three floating point values.
    mVec3_c(f32 fx, f32 fy, f32 fz) { x = fx; y = fy; z = fz; }

    /// @brief Constructs a new vector from an existing vector from the MTX library.
    mVec3_c(const Vec &v) { x = v.x; y = v.y; z = v.z; }

    /// @brief Constructs a new vector from an existing vector from the nw4r::math library.
    mVec3_c(const nw4r::math::VEC3 &v) { x = v.x; y = v.y; z = v.z; }

    /// @brief Destroys the vector.
    ~mVec3_c() {}

    /// @brief Float cast operator.
    operator f32*() { return &x; }

    /// @brief Const float cast operator.
    operator const f32*() const { return &x; }

    /// @brief Vec cast operator.
    operator Vec*() { return (Vec*)&x; }

    /// @brief Const Vec cast operator.
    operator const Vec*() const { return (const Vec*)&x; }

    /// @brief nw4r::math::VEC3 cast operator.
    operator nw4r::math::VEC3*() { return (nw4r::math::VEC3*)&x; }

    /// @brief Const nw4r::math::VEC3 cast operator.
    operator const nw4r::math::VEC3*() const { return (const nw4r::math::VEC3*)&x; }

    /// @brief Augmented addition operator.
    mVec3_c &operator+=(const mVec3_c &v) { x += v.x; y += v.y; z += v.z; return *this; }

    /// @brief Augmented subtraction operator.
    mVec3_c &operator-=(const mVec3_c &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

    /// @brief Augmented scalar product operator.
    mVec3_c &operator*=(f32 f) { x *= f; y *= f; z *= f; return *this; }

    /// @brief Augmented scalar division operator.
    mVec3_c &operator/=(f32 f) { return operator*=(1.0f / f); }

    /// @brief Positive operator.
    mVec3_c operator+() const { return *this; }

    /// @brief Negative operator.
    mVec3_c operator-() const { return mVec3_c(-x, -y, -z); }

    /// @brief Addition operator.
    mVec3_c operator+(const mVec3_c &v) const { return mVec3_c(x + v.x, y + v.y, z + v.z); }

    /// @brief Subtraction operator.
    mVec3_c operator-(const mVec3_c &v) const { return mVec3_c(x - v.x, y - v.y, z - v.z); }

    /// @brief Scalar product operator.
    mVec3_c operator*(f32 f) const { return mVec3_c(f * x, f * y, f * z); }

    /// @brief Scalar division operator.
    mVec3_c operator/(f32 f) const { f32 r = 1.0f / f; return operator*(r); }

    /// @brief Equality operator.
    bool operator==(const mVec3_c &v) const { return x == v.x && y == v.y && z == v.z; }

    /// @brief Inequality operator.
    bool operator!=(const mVec3_c &v) const { return x != v.x || y != v.y || z != v.z; }

    /// @brief Normalizes the vector.
    /// @return The vector's magnitude.
    float normalize();

    /// @brief Normalizes the vector.
    /// @return If the operation was successful.
    bool normalizeRS();

    void rotX(mAng angle); ///< Rotates the vector on the X axis by the given angle.
    void rotY(mAng angle); ///< Rotates the vector on the Y axis by the given angle.

    float x; ///< The coordinates on the X axis.
    float y; ///< The coordinates on the Y axis.
    float z; ///< The coordinates on the Z axis.

    static mVec3_c Zero; ///< The null vector.
    static mVec3_c Ex; ///< The unit vector for the X axis.
    static mVec3_c Ey; ///< The unit vector for the Y axis.
    static mVec3_c Ez; ///< The unit vector for the Z axis.
};

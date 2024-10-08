#pragma once
#include <types.h>
#include <game/mLib/m_angle.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <lib/rvl/mtx/mtx.h>

/// @brief A 3x4 matrix.
/// @ingroup mlib
class mMtx_c {
public:
    /// @brief Constructs an empty matrix.
    mMtx_c() {}

    /// @brief Constructs a matrix with the given components.
    mMtx_c(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23);

    /// @brief Mtx cast operator.
    operator Mtx*() { return &mData; }

    /// @brief Const Mtx cast operator.
    operator const Mtx*() const { return &mData; }

    void XrotS(mAng angle); ///< Generates a rotation matrix for the X axis with the given angle.
    void XrotM(mAng angle); ///< Rotates the matrix on the X axis by the given angle.
    void YrotS(mAng angle); ///< Generates a rotation matrix for the Y axis with the given angle.
    void YrotM(mAng angle); ///< Rotates the matrix on the Y axis by the given angle.
    void ZrotS(mAng angle); ///< Generates a rotation matrix for the Z axis with the given angle.
    void ZrotM(mAng angle); ///< Rotates the matrix on the Z axis by the given angle.

    void ZXYrotM(mAng xRot, mAng yRot, mAng zRot); ///< Rotates the matrix on the Y, X and Z axes by the given angles.
    void XYZrotM(mAng xRot, mAng yRot, mAng zRot); ///< Rotates the matrix on the Z, Y and X axes by the given angles.

    void toRot(mAng3_c &out) const; ///< Extracts the rotation vector from the matrix.
    void multVecZero(nw4r::math::VEC3 &out) const; ///< Extracts the translation vector from the matrix.
    void zero(); ///< Zeroes out the matrix.

    float mData[3][4]; ///< The matrix components.

    static mMtx_c Identity; ///< The identity matrix.
};

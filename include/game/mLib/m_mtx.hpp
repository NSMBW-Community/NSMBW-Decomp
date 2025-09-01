#pragma once
#include <types.h>
#include <game/mLib/m_angle.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/math.h>

/// @brief A 3x4 matrix.
/// @ingroup mlib
class mMtx_c : public nw4r::math::MTX34 {
public:
    /// @brief Constructs an empty matrix.
    mMtx_c() {}

    /// @brief Constructs a matrix with the given components.
    mMtx_c(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23);

    /// @brief Mtx cast operator.
    operator Mtx*() { return &mtx; }

    /// @brief Const Mtx cast operator.
    operator const Mtx*() const { return &mtx; }

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

    mVec3_c getTranslation() const {
        float x = m[0][3];
        float y = m[1][3];
        float z = m[2][3];
        return mVec3_c(x, y, z);
    }

    static mMtx_c createTrans(const mVec3_c &v) { mMtx_c mtx; PSMTXTrans(mtx, v.x, v.y, v.z); return mtx; }

    mMtx_c &concat(const mMtx_c &other) { PSMTXConcat(*this, other, *this); return *this; }
    mMtx_c &trans(const mVec3_c &v) { PSMTXTrans(*this, v.x, v.y, v.z); return *this; }
    mMtx_c &trans(float x, float y, float z) { PSMTXTrans(*this, x, y, z); return *this; }
    mMtx_c &ZXYrotM(const mAng3_c &ang) { ZXYrotM(ang.x, ang.y, ang.z); return *this; }

    float transX() const { return getTranslation().x; }
    float transY() const { return getTranslation().y; }
    float transZ() const { return getTranslation().z; }

    static mMtx_c Identity; ///< The identity matrix.
};

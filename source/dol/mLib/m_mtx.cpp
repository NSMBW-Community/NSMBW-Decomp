#include <types.h>
#include <utility_inlines.hpp>
#include <lib/nw4r/math/arithmetic.hpp>
#include <lib/nw4r/math/trigonometry.hpp>
#include <lib/rvl/mtx/mtx.h>
#include <dol/cLib/c_math.hpp>
#include <dol/mLib/m_mtx.hpp>

mMtx_c mMtx_c::Identity = mMtx_c(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

mMtx_c::mMtx_c(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23) {
    mData[0][0] = _00;
    mData[0][1] = _01;
    mData[0][2] = _02;
    mData[0][3] = _03;
    mData[1][0] = _10;
    mData[1][1] = _11;
    mData[1][2] = _12;
    mData[1][3] = _13;
    mData[2][0] = _20;
    mData[2][1] = _21;
    mData[2][2] = _22;
    mData[2][3] = _23;
}

void mMtx_c::XrotS(mAng angle) {
    float cos = angle.cos();
    float sin = angle.sin();

    mData[0][0] = 1.0f;
    mData[0][1] = 0.0f;
    mData[0][2] = 0.0f;
    mData[0][3] = 0.0f;
    mData[1][0] = 0.0f;
    mData[1][1] = cos;
    mData[1][2] = -sin;
    mData[1][3] = 0.0f;
    mData[2][0] = 0.0f;
    mData[2][1] = sin;
    mData[2][2] = cos;
    mData[2][3] = 0.0f;
}

void mMtx_c::XrotM(mAng angle) {
    if (angle.mAngle != 0) {
        mMtx_c rotatedMtx = mMtx_c();
        rotatedMtx.XrotS(angle);
        PSMTXConcat(*this, rotatedMtx, *this);
    }
}

void mMtx_c::YrotS(mAng angle) {
    float cos = angle.cos();
    float sin = angle.sin();

    mData[0][0] = cos;
    mData[0][1] = 0.0f;
    mData[0][2] = sin;
    mData[0][3] = 0.0f;
    mData[1][0] = 0.0f;
    mData[1][1] = 1.0f;
    mData[1][2] = 0.0f;
    mData[1][3] = 0.0f;
    mData[2][0] = -sin;
    mData[2][1] = 0.0f;
    mData[2][2] = cos;
    mData[2][3] = 0.0f;
}

void mMtx_c::YrotM(mAng angle) {
    if (angle.mAngle != 0) {
        mMtx_c rotatedMtx = mMtx_c();
        rotatedMtx.YrotS(angle);
        PSMTXConcat(*this, rotatedMtx, *this);
    }
}

void mMtx_c::ZrotS(mAng angle) {
    float cos = angle.cos();
    float sin = angle.sin();

    mData[0][0] = cos;
    mData[0][1] = -sin;
    mData[0][2] = 0.0f;
    mData[0][3] = 0.0f;
    mData[1][0] = sin;
    mData[1][1] = cos;
    mData[1][2] = 0.0f;
    mData[1][3] = 0.0f;
    mData[2][0] = 0.0f;
    mData[2][1] = 0.0f;
    mData[2][2] = 1.0f;
    mData[2][3] = 0.0f;
}

void mMtx_c::ZrotM(mAng angle) {
    if (angle.mAngle != 0) {
        mMtx_c rotatedMtx = mMtx_c();
        rotatedMtx.ZrotS(angle);
        PSMTXConcat(*this, rotatedMtx, *this);
    }
}

void mMtx_c::ZXYrotM(mAng xRot, mAng yRot, mAng zRot) {
    YrotM(yRot);
    XrotM(xRot);
    ZrotM(zRot);
}

void mMtx_c::XYZrotM(mAng xRot, mAng yRot, mAng zRot) {
    ZrotM(zRot);
    YrotM(yRot);
    XrotM(xRot);
}

// [This approach is required for matching].
inline float calcLengthSq(float x, float y) {
    x *= x;
    y *= y;
    x += y;
    return x;
}

void mMtx_c::toRot(mAng3_c &out) const {
    float cos = nw4r::math::FSqrt(calcLengthSq(mData[0][2], mData[2][2]));

    short xRot = cM::atan2s(-mData[1][2], cos);
    out.x = xRot;
    if (xRot == 0x4000 || xRot == -0x4000) {
        out.z = 0;
        out.y = cM::atan2s(-mData[2][0], mData[0][0]);
    } else {
        out.y = cM::atan2s(mData[0][2], mData[2][2]);
        out.z = cM::atan2s(mData[1][0], mData[1][1]);
    }
}

void mMtx_c::multVecZero(nw4r::math::VEC3 &out) const {
    out.x = mData[0][3];
    out.y = mData[1][3];
    out.z = mData[2][3];
}

void mMtx_c::zero() {
    mData[0][0] = 0.0f;
    mData[0][1] = 0.0f;
    mData[0][2] = 0.0f;
    mData[0][3] = 0.0f;
    mData[1][0] = 0.0f;
    mData[1][1] = 0.0f;
    mData[1][2] = 0.0f;
    mData[1][3] = 0.0f;
    mData[2][0] = 0.0f;
    mData[2][1] = 0.0f;
    mData[2][2] = 0.0f;
    mData[2][3] = 0.0f;
}

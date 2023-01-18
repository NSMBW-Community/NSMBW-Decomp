#include <utility_inlines.hpp>
#include <dol/mLib/m_angle.hpp>
#include <dol/mLib/m_vec.hpp>
#include <lib/MSL_C/float.h>
#include <lib/MSL_C/math.h>
#include <lib/nw4r/math/trigonometry.hpp>
#include <lib/rvl/mtx/vec.h>

// [This is required to ensure correct sdata2 ordering]
// [It will be deadstripped by the linker later]
float DUMMY_M_VEC(s16 val) {
    return nw4r::math::CosIdx(val);
}

inline bool isZero(float val) {
    const int tmp = 0x34000000; // Minimum positive value that satisfies 1.0f + x != 1.0f
    const float F_ULP = *(const float *)&tmp;
    return (fabsf(val) <= FLT_EPSILON);
}

float mVec3_c::normalize() {
    float mag = PSVECMag(*this);
    if (!isZero(mag)) {
        operator*=(1.0f/mag);
    }

    return mag;
}

bool mVec3_c::normalizeRS() {
    float mag = PSVECMag(*this);
    if (isZero(mag)) {
        return false;
    }

    operator*=(1.0f/mag);
    return true;
}

void mVec3_c::rotX(mAng angle) {
    float cos = getFloat(nw4r::math::CosIdx((s16)angle.mAngle));
    float sin = getFloat(nw4r::math::SinIdx((s16)angle.mAngle));
    float y = this->y;
    float z = this->z;
    this->y = cos * y - sin * z;
    this->z = sin * y + cos * z;
}

void mVec3_c::rotY(mAng angle) {
    float cos = getFloat(nw4r::math::CosIdx((s16)angle.mAngle));
    float sin = getFloat(nw4r::math::SinIdx((s16)angle.mAngle));
    float x = this->x;
    float z = this->z;
    this->x = cos * x + sin * z;
    this->z = -sin * x + cos * z;
}

mVec3_c mVec3_c::Zero = mVec3_c(0.0f, 0.0f, 0.0f);
mVec3_c mVec3_c::Ex = mVec3_c(1.0f, 0.0f, 0.0f);
mVec3_c mVec3_c::Ey = mVec3_c(0.0f, 1.0f, 0.0f);
mVec3_c mVec3_c::Ez = mVec3_c(0.0f, 0.0f, 1.0f);

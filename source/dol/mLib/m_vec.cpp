#include <game/mLib/m_angle.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/MSL_C/float.h>
#include <lib/MSL_C/math.h>
#include <lib/nw4r/math/trigonometry.hpp>
#include <lib/rvl/mtx/vec.h>

// [This is required to ensure correct sdata2 ordering]
// [It will be deadstripped by the linker later]
float DUMMY_M_VEC(s16 val) {
    return nw4r::math::CosS(val);
}

inline bool isZero(float val) {
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
    float cos = angle.cos();
    float sin = angle.sin();
    float y = this->y;
    float z = this->z;
    this->y = cos * y - sin * z;
    this->z = sin * y + cos * z;
}

void mVec3_c::rotY(mAng angle) {
    float cos = angle.cos();
    float sin = angle.sin();
    float x = this->x;
    float z = this->z;
    this->x = cos * x + sin * z;
    this->z = -sin * x + cos * z;
}

mVec3_c mVec3_c::Zero(0.0f, 0.0f, 0.0f);
mVec3_c mVec3_c::Ex(1.0f, 0.0f, 0.0f);
mVec3_c mVec3_c::Ey(0.0f, 1.0f, 0.0f);
mVec3_c mVec3_c::Ez(0.0f, 0.0f, 1.0f);

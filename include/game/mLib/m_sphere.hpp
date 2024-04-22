#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/MSL_C/math.h>

class mSphere_c {
public:
    mSphere_c() : mPos(mVec3_c::Zero), mRadius(0.0) {}

    bool isZero() { return fabsf(mRadius) <= 0.00000011920929f; } // (Interval machine epsilon)

    mVec3_c mPos;
    float mRadius;
};

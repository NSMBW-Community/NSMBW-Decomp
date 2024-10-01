#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/MSL_C/float.h>
#include <lib/MSL_C/math.h>

class mSphere_c {
public:
    mSphere_c() : mPos(mVec3_c::Zero), mRadius(0.0) {}

    bool isZero() { return fabsf(mRadius) <= FLT_EPSILON; }

    mVec3_c mPos; ///< The sphere position.
    float mRadius; ///< The sphere radius.
};

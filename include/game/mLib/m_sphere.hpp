#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/egg/math.hpp>
#include <lib/MSL_C/float.h>
#include <lib/MSL_C/math.h>

class mSphere_c : public EGG::Sphere3f {
public:
    mSphere_c() : EGG::Sphere3f(mVec3_c::Zero, 0.0f) {}

    bool isZero() { return fabsf(mRadius) <= FLT_EPSILON; }
};

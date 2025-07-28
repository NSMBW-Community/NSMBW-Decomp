#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/egg/geom/eggSphere.h>
#include <lib/MSL_C/float.h>
#include <lib/MSL_C/math.h>

class mSphere_c : public EGG::Sphere3f {
public:
    mSphere_c() : EGG::Sphere3f(mVec3_c::Zero, 0.0f) {}
    mSphere_c(const mVec3_c &center, float r) : EGG::Sphere3f(center, r) {}

    bool isZero() { return fabsf(mRadius) <= FLT_EPSILON; }
};

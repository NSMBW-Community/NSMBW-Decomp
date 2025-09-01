#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/egg/geom/eggSphere.h>

class mSphere_c : public EGG::Sphere3f {
public:
    mSphere_c() : EGG::Sphere3f(mVec3_c::Zero, 0.0f) {}

    bool isZero() { return std::fabs(mRadius) <= FLT_EPSILON; }
};

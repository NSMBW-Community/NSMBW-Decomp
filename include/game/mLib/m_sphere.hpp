#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/egg/geom/eggSphere.h>

class mSphere_c : public EGG::Sphere3f {
public:
    mSphere_c() {}
    mSphere_c(const mVec3_c &center, float radius) : EGG::Sphere3f(center, radius) {}

    bool isZero() { return std::fabs(mRadius) <= FLT_EPSILON; }
};

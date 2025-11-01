#pragma once
#include <game/mLib/m_vec.hpp>

class dLightMask_c {
public:
    virtual u32 init(void*, u32) = 0; ///< @unofficial
    virtual u32 execute() = 0;
    virtual u32 draw() = 0;
};

class dCircleLightMask_c : public dLightMask_c {

public:
    dCircleLightMask_c() { reset(); };
    void reset() {
        mRadius = 0.0f;
        mMask = nullptr;
        mQuad = nullptr;
    }
    void set(mVec3_c pos, float radius) {
        mPos = pos;
        mRadius = radius;
    }


    virtual u32 init(void*, u32); ///< @unofficial
    virtual u32 execute();
    virtual u32 draw();

    virtual ~dCircleLightMask_c() {}

    mVec3_c mPos;
    float mRadius;
    void * mMask; // actually a dMask_c *
    void * mQuad; // actually a quad_c *
};

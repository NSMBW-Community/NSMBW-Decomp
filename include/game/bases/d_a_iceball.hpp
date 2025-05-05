#pragma once

#include <game/bases/d_actor_state.hpp>

// TODO: Move this to another TU
class dCircleLightMask_c {

public:
    virtual u32 init(void*, u32);
    virtual u32 execute();
    virtual u32 draw();

    virtual ~dCircleLightMask_c();

private:
    mVec3_c mPos;
    float mRadius;
    void * mMask; // actually a dMask_c *
    void * mQuad; // actually a quad_c *
};

class dHeapAllocator {
    u8 pad[0x1c];
};

class daIceBall_c : dActorState_c {

    daIceBall_c();

    u32 m_3d0;
    u32 m_3d4;
    u8 m_unk_3d8[4];
    u32 mLiquidType; // actually a dBc_c::LiquidType
    float mLiquidHeight;
    mVec3_c mSomePos;
    dHeapAllocator mAllocator;
    dCircleLightMask_c mLightMask;
    u8 m_unk_428[8];

public:
    static int sm_IceBallCount[4];
    static int sm_IceBallAliveCount[4];
};

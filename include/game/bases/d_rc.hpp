#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dActor_c;
class dBc_c;
class dRide_ctr_c;

class dRc_c {
public:
    dRc_c();
    virtual ~dRc_c();

    void set(dActor_c *actor, u8 lineKind);

    void clrLink();
    void chkLink();

    dActor_c *mpOwner;
    mVec3_c *mpOwnerPos;
    dRc_c *mpNext;
    dRide_ctr_c *mpRide[2];
    dBc_c *mpBc;
    float m_1c[4];
    short mRotation;
    u8 mLineKind;
    u8 mRideType;
    u8 pad2[2];
    u8 mLayer;
};

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

    bool check2(unsigned long, unsigned long, unsigned long);

    dActor_c *owner;
    mVec3_c *ownerPos;
    dRc_c *next;
    void *ride;
    void *ride2;
    dBc_c *mpBc;
    float pad1[4];
    short rotation;
    bool chainlinkMode;
    u8 rideType;
    u8 pad2[2];
    u8 mLayer;
};

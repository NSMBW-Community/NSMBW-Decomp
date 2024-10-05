#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dActor_c;
class dBc_c;

class dRc_c {
public:
    dRc_c();
    virtual ~dRc_c();

    void clrLink();
    void chkLink();

    void set(dActor_c *, u8);

    dActor_c *owner;
    mVec3_c *ownerPos;
    dRc_c *next;
    void *ride;
    void *ride2;
    dBc_c *bcRef;
    float pad1[4];
    short rotation;
    bool chainlinkMode;
    u8 rideType;
    u8 pad2[2];
    u8 mLayer;
};

#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dActor_c;
class dBc_c;
class dRc_c;

class dRide_ctr_c {
public:
    virtual ~dRide_ctr_c();

    dActor_c *mOwner;
    dRide_ctr_c *mPrev;
    dRide_ctr_c *mNext;
    dRc_c *mLinkedRc;
    mVec2_c mRight;
    mVec2_c mLeft;
    mVec2_c mDiff;
    mVec2_c mLastLeft;
    float mLineLength;
    float mLeftYDeltaSinceLastCalc;
    u32 mFlags;
    s16 mRotation;
    u8 mType;
    bool m_43;
    bool mIsLinked;
    u8 mChainlinkMode;
    u8 m_46;
    u8 mSubType;
    u8 m_48;
    u8 m_49;
    u8 mLayer;
    u8 m_4B;
};

class dRc_c {
public:
    dRc_c();
    virtual ~dRc_c();

    void set(dActor_c *actor, u8 lineKind);

    void setRide(dRide_ctr_c *rideCtr);

    void clrLink();
    void chkLink();

    bool check(unsigned long, unsigned long, unsigned long);
    bool check2(unsigned long, unsigned long, unsigned long);
    bool isRideFlag(unsigned short);

    const dRide_ctr_c *getRide();

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

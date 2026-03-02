#pragma once

#include <game/mLib/m_vec.hpp>

class dActor_c;

class dPole_ctr_c {
public:
    u8 mPad1[0x8];
    dPole_ctr_c *mpPrev;
    dPole_ctr_c *mpNext;
    u8 mPad2[0x10];
    u8 m_20;
    bool m_21;
    int m_24;
    mVec3_c *m_28;
    u8 mPad[0x8];
    short m_34[4];
};

class dPc_c {
public:
    dPc_c() : mpCtr(0) {}
    virtual ~dPc_c() {}

    void release();
    bool check(int);
    void set(dActor_c *, u8);
    void set(int);
    void chkTimer();

    short getAngle() const;
    mVec2_c getPos() { return m_14; }

    void move(float);

    dActor_c *mpOwner;
    u8 mPad1[0x4];
    dPole_ctr_c *mpCtr;
    dPole_ctr_c *mpCtr2;
    mVec2_c m_14;
    u8 mPad2[0xe];
    u8 m_2a;
    u8 mPad3[0x2];
    u8 m_2d;
    mVec3_c m_30;
};

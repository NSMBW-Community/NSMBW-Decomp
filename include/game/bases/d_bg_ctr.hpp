#pragma once

#include <game/bases/d_actor.hpp>

class dBc_c;

class dBg_ctr_c {
public:
    typedef void CallbackF(dActor_c *self, dActor_c *other);
    typedef void CallbackH(dActor_c *self, dActor_c *other);
    typedef void CallbackW(dActor_c *self, dActor_c *other, u8);
    typedef bool CheckRevF(dActor_c *self, dActor_c *other);
    typedef bool CheckRevH(dActor_c *self, dActor_c *other);
    typedef bool CheckRevW(dActor_c *self, dActor_c *other, u8);

    dBg_ctr_c();
    ~dBg_ctr_c();

    void entry();
    void release();
    void set(dActor_c*, float, float, float, float, CallbackF *, CallbackH *, CallbackW *, u8, u8, mVec3_c *);
    void setOfs(float, float, float, float, mVec3_c *);
    void setOfsX1(float x);
    void setOfsX2(float x);
    void setOfsY1(float y);
    void setOfsY2(float y);
    void setAngleY3(short *);
    void calc();

    void addDokanMoveDiff(mVec3_c *);

    dActor_c *mpActor;
    u8 mPad1[0x8];
    dBc_c *mpTriggeredHead;
    dBc_c *mpTriggeredFoot;
    dBc_c *mpTriggeredWall;
    u8 mPad2[0x20];
    dActor_c *mpCarryActor;
    u8 mPad3[0x4];
    CallbackF *mpCallbackFoot;
    CallbackH *mpCallbackHead;
    CallbackW *mpCallbackWall;
    CheckRevF *mpCheckRevFoot;
    CheckRevH *mpCheckRevHead;
    CheckRevW *mpCheckRevWall;
    u8 mPad4[0x48];
    mVec2_c m_a0;
    mVec2_c m_ac;
    u8 mPad5[0xc];
    short *mRotation;
    short m_c0;
    short m_c2;
    u8 mPad6[0x4];
    int m_c8;
    u32 mFlags2;
    int mFlags;
    int m_d4;
    u32 m_d8;
    bool m_dc;
    u8 mAmiLine;
    u8 mLayer;
    int m_e0;
};

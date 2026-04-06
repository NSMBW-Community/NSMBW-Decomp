#pragma once

#include <game/bases/d_actor.hpp>

class dBg_ctr_c {
public:
    dActor_c *mpActor;
    u8 mPad1[0x9c];
    mVec2_c m_a0;
    mVec2_c m_ac;
    u8 mPad2[0xc];
    short *m_bc;
    u8 mPad4[0x8];
    int m_c8;
    int m_cc;
    u32 mFlags;
    int m_d4;
    u8 mpPad5[0x4];
    bool m_dc;
    u8 mpPad6[0x3];
    bool m_e0;
    u8 m_e1;
    u8 m_e2;

    typedef void CallbackF(dActor_c *self, dActor_c *other);
    typedef void CallbackH(dActor_c *self, dActor_c *other);
    typedef void CallbackW(dActor_c *self, dActor_c *other, u8);

    dBg_ctr_c();
    ~dBg_ctr_c();

    void entry();
    void release();
    void set(dActor_c*, float, float, float, float, CallbackF *, CallbackH *, CallbackW *, u8, u8, mVec3_c *);
    void setOfs(float, float, float, float, mVec3_c *);
    void calc();
};

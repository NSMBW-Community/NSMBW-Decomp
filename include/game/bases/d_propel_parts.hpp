#pragma once

#include <types.h>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_effect.hpp>

class dPropelParts_c {
public:
    enum Mode_e {
        PROPEL_MODE_0,
        PROPEL_MODE_1,
        PROPEL_MODE_2,
        PROPEL_MODE_3
    };

    dPropelParts_c();
    void create(dActor_c *);
    void update(const mMtx_c &);
    void finalizePropelFly(s16);

    bool isMode(Mode_e);

    u8 mPad1[0x8];
    int mMode;
    s16 mRollSpeed;
    float mScale;
    mEf::levelEffect_c mLevelEf;
    u8 mPad3[0x4];
};

#pragma once

#include <types.h>
#include <game/bases/d_effect.hpp>

class dAcPy_c;

class dPropelParts_c {
public:
    dPropelParts_c();
    void create(dAcPy_c *);

    u8 mPad1[0x14];
    mEf::levelEffect_c mLevelEf;
    u8 mPad2[0x4];
};

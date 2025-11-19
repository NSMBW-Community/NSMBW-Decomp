#pragma once

#include <types.h>
#include <game/bases/d_effect.hpp>

class dPropelParts_c {
public:
    dPropelParts_c();

    u8 mPad1[0x14];
    mEf::levelEffect_c mLevelEf;
    u8 mPad2[0x4];
};

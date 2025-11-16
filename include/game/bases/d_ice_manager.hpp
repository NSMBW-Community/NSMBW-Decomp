#pragma once

#include <game/bases/d_actor.hpp>

class dIceMng_c {
public:
    dIceMng_c(dActor_c *owner);
    ~dIceMng_c();

    u8 mPad1[0xc];
    int m_0c;
    u8 mPad2[0x8];
    int m_18;
    u8 mPad3[0x14];
    int mPlrNo;
    u8 mPad4[0x38];
};

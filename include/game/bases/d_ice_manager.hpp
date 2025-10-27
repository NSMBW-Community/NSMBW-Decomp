#pragma once

#include <game/bases/d_actor.hpp>

class dIceMng_c {
public:
    dIceMng_c(dActor_c *owner);
    ~dIceMng_c();

    u8 mPad[0x6c];
};

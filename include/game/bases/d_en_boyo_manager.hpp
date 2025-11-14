#pragma once

#include <game/bases/d_actor.hpp>

class dEnBoyoMng_c {
public:
    dEnBoyoMng_c(dActor_c *owner) {
        mScale = mVec3_c(1.0f, 1.0f, 1.0f);
        mFactor = 1.0f;
        mCounter = 0;
        mpOwner = owner;
    }

    void begin(int time, float scale);
    void execute();

    mVec3_c mScale;
    float mFactor;
    float mFactorDelta;
    int mCounter;
    dActor_c *mpOwner;
    u8 mDirection;

    static float smc_DELTA_SCALE;
    static int smc_BOYON_TIME;
};

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

    void begin(int i, float v);
    void execute();

    mVec3_c mScale;
    float mFactor;
    float mFactorDelta;
    int mCounter;
    dActor_c *mpOwner;

    static float smc_DELTA_SCALE;
    static int smc_BOYON_TIME;
};

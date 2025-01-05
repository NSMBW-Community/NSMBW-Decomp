#pragma once
#include <game/bases/d_actor.hpp>

class dScoreMng_c {
public:
    void FUN_800e24b0(float, float, dActor_c *, bool);
    void FUN_800e2190(float, float, dActor_c *, bool, short);
    void FUN_800e2070(mVec3_c &, int, int, int);
    void FUN_800e2310(mVec3_c &, int, int);

    static dScoreMng_c *getInstance() { return m_instance; }

    static dScoreMng_c *m_instance;
    static float smc_SCORE_X;
    static float smc_SCORE_Y;

    static float calcScoreY(float y) { return smc_SCORE_Y + y; }
};

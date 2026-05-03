#pragma once
#include <game/bases/d_actor.hpp>

class dScoreMng_c {
public:
    void ScoreSet(mVec3_c &, ulong, int, int);
    void ScoreSet(dActor_c *, ulong, int, float x = smc_SCORE_X, float y = smc_SCORE_Y);
    void ScoreSet2(mVec3_c &, ulong, int);
    void UnKnownScoreSet(dActor_c *, ulong, float x = smc_SCORE_X, float y = smc_SCORE_Y);
    void fn_800e25a0(ulong, int, int); ///< @unofficial

    static dScoreMng_c *getInstance() { return m_instance; }

    static float smc_SCORE_X;
    static float smc_SCORE_Y;

    static dScoreMng_c *m_instance;
};

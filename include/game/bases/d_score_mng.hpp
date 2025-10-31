#pragma once
#include <game/bases/d_actor.hpp>

class dScoreMng_c {
public:
    void ScoreSet(float, float, dActor_c *, bool, short);
    void ScoreSet(mVec3_c &, ulong, int, int);
    void ScoreSet2(mVec3_c &, ulong, int);
    void UnKnownScoreSet(float, float, dActor_c *, bool);

    static dScoreMng_c *getInstance() { return m_instance; }

    static float smc_SCORE_X;
    static float smc_SCORE_Y;

    static dScoreMng_c *m_instance;
};

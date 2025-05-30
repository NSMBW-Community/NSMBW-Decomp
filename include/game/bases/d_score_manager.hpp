#pragma once
#include <game/bases/d_actor.hpp>

class dScoreMng_c {
public:
    void ScoreSet(dActor_c *, unsigned long, int, float, float);
    void UnKnownScoreSet(dActor_c *, unsigned long, float, float);

    static float smc_SCORE_X;
    static float smc_SCORE_Y;

    static dScoreMng_c *m_instance;
};

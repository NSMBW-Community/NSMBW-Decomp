#pragma once

#include <game/mLib/m_vec.hpp>

class dWmEffectManager_c {
public:
    enum CourseSelectEffectID_e {
        WM_EFFECT_ENEMY_DEATH = 0xe
    };

    int createEffect(CourseSelectEffectID_e, const mVec3_c &, const mAng3_c *, const mVec3_c *);
    void endEffect(int);

    static dWmEffectManager_c *m_pInstance;
};

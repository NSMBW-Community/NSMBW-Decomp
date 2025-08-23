#pragma once

#include <game/mLib/m_vec.hpp>

class dWmEffectManager_c {
public:
    /// @unofficial
    enum EffectID_e {
        WM_EFFECT_ENEMY_DEATH = 0xe
    };

    /// @unofficial
    int playEffect(int, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    /// @unofficial
    void endEffect(int);

    static dWmEffectManager_c *m_pInstance;
};

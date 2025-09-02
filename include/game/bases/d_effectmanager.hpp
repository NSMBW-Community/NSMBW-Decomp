#pragma once
#include <game/mLib/m_vec.hpp>

class EffectManager_c {
public:
    static void SetIceBallMissshitEffect(mVec3_c *);
    static void calcGroupForWm();
    static void calcGroup();

    static bool m_firstCalc;
};

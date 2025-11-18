#pragma once
#include <game/mLib/m_vec.hpp>

class EffectManager_c {
public:
    static void SetIceBallMissshitEffect(mVec3_c *);
    static void SetIceBallEffect(mVec3_c *);
    static void calcGroupForWm();
    static void calcGroup();
    static void SetVsHitEffect(mVec3_c *); ///< @unofficial

    static bool m_firstCalc;
};

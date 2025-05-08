#pragma once

class EffectManager_c {
public:
    static void SetIceBallMissshitEffect(mVec3_c *);
    static void SetIceBallEffect(mVec3_c *);
    static void calcGroupForWm();
    static void calcGroup();

    static bool m_firstCalc;
};

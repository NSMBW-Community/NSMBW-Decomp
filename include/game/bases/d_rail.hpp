#pragma once
#include <game/bases/d_cd.hpp>

class dRail_c {
public:
    virtual ~dRail_c() {}

    bool calculate(u8 id, mVec2_c *v, u32 a, u32 b, u16 c, bool d);
    void calcNextPoint();
    void calcTargetPos();
    virtual void calcSpeed();

    static sRailInfoData *getRailInfoP(u8 id);

    sRailNodeData *m_04;
    float m_08, m_0c;
    mVec2_c *m_10;
    int m_14, m_18;
    u16 m_1c, m_1e, m_20, m_22, m_24, m_26, m_28;
    bool m_2a;
};

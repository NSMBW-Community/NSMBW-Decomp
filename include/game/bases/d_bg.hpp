#pragma once
#include <types.h>

class dBg_c {
public:
    char pad[0x8fea0];
    float mLoopOffset;

    char pad2[0x8fec4 - 0x8fea4];
    float mLiquidHeight;

    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);

    static dBg_c *m_bg_p;
};

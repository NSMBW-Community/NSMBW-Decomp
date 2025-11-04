#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fe64];
    float mL, mR, mU, mD;
    char mPad2[0x2c];
    float mLoopOffset;

    char pad2[0x8fec4 - 0x8fea4];
    float mLiquidHeight;

    float getLeft() const { return mL; }
    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);

    static dBg_c *m_bg_p;
};

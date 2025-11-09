#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fea0];
    float mLoopOffset;
    u8 mPad2[0x20];
    float mLiquidHeight;
    u8 mPad3[0x144];
    float mDispScale;
    float m_8ffa8;
    float mPrevDispScale;


    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);
    void BgUnitChange(u16, u16, int, u16);

    float getDispScale() { return mDispScale; }
    float getPrevDispScale() { return mPrevDispScale; }

    static dBg_c *m_bg_p;
};

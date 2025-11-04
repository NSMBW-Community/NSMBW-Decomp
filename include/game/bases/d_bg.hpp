#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fe64];
    float mL, mR, mU, mD;
    char mPad2[0x2c];
    float mLoopOffset;
    u8 mPad3[0x20];
    float mLiquidHeight;
    u8 mPad4[0x144];
    float mDispScale;
    float m_8ffa8;
    float mPrevDispScale;


    float getLeft() const { return mL; }
    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);

    float getDispScale() { return mDispScale; }
    float getPrevDispScale() { return mPrevDispScale; }

    static dBg_c *m_bg_p;
};

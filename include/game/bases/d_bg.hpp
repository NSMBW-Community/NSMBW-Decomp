#pragma once
#include <types.h>

class dBg_c {
public:
    u8 mPad1[0x8fe70];
    float m_8fe00;
    u8 mPad2[0x2c];
    float mLoopOffset;
    u8 mPad3[0x20];
    float mLiquidHeight;
    u8 mPad4[0x144];
    float mDispScale;
    float m_8ffa8;
    float mPrevDispScale;
    u8 mPad5[0x59];
    u8 m_90009;
    u8 mPad6[0x4c];
    bool m_9004c;

    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);
    float getLeftLimit();
    float getRightLimit();

    float getDispScale() { return mDispScale; }
    float getPrevDispScale() { return mPrevDispScale; }

    static dBg_c *m_bg_p;
};

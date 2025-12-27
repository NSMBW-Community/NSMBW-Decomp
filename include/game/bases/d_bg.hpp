#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fea0];
    float mLoopOffset;
    u8 mPad2[0x20];
    float mLiquidHeight;
    u8 mPad3[0x144];
    float m_8ffa4;
    float m_8ffa8;
    float m_8ffac;


    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);

    float get_8ffa4() { return m_8ffa4; }
    float get_8ffac() { return m_8ffac; }

    static dBg_c *m_bg_p;
};

#pragma once
#include <types.h>

class dBg_c {
public:
    u8 mPad1[0x8fe70];
    float m_8fe00;
    u8 mPad2[0x2c];
    float mLoopOffset;

    void setWaterInWave(float x, float y, u8 type);

    static dBg_c *m_bg_p;
};

#pragma once
#include <types.h>

class dBg_c {
public:
    u8 mPad1[0x8fe70];
    float m_8fe00;
    u8 mPad2[0x2c];
    float mLoopOffset;
    u8 mPad4[0x1d5];
    u8 m_90009;
    u8 mPad5[0x4c];
    bool m_9004c;

    void setWaterInWave(float x, float y, u8 type);
    float getLeftLimit();
    float getRightLimit();

    static dBg_c *m_bg_p;
};

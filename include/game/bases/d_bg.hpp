#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fe64];
    float mL, mR, mU, mD;
    char mPad2[0x2c];
    float mLoopOffset;

    float getLeft() { return mL; }

    void setWaterInWave(float x, float y, u8 type);

    static dBg_c *m_bg_p;
};

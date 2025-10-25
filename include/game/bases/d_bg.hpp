#pragma once
#include <types.h>

class dBg_c {
public:
    char mPad1[0x8fea0];
    float mLoopOffset;
    u8 mPad2[0x168];
    float m_8ffa4;
    float m_8ffa8;
    float mDispScale;

    void setWaterInWave(float x, float y, u8 type);

    float getDispScale() {
        return mDispScale;
    }
    float getM_8ffa4() {
        return m_8ffa4;
    }

    static dBg_c *m_bg_p;
};

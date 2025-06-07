#pragma once
#include <types.h>

class dBg_c {
public:
    char pad[0x8fea0];
    float mLoopOffset;

    void setWaterInWave(float x, float y, u8 type);

    static dBg_c *m_bg_p;
};

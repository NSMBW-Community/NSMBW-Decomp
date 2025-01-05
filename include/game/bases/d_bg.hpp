#pragma once

class dBg_c {
public:
    void setWaterInWave(float, float, u8);

    char pad[0x8fea0];
    float mLoopOffset;

    static dBg_c *m_bg_p;
};

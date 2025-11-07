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
    float mWaveRelated[80];
    u8 mPad5[0x118];
    float mDispScale;
    float m_8ffa8;
    float mPrevDispScale;
    float mWaveX[20];
    float mWaveY[20];
    u8 mWaveType[20];
    u8 mWaveActive[20];

    static dBg_c *getInstance() { return m_bg_p; }

    float getLeft() { return mL; }
    float getWaveX(int i) const { return mWaveX[i]; }
    float &getWaveRelated(int i) { return mWaveRelated[i]; }
    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);

    float getDispScale() { return mDispScale; }
    float getPrevDispScale() { return mPrevDispScale; }

    static dBg_c *m_bg_p;
};

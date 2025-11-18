#pragma once
#include <game/mLib/m_vec.hpp>

class dBg_c {
    class dBg_autoScroll_c {
    public:
        dBg_autoScroll_c() {}
        ~dBg_autoScroll_c() {}

        mVec3_c mPos;
        float m_0c;
        float m_10;
        u8 m_14, m_15, m_16, m_17;
        u8 m_18;
        bool m_19;
        bool mActive;
    };

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
    u8 mPad5[0x61];
    u8 m_90009;
    u8 mPad6[0x30];
    dBg_autoScroll_c mAutoscrolls[2];
    u8 mPad7[0x1a];
    u8 m_9008e;

    float getLiquidHeight() const { return mLiquidHeight; }

    void setWaterInWave(float x, float y, u8 type);
    float getLeftLimit();
    float getRightLimit();

    float getDispScale() { return mDispScale; }
    float getPrevDispScale() { return mPrevDispScale; }

    static dBg_c *m_bg_p;
};

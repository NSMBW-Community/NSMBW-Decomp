#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dQuake_c {
public:
    enum TYPE_SHOCK_e {
        TYPE_0,
        TYPE_1,
        TYPE_2,
        TYPE_3,
        TYPE_4,
        TYPE_5,
        TYPE_6,
        TYPE_7,
        TYPE_8
    };

    float mSpeed;
    float mResult;
    float mMultiplier;
    float mAccel;
    float mMin;
    float mSpeed2;
    float mResult2;
    float mMultiplier2;
    float mAccel2;
    float mMin2;
    mVec2_c mScreenOffset;
    u32 mFlags; ///< &1 to call dEn_c::add2ToYSpeed() &2 to call dEn_c::powBlockActivated(0) &4 to call dEn_c::powBlockActivated(1)
    u32 m_34;
    bool mQuakeOn;
    u8 mPlayerId;
    u8 mPadding[2];
    int mPOWLength;
    int mMPGPLength;

    static dQuake_c *m_instance;
};

#pragma once

#include <game/mLib/m_vec.hpp>

/// @unofficial
struct sPowerChangeSpeedData {
    float mDefaultAccel;
    float mNoInputAccel;
    float mTurnNoInputAccel;
    float mTurnAccel;
    float mVerySlowAccel;
    float mSlowAccel;
    float mRunSlowAccel;
    float mMediumAccel;
    float mFastAccel;
};

/// @unofficial
struct sSpeedData {
    float mLowSpeed, mMediumSpeed, mHighSpeed;
    sPowerChangeSpeedData mPowerChangeNormal;
    sPowerChangeSpeedData mPowerChangeIce;
    sPowerChangeSpeedData mPowerChangeLowSlip;
};

class dAcPy_HIO_Speed_c {
public:
    dAcPy_HIO_Speed_c();
    ~dAcPy_HIO_Speed_c();

    sSpeedData mDataNormal;
    sSpeedData mDataStar;

    static sSpeedData sc_player_mame;
    static sSpeedData sc_player_mame_star;
    static sSpeedData sc_player_normal;
    static sSpeedData sc_player_normal_star;
    static const float sc_gravity_data[];
};

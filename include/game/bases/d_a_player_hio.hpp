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
};

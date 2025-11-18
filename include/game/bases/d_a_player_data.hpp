#pragma once

/// @unofficial
struct sAirTurnPowerData {
    float mNoButton;
    float mStand;
    float mSlowNoDash;
    float mSlowDash;
    float mMedium;
    float mFast;
    float mTurnAround;
};

/// @unofficial
struct sTurnPowerData {
    float mNormal;
    float mSakaUp;
    float mSakaDown;
    float mAir;
};

/// @unofficial
struct sPowerChangeData {
    sAirTurnPowerData mAirPower[2];
    sTurnPowerData mRangeType0[2];
    sTurnPowerData mRangeType1[2];
    sTurnPowerData mRangeType2[2];
    float mGravityData[2][18];
    float mGravityData2[18];
    float mJumpSpeedValues1[3];
    float mJumpSpeedValues2[4];
};

/// @unofficial
struct SpeedData_t {
    float data[9];

    SpeedData_t *operator=(const SpeedData_t &other) {
        for (int i = 0; i < 9; ++i) {
            data[i] = other.data[i];
        }
        return this;
    }
};

/// @unofficial
namespace daPlayerData_c {
    extern const sPowerChangeData smc_POWER_CHANGE_DATA;
}

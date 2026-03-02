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
    sTurnPowerData mTurnPowerNormal[2];
    sTurnPowerData mTurnPowerIce[2];
    sTurnPowerData mTurnPowerLowSlip[2];
    float mGravityData[2][18];
    float mGravityData2[18];
    float mJumpSpeedValues1[3];
    float mJumpSpeedValues2[4];
};

/// @unofficial
namespace daPlayerData_c {
    extern const sPowerChangeData smc_POWER_CHANGE_DATA;
}

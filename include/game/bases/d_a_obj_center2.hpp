#pragma once

#include <game/bases/d_actor_state.hpp>

/// @brief A controller for rotating objects around a center point.
/// @statetable
/// @paramtable
/// @ingroup bases
class daObjCenter2_c : public dActorState_c {
public:
    daObjCenter2_c() {}
    ~daObjCenter2_c() {}

    virtual int create() override;
    virtual int execute() override;

    STATE_FUNC_DECLARE(daObjCenter2_c, ReturnHome);
    STATE_FUNC_DECLARE(daObjCenter2_c, GoAim);
    STATE_FUNC_DECLARE(daObjCenter2_c, Move);

    void coinSE();
    short getAngle();
    void decideCoinSound();

    int mAngDelta;
    int mPad; ///< @unused
    short mRotationSpeed;
    short mRotation;
    short mTargetRotation;
    bool mCoinSoundRelated;
    bool mCoinSoundPlayed;
    u8 mInitialState;
    u8 mDirection;
    u8 mRotationID;
    u8 mRotationMode;

    ACTOR_PARAM_CONFIG(RotID, 0, 8);
    ACTOR_PARAM_CONFIG(RotateShock, 8, 1);
    ACTOR_PARAM_CONFIG(PlayPOW, 9, 1);
    ACTOR_PARAM_CONFIG(RotationMode, 10, 2);
    ACTOR_PARAM_CONFIG(IsCCW, 12, 1);
    ACTOR_PARAM_CONFIG(InitialState, 16, 2);
    ACTOR_PARAM_CONFIG(RotationSpeed, 20, 3);
    ACTOR_PARAM_CONFIG(InitialDir2, 24, 4);
    ACTOR_PARAM_CONFIG(InitialDir, 28, 4);
};

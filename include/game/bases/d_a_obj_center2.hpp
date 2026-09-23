#pragma once

#include <game/bases/d_actor_state.hpp>

/// @brief A controller for rotating objects around a center point.
/// @statetable
/// @paramtable
/// @ingroup bases
class daObjCenter2_c : public dActorState_c {
public:
    /// @brief The possible rotation modes of the controller.
    enum RotationMode_e {
        MODE_BACK_AND_FORTH, ///< Move to the stop target when active, move back when inactive.
        MODE_PERMANENT, ///< Permamently rotate in one direction.
        MODE_WHILE_ACTIVE ///< Rotate by TargetRotationOffset when the linked event is active.
    };

    daObjCenter2_c() {} ///< @copydoc dActorState_c::dActorState_c
    ~daObjCenter2_c() {} //@copydoc dActorState_c::~dActorState_c

    virtual int create() override;
    virtual int execute() override;

    STATE_FUNC_DECLARE(daObjCenter2_c, ReturnHome); ///< Returning to the home rotation.
    STATE_FUNC_DECLARE(daObjCenter2_c, GoAim); ///< Rotating to the stop rotation.
    STATE_FUNC_DECLARE(daObjCenter2_c, Move); ///< Rotating in one direction.

    void coinSE(); ///< Plays a coin sound effect if the actor is configured to do so.
    short getAngle(); ///< Returns the home rotation angle of this rotation controller.
    void tryPlayCoinSE(); ///< Plays a coin sound effect if it has not been played yet this rotation.

    int mAngDelta; ///< The incremental change in rotation per frame.
    u8 mPad[4]; ///< @unused
    short mRotationSpeed; ///< The angular speed of the rotation.
    short mHomeRotation; ///< The starting rotation.
    short mTargetRotation; ///< The current rotation the controller is targetting.
    bool mCoinSoundReady; ///< Whether the coin sound is ready to be played.
    bool mCoinSoundPlayed; ///< Whether the coin sound has been played.
    u8 mRotationMode; ///< The rotation mode. See RotationMode_e.
    u8 mRotDir; ///< The rotation direction. See DIRECTION_LR_e (right = clockwise, left = counter-clockwise).
    u8 mRotationID; ///< The rotation ID to be set on the affected objects.
    u8 mZoomControl; ///< Whether the zoom level is controlled by the rotation controller. [Unknown what 1 and 2 mean].

    ACTOR_PARAM_CONFIG(RotID, 0, 8); ///< @copydoc mRotationID
    ACTOR_PARAM_CONFIG(RotateShock, 8, 1); ///< Shakes the screen when the rotation is complete.
    ACTOR_PARAM_CONFIG(RotateCoinSE, 9, 1); ///< Enables playing a coin sound effect at the end of a rotation.
    ACTOR_PARAM_CONFIG(ZoomControl, 10, 2); ///< @copydoc mZoomControl
    ACTOR_PARAM_CONFIG(IsCCW, 12, 1); ///< The rotation direction, 0 for clockwise, 1 for counter-clockwise.
    ACTOR_PARAM_CONFIG(RotationMode, 16, 2); ///< @copydoc mRotationMode
    ACTOR_PARAM_CONFIG(RotationSpeed, 20, 3); ///< The angular speed of the rotation. See l_RotSpeed in create() for the possible values.
    ACTOR_PARAM_CONFIG(TargetRotationOffset, 24, 4); ///< The target rotation offset for #MODE_BACK_AND_FORTH and #MODE_WHILE_ACTIVE.
    ACTOR_PARAM_CONFIG(InitialRotation, 28, 4); ///< The starting rotation.
};

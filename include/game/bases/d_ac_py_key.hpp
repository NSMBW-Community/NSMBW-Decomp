#pragma once
#include <types.h>

/// @brief A class for managing controller input.
class dAcPyKey_c {
public:
    /// @brief Status flags for the controller manager.
    enum STATUS_e {
        STATUS_NO_INPUT = 0, ///< Disable all inputs.
        STATUS_DISABLE_LR = 1, ///< Disable left and right directional buttons.
        STATUS_FORCE_JUMP = 3, ///< Force the jump button to be pressed down.
        STATUS_FORCE_NO_JUMP = 4, ///< Force a jump input to be ignored.
        STATUS_SHAKE_COOLDOWN = 6, ///< Is in cooldown for shake events
        STATUS_DEMO = 7, ///< Do not use inputs from the remote.
        STATUS_FORCE_RIGHT = 8, ///< Override the input to always only be the right button.

        FLAG_NO_INPUT = BIT_FLAG(STATUS_NO_INPUT),
        FLAG_DISABLE_LR = BIT_FLAG(STATUS_DISABLE_LR),
        FLAG_FORCE_JUMP = BIT_FLAG(STATUS_FORCE_JUMP),
        FLAG_FORCE_NO_JUMP = BIT_FLAG(STATUS_FORCE_NO_JUMP),
        FLAG_SHAKE_COOLDOWN = BIT_FLAG(STATUS_SHAKE_COOLDOWN),
        FLAG_DEMO = BIT_FLAG(STATUS_DEMO),
        FLAG_FORCE_RIGHT = BIT_FLAG(STATUS_FORCE_RIGHT),
    };

    /// @brief Button flags for the controller manager.
    enum BUTTONS_e {
        BUTTON_DOWN = BIT_FLAG(0),
        BUTTON_UP = BIT_FLAG(1),
        BUTTON_RIGHT = BIT_FLAG(2),
        BUTTON_LEFT = BIT_FLAG(3),
        BUTTON_CROSS = BUTTON_LEFT | BUTTON_RIGHT | BUTTON_UP | BUTTON_DOWN,
        BUTTON_TWO = BIT_FLAG(8),
        BUTTON_ONE = BIT_FLAG(9),
        BUTTON_B = BIT_FLAG(10),
        BUTTON_A = BIT_FLAG(11),
    };

    int mRemoconID; ///< This remote's ID, -1 if invalid.
    u16 mDownButtons; ///< The buttons currently pressed down.
    u16 mTriggeredButtons; ///< The buttons newly pressed down.
    u16 mPrevDownButtons; ///< The buttons pressed down in the last frame.
    u16 mPrevTriggeredButtons; ///< The buttons newly pressed down in the last frame.
    u16 mDownButtonsDemo; ///< The buttons currently pressed down in demo mode.
    u16 mTriggeredButtonsDemo; ///< The buttons newly pressed down in demo mode.
    u16 mDemoDoShake; ///< Trigger a shake event in demo mode.
    u16 mStatus; ///< Uses FLAGS_e enum.
    u8 mHipAtkHoldCount; ///< Counts how long the down button is held for the hip attack.
    bool mHipAtkRead; ///< Whether hip attack was checked for this frame.
    bool mHipAtkHoldStart; ///< Whether the hip attack holding started.
    u8 mTriggerShakeJump; ///< Whether a shake was triggered this frame.
    u8 mActiveShakeJump; ///< Whether a shake is active this frame.
    bool mActionTriggered; ///< Set if eat, fire or jump is triggered.
    int mShakeCooldown; ///< Cooldown counter for shake events.
    int m_20; ///< [Unused].

    // [Were these meant to be static?]
    int mJumpTriggeredHistory[4][10]; ///< Stores the last 10 frames of jump events for each remote.
    int mJumpButtonHistory[4][10]; ///< Stores the last 10 frames of the jump button state for each remote.

    dAcPyKey_c(); ///< Constructs a new controller manager.
    ~dAcPyKey_c(); ///< Destroys the controller manager.

    void init(); ///< Initialize the button manager.
    void update(); ///< Call each frame to process the inputs.
    void updateEnd(); ///< Get the new button states for the next frame.

    void onStatus(u16 status); ///< Enables a status flag.
    void offStatus(u16 status); ///< Disables a status flag.

    void clearAllKey(); ///< Clears all input state.
    void clearCrossKey(); ///< Clears the left and right directional buttons.

    int buttonCross() const; ///< Returns whether a directional button is pressed down.
    int buttonUp() const; ///< Returns whether the up button is pressed down.
    int buttonDown() const; ///< Returns whether the down button is pressed down.
    int buttonLeft() const; ///< Returns whether the left button is pressed down.
    int buttonRight() const; ///< Returns whether the right button is pressed down.
    int buttonTwo() const; ///< Returns whether the two button is pressed down.
    int buttonOne() const; ///< Returns whether the one button is pressed down.
    int buttonDush() const; ///< Returns whether the run button is pressed down.
    int buttonCarry() const; ///< Returns whether the carry button (1 / B) is pressed down.
    int buttonJump(); ///< Returns whether the jump button (2) is pressed down.
    int buttonYoshiJump() const; ///< Returns whether the Yoshi jump button (2) is pressed down.
    bool buttonWalk(int *direction) const; ///< Returns whether left / right is pressed down. Sets the direction to 0 for right and 1 for left.
    int buttonCrouch() const; ///< Returns whether the crouch button (down) is pressed down.
    int buttonHipAttack() const; ///< Returns whether the correct buttons (down, no other directions) for a hip attack is pressed down.
    int buttonDoor() const; ///< Returns whether the correct buttons (up, no left or right) for a door open is pressed down.

    int triggerA() const; ///< Returns whether A was pressed this frame.
    int triggerCross() const; ///< Returns whether a directional button was pressed this frame.
    int triggerUp() const; ///< Returns whether the up button was pressed this frame.
    int triggerDown() const; ///< Returns whether the down button was pressed this frame.
    int triggerLeft() const; ///< Returns whether the left button was pressed this frame.
    int triggerRight() const; ///< Returns whether the right button was pressed this frame.
    int triggerTwo() const; ///< Returns whether the two button was pressed this frame.
    int triggerOne() const; ///< Returns whether the one button was pressed this frame.
    int triggerAttack() const; ///< Returns whether the attack button (1 / B) was pressed this frame.
    bool triggerEat(); ///< Returns whether the eat button (1 / B) was pressed this frame.
    bool triggerFire(); ///< Returns whether the fire button was pressed this frame.
    int triggerJumpBase() const; ///< Returns whether the jump button (2) was pressed this frame.
    bool triggerJump(); ///< Returns whether the jump button (2) was pressed this frame.

    bool checkHipAttack(); ///< Checks whether a hip attack should be performed this frame.

    u8 triggerShakeJump() const; ///< Returns whether a shake event was triggered this frame.
    void clearShakeJump(); ///< Clears the shake event state.

    bool triggerJumpBuf(int n); ///< Returns whether a jump occurred in the last n frames.

    void onDemoButton(int); ///< Sets a button for demo mode to pressed down.
    void offDemoButton(int); ///< Sets a button for demo mode to not pressed down.
    void onDemoTrigger(int); ///< Sets a button for demo mode to pressed this frame.
    void onDemoShake(); ///< Sets a shake event for demo mode to be triggered this frame.
};

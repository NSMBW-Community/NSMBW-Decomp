#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_game_key.hpp>

dAcPyKey_c::dAcPyKey_c() :
    mRemoconID(0),
    mDownButtons(0), mTriggeredButtons(0),
    mDownButtonsDemo(0), mTriggeredButtonsDemo(0), mDemoDoShake(0) {
}

dAcPyKey_c::~dAcPyKey_c() {}

void dAcPyKey_c::init() {
    mRemoconID = -1;
}

void dAcPyKey_c::update() {
    mPrevDownButtons = mDownButtons;
    mPrevTriggeredButtons = mTriggeredButtons;

    if (mRemoconID == -1) {
        mDownButtons = 0;
        mTriggeredButtons = 0;
        return;
    }

    if (mStatus & FLAG_NO_INPUT) {
        mDownButtons = 0;
        mTriggeredButtons = 0;
    } else if (mStatus & FLAG_DEMO) {
        mDownButtons = mDownButtonsDemo | mTriggeredButtonsDemo;
        mTriggeredButtons = mDownButtons & ~mPrevDownButtons;
    } else if (mStatus & FLAG_FORCE_RIGHT) {
        mDownButtons = BUTTON_RIGHT;
        mTriggeredButtons = 0;
    } else {
        dGameKey_c *gameKey = dGameKey_c::m_instance;
        mDownButtons = gameKey->mRemocon[mRemoconID]->mDownButtons;
        mTriggeredButtons = gameKey->mRemocon[mRemoconID]->mTriggeredButtons;
        if (mStatus & FLAG_DISABLE_LR) {
            clearCrossKey();
        }
    }

    if (mStatus & FLAG_SHAKE_COOLDOWN) {
        offStatus(STATUS_SHAKE_COOLDOWN);
        mShakeCooldown = 10;
    }
    if (mShakeCooldown != 0) {
        mShakeCooldown--;
    }

    mActiveShakeJump = 0;
    mTriggerShakeJump = 0;
    if (!(mStatus & FLAG_NO_INPUT) && !(mStatus & FLAG_FORCE_RIGHT)) {
        if (mStatus & FLAG_DEMO) {
            mActiveShakeJump = mDemoDoShake;
            mTriggerShakeJump = mDemoDoShake;
        } else if (mShakeCooldown == 0 && dGameKey_c::m_instance->mRemocon[mRemoconID]->mIsShaking) {
            mActiveShakeJump = 1;
            mTriggerShakeJump = 1;
        }
    }

    if (!mHipAtkRead) {
        mHipAtkHoldStart = false;
        mHipAtkHoldCount = 0;
    }
    mHipAtkRead = false;

    int *triggerHistory = mJumpTriggeredHistory[mRemoconID];
    int *buttonHistory = mJumpButtonHistory[mRemoconID];
    for (int i = ARRAY_SIZE(mJumpTriggeredHistory[0]) - 1; i >= 1; i--) {
        triggerHistory[i] = triggerHistory[i - 1];
        buttonHistory[i] = buttonHistory[i - 1];
    }
    triggerHistory[0] = triggerJumpBase();
    buttonHistory[0] = buttonJump();
    mTriggeredButtonsDemo = 0;
    mDemoDoShake = 0;
}

void dAcPyKey_c::updateEnd() {
    mActionTriggered = false;
    if (mRemoconID == -1) {
        return;
    }
    if (mStatus & FLAG_DEMO) {
        return;
    }
    mDownButtons = dGameKey_c::m_instance->mRemocon[mRemoconID]->mDownButtons;
    mTriggeredButtons = dGameKey_c::m_instance->mRemocon[mRemoconID]->mTriggeredButtons;
}

bool dAcPyKey_c::checkHipAttack() {
    mHipAtkRead = true;
    if (!mHipAtkHoldStart) {
        if (triggerDown()) {
            mHipAtkHoldStart = true;
            mHipAtkHoldCount = 0;
        }
    } else if (buttonDown()) {
        if (buttonHipAttack() && ++mHipAtkHoldCount >= 4) {
            return true;
        }
    } else {
        mHipAtkHoldStart = false;
    }
    return false;
}

void dAcPyKey_c::onStatus(u16 status) {
    switch (status) {
        case STATUS_FORCE_JUMP:
            offStatus(STATUS_FORCE_NO_JUMP);
            break;

        case STATUS_FORCE_NO_JUMP:
            offStatus(STATUS_FORCE_JUMP);
            break;

        case STATUS_NO_INPUT:
        case STATUS_DEMO:
        case STATUS_FORCE_RIGHT:
            clearAllKey();
            break;

        case STATUS_DISABLE_LR:
            clearCrossKey();
            break;

        default:
            break;
    }
    mStatus |= (1 << status);
}

void dAcPyKey_c::offStatus(u16 status) {
    mStatus &= ~(1 << status);
}

void dAcPyKey_c::clearAllKey() {
    mDownButtons = 0;
    mTriggeredButtons = 0;
    mDownButtonsDemo = 0;
    mTriggeredButtonsDemo = 0;
    mActiveShakeJump = 0;
    mTriggerShakeJump = 0;
}

void dAcPyKey_c::clearCrossKey() {
    mDownButtons &= ~(BUTTON_LEFT | BUTTON_RIGHT);
    mTriggeredButtons &= ~(BUTTON_LEFT | BUTTON_RIGHT);
}

int dAcPyKey_c::triggerA() const {
    return mTriggeredButtons & BUTTON_A;
}

int dAcPyKey_c::buttonCross() const {
    return mDownButtons & BUTTON_CROSS;
}

int dAcPyKey_c::triggerCross() const {
    return mTriggeredButtons & BUTTON_CROSS;
}

int dAcPyKey_c::buttonUp() const {
    return mDownButtons & BUTTON_UP;
}

int dAcPyKey_c::buttonDown() const {
    return mDownButtons & BUTTON_DOWN;
}

int dAcPyKey_c::buttonLeft() const {
    return mDownButtons & BUTTON_LEFT;
}

int dAcPyKey_c::buttonRight() const {
    return mDownButtons & BUTTON_RIGHT;
}

int dAcPyKey_c::triggerUp() const {
    return mTriggeredButtons & BUTTON_UP;
}

int dAcPyKey_c::triggerDown() const {
    return mTriggeredButtons & BUTTON_DOWN;
}

int dAcPyKey_c::triggerLeft() const {
    return mTriggeredButtons & BUTTON_LEFT;
}

int dAcPyKey_c::triggerRight() const {
    return mTriggeredButtons & BUTTON_RIGHT;
}

int dAcPyKey_c::triggerTwo() const {
    return mTriggeredButtons & BUTTON_TWO;
}

int dAcPyKey_c::buttonTwo() const {
    return mDownButtons & BUTTON_TWO;
}

int dAcPyKey_c::triggerOne() const {
    if (dGameKey_c::m_instance->mRemocon[mRemoconID]->mAttachedExtension == dGameKeyCore_c::EXTENSION_NUNCHUK) {
        return mTriggeredButtons & BUTTON_B;
    } else {
        return mTriggeredButtons & BUTTON_ONE;
    }
}

int dAcPyKey_c::buttonOne() const {
    if (dGameKey_c::m_instance->mRemocon[mRemoconID]->mAttachedExtension == dGameKeyCore_c::EXTENSION_NUNCHUK) {
        return mDownButtons & BUTTON_B;
    } else {
        return mDownButtons & BUTTON_ONE;
    }
}

int dAcPyKey_c::buttonDush() const {
    return mDownButtons & BUTTON_ONE;
}

int dAcPyKey_c::triggerAttack() const {
    return triggerOne();
}

bool dAcPyKey_c::triggerEat() {
    if (triggerOne()) {
        mActionTriggered = true;
        return true;
    }
    return false;
}

int dAcPyKey_c::buttonCarry() const {
    return buttonOne();
}

bool dAcPyKey_c::triggerFire() {
    if (triggerOne()) {
        mActionTriggered = true;
        return true;
    }
    return false;
}

int dAcPyKey_c::triggerJumpBase() const {
    return triggerTwo();
}

bool dAcPyKey_c::triggerJump() {
    if (triggerJumpBase()) {
        mActionTriggered = true;
        return true;
    }
    return false;
}

int dAcPyKey_c::buttonJump() {
    if (mStatus & FLAG_FORCE_JUMP) {
        return true;
    }
    if (mStatus & FLAG_FORCE_NO_JUMP) {
        return false;
    }
    return buttonTwo();
}

int dAcPyKey_c::buttonYoshiJump() const {
    return buttonTwo();
}

int dAcPyKey_c::triggerShakeJump() const {
    return mTriggerShakeJump;
}

void dAcPyKey_c::clearShakeJump() {
    mTriggerShakeJump = 0;
    mActiveShakeJump = 0;
}

bool dAcPyKey_c::buttonWalk(int *direction) const {
    if (buttonRight()) {
        if (direction != nullptr) {
            *direction = 0;
        }
        return true;
    } else if (buttonLeft()) {
        if (direction != nullptr) {
            *direction = 1;
        }
        return true;
    }
    return false;
}

int dAcPyKey_c::buttonCrouch() const {
    return buttonDown();
}

int dAcPyKey_c::buttonHipAttack() const {
    if (buttonCross() & (BUTTON_UP | BUTTON_LEFT | BUTTON_RIGHT)) {
        return 0;
    }
    return buttonDown();
}

int dAcPyKey_c::buttonDoor() const {
    if (mDownButtons & (BUTTON_LEFT | BUTTON_RIGHT)) {
        return 0;
    }
    return buttonUp();
}

bool dAcPyKey_c::triggerJumpBuf(int lastN) {
    if (mRemoconID == -1) {
        return false;
    }
    if (lastN > 10) {
        lastN = 10;
    }
    int *history = mJumpTriggeredHistory[mRemoconID];
    for (int i = 0; i < lastN; i++) {
        if (history[i]) {
            return true;
        }
    }
    return false;
}

void dAcPyKey_c::onDemoButton(int buttons) {
    if (mStatus & FLAG_DEMO) {
        mDownButtonsDemo |= buttons;
    }
}

void dAcPyKey_c::offDemoButton(int buttons) {
    if (mStatus & FLAG_DEMO) {
        mDownButtonsDemo &= ~buttons;
    }
}

void dAcPyKey_c::onDemoTrigger(int buttons) {
    if (mStatus & FLAG_DEMO) {
        mTriggeredButtonsDemo |= buttons;
    }
}

void dAcPyKey_c::onDemoShake() {
    if (mStatus & FLAG_DEMO) {
        mDemoDoShake = 1;
    }
}

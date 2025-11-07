#include <game/bases/d_enemy_carry.hpp>
#include <game/bases/d_a_player_manager.hpp>

STATE_VIRTUAL_DEFINE(dEnemyCarry_c, Carry);
STATE_VIRTUAL_DEFINE(dEnemyCarry_c, Throw);

const float dEnemyCarry_c::smc_DIR_MODE1[2] = {2.5f, -2.5f};
const float dEnemyCarry_c::smc_DIR_MODE2[2] = {3.0f, -3.0f};

dEnemyCarry_c::dEnemyCarry_c() {}

void dEnemyCarry_c::setThrowSpeed(dActor_c *actor) {
    float endSpeed;
    if (mIsCarried == 0) {
        endSpeed = smc_DIR_MODE1[mDirection];
    } else {
        endSpeed = smc_DIR_MODE2[mDirection];
    }
    if (mDirection == (actor->mSpeed.x < 0.0f)) {
        endSpeed += actor->mSpeed.x * 0.5f;
    }

    if (endSpeed < mSpeedMax.x) {
        float tmp;
        if (endSpeed > mSpeedMax.x) {
            tmp = mSpeedMax.x;
        } else {
            tmp = endSpeed;
        }
        endSpeed = tmp;
    } else if (endSpeed > mSpeedMax.x) {
        float tmp;
        if (endSpeed < mSpeedMax.x) {
            tmp = mSpeedMax.x;
        } else {
            tmp = endSpeed;
        }
        endSpeed = tmp;
    }
    mSpeed.set(endSpeed, 0.0f, 0.0f);
}

void dEnemyCarry_c::setThrowChangeState() {
    mIsCarry = false;
    changeState(StateID_Throw);
}

void dEnemyCarry_c::setPutOnChangeState() {
    mIsCarry = true;
    changeState(StateID_Throw);
}

void dEnemyCarry_c::setDeathInfo_CarryBgIn(dActor_c *actor) {
    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieFall,
        -1,
        -1,
        mDirection,
        (u8) *actor->getPlrNo()
    };
}

void dEnemyCarry_c::initializeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    if (player->m_1035 == 1) {
        mAmiLayer = 0;
    } else {
        mAmiLayer = 1;
    }
    mCc.mAmiLine = l_Ami_Line[mAmiLayer];
    mBc.mAmiLine = l_Ami_Line[mAmiLayer];
    mCc.mCcData.mCategoryInteract |= BIT_FLAG(dCc_c::CAT_CANNON); // [Would have expected CAT_PROJECTILE here?]
    mCc.mCcData.mAttackCategory = dCc_c::ATTACK_SHELL;
    mRc.setRide(nullptr);
    mIsCarry = false;
}

void dEnemyCarry_c::finalizeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    player->cancelCarry(this);
    mCc.mCcData.mAttackCategory = dCc_c::CAT_PLAYER_GENERIC;
    mCc.mCcData.mCategoryInteract &= ~BIT_FLAG(dCc_c::CAT_CANNON);
    mRc.setRide(nullptr);
    mBc.mFlags = 0;
    mCarryingFlags &= ~(CARRY_RELEASE | CARRY_THROW);
}

void dEnemyCarry_c::executeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    if (mCarryingFlags & CARRY_RELEASE) {
        mDirection = mThrowDirection;
        mAngle.y = l_base_angleY[mDirection];
        if (checkWallAndBg()) {
            setDeathInfo_CarryBgIn(player);
        } else {
            if ((mBc.mFlags & 0x15 << mDirection) == 0) {
                mPos.x += l_EnMuki[mDirection] * 6.0f;
            }
            mPlayerNo = *player->getPlrNo();
            if (mCarryingFlags & CARRY_THROW) {
                setThrowSpeed(player);
                setThrowChangeState();
            } else {
                mPos.y += 2.0f;
                setPutOnChangeState();
            }
        }
    }
}

void dEnemyCarry_c::initializeState_Throw() {
    mNoHitPlayer.mTimer[mPlayerNo] = 12;
    mCc.mCcData.mCategoryInteract |= BIT_FLAG(dCc_c::CAT_CANNON) | BIT_FLAG(dCc_c::CAT_ITEM);
    if (!mIsCarry) {
        mCc.mCcData.mAttackCategory = dCc_c::ATTACK_SHELL;
    }
}

void dEnemyCarry_c::finalizeState_Throw() {
    mCc.mCcData.mCategoryInteract &= ~(BIT_FLAG(dCc_c::CAT_CANNON) | BIT_FLAG(dCc_c::CAT_ITEM));
    mCc.mCcData.mAttackCategory = dCc_c::CAT_PLAYER_GENERIC;
    mAccelF = 0.0f;
    mSpeedMax.x = 0.0f;
}

void dEnemyCarry_c::executeState_Throw() {}

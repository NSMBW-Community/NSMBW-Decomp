#include <game/bases/d_a_player.hpp>
#include <game/sLib/s_GlobalData.hpp>
#include <constants/sound_list.h>

const float dAcPy_c::msc_JUMP_SPEED = daPlBase_c::sc_JumpSpeed;

STATE_VIRTUAL_DEFINE(dAcPy_c, Walk);
STATE_VIRTUAL_DEFINE(dAcPy_c, Jump);
STATE_VIRTUAL_DEFINE(dAcPy_c, Fall);
STATE_VIRTUAL_DEFINE(dAcPy_c, Land);
STATE_VIRTUAL_DEFINE(dAcPy_c, Crouch);
STATE_VIRTUAL_DEFINE(dAcPy_c, SitJump);
STATE_VIRTUAL_DEFINE(dAcPy_c, Slip);
STATE_VIRTUAL_DEFINE(dAcPy_c, Turn);
STATE_VIRTUAL_DEFINE(dAcPy_c, HipAttack);
STATE_VIRTUAL_DEFINE(dAcPy_c, Swim);
STATE_VIRTUAL_DEFINE(dAcPy_c, Kani);
STATE_VIRTUAL_DEFINE(dAcPy_c, Cloud);

STATE_DEFINE(dAcPy_c, RideOffJump);
STATE_DEFINE(dAcPy_c, SpinHipAttack);
STATE_DEFINE(dAcPy_c, RollSlip);
STATE_DEFINE(dAcPy_c, Vine);
STATE_DEFINE(dAcPy_c, Hang);
STATE_DEFINE(dAcPy_c, Pole);
STATE_DEFINE(dAcPy_c, Fire);
STATE_DEFINE(dAcPy_c, LiftUp);
STATE_DEFINE(dAcPy_c, Throw);
STATE_DEFINE(dAcPy_c, PropelThrow);
STATE_DEFINE(dAcPy_c, WallJump);
STATE_DEFINE(dAcPy_c, WallSlide);
STATE_DEFINE(dAcPy_c, Propel);
STATE_DEFINE(dAcPy_c, CarryPlayer);
STATE_DEFINE(dAcPy_c, RideYoshi);
STATE_DEFINE(dAcPy_c, SpinJump);
STATE_DEFINE(dAcPy_c, PenguinSlide);
STATE_DEFINE(dAcPy_c, KaniJump);
STATE_DEFINE(dAcPy_c, Quake);
STATE_DEFINE(dAcPy_c, ElecShock);
STATE_DEFINE(dAcPy_c, FlyDamage);
STATE_DEFINE(dAcPy_c, IceDamage);
STATE_DEFINE(dAcPy_c, CannonJump);
STATE_DEFINE(dAcPy_c, TarzanRope);
STATE_DEFINE(dAcPy_c, PlayerEat);
STATE_DEFINE(dAcPy_c, Balloon);
STATE_DEFINE(dAcPy_c, BlockJump);
STATE_DEFINE(dAcPy_c, JrCrown);

STATE_VIRTUAL_DEFINE(dAcPy_c, DemoDown);
STATE_DEFINE(dAcPy_c, DemoInDoor);
STATE_DEFINE(dAcPy_c, DemoInJump);
STATE_DEFINE(dAcPy_c, DemoInVine);
STATE_DEFINE(dAcPy_c, DemoOutDoor);
STATE_DEFINE(dAcPy_c, DemoFallDown);
STATE_DEFINE(dAcPy_c, DemoFireDown);
STATE_DEFINE(dAcPy_c, DemoEatDie);
STATE_DEFINE(dAcPy_c, DemoDokanCannon);
STATE_DEFINE(dAcPy_c, DemoCannonWarp);

const float data_802f5a0c[] = {
    0.0f, 0.08f, 0.07f, 0.11f, -1.5f, -1.25f,
    -1.0f, -0.1f, 1.5f, 1.5f, 2.5f, 1.5f,
    1.0f, -6.0f
};

template <>
dAcPy_c::GlobalData_t sGlobalData_c<dAcPy_c>::mData = {
    0.0f, 0.0f,
    1.5f, 4.5f, 0.5f, 4.0f, 1.4f, 1.0f,
    mVec3_c(0.0f, 10.0f, 0.0f),
    mVec3_c(0.0f, 16.0f, 0.0f),
    mVec3_c(0.0f, 28.0f, 0.0f)
};

dAcPy_HIO_Speed_c dAcPy_c::m_speed_hio[2];


// [This again... (See d_s_boot.cpp)]
#pragma push
#pragma pool_data off

bool dAcPy_c::setHipAttackOnEnemy(mVec3_c *hitPos) {
    if (isState(StateID_HipAttack) && isStatus(STATUS_1C)) {
        onStatus(STATUS_22);
        mHitAttackRelated = *hitPos;
        return true;
    } else if (isState(StateID_SpinHipAttack) && isStatus(STATUS_1F)) {
        onStatus(STATUS_22);
        mHitAttackRelated = *hitPos;
        return true;
    }
    return false;
}

void dAcPy_c::setHipAttackEffect() {
    if (isNowBgCross(BGC_14)) {
        setSeaLandSmokeEffect();
        fn_80057e70(SE_PLY_HPDP_SPLASH, 0);
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            fn_80057e70(SE_PLY_HIP_ATTACK_M, 0);
        } else {
            fn_80057e70(SE_PLY_HIP_ATTACK, 0);
        }
    } else {
        daPlBase_c::setHipAttackEffect();
    }
}

bool dAcPy_c::setHipAttackAction() {
    if (
        !isStatus(STATUS_A7) &&
        !isNowBgCross(BGC_IS_FOOT) && !isNowBgCross(BGC_38) &&
        mKey.checkHipAttack() &&
        !isDemoType(DEMO_ENDING_DANCE)
    ) {
        if (!isStatus(STATUS_26)) {
            if (!isCarry()) {
                changeState(StateID_HipAttack, 0);
                return true;
            }
        } else {
            changeState(StateID_SpinHipAttack, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setHipAttack_AttackStart() {
    daPlBase_c::setHipAttack_AttackStart();
    fn_80057e70(SE_PLY_ROLLING, 0);
}

void dAcPy_c::initializeState_HipAttack() {
    daPlBase_c::initializeState_HipAttack();
    mJumpCounter = 0;
    offStatus(STATUS_C1);
    onStatus(STATUS_8F);
}

void dAcPy_c::finalizeState_HipAttack() {
    daPlBase_c::finalizeState_HipAttack();
    offStatus(STATUS_8F);
}

void dAcPy_c::executeState_HipAttack() {
    if (isNowBgCross(BGC_14) && isOldBgCross(BGC_14) == 0) {
        setWaterInEffect();
    }
    daPlBase_c::executeState_HipAttack();
}

void dAcPy_c::setSpinHipAttackEffect() {
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf1, "Wm_mr_spindown", 0, &mPos, nullptr, nullptr);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf2, "Wm_mr_spindownline", 0, &mPos, nullptr, nullptr);
}

void dAcPy_c::initializeState_SpinHipAttack() {
    mSubstate = SPIN_HIP_ATTACK_ACTION_0;
    mPyMdlMng.setAnm(75);
    onStatus(STATUS_1F);
    onStatus(STATUS_A8);
    onStatus(STATUS_8F);
    onStatus(STATUS_2B);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    int changeParam = (int) mStateChangeParam;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    if (changeParam == 0) {
        m_1118 = 0;
        mSpeed.y = 0.0f;
        float v = data_802f5a0c[13];
        mMaxFallSpeed = v;
    } else {
        m_1118 = 1;
        mSpeed.y = -6.0f;
        mMaxFallSpeed = -6.0f;
    }
    m_2e8 = 0x1000;
    mSubstateTimer = 10;
    setScrollMode(2);
}

void dAcPy_c::executeState_SpinHipAttack() {
    offStatus(STATUS_21);
    if (isStatus(STATUS_1F)) {
        setCcAtSpinFall();
    }
    if (isNowBgCross(BGC_14) && isOldBgCross(BGC_14) == 0) {
        setWaterInEffect();
    }
    if (mPowerup != POWERUP_PROPELLER_SHROOM && getCarryPropelActor() == nullptr) {
        changeState(StateID_Fall, 0);
        return;
    }
    switch ((SpinHipAttackSubstate_e) mSubstate) {
        case SPIN_HIP_ATTACK_ACTION_0:
            fn_80057f60(SE_PLY_PRPL_LETDOWN_FAST, 0);
            if (isNowBgCross(BGC_14)) {
                mMaxFallSpeed = sc_WaterMaxFallSpeed;
            } else {
                sLib::chase(&mMaxFallSpeed, data_802f5a0c[13], 0.1f);
            }
            mAccelY = getGravityData()[0];
            onStatus(STATUS_AA);
            if (isNowBgCross(BGC_16)) {
                offStatus(STATUS_AA);
            }
            if (isNowBgCross(BGC_62)) {
                mSpeed.y = 0.0f;
                m_2e8 = 0x1000;
                offNowBgCross(BGC_IS_FOOT);
            }
            if (isNowBgCross(BGC_IS_FOOT) && !isOldBgCross(BGC_62)) {
                startQuakeShock(dQuake_c::TYPE_7);
                fn_80057e70(SE_PLY_PRPL_LETDOWN_FAST_LAND, 0);
                if (isNowBgCross(BGC_40)) {
                    changeState(StateID_Kani, (void *) 2);
                    return;
                }
                s16 curr = m_2e8;
                if (checkCrouch()) {
                    m_2e8 = curr;
                    return;
                }
                mSubstate = SPIN_HIP_ATTACK_ACTION_1;
                mAngle.y = getMukiAngle(mDirection);
                mPyMdlMng.setAnm(76);
                mSubstateTimer = 20;
                offStatus(STATUS_1F);
                offStatus(STATUS_2B);
                onStatus(STATUS_9F);
                onStatus(STATUS_20);
                onStatus(STATUS_21);
                return;
            }
            if (!mKey.buttonDown() && mSubstateTimer == 0) {
                if (m_1118 == 0) {
                    mMaxFallSpeed = data_802f5a0c[5];
                }
                changeState(StateID_Propel, (void *) 1);
                return;
            }
            if (isNowBgCross(BGC_15)) {
                m_2e8.chase(0, 0x1000);
            } else {
                m_2e8.chase(0x2000, 0x400);
            }
            mAngle.y += m_2e8;
            setSpinHipAttackEffect();
            setPropelSpinSmokeEffect();
            break;
        case SPIN_HIP_ATTACK_ACTION_1:
            m_2e8.chase(0, 0x200);
            mAngle.y += m_2e8;
            if (checkJumpTrigger()) {
                return;
            }
            if (mSubstateTimer == 0 || mPyMdlMng.isAnmStop() || mSpeedF) {
                changeState(StateID_Walk, (void *) 1);
                return;
            }
            break;
    }
    if (isStatus(STATUS_22)) {
        offStatus(STATUS_22);
        mPos.x = mHitAttackRelated.x;
        mPos.y = mHitAttackRelated.y;
    }
}

void dAcPy_c::finalizeState_SpinHipAttack() {
    m_2e8 = 0;
    offStatus(STATUS_2B);
    offStatus(STATUS_1F);
    offStatus(STATUS_20);
    offStatus(STATUS_21);
    offStatus(STATUS_A8);
    offStatus(STATUS_AA);
    offStatus(STATUS_9F);
    offStatus(STATUS_22);
    offStatus(STATUS_8F);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    setScrollMode(0);
}

void dAcPy_c::initializeState_Fall() {
    daPlBase_c::initializeState_Fall();
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_9F);
    onStatus(STATUS_92);
    onStatus(STATUS_8F);
    onStatus(STATUS_A1);
    if (!isNowBgCross(BGC_15)) {
        setWaterWalkFlag();
    }
    mSpeedMax.x = 0.0;
    mJumpCounter = 0;
    mSubstate = FALL_ACTION_1;
    setAddLiftSpeedF();
    if (isStatus(STATUS_C2)) {
        mSpeed.y = 0.5f;
    }
    setJumpGravity();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
}

void dAcPy_c::finalizeState_Fall() {
    daPlBase_c::finalizeState_Fall();
    mAngle.x = 0;
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_92);
    offStatus(STATUS_8F);
    offStatus(STATUS_C2);
    offStatus(STATUS_A1);
}

void dAcPy_c::executeState_Fall() {
    daPlBase_c::executeState_Fall();
    setJumpGravity();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    if (!setKaniActionInitHangHand()) {
        jump_common();
        if (setDelayHelpJump()) {
            return;
        }
    }
}

void dAcPy_c::initializeState_Jump() {
    daPlBase_c::initializeState_Jump();
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    if (!m_1058) {
        onStatus(STATUS_9F);
    }
    onStatus(STATUS_92);
    onStatus(STATUS_8F);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    setStartJumpEffect(0);
    _jumpSet((int) mStateChangeParam);
    if (mJumpCounter != 2) {
        onStatus(STATUS_9E);
    }
}

void dAcPy_c::finalizeState_Jump() {
    daPlBase_c::finalizeState_Jump();
    m_1058 = 0;
    m_90 = 8;
    if (!isStatus(STATUS_61)) {
        onStatus(STATUS_61);
        calcJumpCount();
    }
    offStatus(STATUS_0C);
    offStatus(STATUS_96);
    offStatus(STATUS_48);
    offStatus(STATUS_BF);
    offStatus(STATUS_A8);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_92);
    offStatus(STATUS_8F);
    offStatus(STATUS_A1);
}

void dAcPy_c::executeState_Jump() {
    daPlBase_c::executeState_Jump();
    if (m_1058 != 0) {
        if (--m_1058 == 0) {
            onStatus(STATUS_9F);
        }
    }
    if (isStatus(STATUS_A8) && mSpeed.y < 2.0f) {
        offStatus(STATUS_A8);
    }
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    if (!setKaniActionInitHangHand()) {
        jump_common();
    }
}

void dAcPy_c::calcJumpCount() {
    if (!isStatus(STATUS_AB)) {
        mJumpCounter++;
        if (mJumpCounter > 2) {
            mJumpCounter = 0;
        }
    }
}

/// @todo define data
extern const daPlBase_c::sPowerChangeData l_power_change_data;

float dAcPy_c::getJumpSpeed() {
    float baseSpeed;
    float absSpeed = std::fabs(mSpeedF);
    if (absSpeed < l_power_change_data.mJumpSpeedValues1[0]) {
        baseSpeed = l_power_change_data.mJumpSpeedValues2[0];
    } else if (absSpeed < l_power_change_data.mJumpSpeedValues1[1]) {
        baseSpeed = l_power_change_data.mJumpSpeedValues2[1];
    } else if (absSpeed < l_power_change_data.mJumpSpeedValues1[2]) {
        baseSpeed = l_power_change_data.mJumpSpeedValues2[2];
    } else {
        baseSpeed = l_power_change_data.mJumpSpeedValues2[3];
    }
    if (std::fabs(mSpeedF) >= getSpeedData()[2]) {
        baseSpeed = l_power_change_data.mJumpSpeedValues2[3];
    }
    float jumpSpeed;
    if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            jumpSpeed = sc_JumpSpeedNuma2;
        } else {
            jumpSpeed = sc_JumpSpeedNuma1;
            if (mPos.y < m_db0 - 4.0f) {
                jumpSpeed += -0.15f;
            }
        }
    } else {
        jumpSpeed = sc_JumpSpeed + baseSpeed;
    }
    if (isMameAction()) {
        jumpSpeed *= 0.8125f;
    }
    return jumpSpeed;
}

void dAcPy_c::setJumpSpeed() {
    float jumpSpeed = getJumpSpeed();
    if (mJumpCounter == 2) {
        jumpSpeed *= 1.05f;
    }
    mSpeed.y = jumpSpeed;
}

void dAcPy_c::fn_80127740(int jumpMode, int b) {
    mSubstate = JUMP_TAKE_OFF;
    fn_80145fd0(jumpMode);
    if (isStatus(STATUS_10)) {
        mPyMdlMng.setAnm(121, 0.0f, 0.0f);
    } else if (isStatus(STATUS_2B)) {
        mPyMdlMng.setAnm(71);
    } else if (isStatus(STATUS_0D)) {
        mPyMdlMng.setAnm(67, 0.0f, 0.0f);
    } else if (isStatus(STATUS_0C) && !isCarry()) {
        mPyMdlMng.setAnm(117);
    } else if (isStatus(STATUS_0F)) {
        mPyMdlMng.setAnm(118);
    } else {
        int anmNum;
        switch (mJumpCounter) {
            case 0:
                anmNum = 5;
                break;
            case 1:
                vf434(21, 0);
                anmNum = 8;
                break;
            default:
                vf434(22, 0);
                anmNum = 11;
                break;
        }
        if (!isCarry()) {
            if (mJumpCounter == 1) {
                mPyMdlMng.mpMdl->fn_800d5e00(1);
            } else {
                mPyMdlMng.mpMdl->fn_800d5e00(0);
            }
        }
        if (!b) {
            mPyMdlMng.setAnm(anmNum, 0.0f, 0.0f);
        } else {
            mPyMdlMng.setAnm(anmNum);
        }
    }
}

bool dAcPy_c::jump_common() {
    if (checkCarryThrow()) {
        return true;
    }
    if (setHipAttackAction()) {
        return true;
    }
    if (isNowBgCross(BGC_WALL_TOUCH_L_2) | isNowBgCross(BGC_WALL_TOUCH_R_2)) {
        mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    }
    if (mSpeed.y <= 0.0f) {
        mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    }
    typedef void (dAcPy_c::*JumpActionProc)();
    static JumpActionProc l_JumpActionProc[] = {
        &dAcPy_c::jumpExeTakeOff,
        &dAcPy_c::jumpExeTakeAir
    };
    (this->*l_JumpActionProc[mSubstate])();
    return false;
}

#pragma pop

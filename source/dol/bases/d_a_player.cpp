#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
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
        mSubstateTimer2 = 0;
        mSpeed.y = 0.0f;
        float v = data_802f5a0c[13];
        mMaxFallSpeed = v;
    } else {
        mSubstateTimer2 = 1;
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
                if (mSubstateTimer2 == 0) {
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
    _jumpSet((jmpInf_c *) mStateChangeParam);
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
        &dAcPy_c::jumpExecAir
    };
    (this->*l_JumpActionProc[mSubstate])();
    return false;
}

void dAcPy_c::jumpExeTakeOff() {
    if (isStatus(STATUS_0C)) {
        mSubstate = JUMP_AIR;
    } else if (isStatus(STATUS_10)) {
        if (mPyMdlMng.isAnmStop()) {
            mPyMdlMng.setAnm(6, 3.0f, 0.0f);
            mSubstate = JUMP_AIR;
        }
    } else if (mSpeed.y < 0.0f) {
        if (isStatus(STATUS_0D)) {
            mPyMdlMng.setAnm(68, 10.0f, 0.0f);
        } else if (isStatus(STATUS_0F)) {
            mPyMdlMng.setAnm(119);
        } else if (mJumpCounter != 2) {
            /// @unofficial
            static const int l_AnmIDs[] = { 6, 9, 12 };
            u8 anmID = l_AnmIDs[mJumpCounter];
            mPyMdlMng.setAnm(anmID);
        }
        mSubstate = JUMP_AIR;
    }
    jumpExecAir();
}

void dAcPy_c::jumpExecAir() {
    if (isStatus(STATUS_0C) && !isStar()) {
        offStatus(STATUS_0C);
        mPyMdlMng.setAnm(6, 3.0f, 0.0f);
    }
    if (mJumpCounter == 2) {
        if (mSubstate == JUMP_AIR) {
            turnAngle();
            if (mDirection != m_12f4 || isNowBgCross(BGC_IS_HEAD)) {
                float f = mPyMdlMng.getLastFrame();
                mPyMdlMng.setAnm(11, 0.0f, f);
            }
        }
    } else if (!isStatus(STATUS_0D)) {
        turnAngle();
    }
    setJumpGravity();
    maxFallSpeedSet();
    if (isStatus(STATUS_2D) && mSpeed.y < 0.0f) {
        mAccelY = 0.0f;
        float f = std::fabs(mSpeed.y) * 0.15f;
        if (f < 0.1f) {
            f = 0.1f;
        }
        if (f > 1.0f) {
            f = 1.0f;
        }
        sLib::chase(&mSpeed.y, 0.0f, f);
    }
    if (
        isNowBgCross(BGC_IS_FOOT) &&
        !checkSlip() &&
        !checkJumpTrigger() &&
        !checkCrouch()
    ) {
        mAngle.x = 0;
        if (isStatus(STATUS_0F)) {
            changeState(StateID_Crouch, (void *) 2);
            return;
        }
        if (
            mPowerup != POWERUP_MINI_MUSHROOM &&
            !isCarry() &&
            isStatus(STATUS_0A) &&
            isNowBgCross(BGC_37)
        ) {
            changeState(StateID_Land, (void *) 1);
            return;
        }
        if ((isStatus(STATUS_10) || mJumpCounter == 2) && !mKey.buttonWalk(nullptr)) {
            mSpeedF = 0.0f;
        }
        setLandJumpEffect(0);
        if (mSpeedF) {
            fn_801282d0(0);
            return;
        }
        if (isStatus(STATUS_0D)) {
            mPyMdlMng.setAnm(69, 10.0f, 0.0f);
        } else {
            if (mJumpCounter == 0) {
                mPyMdlMng.setAnm(7);
            } else {
                mPyMdlMng.setAnm(10);
            }
        }
        changeState(StateID_Land, 0);
    }
}

void dAcPy_c::setJumpCommonBase() {
    if (isStatus(STATUS_AC)) {
        int v = getFollowMameKuribo() - 1;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        /// @unofficial
        static const float speedMultiplier[] = { 0.84f, 0.81f, 0.78f, 0.75f };
        mSpeed.y *= speedMultiplier[v];
    }
    offNowBgCross(BGC_IS_FOOT);
    setAddLiftSpeedF();
    setJumpGravity();
    maxFallSpeedSet();
}

void dAcPy_c::_jumpSet(jmpInf_c *jumpInf) {
    setWaterWalkFlag();
    mAngle.x = 0;
    offStatus(STATUS_88);
    if (isStar()) {
        onStatus(STATUS_0C);
    } else {
        offStatus(STATUS_0C);
    }
    if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND))) {
        offStatus(STATUS_0E);
    }
    int jumpMode = 1;
    if (jumpInf != nullptr) {
        jumpMode = jumpInf->m_08;
    }
    if (isStatus(STATUS_C0)) {
        jumpMode = 0;
    }
    if (
        isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) ||
        m_90 == 0 ||
        isCarry() ||
        std::fabs(mSpeedF) < 3.0f ||
        jumpMode == 0
    ) {
        mJumpCounter = 0;
    }
    if (getCarryPlayer() != nullptr && isStatus(STATUS_47)) {
        onStatus(STATUS_48);
    }
    if (jumpInf != nullptr) {
        if (jumpInf->m_04) {
            mSpeed.y = jumpInf->m_04;
        } else {
            setJumpSpeed();
        }
    } else {
        setJumpSpeed();
    }
    int anmRelated = 1;
    if (jumpInf != nullptr) {
        anmRelated = jumpInf->m_0c;
    }
    fn_80127740(jumpMode, anmRelated);
    mAngle.y = getMukiAngle(mDirection);
    setJumpCommonBase();
}

void dAcPy_c::fn_801282d0(int a) {
    if (mKey.buttonWalk(nullptr) && mSpeedF * mMaxSpeedF < 0.0f && checkTurn()) {
        return;
    }
    changeState(StateID_Walk, (void *) a);
}

void dAcPy_c::initializeState_Land() {
    if (mSubstate == LAND_ACTION_1) {
        mSpeedF = 0.0f;
        mPyMdlMng.setAnm(10, 2.0f, 0.0f, 0.0f);
    }
    onStatus(STATUS_9B);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_9F);
    onStatus(STATUS_92);
    onStatus(STATUS_A0);
    onStatus(STATUS_A2);
    onStatus(STATUS_A3);
}

void dAcPy_c::finalizeState_Land() {
    offStatus(STATUS_9B);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_92);
    offStatus(STATUS_A0);
    offStatus(STATUS_A2);
    offStatus(STATUS_A3);
}

void dAcPy_c::executeState_Land() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    if (checkWalkNextAction()) {
        return;
    }
    if (isNowBgCross(BgCross1_e(BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
        mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    }
    if (isIceSlipAnmPlay()) {
        changeState(StateID_Walk, (void *) 1);
        return;
    }
    if (mSubstate == LAND_ACTION_0) {
        if (!mSpeedF && !mPyMdlMng.isAnmStop()) {
            return;
        }
        fn_801282d0(1);
    } else {
        mSpeedF = 0.0f;
        if ((mKey.buttonWalk(nullptr) && mPyMdlMng.mpMdl->mAnm.getFrame() >= 10.0f) || mPyMdlMng.isAnmStop()) {
            fn_801282d0(1);
        }
    }
}

int dAcPy_c::checkWallSlideEnable(int dir) {
    if (
        !isNowBgCross(BGC_14) &&
        m_91 == 0 &&
        !isStatus(STATUS_33) &&
        !isStatus(STATUS_A8) &&
        !isNowBgCross(BGC_38) &&
        !isNowBgCross(BGC_IS_FOOT) &&
        !mIsBgDamage &&
        mBc.getWallAttr(dir) != 7 &&
        !isNowBgCross(BGC_IS_HEAD) &&
        !isCarry()
    ) {
        /// @unofficial
        static const BgCross1_e flags[] = {
            BGC_WALL_TOUCH_R,
            BGC_WALL_TOUCH_L
        };
        if (isNowBgCross(flags[mDirection]) && dir == mDirection) {
            return 0;
        } else {
            return 1;
        }
    }
    return 2;
}

bool dAcPy_c::checkWallJump() {
    if (isStatus(STATUS_13)) {
        return false;
    }
    int dir;
    if (
        mKey.buttonWalk(&dir) &&
        !isNowBgCross(BGC_38) &&
        !checkWallSlideEnable(dir)
    ) {
        if (!isStatus(STATUS_A9) && mSpeed.y < 0.0f) {
            changeState(StateID_WallSlide, 0);
            return true;
        }
        if (mKey.triggerJump()) {
            changeState(StateID_WallJump, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::initializeState_WallSlide() {
    mPyMdlMng.setAnm(29, 0.0f, 0.0f);
    onStatus(STATUS_97);
    onStatus(STATUS_13);
    onStatus(STATUS_9F);
    onStatus(STATUS_8F);
    mBc.mPlayerFlags |= 2;
    mAccelY = getGravityData()[0];
    mAngle.y = getMukiAngle(mDirection);
    mMaxSpeedF = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
}

void dAcPy_c::finalizeState_WallSlide() {
    offStatus(STATUS_13);
    offStatus(STATUS_9F);
    offStatus(STATUS_8F);
    mBc.mPlayerFlags &= ~2;
    mAccelY = getGravityData()[0];
    m_91 = 4;
}

void dAcPy_c::setWallSlideEffect() {
    fn_80057f60(SE_PLY_SLIP_TATE, 0);
    if (getTallType(-1) > 2) {
        return;
    }
    u8 idx = (mDirection ^ 1) + getTallType(-1) * 2;
    if (idx >= 6) {
        return;
    }
    mVec3_c jnt;
    mPyMdlMng.mpMdl->getJointPos(&jnt, 14);
    if (mDirection == 0) {
        jnt.x = mPos.x + m_d2c / 4096.0f;
    } else {
        jnt.x = mPos.x - m_d2c / 4096.0f;
    }
    /// @unofficial
    static const char *effectNames[] = {
        "Wm_mr_wallslip_ss_r",
        "Wm_mr_wallslip_ss_l",
        "Wm_mr_wallslip_s_r",
        "Wm_mr_wallslip_s_l",
        "Wm_mr_wallslip_r",
        "Wm_mr_wallslip_l"
    };
    dEf::createPlayerEffect(mPlayerNo, &mFunsuiSmokeEffect, effectNames[idx], 0, &jnt, nullptr, nullptr);
}

void dAcPy_c::executeState_WallSlide() {
    if (isNowBgCross(BGC_IS_FOOT)) {
        changeState(StateID_Walk, 0);
        return;
    }
    if (mKey.triggerJump()) {
        changeState(StateID_WallJump, 0);
        return;
    }
    if (setHipAttackAction()) {
        return;
    }
    int dir;
    if (mKey.buttonWalk(&dir) && dir != mDirection) {
        mSubstateTimer2 += 1;
        if (mSubstateTimer2 >= 15) {
            changeState(StateID_Fall, 0);
        }
    } else {
        mSubstateTimer2 = 0;
    }
    if (checkWallSlideEnable(mDirection)) {
        changeState(StateID_Fall, 0);
        return;
    }
    if (isMameAction()) {
        mMaxFallSpeed = -0.75f;
    } else {
        mMaxFallSpeed = -2.0f;
    }
    setWallSlideEffect();
    mSpeedF = sc_DirSpeed[mDirection];
}

void dAcPy_c::initializeState_WallJump() {
    onStatus(STATUS_97);
    onStatus(STATUS_A7);
    setWaterWalkFlag();
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mDirection ^= 1;
    mAngle.y = getMukiAngle(mDirection);
    mMaxSpeedF = 0.0f;
    mSpeedF = 0.0f;
    if (getTallType(-1) <= 2) {
        u8 dir = mDirection;
        u8 idx = dir + getTallType(-1) * 2;
        if (idx < 6) {
            float thing = m_d2c / 4096.0f;
            mVec3_c pos(mPos.x - thing * sc_DirSpeed[dir], mPos.y, mPos.z);
            mVec3_c pos2(pos.x - sc_DirSpeed[mDirection] * 8.0f, pos.y, pos.z);
            if (!dBc_c::checkWall(&mPos, &pos2, nullptr, mLayer, m_ca1, nullptr)) {
                pos.y += 8.0f;
                float f;
                if (dBc_c::checkTenjou(&pos2, &f, mLayer, m_ca1) && f < mPos.y + getSomeYOffset()) {
                    pos.y = f + 8.0f;
                }
            }
            /// @unofficial
            static const char *effectNames[] = {
                "Wm_mr_wallkick_ss_r",
                "Wm_mr_wallkick_ss_l",
                "Wm_mr_wallkick_s_r",
                "Wm_mr_wallkick_s_l",
                "Wm_mr_wallkick_r",
                "Wm_mr_wallkick_l"
            };
            dEf::createPlayerEffect(mPlayerNo, effectNames[idx], 0, &pos, nullptr, nullptr);
        }
    }
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    m_15b0 = 35;
}

void dAcPy_c::finalizeState_WallJump() {
    mAccelY = getGravityData()[0];
}

void dAcPy_c::executeState_WallJump() {
    mSpeedF = sc_DirSpeed[mDirection] * 2.25f;
    mMaxSpeedF = mSpeedF;
    onStatus(STATUS_96);
    float f = 3.5f;
    if (isMameAction()) {
        f *= 0.8125f;
    }
    jmpInf_c jumpInf(f, 0, 1);
    changeState(StateID_Jump, &jumpInf);
    vf434(25, 0);
    fn_80057e70(SE_PLY_KABE_KICK, false);
    m_91 = 16;
}

void dAcPy_c::setSpinHoldReq(float f) {
    onStatus(STATUS_2C);
    mSpinHoldReq = f;
}

bool dAcPy_c::setSpinSpeed(float speedF, float speedY) {
    if (isState(StateID_SpinJump)) {
        if (isStatus(STATUS_2C) || m_15ae != 0) {
            return false;
        }
        mSpeed.y = speedY;
        mSpeedF = speedF;
        mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
        m_15ae = 30;
        m_1044 = 30;
        m_1048 = 0x2000;
        offNowBgCross(BGC_IS_FOOT);
        dQuake_c::getInstance()->shockMotor(*getPlrNo(), dQuake_c::TYPE_7, 0, false);
        return true;
    }
    return false;
}

void dAcPy_c::setSpinJumpEffect(int i) {
    if (!isStatus(STATUS_2B)) {
        endSpinJumpEffect();
    } else if (fManager_c::searchBaseByID(mCarryActorID) != nullptr) {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr) {
            carryPlayer->setSpinJumpEffectSub(i);
        }
    } else {
        setSpinJumpEffectSub(i);
    }
}

void dAcPy_c::setSpinJumpEffectSub(int i) {
    if (dScStage_c::m_isStaffCredit) {
        return;
    }
    mVec3_c jnt1, jnt2;
    mPyMdlMng.mpMdl->getJointPos(&jnt1, 11);
    mPyMdlMng.mpMdl->getJointPos(&jnt2, 14);
    mVec3_c pos(mPos.x, 4.0f + (jnt1.y + jnt2.y) / 2, mPos.z);
    float s;
    if (2 <= mDokanCenterOffsetType && mDokanCenterOffsetType <= 3) {
        static const float tallTypeScale[3] = { 0.6f, 0.8f, 1.0f };
        s = tallTypeScale[getTallType(-1)];
    } else {
        static const float tallTypeScale[3] = { 0.6f, 0.7f, 0.8f };
        s = tallTypeScale[getTallType(-1)];
    }
    mVec3_c scale(s, s, s);
    if (i == 1) {
        if (m_1288 == 0) {
            static const char *effectNames[] = {
                "Wm_mr_normalspin",
                "Wm_mr_normalspin_pm"
            };
            int idx = 0;
            if (mPowerup == POWERUP_PENGUIN_SUIT) {
                idx = 1;
            }
            dEf::createPlayerEffect(mPlayerNo, &mFollowEf2, effectNames[idx], 0, &pos, nullptr, &scale);
            m_1288 = 1;
        }
    } else if (m_1288 != 0) {
        if (!mFollowEf2.follow(&pos, nullptr, &scale)) {
            m_1288 = 0;
        }
    }
}

void dAcPy_c::endSpinJumpEffect() {
    if (m_1288 != 0) {
        m_1288 = 0;
        mFollowEf2.kill();
    }
}

void dAcPy_c::setMissSpinJumpEffect(int i) {
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        carryPlayer->setMissSpinJumpEffect(i);
        return;
    }
    if (i == 0 && !m_128c) {
        return;
    }
    mVec3_c jnt;
    mPyMdlMng.mpMdl->getJointPos(&jnt, 1);
    float s = dPyMdlMng_c::m_hio.m_08[mPyMdlMng.mpMdl->m_152];
    mVec3_c scale(s, s, s);
    if (i == 1) {
        dEf::createPlayerEffect(mPlayerNo, &mFollowEf3, "Wm_mr_halfspin", 0, &jnt, nullptr, &scale);
        m_128c = 1;
    } else if (m_128c != 0) {
        if (!mFollowEf3.follow(&jnt, nullptr, &scale)) {
            m_128c = 0;
        }
    }
}

void dAcPy_c::initializeState_SpinJump() {
    m_12f4 = mDirection;
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_92);
    onStatus(STATUS_A9);
    onStatus(STATUS_8F);
    onStatus(STATUS_A1);
    onStatus(STATUS_2B);
    m_1054 = 0;
    m_1044 = 30;
    m_1048 = 0x2000;
    setSpinJumpEffect(1);
    u32 substate = (u32) mStateChangeParam;
    if (substate <= SPIN_JUMP_ACTION_1) {
        fn_80057e70(SE_PLY_SPIN_ATTACK, true);
    }
    if (isStatus(STATUS_2C)) {
        mPyMdlMng.setAnm(71, 0.0f, 0.0f);
    } else {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
        jmpInf_c jumpInf(mSpeed.y, 0, 1);
        if (substate == SPIN_JUMP_ACTION_0) {
            jumpInf.m_04 = sc_JumpSpeed - 0.4f;
        }
        if (
            isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) &&
            (isNowBgCross(BGC_IN_SINK_SAND) || mPos.y < m_db0 - 4.0f)
        ) {
            jumpInf.m_04 = sc_JumpSpeed - 0.7f;
        }
        _jumpSet(&jumpInf);
    }
    setStartJumpEffect(1);
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        carryPlayer->initRideSpinMove();
    }
}

void dAcPy_c::executeState_SpinJump() {
    mAngle.y += m_1048;
    setCcAtSpin();
    if (m_1054) {
        m_1054--;
    }
    if (m_1044) {
        m_1044--;
        if (m_1054 == 0) {
            if (m_1044 == 21) {
                setSpinFireBall();
            } else if (m_1044 == 15) {
                m_12f4 ^= 1;
                setSpinFireBall();
                m_1054 = 20;
            }
        }
    }
    if (isStatus(STATUS_2C)) {
        mKey.onStatus(dAcPyKey_c::STATUS_5);
        if (mKey.triggerShakeJump() && m_1044 < 10) {
            m_1044 = 30;
            m_1048 = 0x2000;
        }
        mSpeed.y = 0.0f;
        mSpeedF = 0.0f;
        mAccelY = 0.0f;
        mAccelF = 0.0f;
        sLib::chase(&mPos.x, mSpinHoldReq, 1.0f);
        if (!checkJumpTrigger()) {
            if (
                mKey.buttonWalk(nullptr) &&
                (mKey.triggerRight() || mKey.triggerLeft())
            ) {
                changeState(StateID_Walk, (void *) 1);
            } else {
                if (m_1044 == 0) {
                    mPyMdlMng.setAnm(72);
                    changeState(StateID_Land, 0);
                }
            }
        }
    } else {
        setJumpGravity();
        maxFallSpeedSet();
        moveSpeedSet();
        airPowerSet();
        onStatus(STATUS_0A);
        if (isNowBgCross(BGC_IS_HEAD)) {
            m_1044 = 0;
        }
        if (isNowBgCross(BGC_IS_FOOT)) {
            setLandJumpEffect(1);
            mPyMdlMng.setAnm(72, 0.0f, 0.0f);
            changeState(StateID_Land, 0);
        }
    }
}

void dAcPy_c::finalizeState_SpinJump() {
    m_1044 = 0;
    m_1054 = 0;
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9F);
    offStatus(STATUS_2B);
    offStatus(STATUS_92);
    offStatus(STATUS_A9);
    offStatus(STATUS_8F);
    offStatus(STATUS_0A);
    offStatus(STATUS_A1);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
    mKey.offStatus(dAcPyKey_c::STATUS_5);
    mAngle.y = getMukiAngle(mDirection);
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        carryPlayer->endRideSpinMove();
    }
}

bool dAcPy_c::isMissSpinEnableAnm() {
    if (mPyMdlMng.getAnm() == 117) {
        return false;
    }
    if (mPyMdlMng.getAnm() == 11 && !mPyMdlMng.isAnmStop()) {
        return false;
    }
    return true;
}

s16 dAcPy_c::getMissSpinAngle() {
    if (isMissSpinEnableAnm()) {
        return m_105c;
    }
    return 0;
}

void dAcPy_c::setMissSpin() {
    m_105c = 0x1400;
    fn_80057e70(SE_PLY_SPIN_ONCE, true);
    if (isMissSpinEnableAnm()) {
        setMissSpinJumpEffect(1);
    }
}

void dAcPy_c::resetMissSpin() {
    m_105c = 0;
}

void dAcPy_c::updateMissSpin() {
    if (m_105c == 0) {
        return;
    }
    s16 prevAngle = m_105c;
    m_105c += 0x1400;
    if (prevAngle < 0 && m_105c >= 0) {
        m_105c = 0;
        return;
    }
    onStatus(STATUS_2D);
}

void dAcPy_c::initializeState_RideOffJump() {
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_10);
    onStatus(STATUS_97);
    onStatus(STATUS_0A);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    mSubstateTimer = 30;
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mAngle.y = getMukiAngle(mDirection);
    mSpeedF = sc_DirSpeed[mDirection] * -1.0f;
    jmpInf_c jumpInf(2.85f, 0, 1);
    _jumpSet(&jumpInf);
    fn_80057e70(SE_PLY_BREAK_FREE_YOSHI, false);
}

void dAcPy_c::finalizeState_RideOffJump() {
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_0A);
    offStatus(STATUS_48);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_88);
    offStatus(STATUS_8F);
    offStatus(STATUS_10);
    offStatus(STATUS_97);
    offStatus(STATUS_A1);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

void dAcPy_c::executeState_RideOffJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    if (mSubstateTimer == 0) {
        onStatus(STATUS_9F);
        onStatus(STATUS_8F);
    }
    jump_common();
}

void dAcPy_c::initializeState_SitJump() {
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9F);
    onStatus(STATUS_9E);
    onStatus(STATUS_8F);
    onStatus(STATUS_0F);
    onStatus(STATUS_A0);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    if ((u32) mStateChangeParam == SIT_JUMP_ACTION_0) {
        _jumpSet(nullptr);
    } else {
        mPyMdlMng.setAnm(119);
        mAngle.y = getMukiAngle(mDirection);
    }
}

void dAcPy_c::finalizeState_SitJump() {
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_0A);
    offStatus(STATUS_48);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_88);
    offStatus(STATUS_8F);
    offStatus(STATUS_0F);
    offStatus(STATUS_A0);
    offStatus(STATUS_A1);
    offStatus(STATUS_AA);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

void dAcPy_c::executeState_SitJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    onStatus(STATUS_0A);
    if (dScStage_c::m_instance->mCurrWorld == WORLD_5 && dScStage_c::m_instance->mCurrLevel == STAGE_GHOST_HOUSE) {
        if (isNowBgCross(BGC_56) && isOldBgCross(BGC_32)) {
            if (mLastPosDelta.x < 0.0f) {
                mPos.x -= 4.0f;
            } else {
                mPos.x += 4.0f;
            }
            mSpeedF = 0.0f;
        }
    }
    jump_common();
    if (mSpeed.y < 0.0f) {
        offStatus(STATUS_AA);
        if (!mKey.buttonCrouch() && !checkStandUpRoof()) {
            jmpInf_c jumpInf(mSpeed.y, 0, 1);
            changeState(StateID_Jump, &jumpInf);
        }
    }
}

void dAcPy_c::initializeState_KaniJump() {
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9F);
    offStatus(STATUS_9E);
    offStatus(STATUS_8F);
    offStatus(STATUS_0D);
    offStatus(STATUS_A2);
    jmpInf_c jumpInf(3.0f, 1, 1);
    _jumpSet(&jumpInf);
    mSpeedMax.x = 0.0f;
    mAngle.y = 0;
}

void dAcPy_c::finalizeState_KaniJump() {
    mAngle.x = 0;
    onStatus(STATUS_0A);
    onStatus(STATUS_48);
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_9F);
    onStatus(STATUS_88);
    onStatus(STATUS_8F);
    onStatus(STATUS_0D);
    onStatus(STATUS_A2);
}

void dAcPy_c::executeState_KaniJump() {
    gravitySet();
    maxFallSpeedSet();
    setKaniMoveSpeed(true);
    onStatus(STATUS_0A);
    jump_common();
    if (mPos.y < m_d8c - 4.0f) {
        changeState(StateID_Fall, 0);
    }
}

void dAcPy_c::initializeState_CannonJump() {
    onStatus(STATUS_BD);
    onStatus(STATUS_11);
    onStatus(STATUS_88);
    onStatus(STATUS_7F);
    if (m_68 == 1) {
        onStatus(STATUS_7A);
    }
    clearNowBgCross();
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_5, 0, false);
    vf434(35, 0);
    mPyMdlMng.setAnm(127);
    mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mAngle.y = getMukiAngle(mDirection);
    int ang = cM::atan2s(std::fabs(mSpeedF), mSpeed.y);
    mAngle.x = 0x4000 - ang;
    mSubstateTimer2 = 0;
    m_12b4 = (u32) mStateChangeParam;
    m_12a8 = mPos;
}

void dAcPy_c::finalizeState_CannonJump() {
    float data = getSpeedData()[2];
    if (mSpeedF > data) {
        mSpeedF = data;
    }
    if (mSpeedF < -data) {
        mSpeedF = -data;
    }
    offStatus(STATUS_7F);
    offStatus(STATUS_11);
    offStatus(STATUS_BD);
    offStatus(STATUS_88);
    offStatus(STATUS_8E);
    offStatus(STATUS_7A);
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mAngle.x = 0;
}

void dAcPy_c::executeState_CannonJump() {
    if (isStatus(STATUS_8E)) {
        onStatus(STATUS_8D);
    }
    switch ((CannonJumpSubstate_e) mSubstate) {
        case CANNON_JUMP_ACTION_0: {
            setCcAtCannon();
            if (m_12b4 != 0) {
                m_12b4--;
                mAccelF = 0.0f;
                mAccelY = 0.0f;
            } else {
                mAccelY = 0.5f;
                if (std::fabs(mSpeedF) >= 1.0f) {
                    mAccelF = 0.03f;
                } else {
                    mAccelF = 0.0f;
                }
            }
            mVec3_c tmp(m_12a8.x - mPos.x, m_12a8.y - mPos.y, 0.0f);
            if (PSVECMag(tmp) >= 8.0f) {
                if (!isStatus(STATUS_8E)) {
                    onStatus(STATUS_7F);
                }
                offZPosSetNone();
            }
            int ang = cM::atan2s(std::fabs(mSpeedF), mSpeed.y);
            mAngle.x = 0x4000 - ang;
            if (isNowBgCross(BGC_IS_HEAD) || isNowBgCross(BgCross1_e(BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
                mSpeed.y = 0.0f;
                changeState(StateID_Fall, 0);
            } else {
                if (mSpeed.y <= 0.0f && std::fabs(mSpeedF) < 0.1f) {
                    changeState(StateID_Fall, 0);
                }
                if (isNowBgCross(BGC_IS_FOOT)) {
                    mPyMdlMng.setAnm(128);
                    mSubstate = CANNON_JUMP_ACTION_1;
                    mAngle.x = 0;
                    offStatus(STATUS_BD);
                }
            }
            break;
        }
        case CANNON_JUMP_ACTION_1:
            if (mSpeedF) {
                setCcAtCannon();
            }
            gravitySet();
            maxFallSpeedSet();
            mMaxSpeedF = 0.0f;
            powerSet();
            if (isNowBgCross(BGC_IS_FOOT)) {
                setCrouchSmokeEffect();
                if (!(std::fabs(mSpeedF) < 1.0f && checkJumpTrigger()) && std::fabs(mSpeedF) <= 0.1f) {
                    mPyMdlMng.setAnm(129);
                    mSubstate = CANNON_JUMP_ACTION_2;
                    mSpeedF = 0.0f;
                }
            } else {
                mSubstate = CANNON_JUMP_ACTION_0;
                mPyMdlMng.setAnm(127);
            }
            break;
        case CANNON_JUMP_ACTION_2:
            if (isNowBgCross(BGC_IS_FOOT)) {
                if (checkJumpTrigger()) {
                    return;
                }
            } else {
                changeState(StateID_Fall, 0);
                return;
            }
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, (void *) 1);
            }
            break;
    }
}

void dAcPy_c::initializeState_BlockJump() {
    onStatus(STATUS_7F);
    onStatus(STATUS_7A);
    mSubstate = (int) mStateChangeParam;
    if (mSubstate == BLOCK_JUMP_ACTION_0) {
        jmpInf_c jumpInf(3.628f, 0, 1);
        _jumpSet(&jumpInf);
    } else {
        mSubstateTimer = 10;
    }
    mSpeedF = 0.5f;
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    if (isItemKinopio()) {
        fn_80057e70(SE_VOC_ITEM_KO_FOUND, false);
    }
}

void dAcPy_c::finalizeState_BlockJump() {
    offStatus(STATUS_7F);
    offStatus(STATUS_7A);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
}

void dAcPy_c::executeState_BlockJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    jump_common();
    if (mSubstate == BLOCK_JUMP_ACTION_0) {
        if (mSpeed.y < 0.0f) {
            changeState(StateID_Fall, 0);
        }
        if (mSubstateTimer == 0) {
            offStatus(STATUS_7F);
            if (isNowBgCross(BGC_IS_HEAD)) {
                changeState(StateID_Fall, 0);
            }
        }
    } else if (mSubstateTimer == 0) {
        changeState(StateID_Fall, 0);
    }
}

template <>
dAcPy_c::GlobalData_t sGlobalData_c<dAcPy_c>::mData = {
    0.0f, 0.0f,
    1.5f, 4.5f, 0.5f, 4.0f, 1.4f, 1.0f,
    mVec3_c(0.0f, 10.0f, 0.0f),
    mVec3_c(0.0f, 16.0f, 0.0f),
    mVec3_c(0.0f, 28.0f, 0.0f)
};

ACTOR_PROFILE(PLAYER, dAcPy_c, 0);

dAcPy_c::dAcPy_c() : mPyMdlMng(daPyMng_c::getCourseInPlayerModelType(ACTOR_PARAM(PlayerNo))) {
    setKind(STAGE_ACTOR_PLAYER);
    mExecStopMask = BIT_FLAG(STAGE_ACTOR_PLAYER);
    mpMdlMng = &mPyMdlMng;
    mPyMdlMng.mpMdl->mpOwner = this;
    mPlayerNo = ACTOR_PARAM(PlayerNo);
    mDokanCenterOffsetType = daPyMng_c::mPlayerType[mPlayerNo];
    mPowerup = daPyMng_c::mPlayerMode[daPyMng_c::mPlayerType[mPlayerNo]];
    mCreateItemRelated = 0;
    if (daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] & 8) {
        mCreateItemRelated = 1;
        dInfo_c::m_instance->m_6c = true;
        mDokanCenterOffsetType = 3;
        mPowerup = daPyMng_c::mKinopioMode;
        daPyMng_c::mPlayerEntry[mPlayerNo] = 0;
        daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] &= ~8;
    }
    daPyMng_c::setPlayer(mPlayerNo, this);
    mPlayerLayer = mPlayerNo;
}

dAcPy_c::~dAcPy_c() {
    daPyMng_c::setPlayer(mPlayerNo, nullptr);
}


#pragma pop

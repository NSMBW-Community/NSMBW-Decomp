#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_player_effect_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_water_entry_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_yoshi_mdl.hpp>
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

inline float getSomeData(int idx) {
    static const float data[] = {
        0.0f, 0.08f, 0.07f, 0.11f, -1.5f, -1.25f,
        -1.0f, -0.1f, 1.5f, 1.5f, 2.5f, 1.5f,
        1.0f, -6.0f
    };
    return data[idx];
}

const float data_802f5a48[] = {
    25.0f, 3.627f, 3.0f, 0.6f, 0.0f,
    1.2f, 2.0f, 1.5f, 2.4f, 0.02f, 0.01f,
    0.05f, 0.2f, 0.3f, 0.3f, 0.8f, 0.7f,
    0.8f, 0.14f, 4.0f, 0.07f, 40.0f
};

inline float getData(int idx) {
    return data_802f5a48[idx];
}

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

#pragma push
#pragma pool_data off
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
                    changeState(StateID_Kani, (void *) KANI_CHANGE_JUMP_HANG);
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
#pragma pop

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
        float f = std::fabs(mSpeed.y * 0.15f);
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
    static const char *sc_wallSlidEffectID[] = {
        "Wm_mr_wallslip_ss_r",
        "Wm_mr_wallslip_ss_l",
        "Wm_mr_wallslip_s_r",
        "Wm_mr_wallslip_s_l",
        "Wm_mr_wallslip_r",
        "Wm_mr_wallslip_l"
    };
    dEf::createPlayerEffect(mPlayerNo, &mFunsuiSmokeEffect, sc_wallSlidEffectID[idx], 0, &jnt, nullptr, nullptr);
}

#pragma push
#pragma pool_data off
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
#pragma pop

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
            static const char *sc_wallKickEffectID[] = {
                "Wm_mr_wallkick_ss_r",
                "Wm_mr_wallkick_ss_l",
                "Wm_mr_wallkick_s_r",
                "Wm_mr_wallkick_s_l",
                "Wm_mr_wallkick_r",
                "Wm_mr_wallkick_l"
            };
            dEf::createPlayerEffect(mPlayerNo, sc_wallKickEffectID[idx], 0, &pos, nullptr, nullptr);
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
    static const float scSpinJumpEffectScale[3] = { 0.6f, 0.8f, 1.0f };
    static const float scSpinJumpEffectScale_K[3] = { 0.6f, 0.7f, 0.8f };
    if (2 <= mPlayerType && mPlayerType <= 3) {
        s = scSpinJumpEffectScale_K[getTallType(-1)];
    } else {
        s = scSpinJumpEffectScale[getTallType(-1)];
    }
    mVec3_c scale(s, s, s);
    if (i == 1) {
        if (m_1288 == 0) {
            static const char *sc_spinEffectID[] = {
                "Wm_mr_normalspin",
                "Wm_mr_normalspin_pm"
            };
            int idx = 0;
            if (mPowerup == POWERUP_PENGUIN_SUIT) {
                idx = 1;
            }
            dEf::createPlayerEffect(mPlayerNo, &mFollowEf2, sc_spinEffectID[idx], 0, &pos, nullptr, &scale);
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
                mAccelY = -0.5f;
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
        jmpInf_c jumpInf(sc_JumpSpeed, 0, 1);
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

void dAcPy_c::setSlipAction() {
    m_d94 = mBc.getSakaAngleBySpeed(mSpeedF);
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        if (isState(StateID_HipAttack)) {
            mDirection = mBc.getSakaDir();
        }
        changeState(StateID_PenguinSlide, 0);
    } else {
        changeState(daPlBase_c::StateID_Slip, 0); // [why daPlBase_c version?]
    }
}

void dAcPy_c::setSlipSE() {
    if (!mSpeedF) {
        return;
    }
    if (isNowBgCross(BGC_15) && mPowerup == POWERUP_MINI_MUSHROOM) {
        fn_80057f60(SE_PLY_SLIP_W, 0);
    } else {
        daPlBase_c::setSlipSE();
    }
}

void dAcPy_c::setSlipEffect() {
    daPlBase_c::setSlipSmokeEffect();
    setSlipSE();
}

void dAcPy_c::slipActionMove(int mode) {
    if (isNowBgCross(BGC_IS_FOOT) && std::fabs(mSpeedF) > 0.1f) {
        setSlipEffect();
    }
    daPlBase_c::slipActionMove(mode);
}

void dAcPy_c::initializeState_Slip() {
    daPlBase_c::initializeState_Slip();
    releaseCarryActor();
    onStatus(STATUS_9F);
    setWaterWalkFlag();
}

void dAcPy_c::finalizeState_Slip() {
    daPlBase_c::finalizeState_Slip();
    offStatus(STATUS_9F);
}

void dAcPy_c::executeState_Slip() {
    daPlBase_c::executeState_Slip();
}

void dAcPy_c::initializeState_RollSlip() {
    m_d8c = mPos.y;
    mSubstate = ROLL_SLIP_ACTION_0;
    mPyMdlMng.setAnm(117);
    mMaxSpeedF = getSlipMaxSpeedF();
    mMaxFallSpeed = -3.0f;
    m_94 = 2.0f;
    mSubstateTimer = 4;
    m_15ac = 10;
    setInvalidKeyTimer_LR(10, 1);
    releaseCarryActor();
    onStatus(STATUS_32);
    onStatus(STATUS_97);
    onStatus(STATUS_8F);
    onStatus(STATUS_31);
    onStatus(STATUS_88);
    onStatus(STATUS_4D);
    onStatus(STATUS_A1);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
}

void dAcPy_c::finalizeState_RollSlip() {
    offStatus(STATUS_32);
    offStatus(STATUS_9F);
    offStatus(STATUS_8F);
    offStatus(STATUS_31);
    offStatus(STATUS_88);
    offStatus(STATUS_4D);
    offStatus(STATUS_A1);
    m_15ac = 0;
    mTimer_a8 = 0;
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    if (std::fabs(mSpeedF) > 1.0f) {
        mSpeedF = sc_DirSpeed[mDirection];
    }
}

void dAcPy_c::executeState_RollSlip() {
    if (mSubstateTimer == 0) {
        offStatus(STATUS_4D);
    }
    gravitySet();
    moveSpeedSet();
    powerSet();
    turnAngle();
    if (mSubstateTimer2 != 0 && mKey.triggerJump()) {
        vf3fc(sc_JumpSpeed, mSpeedF * 0.8f, 1, 0, 1);
        return;
    }
    switch ((RollSlipSubstate_e) mSubstate) {
        case ROLL_SLIP_ACTION_0:
            fn_80057e70(SE_PLY_MOVE_ROLLING, false);
            switch (mSubstateTimer2) {
                case 0:
                    mAccelF = 0.0f;
                    m_94 = 1.5f;
                    mPyMdlMng.mpMdl->setRate(m_94);
                    if (isNowBgCross(BGC_IS_FOOT)) {
                        mSubstateTimer2 = 1;
                        mSpeedF = sc_DirSpeed[mDirection];
                        mSpeed.y = 2.0f;
                        mPyMdlMng.mpMdl->setFrame(0.0f);
                    } else if (isNowBgCross(BgCross1_e(BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
                        mSubstateTimer2 = 1;
                        mDirection = mDirection ^ 1;
                        mSpeedF = sc_DirSpeed[mDirection];
                        mSpeed.y = 0.0f;
                        mPyMdlMng.mpMdl->setFrame(0.0f);
                    }
                    break;
                case 1:
                    mPyMdlMng.mpMdl->setRate(1.0f);
                    if (isNowBgCross(BGC_IS_FOOT)) {
                        if (isNowBgCross(BGC_IS_FOOT)) {
                            if (!mKey.buttonCrouch()) {
                                setSlipAction_ToEnd();
                            } else {
                                setSlipAction_ToStoop();
                            }
                        }
                    }
                    break;
                case 2:
                    if (!isNowBgCross(BGC_IS_FOOT)) {
                        changeState(StateID_Fall, 0);
                        return;
                    }
                    float s = std::fabs(mSpeedF) * 0.5f;
                    if (s > 1.0f) {
                        s = 1.0f;
                    }
                    if (s < 0.1f) {
                        s = 0.1f;
                    }
                    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
                        s = -s;
                    }
                    mPyMdlMng.mpMdl->setRate(s);
                    if (std::fabs(mSpeedF) < 1.1f) {
                        if (!mKey.buttonCrouch()) {
                            setSlipAction_ToEnd();
                        } else {
                            setSlipAction_ToStoop();
                        }
                    }
            }
            // [Fallthrough...?]
        case ROLL_SLIP_ACTION_1:
            mMaxSpeedF = 0.0f;
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Crouch, (void *) 1);
            }
            break;
        case ROLL_SLIP_ACTION_2:
            mSpeedF = 0.0f;
            mMaxSpeedF = 0.0f;
            if (mPyMdlMng.isAnmStop()) {
                changeActionSlipEnd(BLEND_1);
            }
            break;
    }
}

void dAcPy_c::initializeState_PenguinSlide() {
    mSubstate = PENGUIN_SLIDE_ACTION_0;
    mMaxSpeedF = getSlipMaxSpeedF();
    if (isNowBgCross(BGC_IS_FOOT) && !isSlipSaka()) {
        float f = std::fabs(mSpeedF) / getSpeedData()[2] + 0.2f;
        if (f > 1.0f) {
            f = 1.0f;
        }
        if (f < 0.5f) {
            f = 0.5f;
        }
        m_54c = mSpeedF + f * sc_DirSpeed[mDirection];
        if (m_54c > 4.0f) {
            m_54c = 4.0f;
        }
        if (m_54c < -4.0f) {
            m_54c = -4.0f;
        }
        m_548 = 1;
    }
    if (mPos.y >= mWaterHeight - 4.0f) {
        onStatus(STATUS_C3);
    }
    mSubstateTimer = 8;
    releaseCarryActor();
    onStatus(STATUS_30);
    onStatus(STATUS_97);
    onStatus(STATUS_9F);
    onStatus(STATUS_A2);
    onStatus(STATUS_3C);
    onStatus(STATUS_88);

    mPyMdlMng.setAnm(139);
    vf434(27, 0);
    m_540 = 30;
    m_544 = getData(0);
}

void dAcPy_c::finalizeState_PenguinSlide() {
    offStatus(STATUS_30);
    offStatus(STATUS_9F);
    offStatus(STATUS_A2);
    offStatus(STATUS_3C);
    offStatus(STATUS_88);
    offStatus(STATUS_3E);
    offStatus(STATUS_C3);
    m_1598 = 0.0f;
    mAngle.x = 0;
    m_b98 = 10;
}

bool dAcPy_c::checkPenguinSlideJump() {
    if (isNowBgCross(BGC_IS_FOOT) && mKey.triggerJump()) {
        int dir = -1;
        if (mKey.buttonDown() || (mKey.buttonWalk(&dir) && dir == mDirection)) {
            mPyMdlMng.setAnm(140);
            fn_80057e70(SE_PLY_PNGN_JUMP, false);
            onStatus(STATUS_3D);
            offNowBgCross(BGC_IS_FOOT);
            float tmp = std::fabs(mSpeedF) - 1.5f;
            mSpeed.y = data_802f5a0c[2] + tmp * data_802f5a0c[3];
            if (mSpeed.y > data_802f5a0c[1]) {
                mSpeed.y = data_802f5a0c[1];
            }
            if (mSpeed.y < data_802f5a0c[2]) {
                mSpeed.y = data_802f5a0c[2];
            }
        } else {
            vf3fc(1.0f, mSpeedF * 0.8f, 1, 0, 1);
        }
        return true;
    }
    return false;
}

void dAcPy_c::executeState_PenguinSlide() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    m_1598 = 0.0f;
    if (!isStatus(STATUS_C3) && mPos.y >= mWaterHeight - 1.0f) {
        onStatus(STATUS_C3);
    }
    if (isNowBgCross(BGC_17)) {
        mSpeed.y = -2.0f;
    }
    if (m_544 != 0) {
        m_544--;
    }
    if (isNowBgCross(BGC_IS_FOOT) && m_540 != 0) {
        m_540--;
    }
    s16 newAng = 0;
    if (isNowBgCross(BGC_IS_FOOT)) {
        newAng = mBc.getSakaAngle(mDirection);
    }
    sLib::addCalcAngle(&mAngle.x.mAngle, newAng, 4, 0x2000, 0x40);
    if (isNowBgCross(BGC_IS_FOOT)) {
        switch ((PenguinSlideSubstate_e) mSubstate) {
            case PENGUIN_SLIDE_ACTION_0:
                if (checkPenguinSlideJump()) {
                    return;
                }
                break;
            case PENGUIN_SLIDE_ACTION_1:
                if (setCrouchJump()) {
                    return;
                }
                break;
            case PENGUIN_SLIDE_ACTION_2:
                if (checkJumpTrigger()) {
                    return;
                }
                break;
        }
    }
    if (mPowerup != POWERUP_PENGUIN_SUIT) {
        changeState(daPlBase_c::StateID_Walk, 0);
        return;
    }
    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
        mDirection = mDirection ^ 1;
    }
    int ang = turnAngle();
    switch ((PenguinSlideSubstate_e) mSubstate) {
        case PENGUIN_SLIDE_ACTION_0:
            if (mPlayerType == 1) {
                m_1598 = -0.5f;
            } else {
                m_1598 = 1.0f;
            }
            if (!isStatus(STATUS_76) && m_540 == 0) {
                if (!isNowBgCross(BGC_IS_FOOT)) {
                    offStatus(STATUS_3E);
                }
                if (!isStatus(STATUS_3E) && (isNowBgCross(BGC_IS_FOOT) || mSpeed.y < 0.0f)) {
                    int dir = -1;
                    if (!(mKey.buttonDown() || (mKey.buttonWalk(&dir) && dir == mDirection))) {
                        onStatus(STATUS_52);
                        changeState(StateID_Walk, 0);
                        return;
                    }
                }
            }
            if (isNowBgCross(BGC_IS_FOOT)) {
                setPenguinSlideEffect();
                if (mPyMdlMng.getAnm() != 139) {
                    mPyMdlMng.setAnm(139);
                }
                if (isStatus(STATUS_3D)) {
                    offStatus(STATUS_3D);
                    setPenguinSlideLandEffect();
                }
            } else if (isStatus(STATUS_3D)) {
                if (mSpeed.y < 0.0f && mPyMdlMng.getAnm() != 141) {
                    mPyMdlMng.setAnm(141);
                }
            }
            daPlBase_c::slipActionMove(ang);
            if (isNowBgCross(BGC_IS_FOOT)) {
                if (mGroundType == GROUND_TYPE_WATER) {
                    fn_80057fd0(SE_PLY_PNGN_SLIP_SEA, std::fabs(mSpeedF), false);
                } else {
                    fn_80057f60(SE_PLY_PNGN_SLIP, false);
                }
            }
            break;
        case PENGUIN_SLIDE_ACTION_1:
            mMaxSpeedF = 0.0f;
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Crouch, (void *) 1);
                return;
            }
            break;
        case PENGUIN_SLIDE_ACTION_2:
            if (mPyMdlMng.isAnmStop()) {
                changeActionSlipEnd(BLEND_1);
                return;
            }
            break;
    }
    if (isNowBgCross(BGC_IS_FOOT)) {
        if (isSlipSaka()) {
            if (mMaxSpeedF * mSpeedF > 0.0f) {
                mAccelF = 1.2f * mAccelF;
                m_544 = data_802f5a0c[0];
            } else {
                m_548 = 0;
                if (getPowerChangeType(true)) {
                    mAccelF = 0.01f;
                }
            }
        } else {
            if (getPowerChangeType(true)) {
                mAccelF = 0.005f;
            }
            if (m_544 && mMaxSpeedF == 0.0f) {
                mAccelF = 0.0f;
            }
        }
    } else {
        mAccelF = 0.0f;
        mMaxSpeedF = std::fabs(mSpeedF);
    }
    if (m_548 == 1) {
        if (isStatus(STATUS_AC) || std::fabs(mSpeedF) >= std::fabs(m_54c)) {
            m_548 = 0;
        } else {
            mAccelF = 0.01;
            mMaxSpeedF = m_54c;
        }
    }
}

void dAcPy_c::setPenguinSlideEffect() {
    if (!isNowBgCross(BGC_IS_FOOT)) {
        return;
    }
    mAng3_c ang(0, 0, 0);
    if (mDirection == 1) {
        ang.y = -0x8000;
    }
    mVec3_c efPos;
    if (mGroundType == GROUND_TYPE_WATER) {
        efPos.set(mPos.x, mWaterHeight, mPos.z);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf4, "Wm_mr_waterswim", 0, &efPos, nullptr, nullptr);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf5, "Wm_en_cmnwater02", 0, &efPos, &ang, nullptr);
    } else {
        mpMdlMng->mpMdl->getJointPos(&efPos, 1);
        static const char *sc_penguinSlidEffectID[] = {
            "Wm_mr_penguinsmoke",
            "Wm_mr_penguinsnow",
            "Wm_mr_pdesertsmoke",
            "Wm_mr_penguinice",
            "Wm_mr_penguinsmoke",
            "Wm_mr_waterswim",
            "Wm_mr_penguinsmoke",
            "Wm_mr_pdesertsmoke",
            "Wm_mr_penguinsmoke",
            "Wm_mr_pbeachsmoke",
            "Wm_mr_penguinsmoke",
            "Wm_mr_penguinsmoke",
            "Wm_mr_penguinsmoke"
        };
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf4, sc_penguinSlidEffectID[mGroundType], 0, &efPos, nullptr, nullptr);
        switch (mGroundType) {
            case GROUND_TYPE_SNOW:
                dEf::createPlayerEffect(mPlayerNo, &mLevelEf5, "Wm_mr_p_snowslip", 0, &efPos, &ang, nullptr);
                break;
            case GROUND_TYPE_ICE:
                dEf::createPlayerEffect(mPlayerNo, &mLevelEf5, "Wm_mr_p_iceslip", 0, &efPos, &ang, nullptr);
                break;
            default:
            break;
        }
    }
}

void dAcPy_c::setPenguinSlideLandEffect() {
    setLandSmokeEffect(0);
    if (mGroundType == GROUND_TYPE_WATER) {
        fn_80057e70(SE_PLY_SPLASH_SHALLOW, false);
    }
}

bool dAcPy_c::isWaitFrameCountMax() {
    return mWaitFrameCount >= 80;
}

#pragma push
#pragma pool_data off
bool dAcPy_c::checkWalkNextAction() {
    if (checkCarryThrow()) {
        return true;
    }
    if (checkSlip()) {
        return true;
    }
    if (checkJumpTrigger()) {
        return true;
    }
    if (!isNowBgCross(BGC_IS_FOOT)) {
        mSpeed.y = 0.0f;
        if (setDelayHelpJump()) {
            return true;
        }
        if (m_da0 == 0) {
            changeState(StateID_Fall, 0);
            return true;
        }
    }
    if (isState(StateID_Walk) && mKey.buttonWalk(nullptr) && mDirection != m_ca0) {
        mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
        if (checkTurn()) {
            return true;
        }
    }
    if (isState(StateID_Walk) || isState(StateID_Land)) {
        if (checkCrouch()) {
            return true;
        }
    }
    return false;
}
#pragma pop

void dAcPy_c::setWalkActionAnm(AnmBlend_e blend) {
    float speed = std::fabs(mSpeedF);
    float f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        if (speed <= getSpeedData()[0]) {
            float tmp = speed * 2.0f * 1.45f;
            f = (tmp < 2.0f) ? 2.0f : tmp;
        } else {
            float tmp = speed * 1.5f * 1.45f;
            f = (tmp < 0.5f) ? 0.5f : tmp;
        }
    } else {
        if (speed <= getSpeedData()[0]) {
            float tmp = speed * 2.0f;
            f = (tmp < 2.0f) ? 2.0f : tmp;
        } else {
            float tmp = speed * 1.5f;
            f = (tmp < 0.5f) ? 0.5f : tmp;
        }
    }
    float g = 4.0f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        if (speed <= getSpeedData()[0]) {
            g = 8.7f;
        } else {
            g = 5.8f;
        }
    }
    if (
        isNowBgCross(BGC_ON_BELT_L) && mSpeedF > 0.0f ||
        isNowBgCross(BGC_ON_BELT_R) && mSpeedF < 0.0f
    ) {
        float tmp = speed * 2.5f;
        f = (tmp < 2.0f) ? 2.0f : ((tmp > g) ? g : tmp);
    }
    float h = f;
    if (isSaka() && mBc.getSakaUpDown(mDirection) == 1 && mMaxSpeedF && mSpeedF * sc_DirSpeed[mDirection] >= 0.0f) {
        h = f * 2.0f;
        if (h > g) {
            h = g;
        }
    }
    if (mPowerup != POWERUP_PENGUIN_SUIT && isNowBgCross(BGC_ON_ICE) && speed < getSpeedData()[1]) {
        float tmp = h * 8.0f;
        h = (tmp > g) ? g : tmp;
    }
    if (isNowBgCross(BgCross1_e(BGC_ON_SINK_SAND | BGC_IN_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            h *= 0.7f;
        } else {
            h *= 0.5f;
        }
    }
    setNormalWalkAnm(blend, h);
}

void dAcPy_c::setWaitActionAnm(AnmBlend_e blend) {
    if (dScStage_c::m_isStaffCredit && isStatus(STATUS_75)) {
        mPyMdlMng.setAnm(175);
        return;
    }
    if (isStatus(STATUS_63)) {
        mPyMdlMng.setAnm(172);
        return;
    }
    /// @unofficial
    static const float scSakaCheckOffset[] = { 2.0f, 0.0f, -2.0f, 4.0f, 0.0f, -4.0f };
    s16 ang = m_d98;
    int count = 0;
    float sum = 0.0f;
    for (int i = 0; i < 3; i++) {
        mVec3_c p(mPos.x + scSakaCheckOffset[3 + i], mPos.y + 5.0f, mPos.z);
        s16 tmpAng;
        float f;
        if (dBc_c::checkGroundAngle(&p, &f, &tmpAng, mLayer, m_ca1, -1, nullptr, 0) && std::fabs(f - mPos.y) < 8.0f) {
            count++;
            sum += tmpAng;
        }
    }
    if (count != 0) {
        ang = sum / count;
    }
    static const int scSlopeWaitAnm[] = { 150, 151 };
    static const int scLongWaitAnm[] = { 135, 136 };
    if (ang != 0 || m_80 != 0) {
        ang = -ang * sc_DirSpeed[mDirection];
        float f = ang * mAng::AngleToDegreeCoefficient + 70.0f;
        if (f < 0.0f) {
            f = 0.0f;
        }
        if (f > 140.0f) {
            f = 140.0f;
        }
        if (blend == BLEND_1) {
            u8 anmID = scLongWaitAnm[mDirection];
            mPyMdlMng.setAnm(anmID, f);
        } else {
            u8 anmID = scSlopeWaitAnm[mDirection];
            mPyMdlMng.setAnm(anmID, 0.0f, f);
        }
        mPyMdlMng.mpMdl->m_17c |= 0x40;
        m_80 = 1;
    } else {
        if (blend == BLEND_1) {
            u8 anmID = scLongWaitAnm[mDirection];
            mPyMdlMng.setAnm(anmID);
        } else {
            u8 anmID = scSlopeWaitAnm[mDirection];
            mPyMdlMng.setAnm(anmID, 0.0f, 0.0f);
        }
    }
}

bool dAcPy_c::isIceSlipAnmPlay() {
    if (isNowBgCross(BGC_ON_ICE) && !mKey.buttonWalk(nullptr) && mPowerup != POWERUP_PENGUIN_SUIT) {
        return true;
    }
    return false;
}

void dAcPy_c::setNormalWalkAnm(AnmBlend_e blend, float speedRate) {
    if (isIceSlipAnmPlay()) {
        switch (m_f0) {
            case 0:
                if (isNowBgCross(BGC_IS_LIFT) && std::fabs(mBc.mIceSpeed) > 1.0f || std::fabs(mSpeedF) > 0.82f) {
                    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
                        mPyMdlMng.setAnm(125);
                    } else {
                        mPyMdlMng.setAnm(124);
                    }
                    m_f0 = 1;
                } else {
                    setWaitActionAnm(blend);
                }
                break;
            case 1:
                calcAccOnIceLift();
                if (mPyMdlMng.isAnmStop()) {
                    setWaitActionAnm(blend);
                    m_f0 = 2;
                }
                break;
            case 2:
                calcAccOnIceLift();
                setWaitActionAnm(blend);
                break;
        }
        fn_80057fd0(SE_PLY_ICE_SLIP, std::fabs(mSpeedF), false);
    } else {
        m_f0 = 0;
        onStatus(STATUS_62);
        u32 anmID = -1;
        if (isCarry()) {
            anmID = 26;
            if (mPyMdlMng.mpMdl->m_17c & 0x4) {
                anmID = 110;
            }
            blend = BLEND_0;
        } else {
            if (
                isNowBgCross(BGC_ON_BELT_L) && mSpeedF > 0.0f ||
                isNowBgCross(BGC_ON_BELT_R) && mSpeedF < 0.0f
            ) {
                anmID = 2;
            } else {
                float speed = std::fabs(mSpeedF);
                if (speed <= getSpeedData()[0]) {
                    anmID = 2;
                } else if (speed < getSpeedData()[2]) {
                    anmID = 3;
                } else {
                    anmID = 4;
                }
            }
        }
        if (!mPyMdlMng.isAnm(anmID)) {
            float rate = 0.0f;
            if (blend == BLEND_1) {
                rate = dPyMdlMng_c::m_hio.mPyAnm.mAnm[anmID].mBlendDuration;
            }
            mPyMdlMng.setAnm(anmID, speedRate, rate, 0.0f);
        } else {
            mPyMdlMng.mpMdl->setRate(speedRate);
        }
    }
}

void dAcPy_c::walkActionInit_Wait(AnmBlend_e blend) {
    mSubstate = 0;
    mSubstateTimer = 150;
    m_15b4 = 8;
    setWaitActionAnm(blend);
}

void dAcPy_c::walkAction_Wait() {
    if (!isWaitFrameCountMax()) {
        mWaitFrameCount++;
    }
    if (mSpeedF) {
        walkActionInit_Move(BLEND_1);
        return;
    }
    setWaitActionAnm(BLEND_0);
    if (isStatus(STATUS_63)) {
        mAngle.y.chase(0, 0x400);
    } else {
        turnAngle();
    }
    if (mSubstateTimer == 0) {
        m_15b2 = 8;
    }
    if (isStatus(STATUS_C1)) {
        if (!sLib::calcTimer(&m_15b4)) {
            offStatus(STATUS_C1);
            m_15b6 = 30;
        }
    }
}

void dAcPy_c::walkActionInit_Move(AnmBlend_e blend) {
    mSubstate = 1;
    setWalkActionAnm(blend);
}

void dAcPy_c::walkAction_Move() {
    m_80 = 0;
    mWaitFrameCount = 0;
    turnAngle();
    setRunOnWaterEffect();
    daPlBase_c::walkAction_Move();
    if (m_88 == 0 && !isNowBgCross(BGC_15)) {
        m_88 = 1;
    }
    if (m_88 == 1) {
        setWaterWalkFlag();
    }
}

void dAcPy_c::initializeState_Walk() {
    AnmBlend_e blend = (AnmBlend_e) (int) mStateChangeParam;
    calcSpeedOnIceLift();
    m_88 = 1;
    if (isOldBgCross(BGC_14)) {
        m_88 = 0;
    }
    if (mSpeedF) {
        walkActionInit_Move(blend);
    } else {
        walkActionInit_Wait(blend);
    }
    if (isItemKinopio()) {
        setControlDemoKinopioWalk();
    }
    onStatus(STATUS_9B);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_9F);
    onStatus(STATUS_92);
    onStatus(STATUS_A0);
    onStatus(STATUS_C2);
    onStatus(STATUS_A2);
    onStatus(STATUS_A3);
    m_80 = 0;
}

void dAcPy_c::executeState_Walk() {
    daPlBase_c::executeState_Walk();
    mAccelY = getGravityData()[0];
    maxFallSpeedSet();
    moveSpeedSet();
    grandPowerSet();
    if (isNowBgCross(BGC_IS_FOOT)) {
        if (isNowBgCross(BGC_17)) {
            onStatus(STATUS_C2);
        } else {
            offStatus(STATUS_C2);
        }
    }
    offStatus(STATUS_8F);
    if (isNowBgCross(BgCross1_e(BGC_ON_SINK_SAND | BGC_IN_SINK_SAND))) {
        onStatus(STATUS_8F);
    }
    if (!checkWalkNextAction()) {
        switch (mSubstate) {
            case 0:
                walkAction_Wait();
                break;
            case 1:
                walkAction_Move();
                break;
        }
        if (!(mPyMdlMng.getFlags() & 0x1000) && mPyMdlMng.getFlags2() & 0x40) {
            mPyMdlMng.mpMdl->m_17c &= ~0x40;
            mPyMdlMng.mpMdl->releaseBodyAnm(0.0f);
        }
    }
}

void dAcPy_c::finalizeState_Walk() {
    daPlBase_c::finalizeState_Walk();
    mWaitFrameCount = 0;
    offStatus(STATUS_9B);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_92);
    offStatus(STATUS_A0);
    offStatus(STATUS_A2);
    offStatus(STATUS_8F);
    offStatus(STATUS_A3);
    m_f0 = 0;
    if (mPyMdlMng.getFlags2() & 0x40) {
        mPyMdlMng.mpMdl->m_17c &= ~0x40;
        mPyMdlMng.mpMdl->releaseBodyAnm(0.0f);
    }
}

void dAcPy_c::setRunOnWaterEffect() {
    if (isNowBgCross(BGC_17)) {
        static const char *sc_waterRunEffectID[] = {
            "Wm_mr_waterrun_l_ss",
            "Wm_mr_waterrun_r_ss"
        };
        dEf::createPlayerEffect(mPlayerNo, &mFunsuiSmokeEffect, sc_waterRunEffectID[mDirection], 0, &mPos, nullptr, nullptr);
    }
}

void dAcPy_c::initializeState_Turn() {
    onStatus(STATUS_9F);
    onStatus(STATUS_A2);
    mSubstate = 0;
    if (getPowerChangeType(false) == POWER_CHANGE_1) {
        mPyMdlMng.setAnm(122, 0.0f, 0.0f);
    } else {
        mPyMdlMng.setAnm(14, 0.0f, 0.0f);
    }
    mAngle.y = getMukiAngle((mSpeedF < 0.0f) ? DIR_LR_L : DIR_LR_R);
    mDirection ^= 1;
    mMaxSpeedF = 0.0f;
    mAccelY = getGravityData()[0];
    maxFallSpeedSet();
    turnPowerSet();
    mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
}

void dAcPy_c::finalizeState_Turn() {
    offStatus(STATUS_9F);
    offStatus(STATUS_A2);
    fadeOutTurnEffect();
}

void dAcPy_c::turnPowerSet() {
    u8 dir;
    if (mSpeedF < 0.0f) {
        dir = 1;
    } else if (mSpeedF > 0.0f) {
        dir = 0;
    } else {
        dir = mDirection;
    }
    sTurnPowerData data;
    getTurnPower(data);
    switch (mSubstate) {
        case 0:
        case 1:
            if (isSaka()) {
                if (mBc.getSakaUpDown(dir) == 1) {
                    mAccelF = data.mSakaUp;
                } else {
                    mAccelF = data.mSakaDown;
                }
            } else {
                mAccelF = data.mNormal;
            }
            if (isStatus(STATUS_89)) {
                mAccelF = getAccelF() * scTurnPowerUpRate;
            }
            break;
        case 2:
            mAccelF = data.mAir;
            break;
    }
}

void dAcPy_c::setTurnSmokeEffect() {
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 7);
    setBrakeSmokeEffect(efPos);
    setTurnSmokeEffect();
}

void dAcPy_c::executeState_Turn() {
    turnPowerSet();
    if (!checkWalkNextAction()) {
        switch (mSubstate) {
            case 0:
                setTurnSmokeEffect();
                if (mSpeedF == 0.0f) {
                    mSubstateTimer = 8;
                    mSubstate = 1;
                }
                if (!mKey.buttonWalk(nullptr)) {
                    if (mSpeedF) {
                        mDirection = mSpeedF < 0.0f ? DIR_LR_L : DIR_LR_R;
                    }
                    setTurnEnd();
                }
                break;
            case 1:
                setTurnSmokeEffect();
                if (mDirection != m_ca0) {
                    setTurnEnd();
                } else if (mSubstateTimer == 0) {
                    fadeOutTurnEffect();
                    mSubstate = 2;
                    if (getPowerChangeType(false) == POWER_CHANGE_1) {
                        mPyMdlMng.setAnm(123, 0.0f, 0.0f);
                    } else {
                        mPyMdlMng.setAnm(15, 0.0f, 0.0f);
                    }
                }
                break;
            case 2:
                float maxSpeed = 0.0f;
                int dir;
                if (mKey.buttonWalk(&dir)) {
                    if (mKey.buttonDush()) {
                        maxSpeed = sc_DirSpeed[dir] * getSpeedData()[2];
                    } else {
                        maxSpeed = sc_DirSpeed[dir] * getSpeedData()[0];
                    }
                }
                mMaxSpeedF = maxSpeed;
                if (mDirection != m_ca0 || mPyMdlMng.isAnmStop()) {
                    setTurnEnd();
                }
        }
    }
}

bool dAcPy_c::checkCrouch() {
    if (
        !isStatus(STATUS_51) &&
        !isStatus(STATUS_0F) &&
        !isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) &&
        !isStatus(STATUS_BE)
    ) {
        if (isCarry()) {
            return false;
        }
        if (mKey.buttonCrouch()) {
            if (mPowerup == POWERUP_PENGUIN_SUIT && isStatus(STATUS_0B)) {
                changeState(StateID_PenguinSlide, 0);
                return true;
            }
            changeState(StateID_Crouch, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setCrouchSmokeEffect() {
    if (!mSpeedF) {
        return;
    }
    if (!isNowBgCross(BGC_14)) {
        mVec3_c efPos;
        mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
        setBrakeSmokeEffect(efPos);
    }
    setSlipSE();
}

void dAcPy_c::initializeState_Crouch() {
    if (!isNowBgCross(BGC_14)) {
        mSubstate = CROUCH_GROUND;
        onStatus(STATUS_9F);
    } else {
        mSubstate = CROUCH_WATER;
        onStatus(STATUS_AA);
    }
    switch ((int) mStateChangeParam) {
        case 0:
            if (!isNowBgCross(BGC_14)) {
                mPyMdlMng.setAnm(21, 3.0f);
            } else {
                mPyMdlMng.setAnm(21, dPyMdlBase_c::scWaterCrouchAnmSpeed, 3.0f, 0.0f);
            }
            break;
        case 1:
            mPyMdlMng.setAnm(20, 3.0f, 0.0f);
            break;
        case 2:
            mPyMdlMng.setAnm(120);
            break;
    }
    mAngle.y = getMukiAngle(mDirection);
    fn_80057e70(SE_PLY_QUAT, true);
    onStatus(STATUS_A8);
    onStatus(STATUS_51);
    onStatus(STATUS_A2);
    onStatus(STATUS_A3);
}

void dAcPy_c::finalizeState_Crouch() {
    offStatus(STATUS_A8);
    offStatus(STATUS_9D);
    offStatus(STATUS_9F);
    offStatus(STATUS_AA);
    offStatus(STATUS_51);
    offStatus(STATUS_8F);
    offStatus(STATUS_A2);
    offStatus(STATUS_A3);
    m_2e8 = 0;
}

void dAcPy_c::CrouchActionGround() {
    if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) && setCancelCrouch()) {
        return;
    }
    if (!setCrouchJump() && !checkCrouchSlip()) {
        if (!isNowBgCross(BGC_IS_FOOT)) {
            mSpeed.y = 0.0f;
            changeState(StateID_SitJump, (void *) 1);
        } else {
            setCrouchSmokeEffect();
            if (mKey.buttonCrouch() || !setCancelCrouch()) {
                offStatus(STATUS_C1);
            }
        }
    }
}

void dAcPy_c::CrouchActionWater() {
    if (mKey.triggerJump()) {
        if (!isNowBgCross(BGC_16)) {
            setCrouchJump();
            onStatus(STATUS_AA);
        } else {
            if (!checkStandUpRoofOnLift()) {
                changeState(StateID_Swim, (void *) 0);
                setWaterGroundJump();
            }
        }
    } else {
        if (!isNowBgCross(BGC_IS_FOOT)) {
            changeState(StateID_Swim, (void *) 0);
        } else {
            if (!mKey.buttonCrouch() && setCancelCrouch()) {
                changeState(StateID_Swim, (void *) 0);
            }
        }
    }
}

void dAcPy_c::executeState_Crouch() {
    gravitySet();
    maxFallSpeedSet();
    if (isNowBgCross(BGC_IS_FOOT)) {
        mMaxSpeedF = 0.0f;
        offStatus(STATUS_8F);
    } else {
        simpleMoveSpeedSet();
        onStatus(STATUS_8F);
    }
    powerSet();
    turnAngle();
    m_2e8.chase(0, 0x200);
    mAngle.y += m_2e8;
    typedef void (dAcPy_c::*ActionProc)();
    static ActionProc l_CrouchActionProc[] = {
        &dAcPy_c::CrouchActionGround,
        &dAcPy_c::CrouchActionWater
    };
    (this->*l_CrouchActionProc[mSubstate])();
    int anmID = mPyMdlMng.getAnm();
    if (anmID == 21 || anmID == 120) {
        if (mPyMdlMng.isAnmStop()) {
            mPyMdlMng.setAnm(20);
        }
    }
}

bool dAcPy_c::setCancelCrouch() {
    mAngle.y = getMukiAngle(mDirection);
    return daPlBase_c::setCancelCrouch();
}

dAcPy_c *dAcPy_c::getCarryPlayer() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr && actor->mKind == STAGE_ACTOR_PLAYER) {
        return (dAcPy_c *) actor;
    }
    return nullptr;
}

dActor_c *dAcPy_c::getCarryPropelBlock() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr && actor->mProfName == fProfile::AC_PROP_BLOCK) {
        return actor;
    }
    return nullptr;
}

dActor_c *dAcPy_c::getCarryPropelActor() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr) {
        if (actor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *player = (dAcPy_c *) actor;
            if (player->mPowerup == POWERUP_PROPELLER_SHROOM) {
                return actor;
            }
        } else if (actor->mProfName == fProfile::AC_PROP_BLOCK) {
            return actor;
        }
    }
    return nullptr;
}

dActor_c *dAcPy_c::getCarryHardBlock() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr && (
        actor->mProfName == fProfile::AC_LIGHT_BLOCK ||
        actor->mProfName == fProfile::AC_PROP_BLOCK
    )) {
        return actor;
    }
    return nullptr;
}

bool dAcPy_c::isLiftUp() {
    if (isCarry() && m_1308 == 0) {
        return true;
    }
    return false;
}

bool dAcPy_c::isCarryMamePlayer() {
    dAcPy_c *player = getCarryPlayer();
    if (player != nullptr && player->mPowerup == POWERUP_MINI_MUSHROOM) {
        return true;
    }
    return false;
}

bool dAcPy_c::isLiftUpExceptMame() {
    if (isLiftUp() && !isCarryMamePlayer()) {
        return true;
    }
    return false;
}

float dAcPy_c::getLiftUpOffset() {
    return m_1304 * sGlobalData_c<dAcPy_c>::mData.mPos[getTallType(-1)].y;
}

mVec3_c dAcPy_c::getCarryPos() {
    mVec3_c jnt1, jnt2;
    mPyMdlMng.mpMdl->getJointPos(&jnt1, 11);
    mPyMdlMng.mpMdl->getJointPos(&jnt2, 14);
    if (isCarry()) {
        mVec3_c res = (jnt1 + jnt2) / 2.0f;
        res.x = dScStage_c::getLoopPosX(res.x);
        if (mPyMdlMng.getFlags2() & 4) {
            res.y = mPos.y + getLiftUpOffset();
        }
        return res;
    } else {
        jnt2.x = dScStage_c::getLoopPosX(jnt2.x);
        return jnt2;
    }
}

mVec3_c dAcPy_c::getLiftUpPos() {
    mMtx_c mtx;
    getModel()->getJointMtx(&mtx, 11);
    mtx.concat(mMtx_c::createTrans(5.0f, 0.0f, 0.0f));
    mVec3_c tmp;
    mtx.multVecZero(tmp);

    getModel()->getJointMtx(&mtx, 14);
    mtx.concat(mMtx_c::createTrans(5.0f, 0.0f, 0.0f));
    mVec3_c tmp2;
    mtx.multVecZero(tmp2);

    mVec3_c res = (tmp + tmp2) / 2.0f;
    res.x = dScStage_c::getLoopPosX(res.x);
    if (mPyMdlMng.getFlags2() & 4) {
        res.y = mPos.y + getLiftUpOffset();
        if (res.y < mPos.y + 2.0f) {
            res.y = mPos.y + 2.0f;
        }
    }
    return res;
}

void dAcPy_c::clearSpinLiftUpReserve() {
    m_12fc = BASE_ID_NULL;
    m_1300 = 256.0f;
}

void dAcPy_c::checkSpinLiftUpReserve(dCc_c *cc) {
    daPlBase_c *actor = (daPlBase_c *) cc->getOwner();
    if (
        actor == nullptr ||
        !actor->isSpinLiftUpEnable() ||
        !fn_8012e540(actor, false)
    ) {
        return;
    }
    float f = std::fabs(mPos.x - cc->getCenterPosX());
    if (m_1300 > f) {
        m_12fc = actor->mUniqueID;
        m_1300 = f;
    }
}

void dAcPy_c::setSpinLiftUpReserve() {
    if (
        !isStatus(STATUS_7A) &&
        !isDemo() &&
        !isCarry() &&
        !isStatus(STATUS_04) &&
        !isStatus(STATUS_06) &&
        !isStatus(STATUS_08)
    ) {
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_12fc);
        if (actor != nullptr && actor->isSpinLiftUpEnable()) {
            mCarryActorID = actor->mUniqueID;
            m_1308 = 0;
            mPyMdlMng.mpMdl->m_17c |= 4;
            if (actor->mKind == STAGE_ACTOR_PLAYER) {
                dAcPy_c *player = (dAcPy_c *) actor;
                mPyMdlMng.mpMdl->mpSpinLiftParentMdl = player->getModel();
            }
            changeState(StateID_LiftUp, 0);
            dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_7, 0, false);
            actor->setSpinLiftUpActor(this);
        }
        clearSpinLiftUpReserve();
    }
}

void dAcPy_c::checkSpinLiftUpRoofHeight() {
    mVec3_c pos(mPos.x, mPos.y + 4.0f, mPos.z);
    float _unused = m_d2c;
    float height;
    float f = 0.0f;
    if (dBc_c::checkTenjou(&pos, &height, mLayer, 1) && f > height) {
        f = height;
    }
    m_12f8 = f;
}

bool dAcPy_c::fn_8012e540(dActor_c *actor, bool b) {
    if (actor == nullptr || isCarry() || !mKey.buttonCarry()) {
        return false;
    }
    if (!b) {
        return true;
    }
    if (isStatus(STATUS_92)) {
        mCarryActorID = actor->mUniqueID;
        m_1308 = 1;
        mPyMdlMng.mpMdl->m_17c |= 2;
        mPyMdlMng.mpMdl->setBodyAnm(28, 1.0f, 0.0f, 0.0f);
        vf434(18, 0);
        return true;
    }
    return false;
}

bool dAcPy_c::cancelCarry(dActor_c *carriedActor) {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor == carriedActor || carriedActor == nullptr) {
        mPyMdlMng.mpMdl->m_17c &= ~0x6;
        mPyMdlMng.mpMdl->releaseBodyAnm(0.0f);
        mPyMdlMng.mpMdl->mpSpinLiftParentMdl = nullptr;
        mCarryActorID = BASE_ID_NULL;
        return true;
    }
    return false;
}

void dAcPy_c::releaseCarryActor() {
    if (!isCarry()) {
        return;
    }
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr) {
        if (actor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *player = (dAcPy_c *) actor;
            player->setCarryOffFall(this);
        } else {
            actor->setCarryFall(this, 20);
            actor->mCarryingFlags |= CARRY_RELEASE;
            actor->mThrowDirection = mDirection;
        }
    }
    cancelCarry(actor);
}

void dAcPy_c::setCarryOffFall(const dAcPy_c *carrier) {
    m_d8c = mPos.y;
    mDirection = carrier->mDirection;
    mSpeedF = sc_DirSpeed[mDirection];
    mPos.x += mSpeedF * 5.0f;

    changeState(StateID_Fall, 0);
    onStatus(STATUS_4D);
}

#pragma push
#pragma pool_data off
bool dAcPy_c::checkEnableThrow() {
    if (!isCarry()) {
        return false;
    }
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor == nullptr) {
        cancelCarry(nullptr);
        return false;
    }
    return !mKey.buttonCarry();
}

bool dAcPy_c::checkCarryThrow() {
    if (checkEnableThrow()) {
        if (isState(StateID_Propel)) {
            changeState(StateID_PropelThrow, 0);
            return true;
        } else {
            changeState(StateID_Throw, 0);
            return true;
        }
    }
    return false;
}
#pragma pop

void dAcPy_c::initializeThrowCommonBase() {
    vf434(19, 0);
    if (isNowBgCross(BGC_14)) {
        mPyMdlMng.setAnm(37);
    } else {
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
        if (actor->mKind == STAGE_ACTOR_PLAYER) {
            mPyMdlMng.setAnm(111, 0.0f, 0.0f);
        } else {
            mPyMdlMng.setAnm(27, 0.0f, 0.0f);
        }
    }
    mAngle.y = getMukiAngle(mDirection);
    onStatus(STATUS_18);
}

void dAcPy_c::initializeThrowCommon() {
    initializeThrowCommonBase();
    onStatus(STATUS_AA);
    onStatus(STATUS_97);
}

void dAcPy_c::finalizeThrowCommonBase() {
    offStatus(STATUS_18);
}

void dAcPy_c::finalizeThrowCommon() {
    finalizeThrowCommonBase();
    if (mSubstate == 0) {
        releaseCarryActor();
    }
    offStatus(STATUS_AA);
    offStatus(STATUS_97);
    offStatus(STATUS_92);
    offStatus(STATUS_8F);
}

void dAcPy_c::setThrowActor() {
    if (!isCarry()) {
        return;
    }
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (actor != nullptr) {
        if (actor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *player = (dAcPy_c *) actor;
            player->changeState(StateID_RollSlip, 0);

            player->mSpeed.y = sGlobalData_c<dAcPy_c>::mData.mThrowSpeed1;
            float speed = getDirSpeed();
            speed *= sGlobalData_c<dAcPy_c>::mData.mThrowSpeed2;
            if (getDirSpeed() * mSpeedF >= 0.0f) {
                speed += mSpeedF * 0.35f;
                if (speed > sGlobalData_c<dAcPy_c>::mData.mThrowSpeedMax) {
                    speed = sGlobalData_c<dAcPy_c>::mData.mThrowSpeedMax;
                }
            }
            player->mSpeedF = speed;
            player->setNoHitPlayer(this, 10);

            mVec3_c pos = player->mPos;

            float spd = speed + sc_DirSpeed[mDirection] * 5.0f;
            if (spd < -8.0f) {
                spd = -8.0f;
            } else if (spd > 8.0f) {
                spd = 8.0f;
            }

            mVec3_c p(mPos.x, pos.y, pos.z);
            pos.x += spd;

            float thing = sc_DirSpeed[mDirection] * std::fabs(getWallBgPointData()->mDist / 4096.0f);
            mVec3_c playerPos(pos.x + thing, pos.y, pos.z);
            float hitX;
            if (dBc_c::checkWall(&p, &playerPos, &hitX, mLayer, 1, nullptr)) {
                bool stop = false;
                if (mPowerup == POWERUP_NONE) {
                    mVec3_c revPos(hitX, pos.y + 16.0f, pos.z);
                    float hitY;
                    if (dBc_c::checkGround(&revPos, &hitY, mLayer, 1, -1)) {
                        if (hitY <= mPos.y + 16.0f) {
                            pos.y = mPos.y + 16.0f;
                            stop = true;
                        }
                    }
                }
                if (!stop) {
                    player->mSpeedF = 0.0f;
                    pos.x = hitX - thing;
                }
            }
            p.set(pos.x, pos.y + 1.0f, pos.z);
            if (dBc_c::checkTenjou(&p, &hitX, mLayer, 1)) {
                float offsX = std::fabs(getHeadBgPointData()->mDist / 4096.0f);
                // float tmp = hitX - offsX;
                if (pos.y > hitX - offsX) {
                    pos.y = hitX - offsX;
                    player->mSpeed.y = 0.0f;
                }
            }
            player->mPos = pos;
            player->mLastPos = player->mPos;
            fn_80055d00();
        } else {
            actor->mCarryingFlags |= CARRY_RELEASE | CARRY_THROW;
            actor->mThrowDirection = mDirection;
        }
    }
    cancelCarry(actor);
}

#pragma push
#pragma pool_data off
void dAcPy_c::executeThrowCommon() {
    if (isNowBgCross(BGC_IS_FOOT)) {
        offStatus(STATUS_8F);
    } else {
        onStatus(STATUS_8F);
    }
    switch ((ThrowSubstate_e) mSubstate) {
        case THROW_ACTION_0:
            if (mPyMdlMng.mpMdl->m_154 == 37) {
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(12.0f)) {
                    mSubstate = THROW_ACTION_2;
                    setThrowActor();
                }
            } else {
                if (isNowBgCross(BGC_IS_FOOT) && mKey.triggerJump()) {
                    fn_80145fd0(1);
                    setJumpSpeed();
                    setJumpCommonBase();
                }
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(5.0f)) {
                    mSubstate = THROW_ACTION_1;
                    setThrowActor();
                    onStatus(STATUS_92);
                }
            }
            break;
        case THROW_ACTION_1:
            if (isNowBgCross(BGC_IS_FOOT) && checkJumpTrigger()) {
                break;
            }
            if (mPyMdlMng.mpMdl->mAnm.getFrame() >= 20.0f) {
                if (isNowBgCross(BGC_IS_FOOT)) {
                    if (mSpeedF) {
                        changeState(StateID_Walk, (void *) 1);
                        break;
                    }
                } else {
                    changeState(StateID_Fall, 0);
                }
            }
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, (void *) 1);
            }
            break;
        case THROW_ACTION_2:
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Swim, 0);
            }
            break;
    }
}
#pragma pop

void dAcPy_c::initializeState_Throw() {
    initializeThrowCommon();
}

void dAcPy_c::finalizeState_Throw() {
    finalizeThrowCommon();
}

void dAcPy_c::executeState_Throw() {
    if (isNowBgCross(BGC_14)) {
        setWaterMoveSpeed();
    } else {
        gravitySet();
        maxFallSpeedSet();
        simpleMoveSpeedSet();
        powerSet();
    }
    executeThrowCommon();
}

void dAcPy_c::initializeState_PropelThrow() {
    initializeThrowCommon();
}

void dAcPy_c::finalizeState_PropelThrow() {
    finalizeThrowCommon();
}

void dAcPy_c::executeState_PropelThrow() {
    if (!isNowBgCross(BGC_IS_FOOT) && mKey.buttonJump()) {
        mAccelY = getSomeData(7);
        mMaxFallSpeed = getSomeData(5);
    } else {
        mAccelY = getGravityData()[0];
        mMaxFallSpeed = -4.0f;
    }
    mAccelF = getSpeedData()[8];
    mMaxSpeedF = 0.0f;
    executeThrowCommon();
}

bool dAcPy_c::isSpinLiftUpEnable() {
    if (
        isStatus(STATUS_7A) ||
        isDemo() ||
        isCarry() ||
        mRideActorID != BASE_ID_NULL ||
        isStatus(STATUS_93) ||
        isStatus(STATUS_04) ||
        isStatus(STATUS_06) ||
        isStatus(STATUS_08)
    ) {
        return false;
    }
    return true;
}

void dAcPy_c::setSpinLiftUpActor(dActor_c *carryingActor) {
    if (mRideActorID != BASE_ID_NULL) {
        return;
    }
    mRideActorID = carryingActor->mUniqueID;
    if (!isState(StateID_CarryPlayer)) {
        changeState(StateID_CarryPlayer, 0);
    }

    dQuake_c::getInstance()->shockMotor(*getPlrNo(), dQuake_c::TYPE_7, 0, false);
}

void dAcPy_c::initializeState_LiftUp() {
    onStatus(STATUS_8A);
    onStatus(STATUS_97);
    onStatus(STATUS_46);
    m_1304 = 0.0f;
    dAcPy_c *carryPlayer = getCarryPlayer();
    setVirusStar(carryPlayer);
    if (carryPlayer != nullptr) {
        vf434(20, 0);
    } else {
        vf434(18, 0);
    }
    fn_80057e70(SE_PLY_OTHER_ON, false);
    mPyMdlMng.setAnm(108, 0.0f, 0.0f);
    mSpeedF = 0.0f;
    mSpeed.y = 0.3f;
    m_1048 = 10000;
    m_130c = 0;
}

void dAcPy_c::finalizeState_LiftUp() {
    offStatus(STATUS_97);
    offStatus(STATUS_46);
    m_1304 = 1.0f;
    mAngle.y = getMukiAngle(mDirection);
}

void dAcPy_c::executeState_LiftUp() {
    m_130c++;
    float f = m_130c / 30.0f;
    if (f > 1.0f) {
        f = 1.0f;
    }
    m_1304 = f;
    sLib::chase(&m_1048, 0, 250);
    if (m_1048 != 0) {
        mAngle.y += m_1048;
    } else {
        turnAngle();
    }
    setCcAtSpin();
    gravitySet();
    maxFallSpeedSet();
    if (m_130c <= 18) {
        mMaxSpeedF = 0.0f;
        powerSet();
    } else {
        simpleMoveSpeedSet();
        powerSet();
        if (checkCarryThrow()) {
            int dir;
            if (mKey.buttonWalk(&dir)) {
                mDirection = dir;
                mAngle.y = getMukiAngle(mDirection);
            }
            return;
        } else if (checkJumpTrigger()) {
            // [TODO...]
            onStatus(STATUS_48);
            return;
        }
    }
    if (mPyMdlMng.isAnmStop()) {
        changeState(StateID_Walk, (void *) 1);
    }
}

void dAcPy_c::calcUzuSwimSpeed(float f1, float f2, float *fOut) {
    if (!isStatus(STATUS_3B)) {
        return;
    }

    *fOut *= data_802f5a48[15];
    if (*fOut * f1 >= 0.0f && std::fabs(f2) > data_802f5a48[17]) {
        *fOut = 0.0f;
        return;
    }

    if (*fOut * f1 < 0.0f) {
        if (f2 * f1 > 0.0f) {
            if (std::fabs(f2) > 0.5f) {
                return;
            }
        } else {
            *fOut *= data_802f5a48[16];
        }
        float lim = data_802f5a48[11] - 0.01f;
        if (std::fabs(*fOut) > lim) {
            if (*fOut < 0.0f) {
                *fOut = -lim;
            } else {
                *fOut = lim;
            }
        }
    }
}

void dAcPy_c::setUzuSpeedY(float f) {
    if (std::fabs(mSpeed.y) < 2.0f) {
        if (f > 0.0f) {
            if (isNowBgCross(BGC_IS_FOOT)) {
                setUzuSwimAction();
            }
            if (isNowBgCross(BGC_IS_HEAD)) {
                return;
            }
        }
        calcUzuSwimSpeed(m_b94, mSpeed.y, &f);
        if (f > 0.0f) {
            m_b88 = 20;
        }
        if (f * m_b94 < 0.0f) {
            onStatus(STATUS_42);
        }
        mSpeed.y += f;
    }
}

void dAcPy_c::setUzuSpeedF(float f) {
    if (std::fabs(mSpeedF) < 4.0f) {
        calcUzuSwimSpeed(m_b90, mSpeedF, &f);
        if (f * m_b90 < 0.0f) {
            onStatus(STATUS_41);
        }
        mSpeedF += f;
    }
}

bool dAcPy_c::setUzuSwimAction() {
    if (isNowBgCross(BGC_14) && isState(StateID_Swim) && mSubstate == 1) {
        setSwimAction_Swim(BLEND_1);
        return true;
    }
    return false;
}

bool dAcPy_c::setSwimAction() {
    if (isStatus(STATUS_45) || isStatus(STATUS_AA) || isStatus(STATUS_3A)) {
        return false;
    }
    if (isNowBgCross(BGC_14)) {
        if (isOldBgCross(BGC_14)) {
            changeState(StateID_Swim, 0);
        } else if (isStatus(STATUS_A6)) {
            changeState(StateID_Swim, (void *) 2);
        } else {
            changeState(StateID_Swim, (void *) 1);
        }
        return true;
    }
    return false;
}

void dAcPy_c::setWaterInEffect() {
    dScStage_c *stage = dScStage_c::m_instance;
    bool bigSplash = false;

    if (isNowBgCross(BGC_WATER_BUBBLE)) {
        fn_80057e70(SE_PLY_SPLASH_GEL, false);
        mAng3_c efAng(0, 0, -mAirWaterHitAngle);
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_wfloatsplash", 0, &mAirWaterHitPos, &efAng, nullptr);
        return;
    }
    if (
        stage->mCurrWorld == WORLD_4 &&
        stage->mCurrLevel == STAGE_CASTLE &&
        stage->mCurrFile == 1
    ) {
        bigSplash = true;
    }
    mVec3_c efPos(mPos.x, mWaterHeight, mPos.z);
    if (bigSplash || m_dac == 3) {
        float f = 1.0f;
        u32 i;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            fn_80057e70(SE_PLY_SPLASH_MAME, false);
            i = 3;
        } else {
            fn_80057e70(SE_PLY_SPLASH, false);
            i = 2;
            if (mPowerup == POWERUP_NONE) {
                f = 0.8f;
            }
        }
        dPyEffectMng_c::mspInstance->fn_800d2de0(f, i, efPos, mLayer);
        dBg_c::m_bg_p->setWaterInWave(efPos.x, efPos.y, 1);
    } else {
        float f = 1.0f;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            f = 0.6f;
        } else if (mPowerup == POWERUP_NONE) {
            f = 0.8f;
        }
        dPyEffectMng_c::mspInstance->fn_800d2de0(f, 4, efPos, mLayer);
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            fn_80057e70(SE_PLY_SPLASH_SHALLOW_MAME, false);
        } else {
            fn_80057e70(SE_PLY_SPLASH_SHALLOW, false);
        }
        dBg_c::m_bg_p->setWaterInWave(efPos.x, efPos.y, 0);
    }
}

void dAcPy_c::setWaterOutEffect() {
    if (isOldBgCross(BGC_WATER_BUBBLE)) {
        fn_80057e70(SE_PLY_SPLASH_GEL_OUT, false);
        mAng3_c efAng(0, 0, -mAirWaterHitAngle);
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_wfloatsplash", 0, &mAirWaterHitPos, &efAng, nullptr);
        return;
    }
    mVec3_c efPos(mPos.x, mPrevWaterHeight, mPos.z);

    if (m_dac == 3) {
        float f = 1.0f;
        u32 i;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            i = 1;
        } else {
            i = 0;
            if (mPowerup == POWERUP_NONE) {
                f = 0.8f;
            }
        }
        dPyEffectMng_c::mspInstance->fn_800d2de0(f, i, efPos, mLayer);
        dBg_c::m_bg_p->setWaterInWave(efPos.x, efPos.y, 3);
        fn_80057e70(SE_PLY_SPLASH_OUT, false);
    } else {
        float f = 1.0f;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            f = 0.6f;
        } else if (mPowerup == POWERUP_NONE) {
            f = 0.8f;
        }
        dPyEffectMng_c::mspInstance->fn_800d2de0(f, 4, efPos, mLayer);
        fn_80057e70(SE_PLY_SPLASH_SHALLOW_OUT, false);
        dBg_c::m_bg_p->setWaterInWave(efPos.x, efPos.y, 2);
    }
}

void dAcPy_c::setPaddleSwimEffect() {
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 11);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf9, "Wm_mr_swimpaddle", 0, &efPos, nullptr, nullptr);
    mPyMdlMng.mpMdl->getJointPos(&efPos, 14);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf10, "Wm_mr_swimpaddle", 0, &efPos, nullptr, nullptr);
}

void dAcPy_c::resetPaddleSwimEffect() {
    mLevelEf9.fade();
    mLevelEf10.fade();
}

void dAcPy_c::setWaterSurfaceSwimEffect() {
    if (getWaterCheckPosY() < mWaterHeight - 4.0f) {
        return;
    }
    mVec3_c efPos(mPos.x, mWaterHeight, mPos.z);
    mAng3_c efAng(0, 0, 0);
    if (mDirection == DIR_LR_L) {
        efAng.y = -0x8000;
    }
    static const float sc_waterFaceEffectScale[] = { 0.65f, 0.8f, 1.0f };
    float s = sc_waterFaceEffectScale[getTallType(-1)];
    mVec3_c scale(s, s, s);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf11, "Wm_mr_waterswim", 0, &efPos, nullptr, &scale);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf12, "Wm_en_cmnwater02", 0, &efPos, &efAng, &scale);
}

void dAcPy_c::setFlutterKickEffect() {
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 4);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf9, "Wm_mr_flutterkick", 0, &efPos, nullptr, nullptr);
    mPyMdlMng.mpMdl->getJointPos(&efPos, 7);
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf10, "Wm_mr_flutterkick", 0, &efPos, nullptr, nullptr);
}

void dAcPy_c::initializeState_Swim() {
    onStatus(STATUS_3A);
    onStatus(STATUS_AA);
    onStatus(STATUS_A8);
    onStatus(STATUS_92);
    offStatus(STATUS_C1);
    SwimParam_e param = (SwimParam_e) (int) mStateChangeParam;
    m_b84 = 0;
    m_b88 = 0;
    m_b80 = 0.0f;
    if (param == SWIM_PARAM_1 || param == SWIM_PARAM_2 || param == SWIM_PARAM_3) {
        m_b80 = 1;
        m_15b6 = 30;
        if (mSpeed.y < -1.5f) {
            mSpeed.y = -1.5f;
        }
        if (m_b98 != 0 && mSpeed.y < -0.5f) {
            mSpeed.y = -0.5f;
        }
    }
    if (param == SWIM_PARAM_2) {
        setInitSwimAction_FireBall();
    } else {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr) {
            mVec3_c pos(mPos.x, mPos.y + 4.0f, mPos.z);
            float height;
            if (dBc_c::checkTenjou(&pos, &height, mLayer, m_ca1)) {
                if (mPos.y + getSomeYOffset() + getBgPointData_Powerup(mPowerup, 1)[1].mDist / 4096.0f > height) {
                    releaseCarryActor();
                }
            }
        }
        AnmBlend_e blend = BLEND_1;
        if (param == SWIM_PARAM_3) {
            blend = BLEND_0;
        }
        if (mPowerup == POWERUP_PENGUIN_SUIT) {
            if (!isCarry()) {
                setSwimAction_Surface(blend);
            } else {
                setSwimAction_Swim(blend);
            }
        } else if (isNowBgCross(BGC_IS_FOOT)) {
            setSwimAction_Swim(blend);
        } else {
            setSwimAction_Dive(blend);
        }
    }
    if (isNowBgCross(BGC_14) && isOldBgCross(BGC_14) == 0) {
        setWaterInEffect();
    }
    if (isItemKinopio()) {
        setControlDemoKinopioSwim();
    }
}

bool dAcPy_c::setWaterSurfaceJump() {
    if (mKey.triggerJump()) {
        if (mSubstate == SWIM_ACTION_2) {
            if (mKey.buttonUp()) {
                m_b88 = 20;
            }
        } else {
            m_b88 = 20;
        }
    }
    if (!isNowBgCross(BGC_14)) {
        if (!isNowBgCross(BGC_15) && (!isCarry() || mPowerup != POWERUP_MINI_MUSHROOM)) {
            if (mSubstate == SWIM_ACTION_3 && m_b89 == 0) {
                createFireBall(0);
            }
            changeState(StateID_Fall, 0);
            return true;
        }
        if (m_b88 != 0) {
            if (mSubstate == SWIM_ACTION_3 && m_b89 == 0) {
                createFireBall(0);
            }
            float h = 3.5f;
            if (isMameAction()) {
                h = 2.625f;
            }
            jmpInf_c jmpInf(h, 1, 1);
            if (mSubstate == SWIM_ACTION_2) {
                jmpInf.m_0c = 0;
            }
            changeState(StateID_Jump, &jmpInf);
            mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
            onStatus(STATUS_A8);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setSwimAction_Swim(AnmBlend_e blend) {
    m_b89 = 0;
    mSubstate = SWIM_ACTION_0;
    m_b84 = 4;
    m_b88 = 0;
    if (blend == BLEND_1) {
        mPyMdlMng.setAnm(36);
    } else {
        mPyMdlMng.setAnm(36, 0.0f, 0.0f);
    }
    mAngle.x = 0;
}

void dAcPy_c::SwimAction_Swim() {
    if (mPowerup == POWERUP_PENGUIN_SUIT && !isCarry()) {
        setSwimAction_Penguin(BLEND_1);
        return;
    }
    setWaterMoveSpeed();
    turnAngle();
    if (checkEnableThrow()) {
        vf434(19, 0);
        setThrowActor();
        return;
    }
    if (setWaterSurfaceJump()) {
        return;
    }
    if (isNowBgCross(BGC_IS_FOOT) && mPowerup != POWERUP_PENGUIN_SUIT) {
        setSwimAction_Walk(BLEND_1);
        return;
    }
    setWaterSurfaceSwimEffect();
    if (isNowBgCross(BGC_14)) {
        if (mKey.triggerJump()) {
            fn_80057e70(SE_PLY_SWIM, 0);
            mSpeed.y += 1.0f;
            if (mSpeed.y > 1.8f) {
                mSpeed.y = 1.8f;
            }
        }
        if (isNowBgCross(BGC_53)) {
            mSpeed.y = -1.875f;
        }
    }
    switch (m_b89) {
        case 0:
            if (mKey.triggerJump()) {
                m_b89 = 1;
                mPyMdlMng.setAnm(40);
                onStatus(STATUS_40);
            } else if (mKey.buttonJump()) {
                m_b89 = 3;
                mPyMdlMng.setAnm(35);
            }
            break;
        case 1:
            setPaddleSwimEffect();
            if (mKey.triggerJump() && mPyMdlMng.mpMdl->mAnm.getFrame() > 9.0f) {
                mPyMdlMng.mpMdl->setFrame(0.0f);
                resetPaddleSwimEffect();
                onStatus(STATUS_40);
            }
            if (mPyMdlMng.isAnmStop()) {
                if (mKey.buttonJump()) {
                    m_b89 = 3;
                    mPyMdlMng.setAnm(35);
                } else {
                    m_b89 = 2;
                    mPyMdlMng.setAnm(41);
                }
            }
            break;
        case 3:
            mPyMdlMng.mpMdl->setRate(1.5f);
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(9.0f)) {
                fn_80057e70(SE_PLY_SWIM_KICK, 0);
            }
            setFlutterKickEffect();
            if (!mKey.buttonJump()) {
                m_b89 = 2;
                mPyMdlMng.setAnm(41);
            }
            break;
        case 2:
            if (mPyMdlMng.isAnmStop()) {
                m_b89 = 0;
                mPyMdlMng.setAnm(36);
            }
            break;
        case 4:
            if (!isCarry()) {
                m_b89 = 0;
                mPyMdlMng.setAnm(36);
            }
            break;
    }
}

void dAcPy_c::setSeaLandSmokeEffect() {
    static const char *sc_hipAttackEffectID[] = {
        "Wm_mr_sealandsmk_ss",
        "Wm_mr_sealandsmk_s",
        "Wm_mr_sealandsmk",
    };
    dEf::createPlayerEffect_change(mPlayerNo, sc_hipAttackEffectID[getTallType(-1)], 0, &mPos, nullptr, nullptr);
}

void dAcPy_c::setSwimActionWalkAnm() {
    if (!mSpeedF) {
        mPyMdlMng.setAnm(39);
        return;
    }
    float f = 1.0f;
    if (mMaxSpeedF && mSpeedF * mMaxSpeedF < 0.0f) {
        f = std::fabs(mSpeedF) - sc_WaterWalkSpeed + 1.0f;
        if (f < 1.0f) {
            f = 1.0f;
        } else if (f > 2.0f) {
            f = 2.0f;
        }
    }
    mPyMdlMng.setAnm(38, f, 10.0f, 0.0f);
}

void dAcPy_c::setSwimAction_Walk(AnmBlend_e blend) {
    if (!isOldBgCross(BGC_IS_FOOT)) {
        setSeaLandSmokeEffect();
    }
    m_b89 = 0;
    mSubstate = SWIM_ACTION_1;
    if (blend == BLEND_1) {
        setSwimActionWalkAnm();
    } else {
        mPyMdlMng.setAnm(39);
    }
}

void dAcPy_c::setWaterGroundJump() {
    mSpeed.y = sc_WaterJumpSpeed;
    setSwimAction_Swim(BLEND_1);
    fn_80057e70(SE_PLY_SWIM, 0);
    m_b88 = 20;
}

void dAcPy_c::SwimAction_Walk() {
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        setSwimAction_Penguin(BLEND_1);
        return;
    }
    setWaterMoveSpeed();
    turnAngle();
    if (mKey.triggerJump()) {
        if (!setWaterSurfaceJump()) {
            setWaterGroundJump();
        }
    } else {
        if (!isNowBgCross(BGC_IS_FOOT)) {
            setSwimAction_Swim(BLEND_1);
        } else {
            if (!checkCrouch()) {
                setSwimActionWalkAnm();
            }
        }
    }
}

s16 dAcPy_c::getPenguinSwinAngleX() {
    float f = std::fabs(m_b90);
    if (f < 0.1f) {
        f = 0.1f;
    }
    float cos = m_b94;
    if (mKey.buttonUp()) {
        if (cos < 0.0f) {
            cos = 0.0f;
        }
    }
    if (mKey.buttonDown()) {
        if (cos < 0.0f) {
            cos = 0.0f;
        }
    }
    // [cos gets discarded here?]
    return -(cM::atan2s(f, m_b94) - 0x4000);
}

void dAcPy_c::setSwimAction_Penguin(AnmBlend_e blend) {
    mSubstate = SWIM_ACTION_2;
    m_b88 = 0;
    if (m_b80 != 0 || getOldState() == StateID_HipAttack) {
        m_b80 = 0;
        if (blend == BLEND_1) {
            mPyMdlMng.setAnm(34);
        } else {
            mPyMdlMng.setAnm(34, 0.0f, 0.0f);
        }
        mSubstateTimer = 15;
        m_b89 = 0;
        return;
    }
    if (mSpeedF > 0.1f || mSpeed.y > 0.1f) {
        m_b8c = 1.0f;
    } else {
        m_b8c = 0.0f;
    }
    m_b89 = 1;
    if (blend == BLEND_1) {
        mPyMdlMng.setAnm(34);
    } else {
        mPyMdlMng.setAnm(34, 0.0f, 0.0f);
    }
}

void dAcPy_c::setPenWaterMoveSpeed(int i) {
    if (m_b98 != 0 && mKey.triggerDown()) {
        m_b98 = 0;
    }
    bool noUpDown = false;
    if (mKey.buttonUp() || m_b98 == 0 && mKey.buttonDown()) {
        noUpDown = true;
    }
    bool b = 0;
    if (mKey.buttonWalk(nullptr)) {
        b = 1;
    }
    s16 ang = 0;
    if (noUpDown) {
        ang = 0x3e00;
        if (b) {
            ang = 0x2000;
        }
        if (mKey.buttonDown()) {
            ang = -ang;
        }
    }
    mAngle.y.chase(getMukiAngle(mDirection), 0x400);
    mAngle.x.chase(getPenguinSwinAngleX(), 0x300);
    float f3 = getData(9);
    float f2 = 0.0f;
    if (i != 0) {
        if (noUpDown) {
            f2 = getData(6);
            if (isStatus(STATUS_42)) {
                f3 = getData(18);
            } else {
                f3 = getData(12);
            }
        }
    } else {
        if (noUpDown) {
            f2 = getData(5);
            f3 = getData(11);
        } else if (b) {
            f3 = getData(14);
        }
        float d = getData(5);
        if (std::fabs(mSpeed.y) >= d) {
            f3 = getData(13);
        }
    }
    float target = mAng(ang).sin() * f2;
    m_b94 = target;
    float offs = mPos.y + getSomeYOffset() / 2.0f;
    if (mWaterHeight <= offs) {
        target = -1.5f;
        if (mSpeed.y > 0.0f) {
            f3 = std::fabs(-24.0f / 256.0f);
        } else {
            f3 = std::fabs(-7.0f / 256.0f);
        }
    } else {
        if (mSpeed.y < -getData(6)) {
            f3 = std::fabs(-24.0f / 256.0f);
        }
    }
    sLib::addCalc(&mSpeed.y, target, 0.5f, f3, 0.01f);
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAccelY = 0.0f;
    if (i != 0) {
        f2 = getData(8);
        if (isStatus(STATUS_41)) {
            f3 = getData(18);
        } else {
            f3 = getData(12);
        }
    } else {
        if (b) {
            f2 = getData(7);
            f3 = getData(11);
        } else {
            f2 = 0.3f;
            if (std::fabs(mSpeedF) < 0.3f) {
                f3 = 0.005f;
            } else if (noUpDown) {
                f3 = getData(14);
            } else {
                f3 = getData(10);
            }
        }
        float d = getData(7);
        if (std::fabs(mSpeedF) >= d) {
            f3 = getData(13);
        }
    }
    f3 = sc_DirSpeed[mDirection] * f3;
    m_b90 = mAng(ang).cos() * f2 * sc_DirSpeed[mDirection];
    float f4 = 0.0f;
    if (dWaterEntryMng_c::m_instance->m_8c0) {
        f4 = 8.25f;
    }
    float f5 = m_b90 + f4 * 0.3f;
    float f10 = f3 + f4 * 0.005f;
    if (m_b90 * f4 < 0.0f) {
        if (i != 0) {
            f5 = m_b90 + f4 * 0.1f;
            f10 = f3 + f4 * 0.0001f;
        } else if (b) {
            f5 = m_b90 + f4 * 0.13f;
            if (std::fabs(mSpeedF) < 1.0f) {
                f10 = f10;
            } else {
                f10 = f3 + f4 * 0.003f;
            }
        }
    }
    mMaxSpeedF = f5;
    if (f3 < 0.0f) {
        f3 = f3;
    }
    mAccelF = std::fabs(f10);
    if (b || noUpDown || i != 0) {
        m_b8c = 1.0f;
    } else {
        m_b8c = 0.3f;
    }
}

bool dAcPy_c::setPenguinPaddleSwim() {
    if (mKey.triggerJump()) {
        m_b89 = 2;
        mPyMdlMng.setAnm(160);
        fn_80057e70(SE_PLY_PNGN_SWIM, false);
        onStatus(STATUS_40);
        return true;
    }
    return false;
}

void dAcPy_c::SwimAction_Penguin() {
    if (mPowerup != POWERUP_PENGUIN_SUIT || isCarry()) {
        setSwimAction_Swim(BLEND_1);
        return;
    }
    if (setWaterSurfaceJump()) {
        return;
    }
    setWaterSurfaceSwimEffect();
    int moveMode = 0;
    switch (m_b89) {
        case 0:
            if (mSubstateTimer == 0) {
                m_b8c = 1.0f;
                m_b89 = 1;
                mPyMdlMng.setAnm(34);
            }
            break;
        case 1: {
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(9.0f)) {
                fn_80057e70(SE_PLY_SWIM_KICK_PENGUIN, false);
            }
            float rate = mPyMdlMng.mpMdl->mAnm.getRate();
            if (m_b8c == 1.0f) {
                setFlutterKickEffect();
                sLib::chase(&rate, m_b8c, 0.1f);
            } else {
                sLib::chase(&rate, m_b8c, 0.03f);
            }
            mPyMdlMng.mpMdl->setRate(rate);
            if (setPenguinPaddleSwim()) {
                return;
            }
            break;
        }
        case 2:
            setPaddleSwimEffect();
            if (mPyMdlMng.mpMdl->mAnm.getFrame() < 9.0f) {
                moveMode = 1;
            }
            if (mPyMdlMng.isAnmStop()) {
                m_b89 = 3;
                mPyMdlMng.setAnm(161);
            }
            break;
        case 3:
            if (setPenguinPaddleSwim()) {
                return;
            }
            if (mPyMdlMng.isAnmStop()) {
                m_b8c = 1.0f;
                m_b89 = 1;
                mPyMdlMng.setAnm(34);
            }
            break;
    }
    onStatus(STATUS_3B);
    setPenWaterMoveSpeed(moveMode);
}

void dAcPy_c::setInitSwimAction_FireBall() {
    mSubstate = SWIM_ACTION_3;
    m_b89 = 1;
    createFireBall(0);
    mPyMdlMng.setAnm(31);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
}

void dAcPy_c::setSwimAction_FireBall() {
    mSubstate = SWIM_ACTION_3;
    m_b89 = 0;
    if (mAngle.x > 0x1800) {
        mPyMdlMng.setAnm(32);
    } else {
        mPyMdlMng.setAnm(31);
    }
    mPyMdlMng.mpMdl->setFrame(0.0f);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
}

void dAcPy_c::SwimAction_FireBall() {
    if (setWaterSurfaceJump()) {
        return;
    }
    if (mPowerup != POWERUP_PENGUIN_SUIT) {
        setWaterMoveSpeed();
    } else {
        onStatus(STATUS_3B);
        setPenWaterMoveSpeed(0);
    }
    switch (m_b89) {
        case 0:
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(4.0f)) {
                createFireBall(0);
                m_b89 = 1;
            }
            break;
        case 1:
            if (checkSetFireBall()) {
                setSwimAction_FireBall();
            } else if (mPyMdlMng.isAnmStop()) {
                if (mPowerup == POWERUP_PENGUIN_SUIT) {
                    setSwimAction_Penguin(BLEND_1);
                } else if (isNowBgCross(BGC_IS_FOOT)) {
                    setSwimAction_Walk(BLEND_1);
                } else {
                    setSwimAction_Swim(BLEND_1);
                }
            }
            break;
    }
}

void dAcPy_c::setWaterMoveSpeed() {
    float targetY = -1.5f;
    float accelY = getGravityData()[0];
    if (isCarry()) {
        float posY = mPos.y + getSomeYOffset() / 2.0f;
        if (std::fabs(mWaterHeight - posY) < 0.2f && std::fabs(mSpeed.y) < 0.1f) {
            targetY = 0.0f;
            mSpeed.y = 0.0f;
            accelY = targetY;
        } else if (mWaterHeight > posY) {
            targetY = 1.0f;
            accelY = -0.04296875f;
        } else {
            targetY = -1.5f;
            accelY = -0.04296875f;
            if (mSpeed.y > 0.0f) {
                sLib::chase(&mSpeed.y, 0.0f, 0.1f);
            }
        }
    } else {
        if (isNowBgCross(BGC_14)) {
            if (isStatus(STATUS_39)) {
                targetY = 1.0f;
            } else if (!mKey.buttonJump()) {
                targetY = -1.5f;
            } else {
                targetY = -0.5f;
            }
            float speedY = mSpeed.y;
            if (speedY < 0.0f) {
                if (speedY < targetY) {
                    accelY = -0.09375f;
                } else {
                    accelY = -0.02734375f;
                }
            } else {
                accelY = -0.04296875f;
            }
        }
    }
    sLib::chase(&mSpeed.y, targetY, std::fabs(accelY));
    mMaxFallSpeed = -4.0f;
    mAccelY = 0.0f;
    float maxSpeedF;
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        if (isNowBgCross(BGC_IS_FOOT)) {
            maxSpeedF = 0.84375f;
        } else {
            maxSpeedF = 1.6875f;
        }
    } else if (isNowBgCross(BGC_IS_FOOT)) {
        maxSpeedF = sc_WaterWalkSpeed;
    } else {
        maxSpeedF = sc_WaterSwimSpeed;
        if (m_b84 != 0 && std::fabs(mSpeedF) > std::fabs(sc_WaterSwimSpeed)) {
            sLib::chase(&mSpeedF, sc_WaterSwimSpeed, 0.5f);
        }
    }
    float targetF = 0.0f;
    if (mKey.buttonWalk(nullptr)) {
        targetF = maxSpeedF * sc_DirSpeed[mDirection];
    }
    targetF += dWaterEntryMng_c::m_instance->get_8c0() * 0.3f;
    mMaxSpeedF = targetF;
    float absSpeedF = std::fabs(mSpeedF);
    float speedF = mSpeedF;
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        if (targetF) {
            if (speedF * targetF < 0.0f) {
                mAccelF = 0.0625f;
            } else if (absSpeedF > std::fabs(targetF)) {
                mAccelF = 0.0546875f;
            } else {
                mAccelF = 0.0625f;
            }
        } else if (isState(StateID_Crouch)) {
            mAccelF = 0.0546875f;
        } else {
            mAccelF = 0.015625f;
        }
    } else {
        if (targetF) {
            if (speedF * targetF < 0.0f) {
                mAccelF = 0.017f;
            } else if (absSpeedF > std::fabs(targetF)) {
                if (isNowBgCross(BGC_IS_FOOT)) {
                    mAccelF = 0.05f;
                } else {
                    mAccelF = 0.017f;
                }
            } else {
                mAccelF = 0.025f;
            }
        } else if (absSpeedF > std::fabs(targetF)) {
            if (isNowBgCross(BGC_IS_FOOT)) {
                mAccelF = 0.05f;
            } else {
                mAccelF = 0.017f;
            }
        } else if (isState(StateID_Crouch)) {
            mAccelF = 0.027f;
        } else {
            mAccelF = 0.004f;
        }
    }
    if (mKey.buttonWalk(nullptr)) {
        mAccelF = std::fabs(sc_DirSpeed[mDirection] * mAccelF + dWaterEntryMng_c::m_instance->get_8c0() * 0.03f);
    }
}

void dAcPy_c::executeState_Swim() {
    if (isStatus(STATUS_60) && isNowBgCross(BGC_IS_FOOT)) {
        offNowBgCross(BGC_IS_FOOT);
        mSpeed.y = 1.0f;
    }
    if (m_b84 != 0) {
        m_b84--;
    }
    if (m_b88 != 0) {
        m_b88--;
    }
    if (isStatus(STATUS_39)) {
        if (mSpeed.y >= -0.25f) {
            offStatus(STATUS_39);
        }
    }
    if (!isNowBgCross(BGC_14) && isNowBgCross(BGC_IS_FOOT)) {
        changeState(StateID_Walk, 0);
    } else {
        if (mSubstate != SWIM_ACTION_3 && checkSetFireBall()) {
            setSwimAction_FireBall();
        }
        offStatus(STATUS_3B);
        typedef void (dAcPy_c::*SwimActionProc)();
        static SwimActionProc l_SwimActionProc[] = {
            &dAcPy_c::SwimAction_Swim,
            &dAcPy_c::SwimAction_Walk,
            &dAcPy_c::SwimAction_Penguin,
            &dAcPy_c::SwimAction_FireBall,
        };
        (this->*l_SwimActionProc[mSubstate])();
        calcPenguinSwimGroundRev();
    }
}

void dAcPy_c::calcPenguinSwimGroundRev() {
    if (!isStatus(STATUS_3B)) {
        m_1594 = 0.0f;
        m_1598 = 0.0f;
        return;
    }
    float f1 = 0.0f;
    float f2 = 0.0f;
    mMtx_c rotMtx;
    rotMtx.trans(mPos);
    rotMtx.YrotM(mAngle.y);
    rotMtx.concat(mMtx_c::createTrans(0.0f, 8.0f, 0.0f));
    rotMtx.XrotM(-mAngle.x);
    rotMtx.concat(mMtx_c::createTrans(0.0f, 8.0f, 0.0f));
    rotMtx.ZrotM(mAngle.z);
    rotMtx.concat(mMtx_c::createTrans(0.0f, 0.0f, 20.0f));
    mVec3_c resVec;
    rotMtx.multVecZero(resVec);
    sBcPointData *bgPointData = getWallBgPointData();
    if (bgPointData != nullptr) {
        float bgOffs[2];
        bgOffs[0] = bgPointData->mOffsetX / 4096.0f;
        bgOffs[1] = bgPointData->mOffsetY / 4096.0f;
        mVec3_c wallPos1(mPos.x, mPos.y + bgOffs[0], mPos.z);
        mVec3_c wallPos2(resVec.x, mPos.y + bgOffs[0], resVec.z);
        for (int i = 0; i < 2; i++) {
            wallPos1.set(mPos.x, mPos.y + bgOffs[i], mPos.z);
            wallPos2.set(resVec.x, mPos.y + bgOffs[i], resVec.z);
            float wallX;
            if (dBc_c::checkWall(&wallPos1, &wallPos2, &wallX, mLayer, 1, nullptr)) {
                f1 = wallX - resVec.x;
                if (mPos.x < resVec.x) {
                    resVec.x = wallX - 1.0f;
                } else {
                    resVec.x = wallX + 1.0f;
                }
            }
        }
    }
    mVec3_c groundPos(resVec.x, mPos.y + 4.0f, resVec.z);
    float groundHeight;
    if (dBc_c::checkGround(&groundPos, &groundHeight, mLayer, 1, -1) && groundHeight > resVec.y) {
        f2 = groundHeight - resVec.y;
    }
    sLib::chase(&m_1594, f1, 1.0f);
    sLib::chase(&m_1598, f2, 1.0f);
}

void dAcPy_c::finalizeState_Swim() {
    m_1594 = 0.0f;
    m_1598 = 0.0f;
    m_b88 = 0;
    mAngle.z = 0;
    mAngle.x = 0;
    offStatus(STATUS_3A);
    if (isStatus(STATUS_3B)) {
        float groundHeight;
        mVec3_c checkPos(mPos.x, mPos.y - 5.0f, mPos.z);
        if (dBc_c::checkGround(&mPos, &groundHeight, mLayer, 1, -1)) {
            if (mPos.y <= groundHeight) {
                mPos.y = groundHeight;
                mLastPos.y = mPos.y;
            }
        }
        offStatus(STATUS_3B);
    }
    offStatus(STATUS_97);
    offStatus(STATUS_AA);
    offStatus(STATUS_A8);
    offStatus(STATUS_92);
    if (!isNowBgCross(BGC_14)) {
        setWaterOutEffect();
    }
}

void dAcPy_c::setIvyHangEffect() {
    mMtx_c jntMtx;
    mPyMdlMng.mpMdl->getJointMtx(&jntMtx, 11);
    mVec3_c v1;
    jntMtx.multVecZero(v1);
    mPyMdlMng.mpMdl->getJointMtx(&jntMtx, 14);
    mVec3_c v2;
    jntMtx.multVecZero(v2);
    mVec3_c efPos = (v1 + v2) / 2.0f;
    efPos.z = mPos.z;
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf6, "Wm_mr_ivy", 0, &efPos, nullptr, nullptr);
}

bool dAcPy_c::setVineAction() {
    if (isStatus(STATUS_9B)) {
        if (isCarry()) {
            return false;
        }
        if (mSpeed.y <= 0.0f && mKey.buttonUp() && isNowBgCross(BGC_46)) {
            if (mBc.mFenceType == 1) {
                changeState(StateID_Vine, (void *) 2);
            } else {
                changeState(StateID_Vine, (void *) 0);
            }
            return true;
        }
    }
    return false;
}

float dAcPy_c::getHangBcOffsetY() {
    static const float scHangOffsetY[] = {
        16.0f, 27.0f, 27.0f, 12.0f, 27.0f, 27.0f, 27.0f
    };
    static const float scKinopioHangOffsetY[] = {
        13.0f, 21.0f, 21.0f, 9.0f, 21.0f, 21.0f, 21.0f
    };
    // [probably some inline?]
    if (!(mPlayerType != 0 && mPlayerType != 1)) {
        return scHangOffsetY[mPowerup];
    } else {
        return scKinopioHangOffsetY[mPowerup];
    }
}

bool dAcPy_c::setKaniHangToVineAction() {
    if (mKey.buttonDown()) {
        float y = mPos.y - 1.0f;
        float x2 = mPos.x + (m_14.mWidth / 4096.0f);
        float x1 = mPos.x - (m_14.mWidth / 4096.0f + 1.0f);
        if (
            dBc_c::getUnitType(x1, y, mLayer) & 0x400 ||
            dBc_c::getUnitType(x2, y, mLayer) & 0x400
        ) {
            mPos.y -= getHangBcOffsetY();
            m_60 = 10;
            changeState(StateID_Vine, (void *) 3);
            return true;
        }
    }
    return false;
}

bool dAcPy_c::setVineToKaniHangAction() {
    if (mKey.buttonUp()) {
        sPcRect r = m_14;
        r.mTop = getHangBcOffsetY() * 4096.0f;
        if (!(mBc.checkPole(&r) & 3)) {
            float y = mPos.y + getHangBcOffsetY();
            float x = mPos.x;
            short groundY = (((short) y) & 0xfff0) - 16;
            u32 unitKind = dBc_c::getUnitType(x, groundY + 1.0f, mLayer);
            if ((unitKind >> 16 & 0xff) == 8) {
                mPos.y += getHangBcOffsetY();
                m_60 = 10;
                changeState(StateID_Kani, (void *) KANI_CHANGE_HANG_UP_VINE);
                return true;
            }
        }
    }
    return false;
}

void dAcPy_c::initializeState_Vine() {
    setScrollMode(2);
    onStatus(STATUS_33);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    m_7c4 = 0;
    if (m_ca1 == 1) {
        m_7c0 = -0x8000;
    } else {
        m_7c0 = 0;
    }
    int changeParam = (int) mStateChangeParam;
    mAngle.y = m_7c0;
    mBc.mPlayerFlags |= 1;
    switch (changeParam) {
        case 0:
        case 2:
            vf434(28, 0);
            switch (mBc.mFenceType) {
                case 0:
                    fn_80057e70(SE_PLY_CATCH_IVY, 0);
                    if (!isNowBgCross(BGC_IS_FOOT)) {
                        mPyMdlMng.setAnm(57);
                        mSubstate = VINE_ACTION_IVY;
                        mSubstateTimer2 = 0;
                    } else {
                        mPyMdlMng.setAnm(58);
                        mSubstate = VINE_ACTION_IVY;
                        mSubstateTimer2 = 1;
                    }
                    break;
                case 1:
                    mPyMdlMng.setAnm(52);
                    mSubstate = VINE_ACTION_NET;
                    break;
                case 2:
                    fn_80057e70(SE_PLY_LAND_FENCE, 0);
                    mPyMdlMng.setAnm(52);
                    mSubstate = VINE_ACTION_NET;
                    break;
            }
            break;
        case 1:
            mPyMdlMng.setAnm(58);
            mSubstate = VINE_ACTION_IVY;
            mSubstateTimer2 = 1;
            break;
        case 3:
            mPyMdlMng.setAnm(52, 0.0f, 0.0f);
            mSubstate = VINE_ACTION_NET;
            break;
    }
}

void dAcPy_c::calcVineSpeed() {
    float speed;
    switch (mBc.mFenceType) {
        case 0:
        case 2:
            if (mKey.buttonDush()) {
                speed = 1.5f;
            } else {
                speed = 1.0f;
            }
            break;
        case 1:
            if (mKey.buttonDush()) {
                speed = sGlobalData_c<dAcPy_c>::mData.mVineSpeedRelated[1];
            } else {
                speed = sGlobalData_c<dAcPy_c>::mData.mVineSpeedRelated[0];
            }
            break;
    }
    mSpeed.y = 0.0f;
    if (mKey.buttonUp()) {
        if (isNowBgCross(BGC_46) && isNowBgCross(BGC_48)) {
            mSpeed.y = speed;
        }
    } else if (mKey.buttonDown()) {
        mSpeed.y = -speed;
    }
    mSpeedF = 0.0f;
    float tmp = 0.0f;
    switch (mBc.mFenceType) {
        case 0:
            if (mKey.buttonDush()) {
                tmp = 0.5f;
            } else {
                tmp = 0.7f;
            }
            if (mKey.buttonUp() || mKey.buttonDown()) {
                tmp = 0.0f;
            }
            break;
        case 1:
            if (mKey.buttonDush()) {
                tmp = sGlobalData_c<dAcPy_c>::mData.mVineSpeedRelated[3];
            } else {
                tmp = sGlobalData_c<dAcPy_c>::mData.mVineSpeedRelated[2];
            }
            break;
        case 2:
            if (mKey.buttonDush()) {
                tmp = 1.25f;
            } else {
                tmp = 0.875f;
            }
            break;
    }
    if (mKey.buttonLeft()) {
        if (!isNowBgCross(BGC_WALL_TOUCH_L_2)) {
            mSpeedF = -tmp;
        }
    } else if (mKey.buttonRight()) {
        if (!isNowBgCross(BGC_WALL_TOUCH_R_2)) {
            mSpeedF = tmp;
        }
    }
}

bool dAcPy_c::checkVineEnd() {
    if (!isNowBgCross(BGC_45) || isNowBgCross(BGC_IS_FOOT) && !mKey.buttonUp()) {
        mAngle.y = getMukiAngle(mDirection);
        if (isNowBgCross(BGC_40)) {
            changeState(StateID_Kani, (void *) KANI_CHANGE_WALK_2);
            return true;
        } else {
            changeState(StateID_Walk, (void *) KANI_CHANGE_WALK);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setVineWalkSE() {
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(10.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(30.0f)) {
        switch (mBc.mFenceType) {
            case 0:
                fn_80057e70(SE_PLY_MOVE_IVY, false);
                break;
            case 2:
                fn_80057e70(SE_PLY_WALK_METAL, false);
                break;
            case 1:
                fn_80057e70(SE_PLY_FOOTNOTE_ROCK_CLIMB, false);
                break;
        }
    }
}

void dAcPy_c::VineActionIvy() {
    if (setRideOffPlayerJump(sc_JumpSpeed, 0.0f) || fn_800579c0(0, 1)) {
        return;
    }
    if (checkVineEnd()) {
        return;
    }
    calcVineSpeed();
    if (mSubstateTimer2 == 0) {
        if (mPyMdlMng.isAnmStop()) {
            mSubstateTimer2++;
        }
    } else {
        if (mSpeed.x || mSpeed.y) {
            float blend = 0.0f;
            if (mKey.buttonDush()) {
                blend = 1.875f;
            } else {
                blend = 1.5f;
            }
            if (mSpeed.y < 0.0f) {
                blend = -blend;
            }
            mPyMdlMng.setAnm(56, blend, 0.0f, 0.0f);
            if (mBc.mFenceType == 0 && m_7c4 == 0) {
                setIvyHangEffect();
                setVineWalkSE();
            }
        } else {
            mPyMdlMng.setAnm(58, 0.0f);
            m_7c4 = 3;
        }
    }
}

void dAcPy_c::VineActionNet() {
    if (checkVineEnd()) {
        return;
    }
    switch (mBc.mFenceType) {
        default:
            if (setRideOffPlayerJump(sc_JumpSpeed, 0.0f) || fn_800579c0(0, 0)) {
                switch (mPowerup) {
                    case POWERUP_NONE:
                        fn_80057e70(SE_PLY_JUMP_FENCE_S, false);
                        break;
                    case POWERUP_MINI_MUSHROOM:
                        fn_80057e70(SE_PLY_JUMP_FENCE_SS, false);
                        break;
                    default:
                        fn_80057e70(SE_PLY_JUMP_FENCE, false);
                        break;
                }
                mAngle.y = getMukiAngle(mDirection);
                return;
            }
            if (mKey.triggerAttack()) {
                mSubstate = VINE_ACTION_ATTACK;
                mSubstateTimer2 = 0;
                mPyMdlMng.setAnm(55);
                mSpeed.y = 0.0f;
                mSpeedF = 0.0f;
                onStatus(STATUS_4A);
                return;
            }
            break;
        case 1:
            if (setVineToKaniHangAction()) {
                return;
            }
            break;
    }
    calcVineSpeed();
    float f = 1.0f;
    if (mBc.mpFenceCollision != nullptr) {
        short s = abs(mBc.mpFenceCollision->m_c2);
        if (s > 128) {
            s = 128;
        }
        float tmp = s * 0.5f;
        f = tmp / 128.0f + 1.0f;
    }
    if (!mKey.buttonUp()) {
        m_7c4 = 0;
    }
    if (mSpeed.y) {
        float rate;
        if (mKey.buttonDush()) {
            rate = 1.875f;
        } else {
            rate = 1.5f;
        }
        rate = rate * f;
        if (mSpeed.y < 0.0f) {
            rate = -rate;
        }
        mPyMdlMng.setAnm(53, rate, 0.0f, 0.0f);
        m_7c4 = 5;
        setVineWalkSE();
    } else if (mSpeed.x) {
        if (m_7c4 == 0) {
            float rate;
            if (mKey.buttonDush()) {
                rate = 1.875f;
            } else {
                rate = 1.5f;
            }
            rate = rate * f;
            mPyMdlMng.setAnm(54, rate, 0.0f, 0.0f);
        }
        setVineWalkSE();
    } else if (f > 1.0f) {
        if (mKey.buttonLeft() || mKey.buttonRight()) {
            mPyMdlMng.setAnm(54, 0.6f * f, 0.0f, 0.0f);
        } else {
            mPyMdlMng.setAnm(53, 0.6f * f, 0.0f, 0.0f);
        }
    } else {
        mPyMdlMng.setAnm(52);
    }
}

void dAcPy_c::VineActionAttack() {
    if (checkVineEnd()) {
        return;
    }
    u16 frame = mPyMdlMng.mpMdl->mAnm.getFrame();
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(3.0f)) {
        onStatus(STATUS_49);
        mMtx_c jntMtx;
        mPyMdlMng.mpMdl->getJointMtx(&jntMtx, 14);
        jntMtx.concat(mMtx_c::createTrans(3.0f, 0.0f, 0.0f));
        mVec3_c efPos;
        jntMtx.multVecZero(efPos);
        dEf::createPlayerEffect_change(mPlayerNo, "Wm_mr_wirehit", 0, &efPos, nullptr, nullptr);
        fn_80057e70(SE_PLY_ATTACK_FENCE, false);
    }
    if (!isStatus(STATUS_85)) {
        if (frame >= 12.0f && mKey.buttonCross() || mPyMdlMng.isAnmStop()) {
            mSubstate = VINE_ACTION_NET;
            return;
        }
        if (mKey.triggerAttack()) {
            mSubstateTimer2 = 1;
        }
        if (frame >= 23.0f && mSubstateTimer2 != 0) {
            mPyMdlMng.mpMdl->setFrame(0.0f);
            mSubstateTimer2 = 0;
        }
    }
    if (frame < 24.0f) {
        onStatus(STATUS_4A);
        setCcAtNetPunch();
    }
}

void dAcPy_c::VineActionRoll() {
    onStatus(STATUS_7A);
    switch (m_7bc) {
        case 0:
            if (mPyMdlMng.isAnmStop()) {
                m_7bc = 1;
                mPyMdlMng.setAnm(52);
            }
            break;
        case 2:
            offStatus(STATUS_7A);
            mPyMdlMng.setAnm(52);
            mSubstate = VINE_ACTION_NET;
            break;
    }
}

void dAcPy_c::executeState_Vine() {
    offStatus(STATUS_49);
    offStatus(STATUS_4A);
    if (m_7c4 != 0) {
        m_7c4--;
    }
    typedef void (dAcPy_c::*VineActionProc_t)();
    static VineActionProc_t l_VineActionProc[] = {
        &dAcPy_c::VineActionIvy,
        &dAcPy_c::VineActionNet,
        &dAcPy_c::VineActionAttack,
        &dAcPy_c::VineActionRoll
    };
    (this->*l_VineActionProc[mSubstate])();
}

bool dAcPy_c::checkNetPunch() {
    if (
        isState(StateID_Vine) &&
        mSubstate == VINE_ACTION_ATTACK &&
        mPyMdlMng.mpMdl->mAnm.getFrame() >= 10.0f
    ) {
        mSubstate = VINE_ACTION_ROLL;
        m_7bc = 0;
        return true;
    }
    return false;
}

bool dAcPy_c::setAmiRollAction(mVec3_c *pos) {
    if (isState(StateID_Vine)) {
        if (mSubstate != VINE_ACTION_ROLL) {
            mSubstate = VINE_ACTION_ROLL;
            m_7bc = 1;
            mPyMdlMng.setAnm(52);
        }
        mPos.z = 0.0f;
        mAmiRollPos = *pos;
        mAmiAng = mAngle.y;
        mAmiXDiff = mPos.x - mAmiRollPos.x;
        mAmiRelated = 0.7f;
        return true;
    }
    return false;
}

bool dAcPy_c::isAmiRollAction() {
    if (isState(StateID_Vine) && mSubstate == VINE_ACTION_ROLL) {
        return true;
    }
    return false;
}

void dAcPy_c::setAmiRollPos(short ang, float f) {
    float cos = mAng(ang).cos();
    float sin = mAng(ang).sin();
    mVec3_c v(
        mAmiRollPos.x + cos * mAmiXDiff + sin * mAmiRelated,
        mPos.y,
        mAmiRollPos.z + cos * mAmiRelated - sin * mAmiXDiff
    );
    setAmiRollPos(ang, f, v);
}

void dAcPy_c::setAmiRollPos(short ang, float f, mVec3_c &v) {
    if (isAmiRollAction()) {
        mAngle.y = mAmiAng + ang;
        mPos = v;
        mAmiRelated2 = f;
    }
}

void dAcPy_c::endAmiRollAction(short ang) {
    if (isAmiRollAction()) {
        mAng newAng = m_7c0 + ang;
        m_7c0 = newAng;
        mAngle.y = newAng;
        if (m_7c0 == 0x8000) {
            m_ca1 = 1;
        } else {
            m_ca1 = 2;
        }
        mAmiRelated2 = 1.0f;
        m_7bc = 2;
    }
}

void dAcPy_c::finalizeState_Vine() {
    mAccelY = getGravityData()[0];
    offStatus(STATUS_33);
    offStatus(STATUS_7A);
    offStatus(STATUS_49);
    offStatus(STATUS_4A);
    mBc.mPlayerFlags &= ~1;
}

void dAcPy_c::setCarryPlayerHang(float height) {
    mPos.y = height - m_d00[3] / 4096.0f;
    onNowBgCross(BGC_34);
    changeState(StateID_Hang, 0);
}

bool dAcPy_c::setHangAction() {
    if (isNowBgCross(BGC_34)) {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr && carryPlayer->isStatus(STATUS_9C) && !carryPlayer->isCarry()) {
            float hangHeight = mPos.y + m_d00[3] / 4096.0f;
            releaseCarryActor();
            carryPlayer->setCarryPlayerHang(hangHeight);
            return true;
        }
        if (isStatus(STATUS_9C) && !isCarry() && mSpeed.y >= 0.0f) {
            changeState(StateID_Hang, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::initializeState_Hang() {
    onStatus(STATUS_34);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    m_1598 = 0.0f;
    if (mPlayerType == 2 || mPlayerType == 3) {
        static const float scHangOffsetY[POWERUP_COUNT] = { 4.5f, 10.0f, 10.0f, 1.0f, 10.0f, 9.0f, 10.0f };
        m_1598 = scHangOffsetY[mPowerup];
    }
    mPyMdlMng.setAnm(47);
    setZPositionDirect(400.0f);
    vf434(28, 0);
}

void dAcPy_c::HangActionStart() {
    mSpeedF = 0.0f;
    m_8f0 = 0;
    if (mPyMdlMng.isAnmStop()) {
        setHangActionWait();
    } else if (mKey.buttonRight() || mKey.buttonLeft()) {
        setHangActionMove();
    }
}

void dAcPy_c::setHangActionWait() {
    mSubstate = HANG_ACTION_WAIT;
    static const int scHangWaitAnm[] = { 48, 49 };
    u8 anmID = scHangWaitAnm[m_8f0];
    mPyMdlMng.setAnm(anmID);
}

void dAcPy_c::HangActionWait() {
    mSpeedF = 0.0f;
    if (mKey.buttonRight() || mKey.buttonLeft()) {
        setHangActionMove();
    }
}

void dAcPy_c::setHangActionMove() {
    mSubstate = HANG_ACTION_MOVE;
    static const int scHangMoveAnm[] = { 50, 51 };
    u8 anmID = scHangMoveAnm[m_8f0];
    mPyMdlMng.setAnm(anmID);
    m_8f0 ^= 1;
    fn_80057e70(SE_PLY_MONKEY_BARS, false);
}

void dAcPy_c::HangActionMove() {
    if (mKey.buttonDush()) {
        mSpeedF = sc_DirSpeed[mDirection] * 0.875f;
        mPyMdlMng.mpMdl->setRate(1.5f);
    } else {
        mSpeedF = sc_DirSpeed[mDirection] * 1.125f;
        mPyMdlMng.mpMdl->setRate(1.5f);
    }
    if (mPyMdlMng.isAnmStop()) {
        if (mKey.buttonRight() || mKey.buttonLeft()) {
            setHangActionMove();
        } else {
            setHangActionWait();
        }
    }
}

void dAcPy_c::executeState_Hang() {
    if (!isNowBgCross(BGC_34) || mKey.buttonDown()) {
        changeState(StateID_Fall, 0);
        vf434(29, 0);
    } else {
        turnBesideAngle();
        typedef void (dAcPy_c::*HangActionProc_t)();
        static HangActionProc_t l_HangActionProc[] = {
            &dAcPy_c::HangActionStart,
            &dAcPy_c::HangActionWait,
            &dAcPy_c::HangActionMove,
        };
        (this->*l_HangActionProc[mSubstate])();
    }
}

void dAcPy_c::finalizeState_Hang() {
    m_1598 = 0.0f;
    setScrollMode(0);
    offStatus(STATUS_34);
    offZPosSetNone();
}

void dAcPy_c::setPoleTurnAngle() {
    short target = getBesideMukiAngle(mDirection ^ 1);
    mAngle.y.chase(target, 0x2000);
}

bool dAcPy_c::setPoleAction() {
    if (isStatus(STATUS_9D)) {
        if (m_8fc || isCarry()) {
            return false;
        }
        if (mSpeed.y <= 3.0f && mKey.buttonUp() && isNowBgCross(BGC_43)) {
            changeState(StateID_Pole, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::initializeState_Pole() {
    onStatus(STATUS_35);
    onStatus(STATUS_97);
    onStatus(STATUS_9F);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = getBesideMukiAngle(mDirection);
    m_8f4.x = (int) (mPos.x / 16.0f) * 16.0f + 8.0f;
    setZPositionDirect(400.0f);
    if (isNowBgCross(BGC_43)) {
        m_8f4.y = mPos.y;
    }
    if (isNowBgCross(BGC_IS_FOOT)) {
        setPoleActionUp();
        mSpeed.y = 1.0f;
        offNowBgCross(BGC_IS_FOOT);
        mDirection ^= 1;
        offStatus(STATUS_97);
    } else {
        mPyMdlMng.setAnm(43);
        fn_80057e70(SE_PLY_POLE_CATCH, false);
        vf434(28, 0);
        mSubstate = 0;
    }
}

void dAcPy_c::PoleActionStart() {
    if (mKey.triggerJump()) {
        mSubstateTimer2 = 1;
    }
    if (mPyMdlMng.mpMdl->mAnm.getFrame() >= 5.0f && mSubstateTimer2 != 0) {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            mDirection = dir;
        }
        setPoleJump();

    } else if (mPyMdlMng.isAnmStop()) {
        setPoleActionWait();
        mDirection ^= 1;
        offStatus(STATUS_97);
    }
}

void dAcPy_c::setPoleActionWait() {
    mSubstate = 1;
    mPyMdlMng.setAnm(44);
}

void dAcPy_c::PoleActionWait() {
    if (!isNowBgCross(BGC_43)) {
        mDirection ^= 1;
        changeState(StateID_Fall, 0);
    } else {
        setPoleTurnAngle();
        mSpeed.y = 0.0f;
        if (mKey.buttonDown()) {
            setPoleActionDown();
        } else if (mKey.buttonUp()) {
            setPoleActionUp();
        }
    }
}

void dAcPy_c::setPoleActionUp() {
    mSubstate = 2;
    mPyMdlMng.setAnm(45);
}

void dAcPy_c::PoleActionUp() {
    setPoleTurnAngle();
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(0.0f)) {
        fn_80057e70(SE_PLY_POLE_CLIMB, false);
    }
    mSpeed.y = 0.0f;
    if (!isNowBgCross(BGC_43)) {
        mPos.y = m_8f4.y;
    }
    if (mKey.buttonDown()) {
        setPoleActionDown();
    } else if (!mKey.buttonUp()) {
        setPoleActionWait();
    } else if (mKey.buttonDush()) {
        mPyMdlMng.mpMdl->setRate(1.0f);
        mSpeed.y = 1.5f;
    } else {
        mPyMdlMng.mpMdl->setRate(1.0f);
        mSpeed.y = 1.0f;
    }
}

void dAcPy_c::setPoleActionDown() {
    mSubstate = 3;
    mPyMdlMng.setAnm(46);
}

void dAcPy_c::PoleActionDown() {
    if (!isNowBgCross(BGC_43)) {
        mDirection ^= 1;
        changeState(StateID_Fall, 0);
        return;
    }
    setPoleTurnAngle();
    mSpeed.y = 0.0f;
    if (mKey.buttonDown()) {
        setPoleActionDown();
    } else if (!mKey.buttonUp()) {
        setPoleActionWait();
    } else {
        if (mKey.buttonDush()) {
            float rate = 1.6f;
            mSpeed.y = rate * -2.0f;
            mPyMdlMng.mpMdl->setRate(rate);
        } else {
            mSpeed.y = -2.0f;
            mPyMdlMng.mpMdl->setRate(1.0f);
        }
        fn_80057f60(SE_PLY_POLE_SLIDE, false);
    }
}

void dAcPy_c::setPoleJump() {
    vf434(29, 0);
    m_8fc = 5;
    changeState(StateID_Jump, 0);
    u8 dir = mDirection;
    float baseSpeed = getSpeedData()[0];
    mSpeedF = baseSpeed * sc_DirSpeed[dir];
}

bool dAcPy_c::setPoleShakeJump() {
    u8 dir = mDirection;
    float baseSpeed = getSpeedData()[0];
    if (setRideOffPlayerJump(sc_JumpSpeed, baseSpeed * sc_DirSpeed[dir])) {
        vf434(29, 0);
        m_8fc = 5;
        return true;
    }
    return false;
}

void dAcPy_c::executeState_Pole() {
    if (isNowBgCross(BGC_IS_FOOT)) {
        mDirection ^= 1;
        changeState(StateID_Walk, (void *) 1);
    } else if (isNowBgCross(BgCross1_e(BGC_SIDE_LIMIT_L | BGC_SIDE_LIMIT_R))) {
        mDirection ^= 1;
        changeState(StateID_Walk, (void *) 0);
    } else {
        if (!setPoleShakeJump()) {
            if (mSubstate != 0 && mKey.triggerJump()) {
                setPoleJump();
                return;
            }
            mPos.x = m_8f4.x;
            if (isNowBgCross(BGC_43)) {
                m_8f4.y = mPos.y;
            }
            typedef void (dAcPy_c::*PoleActionProc_t)();
            static PoleActionProc_t l_PoleActionProc[] = {
                &dAcPy_c::PoleActionStart,
                &dAcPy_c::PoleActionWait,
                &dAcPy_c::PoleActionUp,
                &dAcPy_c::PoleActionDown,
            };
            (this->*l_PoleActionProc[mSubstate])();
            float yDelta = m_1068;
            mPos.y += yDelta;
        }
    }
}

void dAcPy_c::finalizeState_Pole() {
    offStatus(STATUS_35);
    offStatus(STATUS_97);
    offStatus(STATUS_9F);
    offZPosSetNone();
}

bool dAcPy_c::setKaniActionInitHangHand() {
    if (isCarry()) {
        return false;
    }
    if (
        !isNowBgCross(BGC_IS_FOOT) &&
        mSpeed.y < 0.0f &&
        !isOldBgCross(BgCross2_e(BGC_41 | BGC_42)) &&
        isNowBgCross(BgCross2_e(BGC_41 | BGC_42))
    ) {
        mPos.y = m_dc8;
        changeState(StateID_Kani, (void *) KANI_CHANGE_HANG_HAND);
        return true;
    }
    return false;
}

bool dAcPy_c::setKaniHangAction() {
    if (isNowBgCross(BGC_40) && mSpeed.y <= 0.0f) {
        if (isStatus(STATUS_A1)) {
            changeState(StateID_Kani, (void *) KANI_CHANGE_JUMP_HANG);
            return true;
        }
        if (isStatus(STATUS_A2)) {
            changeState(StateID_Kani, (void *) KANI_CHANGE_WALK);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setKaniWalkEffect() {
    if (m_b70 != 1) {
        mVec3_c efPos;
        mPyMdlMng.mpMdl->getJointPos(&efPos, 4);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf7, "Wm_mr_movecliff", 0, &efPos, nullptr, nullptr);
        mPyMdlMng.mpMdl->getJointPos(&efPos, 7);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf7, "Wm_mr_movecliff", 0, &efPos, nullptr, nullptr);
    }
}

void dAcPy_c::setKaniHangEffect() {
    if (m_b70 != 1) {
        mVec3_c efPos;
        mPyMdlMng.mpMdl->getJointPos(&efPos, 11);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf7, "Wm_mr_movecliff", 0, &efPos, nullptr, nullptr);
        mPyMdlMng.mpMdl->getJointPos(&efPos, 14);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf7, "Wm_mr_movecliff", 0, &efPos, nullptr, nullptr);
    }
}

void dAcPy_c::setKaniCliffCatchEffect() {
    static const char *scCliffCatchEffectID[] = {
        "Wm_mr_cliffcatch",
        "Wm_en_landsmoke_s"
    };
    static const float sc_cliffCatchScale[] = { 0.5f, 0.8f, 1.0f };
    u8 tallType = getTallType(-1);
    float scale = sc_cliffCatchScale[tallType];
    mVec3_c efSize(scale, scale, scale);
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 11);
    efPos.y = mPos.y;
    dEf::createPlayerEffect(mPlayerNo, scCliffCatchEffectID[m_b70], 0, &efPos, nullptr, &efSize);
    mPyMdlMng.mpMdl->getJointPos(&efPos, 14);
    efPos.y = mPos.y;
    dEf::createPlayerEffect(mPlayerNo, scCliffCatchEffectID[m_b70], 0, &efPos, nullptr, &efSize);
}

void dAcPy_c::initializeState_Kani() {
    onStatus(STATUS_A8);
    mSpeed.y = 0.0f;
    m_b70 = 0;
    if (isNowBgCross(BGC_IS_LIFT)) {
        m_b70 = 1;
    }
    m_b74 = m_b70;
    m_b78 = mPos.y;
    m_b7c = 0;
    KaniChangeParam_e changeParam = (KaniChangeParam_e) (int) mStateChangeParam;
    if (changeParam == KANI_CHANGE_WALK && mKey.buttonDown()) {
        changeParam = KANI_CHANGE_HANG;
    }
    switch (changeParam) {
        case KANI_CHANGE_WALK: setKaniAction_Walk(); break;
        case KANI_CHANGE_HANG: setKaniAction_Hang(); break;
        case KANI_CHANGE_JUMP_HANG: setKaniAction_JumpHang(); break;
        case KANI_CHANGE_WALK_2: setKaniAction_Walk(); break;
        case KANI_CHANGE_HANG_UP_VINE: setKaniAction_HangUpVine(); break;
        case KANI_CHANGE_HANG_HAND: setKaniAction_HangHand(); break;
    }
}

void dAcPy_c::finalizeState_Kani() {
    offStatus(STATUS_9B);
    offStatus(STATUS_38);
    offStatus(STATUS_37);
    offStatus(STATUS_19);
    offStatus(STATUS_A8);
    mAngle.y = getMukiAngle(mDirection);
    offZPosSetNone();
}

void dAcPy_c::setKaniMoveSpeed(bool b) {
    float scale = 0.0f;
    int dir;
    if (mKey.buttonWalk(&dir)) {
        float f;
        if (isStar()) {
            if (mKey.buttonDush()) {
                f = 1.3f;
            } else {
                f = 1.05f;
            }
        } else {
            if (mKey.buttonDush()) {
                f = sGlobalData_c<dAcPy_c>::mData.mKaniSpeedRelated[1];
            } else {
                f = sGlobalData_c<dAcPy_c>::mData.mKaniSpeedRelated[0];
            }
        }
        scale = f * sc_DirSpeed[dir];
    }
    if (b == true) {
        scale *= 0.8f;
    } else {
        scale *= 1.2f;
    }
    mMaxSpeedF = scale;
    normalPowerSet();
}

void dAcPy_c::setKaniAction_Walk() {
    mAngle.y = 0;
    setKaniMoveSpeed(true);
    if (!mMaxSpeedF) {
        mSpeedF = 0.0f;
    } else if (mMaxSpeedF < 0.0f) {
        if (mSpeedF < mMaxSpeedF) {
            mSpeedF = mMaxSpeedF;
        }
    } else if (mSpeedF > mMaxSpeedF) {
        mSpeedF = mMaxSpeedF;
    }
    mSubstate = KANI_ACTION_WALK;
}

void dAcPy_c::setKaniAction_Hang() {
    releaseCarryActor();
    mAngle.y = 0x8000;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(62);
    vf434(34, 0);
    mSubstate = KANI_ACTION_HANG_INIT;
    onStatus(STATUS_38);
}

void dAcPy_c::setKaniAction_JumpHang() {
    releaseCarryActor();
    mAngle.y = 0x8000;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(70);
    setKaniCliffCatchEffect();
    vf434(32, 0);
    mSubstate = KANI_ACTION_JUMP_HANG_INIT;
}

void dAcPy_c::setKaniAction_HangFall() {
    mPyMdlMng.setAnm(64);
    mSubstate = KANI_ACTION_HANG_FALL;
}

void dAcPy_c::setKaniAction_HangUp() {
    releaseCarryActor();
    mSpeedF = 0.0f;
    setKaniHangStartEffect();
    mPyMdlMng.setAnm(63);
    vf434(33, 0);
    mSubstate = KANI_ACTION_HANG_UP;
}

void dAcPy_c::setKaniAction_HangHand() {
    if (isNowBgCross(BGC_42)) {
        m_b70 = 1;
    }
    releaseCarryActor();
    mAngle.y = 0x8000;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(70, 0.0f, 0.0f);
    setKaniCliffCatchEffect();
    mSubstate = KANI_ACTION_JUMP_HANG_INIT;
}

void dAcPy_c::setKaniAction_HangUpVine() {
    releaseCarryActor();
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(64, 0.5f, 0.0f, 0.0f);
    mSubstate = KANI_ACTION_HANG_UP_VINE;
}

void dAcPy_c::setKaniHangStartEffect() {}

void dAcPy_c::KaniAction_Walk() {
    onStatus(STATUS_19);
    onStatus(STATUS_9B);
    if (!isNowBgCross(BGC_40)) {
        changeState(StateID_Walk, 0);
        return;
    }
    if (mKey.triggerDown()) {
        setKaniAction_Hang();
    } else if (mKey.triggerJump()) {
        changeState(StateID_KaniJump, 0);
    } else {
        setKaniMoveSpeed(true);
        if (!mSpeedF) {
            if (mPyMdlMng.getAnm() != 59) {
                mPyMdlMng.setAnm(59);
            }
        } else {
            setKaniWalkEffect();
            float absSpeed = std::fabs(mSpeedF) - 0.5f;
            if (absSpeed < 0.0f) {
                absSpeed = 0.0f;
            }
            if (absSpeed > 1.0f) {
                absSpeed = 1.0f;
            }
            float rate = absSpeed * 2.0f + 1.0f;
            static const int sc_WallWalkAnm[] = { 60, 61 };
            if (!mPyMdlMng.isAnm(sc_WallWalkAnm[mDirection])) {
                mPyMdlMng.setAnm(sc_WallWalkAnm[mDirection], rate, 10.0f, 0.0f);
            }
            mPyMdlMng.mpMdl->setRate(rate);
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f)) {
                fn_80057e70(SE_PLY_SHUFFLE, false);
            }
        }
    }
}

void dAcPy_c::KaniAction_HangInit() {
    onStatus(STATUS_37);
    if (mPyMdlMng.isAnmStop()) {
        setKaniHangStartEffect();
        mSubstate = KANI_ACTION_HANG;
        mPyMdlMng.setAnm(64, 0.0f, 0.0f);
        offStatus(STATUS_38);
    }
}

void dAcPy_c::KaniAction_JumpHangInit() {
    onStatus(STATUS_37);
    mPyMdlMng.mpMdl->mAnm.checkFrame(3.0f);
    if (mPyMdlMng.mpMdl->mAnm.getFrame() > 9.0f) {
        mSubstate = KANI_ACTION_HANG;
    }
}

bool dAcPy_c::checkCliffHangFootGround() {
    float height;
    if (dBc_c::checkGround(&mPos, &height, mLayer, 1, -1)) {
        float currHeight = mPos.y - getHangBcOffsetY();
        if (height > currHeight && (u8) (dBc_c::getUnitType(mPos.x, height, mLayer) >> 16) != 8) {
            mPos.y = currHeight;
            m_60 = 10;
            changeState(StateID_Walk, 0);
            return true;
        }
    }
    return false;
}

bool dAcPy_c::checkCliffHangWater() {
    float waterHeight;
    float currHeight = mPos.y - getHangBcOffsetY();
    u8 waterType = dBc_c::checkWater(mPos.x, currHeight, mLayer, &waterHeight);
    if (waterType == dBc_c::WATER_CHECK_WATER) {
        if (getWaterCheckPosY() - getHangBcOffsetY() <= waterHeight) {
            onNowBgCross(BGC_14);
            mPos.y = currHeight;
            m_60 = 10;
            changeState(StateID_Swim, (void *) 3);
            return true;
        }
    }
    return false;
}

void dAcPy_c::KaniAction_Hang() {
    onStatus(STATUS_37);
    if (!checkCliffHangFootGround() && !checkCliffHangWater() && !setKaniHangToVineAction()) {
        if (!isNowBgCross(BGC_IS_FOOT)) {
            setKaniAction_HangFall();
        } else if (m_b74 == 1 && m_b70 == 0 && m_b7c == 1) {
            setKaniAction_JumpHang();
        } else {
            setKaniMoveSpeed(false);
            mSpeedF = mMaxSpeedF;
            if (!mSpeedF) {
                if (mPyMdlMng.isAnm(70)) {
                    if (mPyMdlMng.isAnmStop()) {
                        mPyMdlMng.setAnm(64);
                    }
                } else if (!mPyMdlMng.isAnm(64)) {
                    mPyMdlMng.setAnm(64);
                }
            } else {
                setKaniHangEffect();
                float rate = std::fabs(mSpeedF);
                if (rate < 0.5f) {
                    rate = 0.5f;
                }
                if (rate > 1.5f) {
                    rate = 1.5f;
                }
                static const int sc_WallHangAnm[] = { 66, 67 };
                if (!mPyMdlMng.isAnm(sc_WallHangAnm[mDirection])) {
                    mPyMdlMng.setAnm(sc_WallHangAnm[mDirection], rate, 10.0f, 0.0f);
                }
                mPyMdlMng.mpMdl->setRate(rate);
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f)) {
                    fn_80057e70(SE_PLY_HANG_MOVE, false);
                }
            }
            int dir;
            if (
                !isNowBgCross(BGC_IS_HEAD) &&
                (
                    mKey.buttonUp() ||
                    mKey.buttonWalk(&dir) && checkBGCrossWall(dir)
                )
            ) {
                setKaniAction_HangUp();
            }
        }
    }
}

void dAcPy_c::KaniAction_HangFall() {
    onStatus(STATUS_37);
    if (!checkCliffHangFootGround() && !checkCliffHangWater()) {
        if (isNowBgCross(BGC_IS_FOOT)) {
            setKaniAction_JumpHang();
        } else {
            setKaniMoveSpeed(false);
            mSpeedF = mMaxSpeedF;
        }
    }
}

void dAcPy_c::KaniAction_HangUp() {
    onStatus(STATUS_37);
    if (mPyMdlMng.isAnmStop()) {
        mPyMdlMng.setAnm(59, 0.0f, 0.0f);
        setKaniAction_Walk();
    }
}

void dAcPy_c::KaniAction_HangUpVine() {
    KaniAction_HangUp();
}

void dAcPy_c::executeState_Kani() {
    if (checkEnableThrow()) {
        releaseCarryActor();
        return;
    }
    m_b74 = m_b70;
    m_b70 = 0;
    if (isNowBgCross(BGC_IS_LIFT)) {
        m_b70 = 1;
        m_b7c = 0;
        if (m_b78 > mPos.y) {
            m_b7c = 1;
        }
    }
    offStatus(STATUS_9B);
    offStatus(STATUS_19);
    offStatus(STATUS_37);
    mMaxSpeedF = 0.0f;
    mAccelF = 0.0f;
    mAccelY = getGravityData()[0];
    maxFallSpeedSet();
    offZPosSetNone();
    if (mSubstate != KANI_ACTION_WALK) {
        setZPosition(3200.0f);
    }
    typedef void (dAcPy_c::*KaniActionProc_t)();
    static KaniActionProc_t l_KaniActionProc[] = {
        &dAcPy_c::KaniAction_Walk,
        &dAcPy_c::KaniAction_HangInit,
        &dAcPy_c::KaniAction_JumpHangInit,
        &dAcPy_c::KaniAction_Hang,
        &dAcPy_c::KaniAction_HangFall,
        &dAcPy_c::KaniAction_HangUp,
        &dAcPy_c::KaniAction_HangUpVine,
    };
    (this->*l_KaniActionProc[mSubstate])();
    m_b78 = mPos.y;
}

void dAcPy_c::setCatchRopeSE() {
    switch (m_914) {
        case 0:
        case 2:
            fn_80057e70(SE_PLY_CATCH_ROPE, false);
            break;
        default:
            fn_80057e70(SE_PLY_CATCH_KUSARI, false);
            break;
    }
    vf434(28, 0);
}

void dAcPy_c::setClimbRopeSE() {
    switch (m_914) {
        case 0:
        case 2:
            fn_80057e70(SE_PLY_CLIMB_ROPE, false);
            break;
        default:
            fn_80057e70(SE_PLY_CLIMB_KUSARI, false);
            break;
    }
}

void dAcPy_c::setSlideRopeSE() {
    switch (m_914) {
        case 0:
        case 2:
            fn_80057e70(SE_PLY_SLIDE_ROPE, false);
            break;
        default:
            fn_80057e70(SE_PLY_SLIDE_KUSARI, false);
            break;
    }
}

void dAcPy_c::setRopeHasigoSE() {
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(10.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(30.0f)) {
        fn_80057e70(SE_PLY_FOOTNOTE_HASHIGO_NAWA, false);
    }
}

void dAcPy_c::releasePoleCheck() {
    mPc.release();
    mPc.m_2d = 0;
}

bool dAcPy_c::setTarzanRopeAction() {
    if (isCarry()) {
        return false;
    }
    if (m_910) {
        return false;
    }
    if (isStatus(STATUS_9C) && mPc.check(0)) {
        if (mPc.mpCtr->m_20 == 3) {
            if (mPos.y + mCenterOffs.y < mPc.mpCtr->m_28->y) {
                changeState(StateID_TarzanRope, (void *) 1);
                return true;
            }
        } else {
            changeState(StateID_TarzanRope, (void *) 0);
            return true;
        }
        releasePoleCheck();
    }
    if (isStatus(STATUS_A3) && mKey.buttonDown() && mPc.check(0)) {
        if (mPc.mpCtr->m_20 == 3) {
            changeState(StateID_TarzanRope, (void *) 2);
            return true;
        }
        releasePoleCheck();
    }
    return false;
}

void dAcPy_c::initializeState_TarzanRope() {
    onStatus(STATUS_97);
    onStatus(STATUS_7F);
    onStatus(STATUS_36);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    m_918 = 0.0f;
    setZPositionDirect(400.0f);
    switch ((int) mStateChangeParam) {
        case 0:
            mAngle.y = getBesideMukiAngle(mDirection);
            mPyMdlMng.setAnm(43);
            if (mPc.mpCtr->m_20 == 2) {
                m_914 = 1;
            } else {
                m_914 = 0;
            }
            mSubstate = 0;
            setCatchRopeSE();
            break;
        case 1:
            mAngle.y = 0x8000;
            m_914 = 2;
            setTarzanRopeActionHasigoMove();
            setCatchRopeSE();
            break;
        case 2:
            offStatus(STATUS_36);
            releasePoleCheck();
            mAngle.y = 0x8000;
            mPyMdlMng.setAnm(62);
            vf434(34, 0);
            m_914 = 2;
            mSubstate = 5;
    }
}

void dAcPy_c::finalizeState_TarzanRope() {
    offStatus(STATUS_97);
    offStatus(STATUS_7F);
    offStatus(STATUS_36);
    mPc.release();
    m_1594 = 0.0f;
    m_1598 = 0.0f;
    mAngle.x = 0;
    mAngle.z = 0,
    offZPosSetNone();
}

bool dAcPy_c::setTarzanRopeJump() {
    if (!mKey.triggerShakeJump() && !mKey.triggerJump()) {
        return false;
    }
    float jumpSpeed = sc_JumpSpeed;
    float h = 0.0f;
    if (!isNowBgCross(BGC_IS_HEAD)) {
        h = mPc.getPos().x;
        if (mKey.buttonLeft()) {
            h -= 1.5f;
        } else if (mKey.buttonRight()) {
            h += 1.5f;
        }
        if (h < 0.0f) {
            jumpSpeed = -4.0f;
            if (h < -4.0f) {
                h = -4.0f;
            } else if (h > -0.5f) {
                h = -0.5f;
            }
        } else if (h > 0.0f) {
            jumpSpeed = 4.0f;
            if (h > 4.0f) {
                h = 4.0f;
            } else if (h < 0.5f) {
                h = 0.5f;
            }
        } else {
            h = sc_DirSpeed[mDirection] * getSpeedData()[0];
        }
        jumpSpeed = mPc.getPos().y;
    }
    if (setRideOffPlayerJump(jumpSpeed, h)) {
        vf434(29, 0);
        if (h * sc_DirSpeed[mDirection] < 0.0f) {
            mDirection ^= 1;
        }
        mNoGravityTimer = 10;
        return true;
    } else {
        if (mKey.triggerJump()) {
            vf434(29, 0);
            if (h * sc_DirSpeed[mDirection] < 0.0f) {
                mDirection ^= 1;
            }
            mNoGravityTimer = 10;
            mSpeedF = h;
            jmpInf_c jump(jumpSpeed, 1, 1);
            changeState(StateID_Jump, &jump);
            onStatus(STATUS_88);
            return true;
        }
    }
    return false;
}

bool dAcPy_c::updateRopeAngle() {
    m_906 = m_904;
    m_904 = mPc.getAngle();
    m_902 = m_900;
    if (mPc.mpCtr != nullptr) {
        if (mPc.mpCtr->m_20 == 1 || mPc.mpCtr->m_20 == 2) {
            m_900 = mPc.mpCtr->m_34[mPlayerNo];
            return true;
        }
    }
    return false;
}

void dAcPy_c::setRopeSwingAnm(float a, float b) {
    mPyMdlMng.mpMdl->m_154 = 1;
    mPyMdlMng.setAnm(126, a, 10.0f, b);
}

bool dAcPy_c::checkStartSwingUp() {
    int baseAngle = m_900;
    int absAngle = abs(baseAngle);
    if (absAngle > 128 && absAngle < abs(m_902)) {
        if (sc_DirSpeed[mDirection] * baseAngle >= 0.0f) {
            m_90c = ROPE_SWING_1;
            setRopeSwingAnm(1.0f, 1.0f);
            return true;
        }
    }
    return false;
}

bool dAcPy_c::checkStartSwingDown() {
    int baseAngle = m_900;
    int absAngle = abs(baseAngle);
    if (absAngle > 128 && absAngle < abs(m_902)) {
        if (sc_DirSpeed[mDirection] * baseAngle <= 0.0f) {
            int dir;
            bool ok = mKey.buttonWalk(&dir) && dir != mDirection;
            if (ok) {
                setRopeSwingAnm(1.0f, 64.0f);
                m_90c = ROPE_SWING_7;
                return true;
            }
            m_90c = ROPE_SWING_0;
        }
    }
    return false;
}

void dAcPy_c::updateRopeSwingAnm() {
    if (updateRopeAngle()) {
        switch (m_90c) {
            case ROPE_SWING_0:
                checkStartSwingUp();
                break;
            case ROPE_SWING_1:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(32.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_2;
                }
                // fallthrough
            case ROPE_SWING_2:
                if (m_900 * m_902 <= 0) {
                    setRopeSwingAnm(1.0f, 32.0f);
                    m_90c = ROPE_SWING_3;
                }
                break;
            case ROPE_SWING_3:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(64.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_4;
                }
                // fallthrough
            case ROPE_SWING_4:
                checkStartSwingDown();
                break;
            case ROPE_SWING_5:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(86.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_8;
                }
                // fallthrough
            case ROPE_SWING_6:
                if (m_900 * m_902 <= 0) {
                    setRopeSwingAnm(-0.5f, 86.0f);
                    m_90c = ROPE_SWING_10;
                }
                break;
            case ROPE_SWING_10:
                if (mPyMdlMng.isAnmStop()) {
                    m_90c = ROPE_SWING_0;
                }
                checkStartSwingUp();
                break;
            case ROPE_SWING_7:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(96.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_6;
                }
                // fallthrough
            case ROPE_SWING_8:
                if (m_900 * m_902 <= 0) {
                    setRopeSwingAnm(1.0f, 96.0f);
                    m_90c = ROPE_SWING_9;
                }
                break;
            case ROPE_SWING_9:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(64.0f)) {
                    m_90c = ROPE_SWING_0;
                }
                checkStartSwingUp();
                break;
        }
    } else {
        m_90c = ROPE_SWING_0;
    }
    if (m_90c == ROPE_SWING_0) {
        mPyMdlMng.setAnm(44, 0.0f, 10.0f);
    }
}

void dAcPy_c::TarzanRopeActionStart() {
    if (mPyMdlMng.isAnmStop()) {
        setTarzanRopeActionWait();
    }
}

void dAcPy_c::setTarzanRopeActionWait() {
    mSubstate = 1;
    m_918 = 0;
    updateRopeAngle();
    m_906 = m_904;
    m_902 = m_900;
    m_90c = ROPE_SWING_0;
    m_908 = 0;
}

void dAcPy_c::TarzanRopeActionWait() {
    turnBesideAngle();
    updateRopeSwingAnm();
    if (!(mPc.m_2a & 4) && mKey.buttonDown()) {
        setTarzanRopeActionDown();
    } else if (!(mPc.m_2a & 2) && mKey.buttonDown()) {
        setTarzanRopeActionUp();
    }
}

void dAcPy_c::setTarzanRopeActionUp() {
    mSubstate = 2;
    mPyMdlMng.setAnm(45);
}

void dAcPy_c::TarzanRopeActionUp() {
    turnBesideAngle();
    if (m_914 == 0) {
        setIvyHangEffect();
    }
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f)) {
        setClimbRopeSE();
    }
    if (mKey.buttonDush()) {
        mPyMdlMng.mpMdl->setRate(1.0f);
        m_918 = 1.0f;
    } else {
        mPyMdlMng.mpMdl->setRate(1.5f);
        m_918 = 1.5f;
    }
    if (!(mPc.m_2a & 4) && mKey.buttonDown()) {
        setTarzanRopeActionDown();
    } else if (!mKey.buttonUp()) {
        setTarzanRopeActionWait();
    }
}

void dAcPy_c::setTarzanRopeActionDown() {
    mSubstate = 3;
    mPyMdlMng.setAnm(46);
}

void dAcPy_c::TarzanRopeActionDown() {
    turnBesideAngle();
    if (m_914 == 0) {
        setIvyHangEffect();
    }
    setSlideRopeSE();
    m_918 = -2.0f;
    if (!(mPc.m_2a & 2) && mKey.buttonUp()) {
        setTarzanRopeActionUp();
    } else if (!mKey.buttonDown()) {
        setTarzanRopeActionWait();
    }
}

void dAcPy_c::setTarzanRopeActionHasigoMove() {
    mSubstate = 4;
}

void dAcPy_c::TarzanRopeActionHasigoMove() {
    if (mKey.buttonUp() && (mPc.m_2a & 2)) {
        setTarzanRopeActionHasigoHangUp();
        return;
    }
    float f1 = mKey.buttonDush() ? 1.5f : 1.0f;
    float f2 = mKey.buttonDush() ? 1.5f : 1.875f;
    if (mKey.buttonUp() && !(mPc.m_2a & 2)) {
        m_918 = f1;
        mPyMdlMng.setAnm(53, f2, 0.0f, 0.0f);
        setRopeHasigoSE();
    } else if (mKey.buttonDown() && !(mPc.m_2a & 4)) {
        m_918 = -f1;
        mPyMdlMng.setAnm(53, -f2, 0.0f, 0.0f);
        setRopeHasigoSE();
    } else {
        m_918 = 0.0f;
        mPyMdlMng.setAnm(52);
    }
}

void dAcPy_c::TarzanRopeActionHasigoHangDown() {
    if (!mPyMdlMng.isAnmStop()) {
        return;
    }
    mPyMdlMng.setAnm(52, 0.0f, 0.0f);
    mPos.y -= getHangBcOffsetY();
    m_60 = 10;
    onStatus(STATUS_36);
    setTarzanRopeActionHasigoMove();
    if (!mPc.check(1)) {
        m_910 = 30;
        mPyMdlMng.setAnm(6, 0.0f, 0.0f);
        changeState(StateID_Fall, (void *) 1);
    }
}

void dAcPy_c::setTarzanRopeActionHasigoHangUp() {
    mVec3_c pos = mPos;
    pos.y += getHangBcOffsetY() + 8.0f;
    float h;
    if (dBc_c::checkGround(&pos, &h, mLayer, 1, -1)) {
        if (mPos.y < h) {
            mPos.y = h;
        }
    }
    m_60 = 10;
    mSubstate = 6;
    m_91c = 0;
    mPyMdlMng.setAnmOnlyRate(64, 0.0f, 0.0f);
    vf434(33, 0);
    offStatus(STATUS_36);
    releasePoleCheck();
}

void dAcPy_c::TarzanRopeActionHasigoHangUp() {
    switch (m_91c) {
        case 0:
            mPyMdlMng.setAnm(63);
            m_91c = 1;
            break;
        case 1:
            if (mPyMdlMng.isAnmStop()) {
                mPyMdlMng.setAnmOnlyRate(59, 0.0f, 0.0f);
                mAngle.y = 0;
                m_91c = 2;
            }
            break;
        case 2:
            changeState(StateID_Walk, (void *) 1);
            break;
    }
}

void dAcPy_c::executeState_TarzanRope() {
    typedef void (dAcPy_c::*TarzanRopeActionProc_t)();
    static TarzanRopeActionProc_t l_TarzanRopeActionProc[] = {
        &dAcPy_c::TarzanRopeActionStart,
        &dAcPy_c::TarzanRopeActionWait,
        &dAcPy_c::TarzanRopeActionUp,
        &dAcPy_c::TarzanRopeActionDown,
        &dAcPy_c::TarzanRopeActionHasigoMove,
        &dAcPy_c::TarzanRopeActionHasigoHangDown,
        &dAcPy_c::TarzanRopeActionHasigoHangUp
    };
    if (!isStatus(STATUS_36)) {
        (this->*l_TarzanRopeActionProc[mSubstate])();
    } else if (isNowBgCross(BgCross1_e(BGC_SIDE_LIMIT_L | BGC_SIDE_LIMIT_R)) || mPc.mpCtr == nullptr) {
        changeState(StateID_Fall, 0);
    } else {
        s16 ang = mPc.getAngle();
        if (m_914 == 0 || m_914 == 1) {
            if (mDirection == DIR_LR_L) {
                ang = -mPc.getAngle();
            }
            mAngle.x = ang;
            float f = -2.5f;
            if (mPowerup == POWERUP_NONE) {
                f = -1.5f;
            }
            m_1594 = f * mAng(ang).cos() * mAng(mAngle.y).sin();
            m_1598 = f * mAng(ang).sin();
        } else {
            mAngle.z = -ang;
        }
        if (!setTarzanRopeJump()) {
            (this->*l_TarzanRopeActionProc[mSubstate])();
            mPc.move(get_1068() + m_918);
            if (mPc.m_2a & 1) {
                changeState(StateID_Fall, 0);
            }
        }
    }
}

void dAcPy_c::startQuakeAction(QuakeMode_e mode) {
    setQuakeActionLocal(0, mode);
    mSubstateTimer2 = 1;
}

void dAcPy_c::setQuakeAction(int i, QuakeMode_e mode) {
    setQuakeActionLocal(i, mode);
    mSubstateTimer2 = 0;
}

void dAcPy_c::setQuakeActionLocal(int i, QuakeMode_e mode) {
    if (!isDemo() && isNowBgCross(BGC_IS_FOOT) && !isState(StateID_Quake)) {
        changeState(StateID_Quake, (void *) mode);
        mSubstateTimer = i;
    }
}

void dAcPy_c::endQuakeAction() {
    if (isState(StateID_Quake)) {
        changeState(StateID_Walk, (void *) 1);
    }
}

void dAcPy_c::initializeState_Quake() {
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = 0.0f;
    mPyMdlMng.setAnm(82);
    onStatus(STATUS_08);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
    vf434(51, 0);
    startQuakeShock(dQuake_c::TYPE_6);
    m_1040 = 8;
    mSubstate = (int) mStateChangeParam;
}

void dAcPy_c::finalizeState_Quake() {
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_08);
}

void dAcPy_c::executeState_Quake() {
    if (m_1040 != 0) {
        m_1040--;
    }
    if (m_1040 == 0) {
        m_1040 = 5;
        startQuakeShock(dQuake_c::TYPE_8);
    }
    setQuakeNumbEffect();
    if (mSubstateTimer2 == 0 && mSubstateTimer == 0) {
        changeState(StateID_Walk, (void *) 1);
    }
}

void dAcPy_c::setQuakeNumbEffect() {
    mVec3_c center = getCenterPos();
    static const float sc_QuakeNumbScale[] = { 0.5f, 0.8f, 1.0f };
    u8 tallType = getTallType(-1);
    float scale = sc_QuakeNumbScale[tallType];
    mVec3_c size(scale, scale, scale);
    static const char *sc_QuakeEffectID[] = {
        "Wm_mr_brosquake",
        "Wm_mr_quakewait"
    };
    dEf::createPlayerEffect(mPlayerNo, &mFunsuiSmokeEffect, sc_QuakeEffectID[mSubstate], 0, &center, nullptr, &size);
}

void dAcPy_c::initializeState_ElecShock() {
    initElecShock();
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = 0;
    onStatus(STATUS_06);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void dAcPy_c::finalizeState_ElecShock() {
    offStatus(STATUS_06);
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void dAcPy_c::executeState_ElecShock() {
    if (!executeElecShock()) {
        changeState(StateID_Walk, (void *) 1);
        mPyMdlMng.setAnm(82);
        setDamage2(nullptr, DAMAGE_1);
    }
}

void dAcPy_c::initElecShock() {
    vf434(48, 0);
    fn_80057e70(SE_PLY_TOUCH_BIRI, false);
    mPyMdlMng.setAnm(82);
    mSubstateTimer = 60;
}

bool dAcPy_c::executeElecShock() {
    setElecEffect();
    return mSubstateTimer != 0;
}

void dAcPy_c::setElecEffect() {
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
    float s = dPyMdlMng_c::m_hio.m_08[mPyMdlMng.mpMdl->m_152];
    mVec3_c efScale(s, s, s);
    int shockType = 0;
    if (mPowerup == POWERUP_MINI_MUSHROOM || mPowerup == POWERUP_NONE) {
        shockType = 1;
    }
    static const char *sc_elecEffectID[] = {
        "Wm_mr_electricshock",
        "Wm_mr_electricshock_s"
    };
    dEf::createPlayerEffect(mPlayerNo, &mFunsuiSmokeEffect, sc_elecEffectID[shockType], 0, &efPos, nullptr, &efScale);
}

bool dAcPy_c::setFlyDamageAction(int action, dActor_c *actor) {
    if (isState(StateID_FlyDamage)) {
        return false;
    }
    bool changeParam = false;
    if (actor != nullptr) {
        if (action == 4) {
            mPos.y += 12.0f;
            mPos.x = mPos.x - 4.0f * sc_DirSpeed[mDirection];
        } else {
            switch ((u32) actor->mKind) {
                case STAGE_ACTOR_PLAYER:
                case STAGE_ACTOR_ENEMY:
                    if (actor->mProfName == fProfile::EN_SLIP_PENGUIN2) {
                        bool dir = 0;
                        if (actor->mSpeed.x < 0.0f) {
                            dir = 1;
                        }
                        if (dir == mDirection && (actor->mLastPos.x - mLastPos.x) * sc_DirSpeed[mDirection] < 0.0f) {
                            changeParam = true;
                        }
                    } else if ((actor->mLastPos.x - mLastPos.x) * sc_DirSpeed[mDirection] < 0.0f) {
                        changeParam = true;
                    }
                    break;
                default:
                    if (mDirection == actor->mDirection) {
                        changeParam = true;
                    }
            }
        }
    }
    int isAction6 = 0;
    u32 finalChangeParam = 0;
    if (action == 2) {
        finalChangeParam = 2;
    } else if (action == 3 || action == 4) {
        finalChangeParam = 4;
    } else if (action == 5) {
        finalChangeParam = 0;
    } else if (action == 6) {
        finalChangeParam = 0;
        isAction6 = 1;
    }
    finalChangeParam = (finalChangeParam + changeParam) & 0xFF;
    if (isAction6 == 1) {
        finalChangeParam |= 0x100;
    }
    changeState(StateID_FlyDamage, (void *) finalChangeParam);
    return true;
}

template <>
dAcPy_c::GlobalData_t sGlobalData_c<dAcPy_c>::mData = {
    0.0f, 0.0f,
    1.5f, 4.5f, 0.5f, 4.0f, 1.4f, 1.0f,
    {
        mVec3_c(0.0f, 10.0f, 0.0f),
        mVec3_c(0.0f, 16.0f, 0.0f),
        mVec3_c(0.0f, 28.0f, 0.0f)
    },
    -3.0f, 3.0f, 3.2f,
    0.5f, 3.2f, 3.0f, 1.2f,
    0.5f, 0.625f, 0.5f, 0.625f
};

dAcPy_HIO_Speed_c dAcPy_c::m_speed_hio[2];

ACTOR_PROFILE(PLAYER, dAcPy_c, 0);

dAcPy_c::dAcPy_c() : mPyMdlMng(daPyMng_c::getCourseInPlayerModelType(ACTOR_PARAM(PlayerNo))) {
    setKind(STAGE_ACTOR_PLAYER);
    mExecStopMask = BIT_FLAG(STAGE_ACTOR_PLAYER);
    mpMdlMng = &mPyMdlMng;
    mPyMdlMng.mpMdl->mpOwner = this;
    mPlayerNo = ACTOR_PARAM(PlayerNo);
    mPlayerType = daPyMng_c::mPlayerType[mPlayerNo];
    mPowerup = daPyMng_c::mPlayerMode[daPyMng_c::mPlayerType[mPlayerNo]];
    mCreateItemRelated = 0;
    if (daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] & 8) {
        mCreateItemRelated = 1;
        dInfo_c::m_instance->m_6c = true;
        mPlayerType = 3;
        mPowerup = daPyMng_c::mKinopioMode;
        daPyMng_c::mPlayerEntry[mPlayerNo] = 0;
        daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] &= ~CREATE_ITEM_RESCUE_TOAD;
    }
    daPyMng_c::setPlayer(mPlayerNo, this);
    mPlayerLayer = mPlayerNo;
}

dAcPy_c::~dAcPy_c() {
    daPyMng_c::setPlayer(mPlayerNo, nullptr);
}

int dAcPy_c::doDelete() {
    if (fn_801477c0() && !isItemKinopio()) {
        dGameCom::fn_800b37b0(mPlayerNo, 1);
    }
    setSceneChangeInfo();
    daPlBase_c::doDelete();
    return SUCCEEDED;
}

void dAcPy_c::setSceneChangeInfo() {
    if (isItemKinopio()) {
        if (dScStage_c::getExitMode() == dScStage_c::EXIT_3) {
            dAcPy_c *ctrlPlayer = daPyMng_c::getCtrlPlayer(mPlayerNo);
            if (
                ctrlPlayer != nullptr &&
                !ctrlPlayer->isStatus(STATUS_65) &&
                !isStatus(STATUS_04) &&
                !isStatus(STATUS_B6)
            ) {
                daPyMng_c::fn_8005f570(mPowerup, mPlayerNo);
            }
        }
        return;
    }

    u32 createItem = CREATE_ITEM_NONE;
    PLAYER_POWERUP_e powerup = mPowerup;

    switch (dScStage_c::getExitMode()) {
        case dScStage_c::EXIT_3: {
            if (!isStatus(STATUS_55) || isNotBalloonCourse()) {
                if (isStatus(STATUS_53)) {
                    createItem = CREATE_ITEM_BUBBLE;
                } else if (isStatus(STATUS_04)) {
                    powerup = POWERUP_NONE;
                    createItem = CREATE_ITEM_BUBBLE;
                }
            }
            daYoshi_c *yoshi = getRideYoshi();
            if (yoshi != nullptr) {
                int x = yoshi->m_d4;
                createItem = CREATE_ITEM_YOSHI;
                int v = ((dYoshiMdl_c *) yoshi->getModel())->m_40;
                daPyMng_c::setCarryOverYoshiInfo(mPlayerNo, v, x);
                onStatus(STATUS_C5);
            }
            break;
        }
        case dScStage_c::EXIT_2:
            powerup = daPyMng_c::mPlayerMode[daPyMng_c::mPlayerType[mPlayerNo]];
            createItem = daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]];
            break;
        case dScStage_c::EXIT_1:
            powerup = POWERUP_NONE;
            break;
        default:
            break;
    }

    daPyMng_c::mPlayerEntry[mPlayerNo] = 1;
    daPyMng_c::mPlayerMode[daPyMng_c::mPlayerType[mPlayerNo]] = powerup;
    daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] = createItem;
    daPyMng_c::m_star_time[mPlayerNo] = mStarTimer;
    daPyMng_c::m_star_count[mPlayerNo] = mStarCount;
}

int dAcPy_c::create() {
    mPyMdlMng.create(mPlayerNo, mPowerup, dPyMdlMng_c::SCENE_TYPE_0);
    mPowerupCopy2 = mPowerup;
    mPowerupCopy = mPowerup;
    setPowerup(mPowerup, 1);
    mDirection = ACTOR_PARAM(Direction);
    mAngle.y = getMukiAngle(mDirection);
    mSpeedMax.set(0.0f, 0.0f, 0.0f);
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAccelY = getGravityData()[0];
    mScale.set(1.0f, 1.0f, 1.0f);
    mAmiRelated2 = 1.0f;
    setStatus(STATUS_00);
    mCarryActorID = BASE_ID_NULL;
    mRideActorID = BASE_ID_NULL;
    mUnkActorID = BASE_ID_NULL;
    if (isItemKinopio()) {
        setScrollMode(1);
    } else {
        setScrollMode(0);
    }
    mPc.m_30 = mPos;
    mPropelParts.create(this);
    initCcData();
    initBcData();
    setCreateAction(ACTOR_PARAM(CreateAction));
    calcModel();
    mPc.set(this, 1);
    if (dScStage_c::m_miniGame == 0 && dScStage_c::m_instance->mCurrLevel != STAGE_PEACH_CASTLE) {
        if ((daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] & CREATE_ITEM_STAR_POWER)) {
            setStar(STAR_SET_2, 660);
        } else {
            if (daPyMng_c::m_star_time[mPlayerNo] != 0) {
                setStar(STAR_SET_2, daPyMng_c::m_star_time[mPlayerNo]);
            }
            mStarCount = daPyMng_c::m_star_count[mPlayerNo];
        }
    }
    if (!isItemKinopio() && dScStage_c::m_gameMode == 0) {
        mSndObj.m_58 = dAudio::getRemotePlayer(mPlayerNo);
    }
    int x = 0;
    switch (mPlayerType) {
        case 0: x = 0; break;
        case 1: x = 1; break;
        case 2: x = 2; break;
        case 3: x = 3; break;
    }
    mSndObj.m_ac = x;
    setSoundPlyMode();
    daPlBase_c::create();
    mKey.mRemoconID = mPlayerNo;
    return SUCCEEDED;
}

int dAcPy_c::preExecute() {
    u32 oldExecStopMask = mExecStopMask;
    daYoshi_c *yoshi = getRideYoshi();
    if (yoshi != nullptr) {
        mExecStopMask = yoshi->mExecStopMask;
    }
    daPlBase_c::preExecute();
    mExecStopMask = oldExecStopMask;
}

#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_en_door.hpp>
#include <game/bases/d_a_fireball_player.hpp>
#include <game/bases/d_a_player_ice.hpp>
#include <game/bases/d_a_iceball.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_player_effect_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_next.hpp>
#include <game/bases/d_mask_manager.hpp>
#include <game/bases/d_mario_mdl.hpp>
#include <game/bases/d_water_entry_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/bases/d_yoshi_mdl.hpp>
#include <game/mLib/m_fader.hpp>
#include <game/sLib/s_GlobalData.hpp>
#include <constants/sound_list.h>

const float dAcPy_c::msc_JUMP_SPEED = daPlBase_c::sc_JumpSpeed;

inline float getSomeData(int idx) {
    return dAcPy_c::data_802f5a0c[idx];
}

inline float getData(int idx) {
    return dAcPy_c::data_802f5a48[idx];
}

bool dAcPy_c::setHipAttackOnEnemy(mVec3_c *hitPos) {
    if (isState(StateID_HipAttack) && isStatus(STATUS_HIP_ATTACK_FALL)) {
        onStatus(STATUS_PRESS_ATTACH);
        mPressAttachPos = *hitPos;
        return true;
    } else if (isState(StateID_SpinHipAttack) && isStatus(STATUS_SPIN_HIP_ATTACK_FALL)) {
        onStatus(STATUS_PRESS_ATTACH);
        mPressAttachPos = *hitPos;
        return true;
    }
    return false;
}

void dAcPy_c::setHipAttackEffect() {
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        setSeaLandSmokeEffect();
        startSound(SE_PLY_HPDP_SPLASH, 0);
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            startSound(SE_PLY_HIP_ATTACK_M, 0);
        } else {
            startSound(SE_PLY_HIP_ATTACK, 0);
        }
    } else {
        daPlBase_c::setHipAttackEffect();
    }
}

bool dAcPy_c::setHipAttackAction() {
    if (
        !isStatus(STATUS_A7) &&
        !isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_37) &&
        mKey.checkHipAttack() &&
        !isDemoType(DEMO_ENDING_DANCE)
    ) {
        if (!isStatus(STATUS_PROPEL)) {
            if (!isCarry()) {
                changeState(StateID_HipAttack, HIP_ATTACK_ARG_PLAYER);
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
    startSound(SE_PLY_ROLLING, 0);
}

void dAcPy_c::initializeState_HipAttack() {
    daPlBase_c::initializeState_HipAttack();
    mJumpCounter = 0;
    offStatus(STATUS_CAN_WATER_WALK);
    onStatus(STATUS_CAN_LAND);
}

void dAcPy_c::finalizeState_HipAttack() {
    daPlBase_c::finalizeState_HipAttack();
    offStatus(STATUS_CAN_LAND);
}

void dAcPy_c::executeState_HipAttack() {
    if (isNowBgCross(BGC_WATER_SHALLOW) && isOldBgCross(BGC_WATER_SHALLOW) == 0) {
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
    mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_JUMP2);
    onStatus(STATUS_SPIN_HIP_ATTACK_FALL);
    onStatus(STATUS_A8);
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_SPIN);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    int changeParam = (int) mStateArg;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    if (changeParam == 0) {
        mSubstateValue = 0;
        mSpeed.y = 0.0f;
        float v = data_802f5a0c[13];
        mMaxFallSpeed = v;
    } else {
        mSubstateValue = 1;
        mSpeed.y = -6.0f;
        mMaxFallSpeed = -6.0f;
    }
    m_2e8 = 0x1000;
    mSubstateTimer = 10;
    setScrollMode(2);
}

void dAcPy_c::executeState_SpinHipAttack() {
    offStatus(STATUS_SPIN_HIP_ATTACK_LANDING);
    if (isStatus(STATUS_SPIN_HIP_ATTACK_FALL)) {
        setCcAtSpinFall();
    }
    if (isNowBgCross(BGC_WATER_SHALLOW) && isOldBgCross(BGC_WATER_SHALLOW) == 0) {
        setWaterInEffect();
    }
    if (mPowerup != POWERUP_PROPELLER_SHROOM && getCarryPropelActor() == nullptr) {
        changeState(StateID_Fall, false);
        return;
    }
    switch ((SpinHipAttackSubstate_e) mSubstate) {
        case SPIN_HIP_ATTACK_ACTION_0:
            holdSound(SE_PLY_PRPL_LETDOWN_FAST, 0);
            if (isNowBgCross(BGC_WATER_SHALLOW)) {
                mMaxFallSpeed = sc_WaterMaxFallSpeed;
            } else {
                sLib::chase(&mMaxFallSpeed, data_802f5a0c[13], 0.1f);
            }
            mAccelY = getGravityData()[0];
            onStatus(STATUS_AA);
            if (isNowBgCross(BGC_WATER_SUBMERGED)) {
                offStatus(STATUS_AA);
            }
            if (isNowBgCross(BGC_62)) {
                mSpeed.y = 0.0f;
                m_2e8 = 0x1000;
                offNowBgCross(BGC_FOOT);
            }
            if (isNowBgCross(BGC_FOOT) && !isOldBgCross(BGC_62)) {
                startQuakeShock(dQuake_c::TYPE_7);
                startSound(SE_PLY_PRPL_LETDOWN_FAST_LAND, 0);
                if (isNowBgCross(BGC_CLIFF)) {
                    changeState(daPlBase_c::StateID_Kani, KANI_ARG_JUMP_HANG); // [why daPlBase_c?]
                    return;
                }
                s16 curr = m_2e8;
                if (checkCrouch()) {
                    m_2e8 = curr;
                    return;
                }
                mSubstate = SPIN_HIP_ATTACK_ACTION_1;
                mAngle.y = getMukiAngle(mDirection);
                mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_JUMP_END);
                mSubstateTimer = 20;
                offStatus(STATUS_SPIN_HIP_ATTACK_FALL);
                offStatus(STATUS_SPIN);
                onStatus(STATUS_9F);
                onStatus(STATUS_SPIN_HIP_ATTACK_LANDED);
                onStatus(STATUS_SPIN_HIP_ATTACK_LANDING);
                return;
            }
            if (!mKey.buttonDown() && mSubstateTimer == 0) {
                if (mSubstateValue == 0) {
                    mMaxFallSpeed = data_802f5a0c[5];
                }
                changeState(StateID_Propel, (void *) 1);
                return;
            }
            if (isNowBgCross(BGC_WATER_TOUCH)) {
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
                changeState(StateID_Walk, BLEND_DEFAULT);
                return;
            }
            break;
    }
    if (isStatus(STATUS_PRESS_ATTACH)) {
        offStatus(STATUS_PRESS_ATTACH);
        mPos.x = mPressAttachPos.x;
        mPos.y = mPressAttachPos.y;
    }
}

void dAcPy_c::finalizeState_SpinHipAttack() {
    m_2e8 = 0;
    offStatus(STATUS_SPIN);
    offStatus(STATUS_SPIN_HIP_ATTACK_FALL);
    offStatus(STATUS_SPIN_HIP_ATTACK_LANDED);
    offStatus(STATUS_SPIN_HIP_ATTACK_LANDING);
    offStatus(STATUS_A8);
    offStatus(STATUS_AA);
    offStatus(STATUS_9F);
    offStatus(STATUS_PRESS_ATTACH);
    offStatus(STATUS_CAN_LAND);
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
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_A1);
    if (!isNowBgCross(BGC_WATER_TOUCH)) {
        setWaterWalkFlag();
    }
    mSpeedMax.x = 0.0;
    mJumpCounter = 0;
    mSubstate = FALL_ACTION_1;
    setAddLiftSpeedF();
    if (isStatus(STATUS_ON_WATER_MOVE)) {
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
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_ON_WATER_MOVE);
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
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    setStartJumpEffect(0);
    _jumpSet(stateArg<jmpInf_c *>());
    if (mJumpCounter != 2) {
        onStatus(STATUS_9E);
    }
}

void dAcPy_c::finalizeState_Jump() {
    daPlBase_c::finalizeState_Jump();
    m_1058 = 0;
    mJumpComboTimer = 8;
    if (!isStatus(STATUS_61)) {
        onStatus(STATUS_61);
        calcJumpCount();
    }
    offStatus(STATUS_STAR_JUMP);
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
    offStatus(STATUS_CAN_LAND);
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

float dAcPy_c::getJumpSpeed() {
    float baseSpeed;
    float absSpeed = std::fabs(mSpeedF);
    if (absSpeed < daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues1[0]) {
        baseSpeed = daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues2[0];
    } else if (absSpeed < daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues1[1]) {
        baseSpeed = daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues2[1];
    } else if (absSpeed < daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues1[2]) {
        baseSpeed = daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues2[2];
    } else {
        baseSpeed = daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues2[3];
    }
    if (std::fabs(mSpeedF) >= getSpeedData()->mHighSpeed) {
        baseSpeed = daPlayerData_c::smc_POWER_CHANGE_DATA.mJumpSpeedValues2[3];
    }
    float jumpSpeed;
    if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            jumpSpeed = sc_JumpSpeedNuma2;
        } else {
            jumpSpeed = sc_JumpSpeedNuma1;
            if (mPos.y < mSinkSandHeight - 4.0f) {
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

void dAcPy_c::fn_80127740(int jumpType, AnmBlend_e blendMode) {
    mSubstate = JUMP_TAKE_OFF;
    fn_80145fd0(jumpType);
    if (isStatus(STATUS_YOSHI_DISMOUNT_JUMP)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_GET_DOWN, 0.0f, 0.0f);
    } else if (isStatus(STATUS_SPIN)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_ST);
    } else if (isStatus(STATUS_KANI_JUMP)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_W_JUMP1, 0.0f, 0.0f);
    } else if (isStatus(STATUS_STAR_JUMP) && !isCarry()) {
        mPyMdlMng.setAnm(PLAYER_ANIM_STAR_ROLL_DUPLICATE);
    } else if (isStatus(STATUS_SIT_JUMP)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_S_JUMP);
    } else {
        int anmNum;
        switch (mJumpCounter) {
            case 0:
                anmNum = PLAYER_ANIM_JUMP;
                break;
            case 1:
                startPlayerVoice(VOICE_JUMP_2ND, 0);
                anmNum = PLAYER_ANIM_2JMP_C_1;
                break;
            default:
                startPlayerVoice(VOICE_JUMP_3RD, 0);
                anmNum = PLAYER_ANIM_ROLL_JUMP;
                break;
        }
        if (!isCarry()) {
            if (mJumpCounter == 1) {
                mPyMdlMng.mpMdl->fn_800d5e00(1);
            } else {
                mPyMdlMng.mpMdl->fn_800d5e00(0);
            }
        }
        if (blendMode == BLEND_NONE) {
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
    if (isStatus(STATUS_STAR_JUMP)) {
        mSubstate = JUMP_AIR;
    } else if (isStatus(STATUS_YOSHI_DISMOUNT_JUMP)) {
        if (mPyMdlMng.isAnmStop()) {
            mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 3.0f, 0.0f);
            mSubstate = JUMP_AIR;
        }
    } else if (mSpeed.y < 0.0f) {
        if (isStatus(STATUS_KANI_JUMP)) {
            mPyMdlMng.setAnm(PLAYER_ANIM_W_JUMP2, 10.0f, 0.0f);
        } else if (isStatus(STATUS_SIT_JUMP)) {
            mPyMdlMng.setAnm(PLAYER_ANIM_S_JUMP2);
        } else if (mJumpCounter != 2) {
            /// @unofficial
            static const int l_AnmIDs[] = { PLAYER_ANIM_JUMP2, PLAYER_ANIM_2JMP_C_2, PLAYER_ANIM_2JUMP2 };
            mPyMdlMng.setAnm(l_AnmIDs[mJumpCounter]);
        }
        mSubstate = JUMP_AIR;
    }
    jumpExecAir();
}

void dAcPy_c::jumpExecAir() {
    if (isStatus(STATUS_STAR_JUMP) && !isStar()) {
        offStatus(STATUS_STAR_JUMP);
        mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 10.0f, 0.0f);
    }
    if (mJumpCounter == 2) {
        if (mSubstate == JUMP_AIR) {
            turnAngle();
            if (mDirection != m_12f4 || isNowBgCross(BGC_HEAD)) {
                float f = mPyMdlMng.getLastFrame();
                mPyMdlMng.setAnm(PLAYER_ANIM_ROLL_JUMP, 0.0f, f);
            }
        }
    } else if (!isStatus(STATUS_KANI_JUMP)) {
        turnAngle();
    }
    setJumpGravity();
    maxFallSpeedSet();
    if (isStatus(STATUS_TWIRL) && mSpeed.y < 0.0f) {
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
        isNowBgCross(BGC_FOOT) &&
        !checkSlip() &&
        !checkJumpTrigger() &&
        !checkCrouch()
    ) {
        mAngle.x = 0;
        if (isStatus(STATUS_SIT_JUMP)) {
            changeState(StateID_Crouch, CROUCH_ARG_FROM_SIT_JUMP);
            return;
        }
        if (
            mPowerup != POWERUP_MINI_MUSHROOM &&
            !isCarry() &&
            isStatus(STATUS_JUMP) &&
            isNowBgCross(BGC_GROUNDED_MOVE_UP)
        ) {
            changeState(StateID_Land, true);
            return;
        }
        if ((isStatus(STATUS_YOSHI_DISMOUNT_JUMP) || mJumpCounter == 2) && !mKey.buttonWalk(nullptr)) {
            mSpeedF = 0.0f;
        }
        setLandJumpEffect(0);
        if (mSpeedF) {
            fn_801282d0(BLEND_NONE);
            return;
        }
        if (isStatus(STATUS_KANI_JUMP)) {
            mPyMdlMng.setAnm(PLAYER_ANIM_2JUMPED_DUPLICATE, 10.0f, 0.0f);
        } else {
            if (mJumpCounter == 0) {
                mPyMdlMng.setAnm(PLAYER_ANIM_JUMPED);
            } else {
                mPyMdlMng.setAnm(PLAYER_ANIM_2JUMPED);
            }
        }
        changeState(StateID_Land, false);
    }
}

void dAcPy_c::setJumpCommonBase() {
    if (isStatus(STATUS_FOLLOW_MAME_KURIBO)) {
        int v = getFollowMameKuribo() - 1;
        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }
        /// @unofficial
        static const float speedMultiplier[] = { 0.84f, 0.81f, 0.78f, 0.75f, 0.72f };
        mSpeed.y *= speedMultiplier[v];
    }
    offNowBgCross(BGC_FOOT);
    setAddLiftSpeedF();
    setJumpGravity();
    maxFallSpeedSet();
}

void dAcPy_c::_jumpSet(jmpInf_c *jumpInf) {
    setWaterWalkFlag();
    mAngle.x = 0;
    offStatus(STATUS_88);
    if (isStar()) {
        onStatus(STATUS_STAR_JUMP);
    } else {
        offStatus(STATUS_STAR_JUMP);
    }
    if (isOnSinkSand()) {
        onStatus(STATUS_SINK_SAND_JUMP);
    }
    int jumpMode = 1;
    if (jumpInf != nullptr) {
        jumpMode = jumpInf->mJumpMode;
    }
    if (isStatus(STATUS_C0)) {
        jumpMode = 0;
    }
    if (
        isOnSinkSand() ||
        mJumpComboTimer == 0 ||
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
        if (jumpInf->mSpeed) {
            mSpeed.y = jumpInf->mSpeed;
        } else {
            setJumpSpeed();
        }
    } else {
        setJumpSpeed();
    }
    AnmBlend_e blendMode = BLEND_DEFAULT;
    if (jumpInf != nullptr) {
        blendMode = jumpInf->mBlendMode;
    }

    fn_80127740(jumpMode, blendMode);
    mAngle.y = getMukiAngle(mDirection);
    setJumpCommonBase();
}

void dAcPy_c::fn_801282d0(AnmBlend_e blendMode) {
    if (mKey.buttonWalk(nullptr) && mSpeedF * mMaxSpeedF < 0.0f && checkTurn()) {
        return;
    }
    changeState(StateID_Walk, blendMode);
}

void dAcPy_c::initializeState_Land() {
    if (mSubstate == LAND_ACTION_1) {
        mSpeedF = 0.0f;
        mPyMdlMng.setAnm(PLAYER_ANIM_2JUMPED, 2.0f, 0.0f, 0.0f);
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
        changeState(StateID_Walk, BLEND_DEFAULT);
        return;
    }
    if (mSubstate == LAND_ACTION_0) {
        if (!mSpeedF && !mPyMdlMng.isAnmStop()) {
            return;
        }
        fn_801282d0(BLEND_DEFAULT);
    } else {
        mSpeedF = 0.0f;
        if ((mKey.buttonWalk(nullptr) && mPyMdlMng.mpMdl->mAnm.getFrame() >= 10.0f) || mPyMdlMng.isAnmStop()) {
            fn_801282d0(BLEND_DEFAULT);
        }
    }
}

int dAcPy_c::checkWallSlideEnable(int dir) {
    if (
        !isNowBgCross(BGC_WATER_SHALLOW) &&
        mWallSlideCooldown == 0 &&
        !isStatus(STATUS_VINE) &&
        !isStatus(STATUS_A8) &&
        !isNowBgCross(BGC_37) &&
        !isNowBgCross(BGC_FOOT) &&
        !mIsBgDamage &&
        mBc.getWallAttr(dir) != 7 &&
        !isNowBgCross(BGC_HEAD) &&
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
    if (isStatus(STATUS_WALL_SLIDE)) {
        return false;
    }
    int dir;
    if (
        mKey.buttonWalk(&dir) &&
        !isNowBgCross(BGC_37) &&
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
    mPyMdlMng.setAnm(PLAYER_ANIM_WSLD, 0.0f, 0.0f);
    onStatus(STATUS_97);
    onStatus(STATUS_WALL_SLIDE);
    onStatus(STATUS_9F);
    onStatus(STATUS_CAN_LAND);
    mBc.mPlayerFlags |= 2;
    mAccelY = getGravityData()[0];
    mAngle.y = getMukiAngle(mDirection);
    mMaxSpeedF = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
}

void dAcPy_c::finalizeState_WallSlide() {
    offStatus(STATUS_WALL_SLIDE);
    offStatus(STATUS_9F);
    offStatus(STATUS_CAN_LAND);
    mBc.mPlayerFlags &= ~2;
    mAccelY = getGravityData()[0];
    mWallSlideCooldown = 4;
}

void dAcPy_c::setWallSlideEffect() {
    holdSound(SE_PLY_SLIP_TATE, 0);
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
        jnt.x = mPos.x + mWallBcData.mOffset / 4096.0f;
    } else {
        jnt.x = mPos.x - mWallBcData.mOffset / 4096.0f;
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
    dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, sc_wallSlidEffectID[idx], 0, &jnt, nullptr, nullptr);
}

void dAcPy_c::executeState_WallSlide() {
    if (isNowBgCross(BGC_FOOT)) {
        changeState(StateID_Walk, BLEND_NONE);
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
        mSubstateValue += 1;
        if (mSubstateValue >= 15) {
            changeState(StateID_Fall, false);
        }
    } else {
        mSubstateValue = 0;
    }
    if (checkWallSlideEnable(mDirection)) {
        changeState(StateID_Fall, false);
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
    offStatus(STATUS_A7);
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
            float thing = mWallBcData.mOffset / 4096.0f;
            mVec3_c pos(mPos.x - thing * sc_DirSpeed[dir], mPos.y, mPos.z);
            mVec3_c pos2(pos.x - sc_DirSpeed[mDirection] * 8.0f, pos.y, pos.z);
            if (!dBc_c::checkWall(&mPos, &pos2, nullptr, mLayer, mAmiLayer, nullptr)) {
                pos.y += 8.0f;
                float f;
                if (dBc_c::checkTenjou(&pos2, &f, mLayer, mAmiLayer) && f < mPos.y + getModelHeight()) {
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
    jmpInf_c jumpInf(f, 0, BLEND_DEFAULT);
    changeState(StateID_Jump, &jumpInf);
    startPlayerVoice(VOICE_WALL_KICK, 0);
    startSound(SE_PLY_KABE_KICK, false);
    mWallSlideCooldown = 16;
}

void dAcPy_c::setSpinHoldReq(float targetX) {
    onStatus(STATUS_IS_SPIN_HOLD_REQ);
    mSpinHoldReqTarget = targetX;
}

bool dAcPy_c::setSpinSpeed(float speedF, float speedY) {
    if (isState(StateID_SpinJump)) {
        if (isStatus(STATUS_IS_SPIN_HOLD_REQ) || m_15ae != 0) {
            return false;
        }
        mSpeed.y = speedY;
        mSpeedF = speedF;
        mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
        m_15ae = 30;
        mSpinTimer = 30;
        m_1048 = 0x2000;
        offNowBgCross(BGC_FOOT);
        dQuake_c::getInstance()->shockMotor(getPlrNo(), dQuake_c::TYPE_7, 0, false);
        return true;
    }
    return false;
}

void dAcPy_c::setSpinJumpEffect(int i) {
    if (!isStatus(STATUS_SPIN)) {
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
    if (PLAYER_YELLOW_TOAD <= mPlayerType && mPlayerType <= PLAYER_BLUE_TOAD) {
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
    float s = mPyMdlMng.getSomeScale2();
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
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_A1);
    onStatus(STATUS_SPIN);
    mSpinFireBallCooldown = 0;
    mSpinTimer = 30;
    m_1048 = 0x2000;
    setSpinJumpEffect(1);
    u32 substate = (u32) mStateArg;
    if (substate <= SPIN_JUMP_ACTION_1) {
        startSound(SE_PLY_SPIN_ATTACK, true);
    }
    if (isStatus(STATUS_IS_SPIN_HOLD_REQ)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_ST, 0.0f, 0.0f);
    } else {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
        jmpInf_c jumpInf(mSpeed.y, 0, BLEND_DEFAULT);
        if (substate == SPIN_JUMP_ACTION_0) {
            jumpInf.mSpeed = sc_JumpSpeed - 0.4f;
        }
        if (
            isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) &&
            (isNowBgCross(BGC_IN_SINK_SAND) || mPos.y < mSinkSandHeight - 4.0f)
        ) {
            jumpInf.mSpeed = sc_JumpSpeed - 0.7f;
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
    if (mSpinFireBallCooldown != 0) {
        mSpinFireBallCooldown--;
    }
    if (mSpinTimer != 0) {
        mSpinTimer--;
        if (mSpinFireBallCooldown == 0) {
            if (mSpinTimer == 21) {
                setSpinFireBall();
            } else if (mSpinTimer == 15) {
                m_12f4 ^= 1;
                setSpinFireBall();
                mSpinFireBallCooldown = 20;
            }
        }
    }
    if (isStatus(STATUS_IS_SPIN_HOLD_REQ)) {
        mKey.onStatus(dAcPyKey_c::STATUS_5);
        if (mKey.triggerShakeJump() && mSpinTimer < 10) {
            mSpinTimer = 30;
            m_1048 = 0x2000;
        }
        mSpeed.y = 0.0f;
        mSpeedF = 0.0f;
        mAccelY = 0.0f;
        mAccelF = 0.0f;
        sLib::chase(&mPos.x, mSpinHoldReqTarget, 1.0f);
        if (!checkJumpTrigger()) {
            if (
                mKey.buttonWalk(nullptr) &&
                (mKey.triggerRight() || mKey.triggerLeft())
            ) {
                changeState(StateID_Walk, BLEND_DEFAULT);
            } else {
                if (mSpinTimer == 0) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_END);
                    changeState(StateID_Land, false);
                }
            }
        }
    } else {
        setJumpGravity();
        maxFallSpeedSet();
        moveSpeedSet();
        airPowerSet();
        onStatus(STATUS_JUMP);
        if (isNowBgCross(BGC_HEAD)) {
            mSpinTimer = 0;
        }
        if (isNowBgCross(BGC_FOOT)) {
            setLandJumpEffect(1);
            mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_END, 0.0f, 0.0f);
            changeState(StateID_Land, false);
        }
    }
}

void dAcPy_c::finalizeState_SpinJump() {
    mSpinTimer = 0;
    mSpinFireBallCooldown = 0;
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9F);
    offStatus(STATUS_SPIN);
    offStatus(STATUS_92);
    offStatus(STATUS_A9);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_JUMP);
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
    if (mPyMdlMng.getAnm() == PLAYER_ANIM_STAR_ROLL_DUPLICATE) {
        return false;
    }
    if (mPyMdlMng.getAnm() == PLAYER_ANIM_ROLL_JUMP && !mPyMdlMng.isAnmStop()) {
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
    startSound(SE_PLY_SPIN_ONCE, true);
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
    onStatus(STATUS_TWIRL);
}

void dAcPy_c::initializeState_RideOffJump() {
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9E);
    onStatus(STATUS_YOSHI_DISMOUNT_JUMP);
    onStatus(STATUS_97);
    onStatus(STATUS_JUMP);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    mSubstateTimer = 30;
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mAngle.y = getMukiAngle(mDirection);
    mSpeedF = sc_DirSpeed[mDirection] * -1.0f;
    jmpInf_c jumpInf(2.85f, 0, BLEND_DEFAULT);
    _jumpSet(&jumpInf);
    startSound(SE_PLY_BREAK_FREE_YOSHI, false);
}

void dAcPy_c::finalizeState_RideOffJump() {
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_JUMP);
    offStatus(STATUS_48);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_88);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_YOSHI_DISMOUNT_JUMP);
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
        onStatus(STATUS_CAN_LAND);
    }
    jump_common();
}

void dAcPy_c::initializeState_SitJump() {
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9F);
    onStatus(STATUS_9E);
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_SIT_JUMP);
    onStatus(STATUS_A0);
    onStatus(STATUS_A1);
    m_12f4 = mDirection;
    mSpeedMax.x = 0.0f;
    if (!stateArg<bool>()) {
        _jumpSet(nullptr);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_S_JUMP2);
        mAngle.y = getMukiAngle(mDirection);
    }
}

void dAcPy_c::finalizeState_SitJump() {
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_JUMP);
    offStatus(STATUS_48);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_88);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_SIT_JUMP);
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
    onStatus(STATUS_JUMP);
    if (dScStage_c::m_instance->mCurrWorld == WORLD_5 && dScStage_c::m_instance->mCurrCourse == STAGE_GHOST_HOUSE) {
        if (isNowBgCross(BGC_BLOCK_HIT) && isOldBgCross(BGC_SEMISOLID)) {
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
            jmpInf_c jumpInf(mSpeed.y, 0, BLEND_DEFAULT);
            changeState(StateID_Jump, &jumpInf);
        }
    }
}

void dAcPy_c::initializeState_KaniJump() {
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_9F);
    onStatus(STATUS_9E);
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_KANI_JUMP);
    onStatus(STATUS_A2);
    jmpInf_c jumpInf(3.0f, 1, BLEND_DEFAULT);
    _jumpSet(&jumpInf);
    mSpeedMax.x = 0.0f;
    mAngle.y = 0;
}

void dAcPy_c::finalizeState_KaniJump() {
    mAngle.x = 0;
    offStatus(STATUS_JUMP);
    offStatus(STATUS_48);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_9E);
    offStatus(STATUS_9F);
    offStatus(STATUS_88);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_KANI_JUMP);
    offStatus(STATUS_A2);
}

void dAcPy_c::executeState_KaniJump() {
    gravitySet();
    maxFallSpeedSet();
    setKaniMoveSpeed(true);
    onStatus(STATUS_JUMP);
    jump_common();
    if (mPos.y < m_d8c - 4.0f) {
        changeState(StateID_Fall, false);
    }
}

void dAcPy_c::initializeState_CannonJump() {
    onStatus(STATUS_BD);
    onStatus(STATUS_CANNON_JUMP);
    onStatus(STATUS_88);
    onStatus(STATUS_7F);
    if (m_68 == 1) {
        onStatus(STATUS_7A);
    }
    clearNowBgCross();
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_5, 0, false);
    startPlayerVoice(VOICE_CANNON_SHOT_S, 0);
    mPyMdlMng.setAnm(PLAYER_ANIM_SHOOT);
    mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mAngle.y = getMukiAngle(mDirection);
    int ang = cM::atan2s(std::fabs(mSpeedF), mSpeed.y);
    mAngle.x = 0x4000 - ang;
    mSubstateValue = 0;
    m_12b4 = (u32) mStateArg;
    m_12a8 = mPos;
}

void dAcPy_c::finalizeState_CannonJump() {
    float data = getSpeedData()->mHighSpeed;
    if (mSpeedF > data) {
        mSpeedF = data;
    }
    if (mSpeedF < -data) {
        mSpeedF = -data;
    }
    offStatus(STATUS_7F);
    offStatus(STATUS_CANNON_JUMP);
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
                    offStatus(STATUS_7F);
                }
                offZPosSetNone();
            }
            int ang = cM::atan2s(std::fabs(mSpeedF), mSpeed.y);
            mAngle.x = 0x4000 - ang;
            if (isNowBgCross(BGC_HEAD) || isNowBgCross(BgCross1_e(BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
                mSpeed.y = 0.0f;
                changeState(StateID_Fall, false);
            } else {
                if (mSpeed.y <= 0.0f && std::fabs(mSpeedF) < 0.1f) {
                    changeState(StateID_Fall, false);
                }
                if (isNowBgCross(BGC_FOOT)) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_SHOOT_SLIP);
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
            if (isNowBgCross(BGC_FOOT)) {
                setCrouchSmokeEffect();
                if (!(std::fabs(mSpeedF) < 1.0f && checkJumpTrigger()) && std::fabs(mSpeedF) <= 0.1f) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_SHOOT_SLIP_END);
                    mSubstate = CANNON_JUMP_ACTION_2;
                    mSpeedF = 0.0f;
                }
            } else {
                mSubstate = CANNON_JUMP_ACTION_0;
                mPyMdlMng.setAnm(PLAYER_ANIM_SHOOT);
            }
            break;
        case CANNON_JUMP_ACTION_2:
            if (isNowBgCross(BGC_FOOT)) {
                if (checkJumpTrigger()) {
                    return;
                }
            } else {
                changeState(StateID_Fall, false);
                return;
            }
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, BLEND_DEFAULT);
            }
            break;
    }
}

void dAcPy_c::initializeState_BlockJump() {
    onStatus(STATUS_7F);
    onStatus(STATUS_7A);
    mSubstate = (int) mStateArg;
    if (mSubstate == BLOCK_JUMP_ACTION_0) {
        jmpInf_c jumpInf(sc_JumpSpeed, 0, BLEND_DEFAULT);
        _jumpSet(&jumpInf);
    } else {
        mSubstateTimer = 10;
    }
    mSpeedF = 0.5f;
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    if (isItemKinopio()) {
        startSound(SE_VOC_ITEM_KO_FOUND, false);
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
            changeState(StateID_Fall, false);
        }
        if (mSubstateTimer == 0) {
            offStatus(STATUS_7F);
            if (isNowBgCross(BGC_HEAD)) {
                changeState(StateID_Fall, false);
            }
        }
    } else if (mSubstateTimer == 0) {
        changeState(StateID_Fall, false);
    }
}

void dAcPy_c::setSlipAction() {
    mMoveSakaAngle = mBc.getSakaAngleBySpeed(mSpeedF);
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
    if (isNowBgCross(BGC_WATER_TOUCH) && mPowerup == POWERUP_MINI_MUSHROOM) {
        holdSound(SE_PLY_SLIP_W, 0);
    } else {
        daPlBase_c::setSlipSE();
    }
}

void dAcPy_c::setSlipEffect() {
    daPlBase_c::setSlipSmokeEffect();
    setSlipSE();
}

void dAcPy_c::slipActionMove(int mode) {
    if (isNowBgCross(BGC_FOOT) && std::fabs(mSpeedF) > 0.1f) {
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
    mPyMdlMng.setAnm(PLAYER_ANIM_STAR_ROLL_DUPLICATE);
    mMaxSpeedF = getSlipMaxSpeedF();
    mMaxFallSpeed = -3.0f;
    m_94 = 2.0f;
    mSubstateTimer = 4;
    mNoInteractTimer = 10;
    setInvalidKeyTimer_LR(10, 1);
    releaseCarryActor();
    onStatus(STATUS_32);
    onStatus(STATUS_97);
    onStatus(STATUS_CAN_LAND);
    onStatus(STATUS_31);
    onStatus(STATUS_88);
    onStatus(STATUS_JUMP_DAI_COOLDOWN);
    onStatus(STATUS_A1);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
}

void dAcPy_c::finalizeState_RollSlip() {
    offStatus(STATUS_32);
    offStatus(STATUS_9F);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_31);
    offStatus(STATUS_88);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);
    offStatus(STATUS_A1);
    mNoInteractTimer = 0;
    mTimer_a8 = 0;
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    if (std::fabs(mSpeedF) > 1.0f) {
        mSpeedF = sc_DirSpeed[mDirection];
    }
}

void dAcPy_c::executeState_RollSlip() {
    if (mSubstateTimer == 0) {
        offStatus(STATUS_JUMP_DAI_COOLDOWN);
    }
    gravitySet();
    moveSpeedSet();
    powerSet();
    turnAngle();
    if (mSubstateValue != 0 && mKey.triggerJump()) {
        setJump(sc_JumpSpeed, mSpeedF * 0.8f, true, 0, 1);
        return;
    }
    switch ((RollSlipSubstate_e) mSubstate) {
        case ROLL_SLIP_ACTION_0:
            holdSound(SE_PLY_MOVE_ROLLING, false);
            switch (mSubstateValue) {
                case 0:
                    mAccelF = 0.0f;
                    m_94 = 1.5f;
                    mPyMdlMng.mpMdl->setRate(m_94);
                    if (isNowBgCross(BGC_FOOT)) {
                        mSubstateValue = 1;
                        mSpeedF = sc_DirSpeed[mDirection];
                        mSpeed.y = 2.0f;
                        mPyMdlMng.mpMdl->setFrame(0.0f);
                    } else if (isNowBgCross(BgCross1_e(BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
                        mSubstateValue = 1;
                        mDirection = mDirection ^ 1;
                        mSpeedF = sc_DirSpeed[mDirection];
                        mSpeed.y = 0.0f;
                        mPyMdlMng.mpMdl->setFrame(0.0f);
                    }
                    break;
                case 1:
                    mPyMdlMng.mpMdl->setRate(0.5f);
                    if (isNowBgCross(BGC_FOOT)) {
                        if (isNowBgCross(BGC_FOOT)) {
                            if (!mKey.buttonCrouch()) {
                                setSlipAction_ToEnd();
                            } else {
                                setSlipAction_ToStoop();
                            }
                        }
                    }
                    break;
                case 2:
                    if (!isNowBgCross(BGC_FOOT)) {
                        changeState(StateID_Fall, false);
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
                changeState(StateID_Crouch, CROUCH_ARG_FROM_OTHER);
            }
            break;
        case ROLL_SLIP_ACTION_2:
            mSpeedF = 0.0f;
            mMaxSpeedF = 0.0f;
            if (mPyMdlMng.isAnmStop()) {
                changeActionSlipEnd(BLEND_DEFAULT);
            }
            break;
    }
}

void dAcPy_c::initializeState_PenguinSlide() {
    mSubstate = PENGUIN_SLIDE_ACTION_0;
    mMaxSpeedF = getSlipMaxSpeedF();
    if (isNowBgCross(BGC_FOOT) && !isSlipSaka()) {
        float f = std::fabs(mSpeedF) / getSpeedData()->mHighSpeed + 0.2f;
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
        onStatus(STATUS_CAN_WATER_SLIDE);
    }
    mSubstateTimer = 8;
    releaseCarryActor();
    onStatus(STATUS_30);
    onStatus(STATUS_97);
    onStatus(STATUS_9F);
    onStatus(STATUS_A2);
    onStatus(STATUS_PENGUIN_SLIDE);
    onStatus(STATUS_88);

    mPyMdlMng.setAnm(PLAYER_ANIM_P_SLIP);
    startPlayerVoice(VOICE_PNGN_SLIDE, 0);
    m_540 = 30;
    m_544 = getData(0);
}

void dAcPy_c::finalizeState_PenguinSlide() {
    offStatus(STATUS_30);
    offStatus(STATUS_9F);
    offStatus(STATUS_A2);
    offStatus(STATUS_PENGUIN_SLIDE);
    offStatus(STATUS_88);
    offStatus(STATUS_INITIAL_SLIDE);
    offStatus(STATUS_CAN_WATER_SLIDE);
    m_1598 = 0.0f;
    mAngle.x = 0;
    m_b98 = 10;
}

bool dAcPy_c::checkPenguinSlideJump() {
    if (isNowBgCross(BGC_FOOT) && mKey.triggerJump()) {
        int dir = -1;
        if (mKey.buttonDown() || (mKey.buttonWalk(&dir) && dir == mDirection)) {
            mPyMdlMng.setAnm(PLAYER_ANIM_P_SLIP_JUMP);
            startSound(SE_PLY_PNGN_JUMP, false);
            onStatus(STATUS_PENGUIN_SLIDE_JUMP);
            offNowBgCross(BGC_FOOT);
            float tmp = std::fabs(mSpeedF) - 1.5f;
            mSpeed.y = data_802f5a48[2] + tmp * data_802f5a48[3];
            if (mSpeed.y > data_802f5a48[1]) {
                mSpeed.y = data_802f5a48[1];
            }
            if (mSpeed.y < data_802f5a48[2]) {
                mSpeed.y = data_802f5a48[2];
            }
        } else {
            setJump(sc_JumpSpeed, mSpeedF * 0.8f, true, 0, 1);
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
    if (!isStatus(STATUS_CAN_WATER_SLIDE) && mPos.y >= mWaterHeight - 1.0f) {
        onStatus(STATUS_CAN_WATER_SLIDE);
    }
    if (isNowBgCross(BGC_ON_WATER_MOVE)) {
        mSpeed.y = sc_MaxFallSpeed_Foot;
    }
    if (m_544 != 0) {
        m_544--;
    }
    if (isNowBgCross(BGC_FOOT) && m_540 != 0) {
        m_540--;
    }
    s16 newAng = 0;
    if (isNowBgCross(BGC_FOOT)) {
        newAng = mBc.getSakaAngle(mDirection);
    }
    sLib::addCalcAngle(&mAngle.x.mAngle, newAng, 4, 0x2000, 0x40);
    if (isNowBgCross(BGC_FOOT)) {
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
        changeState(StateID_Walk, BLEND_NONE);
        return;
    }
    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
        mDirection = mDirection ^ 1;
    }
    int ang = turnAngle();
    switch ((PenguinSlideSubstate_e) mSubstate) {
        case PENGUIN_SLIDE_ACTION_0:
            if (mPlayerType == PLAYER_LUIGI) {
                m_1598 = -0.5f;
            } else {
                m_1598 = 1.0f;
            }
            if (!isStatus(STATUS_DEMO_NEXT_GOTO_BLOCK) && m_540 == 0) {
                if (!isNowBgCross(BGC_FOOT)) {
                    offStatus(STATUS_INITIAL_SLIDE);
                }
                if (!isStatus(STATUS_INITIAL_SLIDE) && (isNowBgCross(BGC_FOOT) || mSpeed.y < 0.0f)) {
                    int dir = -1;
                    if (!(mKey.buttonDown() || (mKey.buttonWalk(&dir) && dir == mDirection))) {
                        onStatus(STATUS_52);
                        changeState(StateID_Walk, BLEND_NONE);
                        return;
                    }
                }
            }
            if (isNowBgCross(BGC_FOOT)) {
                setPenguinSlideEffect();
                if (mPyMdlMng.getAnm() != PLAYER_ANIM_P_SLIP) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_P_SLIP);
                }
                if (isStatus(STATUS_PENGUIN_SLIDE_JUMP)) {
                    offStatus(STATUS_PENGUIN_SLIDE_JUMP);
                    setPenguinSlideLandEffect();
                }
            } else if (isStatus(STATUS_PENGUIN_SLIDE_JUMP)) {
                if (mSpeed.y < 0.0f && mPyMdlMng.getAnm() != PLAYER_ANIM_P_SLIP_JUMP2) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_P_SLIP_JUMP2);
                }
            }
            daPlBase_c::slipActionMove(ang);
            if (isNowBgCross(BGC_FOOT)) {
                if (mGroundType == GROUND_TYPE_WATER) {
                    holdSound(SE_PLY_PNGN_SLIP_SEA, std::fabs(mSpeedF), false);
                } else {
                    holdSound(SE_PLY_PNGN_SLIP, false);
                }
            }
            break;
        case PENGUIN_SLIDE_ACTION_1:
            mMaxSpeedF = 0.0f;
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Crouch, CROUCH_ARG_FROM_OTHER);
                return;
            }
            break;
        case PENGUIN_SLIDE_ACTION_2:
            if (mPyMdlMng.isAnmStop()) {
                changeActionSlipEnd(BLEND_DEFAULT);
                return;
            }
            break;
    }
    if (isNowBgCross(BGC_FOOT)) {
        if (isSlipSaka()) {
            if (mMaxSpeedF * mSpeedF > 0.0f) {
                mAccelF = 1.2f * mAccelF;
                m_544 = data_802f5a48[0];
            } else {
                m_548 = 0;
                if (getPowerChangeType(true) != POWER_CHANGE_NORMAL) {
                    mAccelF = 0.01f;
                }
            }
        } else {
            if (getPowerChangeType(true) != POWER_CHANGE_NORMAL) {
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
        if (isStatus(STATUS_FOLLOW_MAME_KURIBO) || std::fabs(mSpeedF) >= std::fabs(m_54c)) {
            m_548 = 0;
        } else {
            mAccelF = 0.1f;
            mMaxSpeedF = m_54c;
        }
    }
}

void dAcPy_c::setPenguinSlideEffect() {
    if (!isNowBgCross(BGC_FOOT)) {
        return;
    }
    mAng3_c ang(0, 0, 0);
    if (mDirection == 1) {
        ang.y = 0x8000;
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
        startSound(SE_PLY_SPLASH_SHALLOW, false);
    }
}

bool dAcPy_c::isWaitFrameCountMax() {
    return mWaitFrameCount >= 80;
}

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
    if (!isNowBgCross(BGC_FOOT)) {
        mSpeed.y = 0.0f;
        if (setDelayHelpJump()) {
            return true;
        }
        if (mAirWalkTimer == 0) {
            changeState(StateID_Fall, false);
            return true;
        }
    }
    if (isState(StateID_Walk) && mKey.buttonWalk(nullptr) && mDirection != mPrevDirection) {
        mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
        if (checkTurn()) {
            return true;
        }
    }
    if (isState(daPlBase_c::StateID_Walk) || isState(StateID_Land)) { // [why daPlBase_c?]
        if (checkCrouch()) {
            return true;
        }
    }
    return false;
}

void dAcPy_c::setWalkActionAnm(AnmBlend_e blendMode) {
    float speed = std::fabs(mSpeedF);
    float f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        if (speed <= getSpeedData()->mLowSpeed) {
            float tmp = speed * 2.0f * 1.45f;
            f = (tmp < 2.0f) ? 2.0f : tmp;
        } else {
            float tmp = speed * 1.5f * 1.45f;
            f = (tmp < 0.5f) ? 0.5f : tmp;
        }
    } else {
        if (speed <= getSpeedData()->mLowSpeed) {
            float tmp = speed * 2.0f;
            f = (tmp < 2.0f) ? 2.0f : tmp;
        } else {
            float tmp = speed * 1.5f;
            f = (tmp < 0.5f) ? 0.5f : tmp;
        }
    }
    float g = 4.0f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        if (speed <= getSpeedData()->mLowSpeed) {
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
    if (mPowerup != POWERUP_PENGUIN_SUIT && isNowBgCross(BGC_ON_ICE) && speed < getSpeedData()->mMediumSpeed) {
        float tmp = h * 8.0f;
        h = (tmp > g) ? g : tmp;
    }
    if (isNowBgCross(BgCross1_e(BGC_ON_SINK_SAND | BGC_IN_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            h *= 0.5f;
        } else {
            h *= 0.7f;
        }
    }
    setNormalWalkAnm(blendMode, h);
}

void dAcPy_c::setWaitActionAnm(AnmBlend_e blendMode) {
    if (dScStage_c::m_isStaffCredit && isStatus(STATUS_ENDING_DANCE_AUTO)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_ENDING_WAIT);
        return;
    }
    if (isStatus(STATUS_63)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_WAIT_DUPLICATE2);
        return;
    }
    /// @unofficial
    static const float scSakaCheckOffset[] = { 2.0f, 0.0f, -2.0f, 4.0f, 0.0f, -4.0f };
    s16 ang = mStillSakaAngle;
    int count = 0;
    float sum = 0.0f;
    for (int i = 0; i < 3; i++) {
        mVec3_c p(mPos.x + scSakaCheckOffset[3 + i], mPos.y + 5.0f, mPos.z);
        s16 tmpAng;
        float f;
        if (dBc_c::checkGroundAngle(&p, &f, &tmpAng, mLayer, mAmiLayer, -1, nullptr, 0) && std::fabs(f - mPos.y) < 8.0f) {
            count++;
            sum += tmpAng;
        }
    }
    if (count != 0) {
        ang = sum / count;
    }
    static const int scSlopeWaitAnm[] = { PLAYER_ANIM_SLOPE_WAIT_L, PLAYER_ANIM_SLOPE_WAIT_R };
    static const int scLongWaitAnm[] = { PLAYER_ANIM_WAIT_L, PLAYER_ANIM_WAIT_R };
    if (ang != 0 || m_80 != 0) {
        ang = -ang * sc_DirSpeed[mDirection];
        float f = ang * mAng::AngleToDegreeCoefficient + 70.0f;
        if (f < 0.0f) {
            f = 0.0f;
        }
        if (f > 140.0f) {
            f = 140.0f;
        }
        if (blendMode == BLEND_DEFAULT) {
            mPyMdlMng.setAnm(scSlopeWaitAnm[mDirection], f);
        } else {
            mPyMdlMng.setAnm(scSlopeWaitAnm[mDirection], 0.0f, f);
        }
        mPyMdlMng.mpMdl->m_17c |= 0x40;
        m_80 = 1;
    } else {
        if (blendMode == BLEND_DEFAULT) {
            mPyMdlMng.setAnm(scLongWaitAnm[mDirection]);
        } else {
            mPyMdlMng.setAnm(scLongWaitAnm[mDirection], 0.0f, 0.0f);
        }
    }
}

bool dAcPy_c::isIceSlipAnmPlay() {
    if (isNowBgCross(BGC_ON_ICE) && !mKey.buttonWalk(nullptr) && mPowerup != POWERUP_PENGUIN_SUIT) {
        return true;
    }
    return false;
}

void dAcPy_c::setNormalWalkAnm(AnmBlend_e blendMode, float speedRate) {
    if (isIceSlipAnmPlay()) {
        switch (mWalkAnmState) {
            case 0:
                if (isNowBgCross(BGC_LIFT) && std::fabs(mBc.mIceSpeed) > 1.0f || std::fabs(mSpeedF) > 0.82f) {
                    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
                        mPyMdlMng.setAnm(PLAYER_ANIM_ICE_SLIP_B);
                    } else {
                        mPyMdlMng.setAnm(PLAYER_ANIM_ICE_SLIP_F);
                    }
                    mWalkAnmState = 1;
                } else {
                    setWaitActionAnm(blendMode);
                }
                break;
            case 1:
                calcAccOnIceLift();
                if (mPyMdlMng.isAnmStop()) {
                    setWaitActionAnm(blendMode);
                    mWalkAnmState = 2;
                }
                break;
            case 2:
                calcAccOnIceLift();
                setWaitActionAnm(blendMode);
                break;
        }
        holdSound(SE_PLY_ICE_SLIP, std::fabs(mSpeedF), false);
    } else {
        mWalkAnmState = 0;
        onStatus(STATUS_62);
        int anmID;
        if (isCarry()) {
            anmID = (mPyMdlMng.mpMdl->m_17c & 0x4) ? PLAYER_ANIM_CARRY_P_WALK : PLAYER_ANIM_CARRY_WALK;
            blendMode = BLEND_NONE;
        } else {
            if (
                isNowBgCross(BGC_ON_BELT_L) && mSpeedF > 0.0f ||
                isNowBgCross(BGC_ON_BELT_R) && mSpeedF < 0.0f
            ) {
                anmID = PLAYER_ANIM_RUN;
            } else {
                float speed = std::fabs(mSpeedF);
                if (speed <= getSpeedData()->mLowSpeed) {
                    anmID = PLAYER_ANIM_RUN;
                } else if (speed < getSpeedData()->mHighSpeed) {
                    anmID = PLAYER_ANIM_B_DASH;
                } else {
                    anmID = PLAYER_ANIM_B_DASH2;
                }
            }
        }
        if (!mPyMdlMng.isAnm(anmID)) {
            float blendDuration = 0.0f;
            if (blendMode == BLEND_DEFAULT) {
                blendDuration = dPyMdlMng_c::getHIO(anmID).mBlendDuration;
            }
            mPyMdlMng.setAnm(anmID, speedRate, blendDuration, 0.0f);
        } else {
            mPyMdlMng.mpMdl->setRate(speedRate);
        }
    }
}

void dAcPy_c::walkActionInit_Wait(AnmBlend_e blendMode) {
    mSubstate = 0;
    mSubstateTimer = 150;
    mWaterWalkTimer = 8;
    setWaitActionAnm(blendMode);
}

void dAcPy_c::walkAction_Wait() {
    if (!isWaitFrameCountMax()) {
        mWaitFrameCount++;
    }
    if (mSpeedF) {
        walkActionInit_Move(BLEND_DEFAULT);
        return;
    }
    setWaitActionAnm(BLEND_NONE);
    if (isStatus(STATUS_63)) {
        mAngle.y.chase(0, 0x400);
    } else {
        turnAngle();
    }
    if (mSubstateTimer == 0) {
        m_15b2 = 8;
    }
    if (isStatus(STATUS_CAN_WATER_WALK)) {
        if (!sLib::calcTimer(&mWaterWalkTimer)) {
            offStatus(STATUS_CAN_WATER_WALK);
            m_15b6 = 30;
        }
    }
}

void dAcPy_c::walkActionInit_Move(AnmBlend_e blendMode) {
    mSubstate = 1;
    setWalkActionAnm(blendMode);
}

void dAcPy_c::walkAction_Move() {
    m_80 = 0;
    mWaitFrameCount = 0;
    turnAngle();
    setRunOnWaterEffect();
    daPlBase_c::walkAction_Move();
    if (m_88 == 0 && !isNowBgCross(BGC_WATER_TOUCH)) {
        m_88 = 1;
    }
    if (m_88 == 1) {
        setWaterWalkFlag();
    }
}

void dAcPy_c::initializeState_Walk() {
    AnmBlend_e blendMode = stateArg<AnmBlend_e>();
    calcSpeedOnIceLift();
    m_88 = 1;
    if (isOldBgCross(BGC_WATER_SHALLOW)) {
        m_88 = 0;
    }
    if (mSpeedF) {
        walkActionInit_Move(blendMode);
    } else {
        walkActionInit_Wait(blendMode);
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
    offStatus(STATUS_ON_WATER_MOVE);
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
    if (isNowBgCross(BGC_FOOT)) {
        if (isNowBgCross(BGC_ON_WATER_MOVE)) {
            onStatus(STATUS_ON_WATER_MOVE);
        } else {
            offStatus(STATUS_ON_WATER_MOVE);
        }
    }
    offStatus(STATUS_CAN_LAND);
    if (isOnSinkSand()) {
        onStatus(STATUS_CAN_LAND);
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
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_A3);
    mWalkAnmState = 0;
    if (mPyMdlMng.getFlags2() & 0x40) {
        mPyMdlMng.mpMdl->m_17c &= ~0x40;
        mPyMdlMng.mpMdl->releaseBodyAnm(0.0f);
    }
}

void dAcPy_c::setRunOnWaterEffect() {
    if (isNowBgCross(BGC_ON_WATER_MOVE)) {
        static const char *sc_waterRunEffectID[] = {
            "Wm_mr_waterrun_l_ss",
            "Wm_mr_waterrun_r_ss"
        };
        dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, sc_waterRunEffectID[mDirection], 0, &mPos, nullptr, nullptr);
    }
}

void dAcPy_c::initializeState_Turn() {
    onStatus(STATUS_9F);
    onStatus(STATUS_A2);
    mSubstate = 0;
    if (getPowerChangeType(false) == POWER_CHANGE_ICE) {
        mPyMdlMng.setAnm(PLAYER_ANIM_ICE_TURN, 0.0f, 0.0f);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_TURN, 0.0f, 0.0f);
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
    daPlBase_c::setTurnSmokeEffect();
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
                if (mDirection != mPrevDirection) {
                    setTurnEnd();
                } else if (mSubstateTimer == 0) {
                    fadeOutTurnEffect();
                    mSubstate = 2;
                    if (getPowerChangeType(false) == POWER_CHANGE_ICE) {
                        mPyMdlMng.setAnm(PLAYER_ANIM_ICE_TURNED, 0.0f, 0.0f);
                    } else {
                        mPyMdlMng.setAnm(PLAYER_ANIM_TURNED, 0.0f, 0.0f);
                    }
                }
                break;
            case 2:
                float maxSpeed = 0.0f;
                int dir;
                if (mKey.buttonWalk(&dir)) {
                    if (mKey.buttonDush()) {
                        maxSpeed = sc_DirSpeed[dir] * getSpeedData()->mHighSpeed;
                    } else {
                        maxSpeed = sc_DirSpeed[dir] * getSpeedData()->mLowSpeed;
                    }
                }
                mMaxSpeedF = maxSpeed;
                if (mDirection != mPrevDirection || mPyMdlMng.isAnmStop()) {
                    setTurnEnd();
                }
        }
    }
}

bool dAcPy_c::checkCrouch() {
    if (
        !isStatus(STATUS_51) &&
        !isStatus(STATUS_SIT_JUMP) &&
        !isOnSinkSand() &&
        !isStatus(STATUS_BE)
    ) {
        if (isCarry()) {
            return false;
        }
        if (mKey.buttonCrouch()) {
            if (mPowerup == POWERUP_PENGUIN_SUIT && isStatus(STATUS_CAN_PENGUIN_SLIDE)) {
                changeState(StateID_PenguinSlide, 0);
                return true;
            }
            changeState(StateID_Crouch, CROUCH_ARG_FROM_WALK);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setCrouchSmokeEffect() {
    if (!mSpeedF) {
        return;
    }
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
        mVec3_c efPos;
        mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
        setBrakeSmokeEffect(efPos);
    }
    setSlipSE();
}

void dAcPy_c::initializeState_Crouch() {
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
        mSubstate = CROUCH_GROUND;
        onStatus(STATUS_9F);
    } else {
        mSubstate = CROUCH_WATER;
        onStatus(STATUS_AA);
    }
    switch (stateArg<CrouchArg_e>()) {
        case CROUCH_ARG_FROM_WALK:
            if (!isNowBgCross(BGC_WATER_SHALLOW)) {
                mPyMdlMng.setAnm(PLAYER_ANIM_STOOP_START);
            } else {
                mPyMdlMng.setAnm(PLAYER_ANIM_STOOP_START, dPyMdlBase_c::scWaterCrouchAnmSpeed, 3.0f, 0.0f);
            }
            break;
        case CROUCH_ARG_FROM_OTHER:
            mPyMdlMng.setAnm(PLAYER_ANIM_STOOP, 3.0f, 0.0f);
            break;
        case CROUCH_ARG_FROM_SIT_JUMP:
            mPyMdlMng.setAnm(PLAYER_ANIM_S_JUMPED);
            break;
    }
    mAngle.y = getMukiAngle(mDirection);
    startSound(SE_PLY_QUAT, true);
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
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_A2);
    offStatus(STATUS_A3);
    m_2e8 = 0;
}

void dAcPy_c::CrouchActionGround() {
    if (isOnSinkSand() && setCancelCrouch()) {
        return;
    }
    if (!setCrouchJump() && !checkCrouchSlip()) {
        if (!isNowBgCross(BGC_FOOT)) {
            mSpeed.y = 0.0f;
            changeState(StateID_SitJump, true);
        } else {
            setCrouchSmokeEffect();
            if (mKey.buttonCrouch() || !setCancelCrouch()) {
                offStatus(STATUS_CAN_WATER_WALK);
            }
        }
    }
}

void dAcPy_c::CrouchActionWater() {
    if (mKey.triggerJump()) {
        if (!isNowBgCross(BGC_WATER_SUBMERGED)) {
            setCrouchJump();
            onStatus(STATUS_AA);
        } else {
            if (!checkStandUpRoofOnLift()) {
                changeState(StateID_Swim, SWIM_ARG_INITIAL);
                setWaterGroundJump();
            }
        }
    } else {
        if (!isNowBgCross(BGC_FOOT)) {
            changeState(StateID_Swim, SWIM_ARG_INITIAL);
        } else {
            if (!mKey.buttonCrouch() && setCancelCrouch()) {
                changeState(StateID_Swim, SWIM_ARG_INITIAL);
            }
        }
    }
}

void dAcPy_c::executeState_Crouch() {
    gravitySet();
    maxFallSpeedSet();
    if (isNowBgCross(BGC_FOOT)) {
        mMaxSpeedF = 0.0f;
        offStatus(STATUS_CAN_LAND);
    } else {
        simpleMoveSpeedSet();
        onStatus(STATUS_CAN_LAND);
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
    if (anmID == PLAYER_ANIM_STOOP_START || anmID == PLAYER_ANIM_S_JUMPED) {
        if (mPyMdlMng.isAnmStop()) {
            mPyMdlMng.setAnm(PLAYER_ANIM_STOOP);
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
        !spinLiftUp(actor, false)
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
        !isStatus(STATUS_OUT_OF_PLAY) &&
        !isStatus(STATUS_STUNNED) &&
        !isStatus(STATUS_QUAKE)
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
    float _unused = mWallBcData.mOffset;
    float height;
    float f = 0.0f;
    if (dBc_c::checkTenjou(&pos, &height, mLayer, 1) && f > height) {
        f = height;
    }
    m_12f8 = f;
}

bool dAcPy_c::spinLiftUp(dActor_c *actor, bool b) {
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
        mPyMdlMng.mpMdl->setBodyAnm(PLAYER_ANIM_RCARRY_WAIT, 1.0f, 0.0f, 0.0f);
        startPlayerVoice(VOICE_MOTIAGE, 0);
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
    mPos.x += sc_DirSpeed[mDirection] * 5.0f;

    *(const dAcPy_c **) &carrier = nullptr; // [nice fake match]

    changeState(StateID_Fall, false);
    onStatus(STATUS_JUMP_DAI_COOLDOWN);
}

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

void dAcPy_c::initializeThrowCommonBase() {
    startPlayerVoice(VOICE_NAGERU, 0);
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_THROW);
    } else {
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
        if (actor->mKind == STAGE_ACTOR_PLAYER) {
            mPyMdlMng.setAnm(PLAYER_ANIM_CARRY_P_THROW, 0.0f, 0.0f);
        } else {
            mPyMdlMng.setAnm(PLAYER_ANIM_CARRY_THROW, 0.0f, 0.0f);
        }
    }
    mAngle.y = getMukiAngle(mDirection);
    onStatus(STATUS_THROW);
}

void dAcPy_c::initializeThrowCommon() {
    initializeThrowCommonBase();
    onStatus(STATUS_AA);
    onStatus(STATUS_97);
}

void dAcPy_c::finalizeThrowCommonBase() {
    offStatus(STATUS_THROW);
}

void dAcPy_c::finalizeThrowCommon() {
    finalizeThrowCommonBase();
    if (mSubstate == 0) {
        releaseCarryActor();
    }
    offStatus(STATUS_AA);
    offStatus(STATUS_97);
    offStatus(STATUS_92);
    offStatus(STATUS_CAN_LAND);
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

            float thing = sc_DirSpeed[mDirection] * std::fabs(getWallBgPointData()->mOffset / 4096.0f);
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
                float offsX = std::fabs(getHeadBgPointData()->mOffset / 4096.0f);
                // float tmp = hitX - offsX;
                if (pos.y > hitX - offsX) {
                    pos.y = hitX - offsX;
                    player->mSpeed.y = 0.0f;
                }
            }
            player->mPos = pos;
            player->mLastPos = player->mPos;
            calcDispSideLimit();
        } else {
            actor->mCarryingFlags |= CARRY_RELEASE | CARRY_THROW;
            actor->mThrowDirection = mDirection;
        }
    }
    cancelCarry(actor);
}

void dAcPy_c::executeThrowCommon() {
    if (isNowBgCross(BGC_FOOT)) {
        offStatus(STATUS_CAN_LAND);
    } else {
        onStatus(STATUS_CAN_LAND);
    }
    switch ((ThrowSubstate_e) mSubstate) {
        case THROW_ACTION_0:
            if (mPyMdlMng.mpMdl->mCurrAnmID == PLAYER_ANIM_SWIM_THROW) {
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(12.0f)) {
                    mSubstate = THROW_ACTION_2;
                    setThrowActor();
                }
            } else {
                if (isNowBgCross(BGC_FOOT) && mKey.triggerJump()) {
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
            if (isNowBgCross(BGC_FOOT) && checkJumpTrigger()) {
                break;
            }
            if (mPyMdlMng.mpMdl->mAnm.getFrame() >= 20.0f) {
                if (isNowBgCross(BGC_FOOT)) {
                    if (mSpeedF) {
                        changeState(StateID_Walk, BLEND_DEFAULT);
                        break;
                    }
                } else {
                    changeState(StateID_Fall, false);
                }
            }
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, BLEND_DEFAULT);
            }
            break;
        case THROW_ACTION_2:
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Swim, SWIM_ARG_INITIAL);
            }
            break;
    }
}

void dAcPy_c::initializeState_Throw() {
    initializeThrowCommon();
}

void dAcPy_c::finalizeState_Throw() {
    finalizeThrowCommon();
}

void dAcPy_c::executeState_Throw() {
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
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
    if (!isNowBgCross(BGC_FOOT) && mKey.buttonJump()) {
        mAccelY = getSomeData(7);
        mMaxFallSpeed = getSomeData(5);
    } else {
        mAccelY = getGravityData()[0];
        mMaxFallSpeed = sc_MaxFallSpeed;
    }
    mAccelF = getSpeedData()->mPowerChangeNormal.mSlowAccel;
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
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_QUAKE)
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

    dQuake_c::getInstance()->shockMotor(getPlrNo(), dQuake_c::TYPE_7, 0, false);
}

void dAcPy_c::initializeState_LiftUp() {
    offStatus(STATUS_8A);
    onStatus(STATUS_97);
    onStatus(STATUS_46);
    m_1304 = 0.0f;
    dAcPy_c *carryPlayer = getCarryPlayer();
    setVirusStar(carryPlayer);
    if (carryPlayer != nullptr) {
        startPlayerVoice(VOICE_MOTIAGE_PLAYER, 0);
    } else {
        startPlayerVoice(VOICE_MOTIAGE, 0);
    }
    startSound(SE_PLY_OTHER_ON, false);
    mPyMdlMng.setAnm(PLAYER_ANIM_CARRY_P_START, 0.0f, 0.0f);
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
            sBcPointData d1 = mWallBcData;
            sBcPointData d2 = mHeadBcData;
            fn_80143060(d1, d2, true);
            if (!fn_80143220(d1, d2)) {
                return;
            }
            onStatus(STATUS_48);
            return;
        }
    }
    if (mPyMdlMng.isAnmStop()) {
        changeState(StateID_Walk, BLEND_DEFAULT);
    }
}

void dAcPy_c::calcUzuSwimSpeed(float f1, float f2, float *fOut) {
    if (!isStatus(STATUS_PENGUIN_SWIM)) {
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
            if (isNowBgCross(BGC_FOOT)) {
                setUzuSwimAction();
            }
            if (isNowBgCross(BGC_HEAD)) {
                return;
            }
        }
        calcUzuSwimSpeed(mUzuSwimSpeed.y, mSpeed.y, &f);
        if (f > 0.0f) {
            m_b88 = 20;
        }
        if (f * mUzuSwimSpeed.y < 0.0f) {
            onStatus(STATUS_SWIM_AGAINST_JET_V);
        }
        mSpeed.y += f;
    }
}

void dAcPy_c::setUzuSpeedF(float f) {
    if (std::fabs(mSpeedF) < 2.0f) {
        calcUzuSwimSpeed(mUzuSwimSpeed.x, mSpeedF, &f);
        if (f * mUzuSwimSpeed.x < 0.0f) {
            onStatus(STATUS_SWIM_AGAINST_JET_H);
        }
        mSpeedF += f;
    }
}

bool dAcPy_c::setUzuSwimAction() {
    if (isNowBgCross(BGC_WATER_SHALLOW) && isState(StateID_Swim) && mSubstate == 1) {
        setSwimAction_Swim(BLEND_DEFAULT);
        return true;
    }
    return false;
}

bool dAcPy_c::setSwimAction() {
    if (isStatus(STATUS_45) || isStatus(STATUS_AA) || isStatus(STATUS_SWIM)) {
        return false;
    }
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        if (isOldBgCross(BGC_WATER_SHALLOW)) {
            changeState(StateID_Swim, SWIM_ARG_INITIAL);
        } else if (isStatus(STATUS_FIREBALL_PREPARE_SHOOT)) {
            changeState(StateID_Swim, SWIM_ARG_FIREBALL);
        } else {
            changeState(StateID_Swim, SWIM_ARG_ENTERING);
        }
        return true;
    }
    return false;
}

void dAcPy_c::setWaterInEffect() {
    dScStage_c *stage = dScStage_c::m_instance;
    bool bigSplash = false;

    if (isNowBgCross(BGC_WATER_BUBBLE)) {
        startSound(SE_PLY_SPLASH_GEL, false);
        mAng3_c efAng(0, 0, -mAirWaterHitAngle);
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_wfloatsplash", 0, &mAirWaterHitPos, &efAng, nullptr);
        return;
    }
    if (
        stage->mCurrWorld == WORLD_4 &&
        stage->mCurrCourse == STAGE_CASTLE &&
        stage->mCurrFile == 1
    ) {
        bigSplash = true;
    }
    mVec3_c efPos(mPos.x, mWaterHeight, mPos.z);
    if (bigSplash || mWaterDepth == 3) {
        float f = 1.0f;
        u32 i;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            startSound(SE_PLY_SPLASH_MAME, false);
            i = 3;
        } else {
            startSound(SE_PLY_SPLASH, false);
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
            startSound(SE_PLY_SPLASH_SHALLOW_MAME, false);
        } else {
            startSound(SE_PLY_SPLASH_SHALLOW, false);
        }
        dBg_c::m_bg_p->setWaterInWave(efPos.x, efPos.y, 0);
    }
}

void dAcPy_c::setWaterOutEffect() {
    if (isOldBgCross(BGC_WATER_BUBBLE)) {
        startSound(SE_PLY_SPLASH_GEL_OUT, false);
        mAng3_c efAng(0, 0, -mAirWaterHitAngle);
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_wfloatsplash", 0, &mAirWaterHitPos, &efAng, nullptr);
        return;
    }
    mVec3_c efPos(mPos.x, mPrevWaterHeight, mPos.z);

    if (mWaterDepth == 3) {
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
        startSound(SE_PLY_SPLASH_OUT, false);
    } else {
        float f = 1.0f;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            f = 0.6f;
        } else if (mPowerup == POWERUP_NONE) {
            f = 0.8f;
        }
        dPyEffectMng_c::mspInstance->fn_800d2de0(f, 4, efPos, mLayer);
        startSound(SE_PLY_SPLASH_SHALLOW_OUT, false);
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
        efAng.y = 0x8000;
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
    onStatus(STATUS_SWIM);
    onStatus(STATUS_AA);
    onStatus(STATUS_A8);
    onStatus(STATUS_92);
    offStatus(STATUS_CAN_WATER_WALK);
    SwimArg_e param = stateArg<SwimArg_e>();
    m_b84 = 0;
    m_b88 = 0;
    m_b80 = 0.0f;
    if (param == SWIM_ARG_ENTERING || param == SWIM_ARG_FIREBALL || param == SWIM_ARG_CLIFF_HANG) {
        m_b80 = 1;
        m_15b6 = 30;
        if (mSpeed.y < -1.5f) {
            mSpeed.y = -1.5f;
        }
        if (m_b98 != 0 && mSpeed.y < -0.5f) {
            mSpeed.y = -0.5f;
        }
    }
    if (param == SWIM_ARG_FIREBALL) {
        setInitSwimAction_FireBall();
    } else {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr) {
            mVec3_c pos(mPos.x, mPos.y + 4.0f, mPos.z);
            float height;
            if (dBc_c::checkTenjou(&pos, &height, mLayer, mAmiLayer)) {
                if (mPos.y + getModelHeight() + getBgPointData_Powerup(mPowerup, 1)->mHead.mOffset / 4096.0f > height) {
                    releaseCarryActor();
                }
            }
        }
        AnmBlend_e blendMode = BLEND_DEFAULT;
        if (param == SWIM_ARG_CLIFF_HANG) {
            blendMode = BLEND_NONE;
        }
        if (mPowerup == POWERUP_PENGUIN_SUIT) {
            if (!isCarry()) {
                setSwimAction_Penguin(blendMode);
            } else {
                setSwimAction_Swim(blendMode);
            }
        } else if (isNowBgCross(BGC_FOOT)) {
            setSwimAction_Walk(blendMode);
        } else {
            setSwimAction_Swim(blendMode);
        }
    }
    if (isNowBgCross(BGC_WATER_SHALLOW) && isOldBgCross(BGC_WATER_SHALLOW) == 0) {
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
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
        if (!isNowBgCross(BGC_WATER_TOUCH) && (!isCarry() || mPowerup != POWERUP_MINI_MUSHROOM)) {
            if (mSubstate == SWIM_ACTION_3 && m_b89 == 0) {
                createFireBall(0);
            }
            changeState(StateID_Fall, false);
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
            jmpInf_c jmpInf(h, 1, BLEND_DEFAULT);
            if (mSubstate == SWIM_ACTION_2) {
                jmpInf.mBlendMode = BLEND_NONE;
            }
            changeState(StateID_Jump, &jmpInf);
            mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
            onStatus(STATUS_A8);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setSwimAction_Swim(AnmBlend_e blendMode) {
    m_b89 = 0;
    mSubstate = SWIM_ACTION_0;
    m_b84 = 4;
    m_b88 = 0;
    if (blendMode == BLEND_DEFAULT) {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_WAIT);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_WAIT, 0.0f, 0.0f);
    }
    mAngle.x = 0;
}

void dAcPy_c::SwimAction_Swim() {
    if (mPowerup == POWERUP_PENGUIN_SUIT && !isCarry()) {
        setSwimAction_Penguin(BLEND_DEFAULT);
        return;
    }
    setWaterMoveSpeed();
    turnAngle();
    if (checkEnableThrow()) {
        startPlayerVoice(VOICE_NAGERU, 0);
        setThrowActor();
        return;
    }
    if (setWaterSurfaceJump()) {
        return;
    }
    if (isNowBgCross(BGC_FOOT) && mPowerup != POWERUP_PENGUIN_SUIT) {
        setSwimAction_Walk(BLEND_DEFAULT);
        return;
    }
    setWaterSurfaceSwimEffect();
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        if (mKey.triggerJump()) {
            startSound(SE_PLY_SWIM, 0);
            mSpeed.y += 1.0f;
            if (mSpeed.y > 1.8f) {
                mSpeed.y = 1.8f;
            }
        }
        if (isNowBgCross(BGC_NON_BREAK_BLOCK_HIT)) {
            mSpeed.y = -1.875f;
        }
    }
    switch (m_b89) {
        case 0:
            if (mKey.triggerJump()) {
                m_b89 = 1;
                mPyMdlMng.setAnm(PLAYER_ANIM_PADDLE_1);
                onStatus(STATUS_40);
            } else if (mKey.buttonJump()) {
                m_b89 = 3;
                mPyMdlMng.setAnm(PLAYER_ANIM_SWIM);
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
                    mPyMdlMng.setAnm(PLAYER_ANIM_SWIM);
                } else {
                    m_b89 = 2;
                    mPyMdlMng.setAnm(PLAYER_ANIM_PADDLE_2);
                }
            }
            break;
        case 3:
            mPyMdlMng.mpMdl->setRate(1.0f);
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(9.0f)) {
                startSound(SE_PLY_SWIM_KICK, 0);
            }
            setFlutterKickEffect();
            if (!mKey.buttonJump()) {
                m_b89 = 2;
                mPyMdlMng.setAnm(PLAYER_ANIM_PADDLE_2);
            }
            break;
        case 2:
            if (mPyMdlMng.isAnmStop()) {
                m_b89 = 0;
                mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_WAIT);
            }
            break;
        case 4:
            if (!isCarry()) {
                m_b89 = 0;
                mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_WAIT);
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
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_STANDING);
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
    mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_WALK, f, 10.0f, 0.0f);
}

void dAcPy_c::setSwimAction_Walk(AnmBlend_e blendMode) {
    if (!isOldBgCross(BGC_FOOT)) {
        setSeaLandSmokeEffect();
    }
    m_b89 = 0;
    mSubstate = SWIM_ACTION_1;
    if (blendMode == BLEND_DEFAULT) {
        setSwimActionWalkAnm();
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_STANDING);
    }
}

void dAcPy_c::setWaterGroundJump() {
    mSpeed.y = sc_WaterJumpSpeed;
    setSwimAction_Swim(BLEND_DEFAULT);
    startSound(SE_PLY_SWIM, 0);
    m_b88 = 20;
}

void dAcPy_c::SwimAction_Walk() {
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        setSwimAction_Penguin(BLEND_DEFAULT);
        return;
    }
    setWaterMoveSpeed();
    turnAngle();
    if (mKey.triggerJump()) {
        if (!setWaterSurfaceJump()) {
            setWaterGroundJump();
        }
    } else {
        if (!isNowBgCross(BGC_FOOT)) {
            setSwimAction_Swim(BLEND_DEFAULT);
        } else {
            if (!checkCrouch()) {
                setSwimActionWalkAnm();
            }
        }
    }
}

s16 dAcPy_c::getPenguinSwinAngleX() {
    float f = std::fabs(mUzuSwimSpeed.x);
    if (f < 0.1f) {
        f = 0.1f;
    }
    float cos = mUzuSwimSpeed.y;
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
    return -(cM::atan2s(f, mUzuSwimSpeed.y) - 0x4000);
}

void dAcPy_c::setSwimAction_Penguin(AnmBlend_e blendMode) {
    mSubstate = SWIM_ACTION_2;
    m_b88 = 0;
    if (m_b80 != 0 || getOldState() == StateID_HipAttack) {
        m_b80 = 0;
        if (blendMode == BLEND_DEFAULT) {
            mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM);
        } else {
            mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM, 0.0f, 0.0f);
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
    if (blendMode == BLEND_DEFAULT) {
        mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM, 0.0f, 0.0f);
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
            if (isStatus(STATUS_SWIM_AGAINST_JET_V)) {
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
    mUzuSwimSpeed.y = target;
    float offs = mPos.y + getModelHeight() / 2.0f;
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
        if (isStatus(STATUS_SWIM_AGAINST_JET_H)) {
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
    mUzuSwimSpeed.x = mAng(ang).cos() * f2 * sc_DirSpeed[mDirection];
    float f4 = 0.0f;
    if (dWaterEntryMng_c::m_instance->m_8c0) {
        f4 = 8.25f;
    }
    float f5 = mUzuSwimSpeed.x + f4 * 0.3f;
    float f10 = f3 + f4 * 0.005f;
    if (mUzuSwimSpeed.x * f4 < 0.0f) {
        if (i != 0) {
            f5 = mUzuSwimSpeed.x + f4 * 0.1f;
            f10 = f3 + f4 * 0.0001f;
        } else if (b) {
            f5 = mUzuSwimSpeed.x + f4 * 0.13f;
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
        mPyMdlMng.setAnm(PLAYER_ANIM_P_PADDLE_1);
        startSound(SE_PLY_PNGN_SWIM, false);
        onStatus(STATUS_40);
        return true;
    }
    return false;
}

void dAcPy_c::SwimAction_Penguin() {
    if (mPowerup != POWERUP_PENGUIN_SUIT || isCarry()) {
        setSwimAction_Swim(BLEND_DEFAULT);
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
                mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM);
            }
            break;
        case 1: {
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(9.0f)) {
                startSound(SE_PLY_SWIM_KICK_PENGUIN, false);
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
                mPyMdlMng.setAnm(PLAYER_ANIM_P_PADDLE_2);
            }
            break;
        case 3:
            if (setPenguinPaddleSwim()) {
                return;
            }
            if (mPyMdlMng.isAnmStop()) {
                m_b8c = 1.0f;
                m_b89 = 1;
                mPyMdlMng.setAnm(PLAYER_ANIM_P_SWIM);
            }
            break;
    }
    onStatus(STATUS_PENGUIN_SWIM);
    setPenWaterMoveSpeed(moveMode);
}

void dAcPy_c::setInitSwimAction_FireBall() {
    mSubstate = SWIM_ACTION_3;
    m_b89 = 1;
    createFireBall(0);
    mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_FIRE_AT, dPyMdlBase_c::scFireShootFrame);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
}

void dAcPy_c::setSwimAction_FireBall() {
    mSubstate = SWIM_ACTION_3;
    m_b89 = 0;
    if (mAngle.x > 0x1800) {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_FIRE_AT2);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_SWIM_FIRE_AT);
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
        onStatus(STATUS_PENGUIN_SWIM);
        setPenWaterMoveSpeed(0);
    }
    switch (m_b89) {
        case 0:
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(dPyMdlBase_c::scFireShootFrame)) {
                createFireBall(0);
                m_b89 = 1;
            }
            break;
        case 1:
            if (checkSetFireBall()) {
                setSwimAction_FireBall();
            } else if (mPyMdlMng.isAnmStop()) {
                if (mPowerup == POWERUP_PENGUIN_SUIT) {
                    setSwimAction_Penguin(BLEND_DEFAULT);
                } else if (isNowBgCross(BGC_FOOT)) {
                    setSwimAction_Walk(BLEND_DEFAULT);
                } else {
                    setSwimAction_Swim(BLEND_DEFAULT);
                }
            }
            break;
    }
}

void dAcPy_c::setWaterMoveSpeed() {
    float targetY = -1.5f;
    float accelY = getGravityData()[0];
    if (isCarry()) {
        float posY = mPos.y + getModelHeight() / 2.0f;
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
        if (isNowBgCross(BGC_WATER_SHALLOW)) {
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
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAccelY = 0.0f;
    float maxSpeedF;
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        if (isNowBgCross(BGC_FOOT)) {
            maxSpeedF = 0.84375f;
        } else {
            maxSpeedF = 1.6875f;
        }
    } else if (isNowBgCross(BGC_FOOT)) {
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
                mAccelF = 0.0625f;
            } else {
                mAccelF = 0.0546875f;
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
                if (isNowBgCross(BGC_FOOT)) {
                    mAccelF = 0.05f;
                } else {
                    mAccelF = 0.017f;
                }
            } else {
                mAccelF = 0.025f;
            }
        } else if (absSpeedF > std::fabs(targetF)) {
            if (isNowBgCross(BGC_FOOT)) {
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
        mAccelF = std::fabs(sc_DirSpeed[mDirection] * mAccelF + dWaterEntryMng_c::m_instance->get_8c0() * 0.003f);
    }
}

void dAcPy_c::executeState_Swim() {
    if (isStatus(STATUS_ENEMY_STAGE_CLEAR) && isNowBgCross(BGC_FOOT)) {
        offNowBgCross(BGC_FOOT);
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
    if (!isNowBgCross(BGC_WATER_SHALLOW) && isNowBgCross(BGC_FOOT)) {
        changeState(StateID_Walk, BLEND_NONE);
    } else {
        if (mSubstate != SWIM_ACTION_3 && checkSetFireBall()) {
            setSwimAction_FireBall();
        }
        offStatus(STATUS_PENGUIN_SWIM);
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
    if (!isStatus(STATUS_PENGUIN_SWIM)) {
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
    const sBcPointData *bgPointData = getWallBgPointData();
    if (bgPointData != nullptr) {
        float bgOffs[2];
        bgOffs[0] = bgPointData->mInfMargin / 4096.0f;
        bgOffs[1] = bgPointData->mSupMargin / 4096.0f;
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
    offStatus(STATUS_SWIM);
    if (isStatus(STATUS_PENGUIN_SWIM)) {
        float groundHeight;
        mVec3_c checkPos(mPos.x, mPos.y - 5.0f, mPos.z);
        if (dBc_c::checkGround(&mPos, &groundHeight, mLayer, 1, -1)) {
            if (mPos.y <= groundHeight) {
                mPos.y = groundHeight;
                mLastPos.y = mPos.y;
            }
        }
        offStatus(STATUS_PENGUIN_SWIM);
    }
    offStatus(STATUS_97);
    offStatus(STATUS_AA);
    offStatus(STATUS_A8);
    offStatus(STATUS_92);
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
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
        if (mSpeed.y <= 0.0f && mKey.buttonUp() && isNowBgCross(BGC_VINE_TOUCH_U)) {
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
    if (!(mPlayerType != PLAYER_MARIO && mPlayerType != PLAYER_LUIGI)) {
        return scHangOffsetY[mPowerup];
    } else {
        return scKinopioHangOffsetY[mPowerup];
    }
}

bool dAcPy_c::setKaniHangToVineAction() {
    if (mKey.buttonDown()) {
        float y = mPos.y - 1.0f;
        float x2 = mPos.x + (mVineBcData.mOffset / 4096.0f);
        float x1 = mPos.x - (mVineBcData.mOffset / 4096.0f + 1.0f);
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
        sBcPointData r = mVineBcData;
        r.mInfMargin = getHangBcOffsetY() * 4096.0f;
        if (!(mBc.checkCollision2(&r) & 3)) {
            float y = mPos.y + getHangBcOffsetY();
            float x = mPos.x;
            short groundY = (((short) y) & 0xfff0) - 16;
            u32 unitKind = dBc_c::getUnitKind(x, groundY + 1.0f, mLayer);
            if ((unitKind >> 16 & 0xff) == 8) {
                mPos.y += getHangBcOffsetY();
                m_60 = 10;
                changeState(StateID_Kani, KANI_ARG_HANG_UP_VINE);
                return true;
            }
        }
    }
    return false;
}

void dAcPy_c::initializeState_Vine() {
    setScrollMode(2);
    onStatus(STATUS_VINE);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mAccelF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    m_7c4 = 0;
    if (mAmiLayer == 1) {
        m_7c0 = 0x8000;
    } else {
        m_7c0 = 0;
    }
    int changeParam = (int) mStateArg;
    mAngle.y = m_7c0;
    mBc.mPlayerFlags |= 1;
    switch (changeParam) {
        case 0:
        case 2:
            startPlayerVoice(VOICE_ROPE_CATCH, 0);
            switch (mBc.mFenceType) {
                case 0:
                    startSound(SE_PLY_CATCH_IVY, 0);
                    if (!isNowBgCross(BGC_FOOT)) {
                        mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT_ST);
                        mSubstate = VINE_ACTION_IVY;
                        mSubstateValue = 0;
                    } else {
                        mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT_WAIT);
                        mSubstate = VINE_ACTION_IVY;
                        mSubstateValue = 1;
                    }
                    break;
                case 1:
                    mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
                    mSubstate = VINE_ACTION_NET;
                    break;
                case 2:
                    startSound(SE_PLY_LAND_FENCE, 0);
                    mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
                    mSubstate = VINE_ACTION_NET;
                    break;
            }
            break;
        case 1:
            mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT_WAIT);
            mSubstate = VINE_ACTION_IVY;
            mSubstateValue = 1;
            break;
        case 3:
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT, 0.0f, 0.0f);
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
        if (isNowBgCross(BGC_VINE_TOUCH_U) && isNowBgCross(BGC_VINE_TOUCH_2)) {
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
                tmp = 0.7f;
            } else {
                tmp = 0.5f;
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
    if (!isNowBgCross(BGC_VINE_TOUCH_FULL) || isNowBgCross(BGC_FOOT) && !mKey.buttonUp()) {
        mAngle.y = getMukiAngle(mDirection);
        if (isNowBgCross(BGC_CLIFF)) {
            changeState(StateID_Kani, KANI_ARG_WALK_FORCE);
            return true;
        } else {
            changeState(StateID_Walk, BLEND_NONE);
            return true;
        }
    }
    return false;
}

void dAcPy_c::setVineWalkSE() {
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(10.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(30.0f)) {
        switch (mBc.mFenceType) {
            case 0:
                startSound(SE_PLY_MOVE_IVY, false);
                break;
            case 2:
                startSound(SE_PLY_WALK_METAL, false);
                break;
            case 1:
                startSound(SE_PLY_FOOTNOTE_ROCK_CLIMB, false);
                break;
        }
    }
}

void dAcPy_c::VineActionIvy() {
    if (setRideOffPlayerJump(sc_JumpSpeed, 0.0f) || startJump(BLEND_NONE, 1)) {
        return;
    }
    if (checkVineEnd()) {
        return;
    }
    calcVineSpeed();
    if (mSubstateValue == 0) {
        if (mPyMdlMng.isAnmStop()) {
            mSubstateValue++;
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
            mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT, blend, 0.0f, 0.0f);
            if (mBc.mFenceType == 0 && m_7c4 == 0) {
                setIvyHangEffect();
                setVineWalkSE();
            }
        } else {
            mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT_WAIT, 0.0f);
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
            if (setRideOffPlayerJump(sc_JumpSpeed, 0.0f) || startJump(BLEND_NONE, 0)) {
                switch (mPowerup) {
                    case POWERUP_NONE:
                        startSound(SE_PLY_JUMP_FENCE_S, false);
                        break;
                    case POWERUP_MINI_MUSHROOM:
                        startSound(SE_PLY_JUMP_FENCE_SS, false);
                        break;
                    default:
                        startSound(SE_PLY_JUMP_FENCE, false);
                        break;
                }
                mAngle.y = getMukiAngle(mDirection);
                return;
            }
            if (mKey.triggerAttack()) {
                mSubstate = VINE_ACTION_ATTACK;
                mSubstateValue = 0;
                mPyMdlMng.setAnm(PLAYER_ANIM_NET_ATTACK);
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
        mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK1, rate, 0.0f, 0.0f);
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
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK2, rate, 0.0f, 0.0f);
        }
        setVineWalkSE();
    } else if (f > 1.0f) {
        if (mKey.buttonLeft() || mKey.buttonRight()) {
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK2, 0.6f * f, 0.0f, 0.0f);
        } else {
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK1, 0.6f * f, 0.0f, 0.0f);
        }
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
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
        startSound(SE_PLY_ATTACK_FENCE, false);
    }
    if (!isStatus(STATUS_85)) {
        if (frame >= 12.0f && mKey.buttonCross() || mPyMdlMng.isAnmStop()) {
            mSubstate = VINE_ACTION_NET;
            return;
        }
        if (mKey.triggerAttack()) {
            mSubstateValue = 1;
        }
        if (frame >= 23.0f && mSubstateValue != 0) {
            mPyMdlMng.mpMdl->setFrame(0.0f);
            mSubstateValue = 0;
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
                mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
            }
            break;
        case 2:
            offStatus(STATUS_7A);
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
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
            mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
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
            mAmiLayer = 1;
        } else {
            mAmiLayer = 2;
        }
        mAmiRelated2 = 1.0f;
        m_7bc = 2;
    }
}

void dAcPy_c::finalizeState_Vine() {
    mAccelY = getGravityData()[0];
    offStatus(STATUS_VINE);
    offStatus(STATUS_7A);
    offStatus(STATUS_49);
    offStatus(STATUS_4A);
    mBc.mPlayerFlags &= ~1;
}

void dAcPy_c::setCarryPlayerHang(float height) {
    mPos.y = height - mHeadBcData.mOffset / 4096.0f;
    onNowBgCross(BGC_HANG_ROPE);
    changeState(StateID_Hang, 0);
}

bool dAcPy_c::setHangAction() {
    if (isNowBgCross(BGC_HANG_ROPE)) {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr && carryPlayer->isStatus(STATUS_9C) && !carryPlayer->isCarry()) {
            float hangHeight = mPos.y + mHeadBcData.mOffset / 4096.0f;
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
    onStatus(STATUS_HANG);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    m_1598 = 0.0f;
    if (mPlayerType == PLAYER_YELLOW_TOAD || mPlayerType == PLAYER_BLUE_TOAD) {
        static const float scHangOffsetY[POWERUP_COUNT] = { 4.5f, 10.0f, 10.0f, 1.0f, 10.0f, 9.0f, 10.0f };
        m_1598 = scHangOffsetY[mPowerup];
    }
    mPyMdlMng.setAnm(PLAYER_ANIM_MONKEY_START);
    setZPositionDirect(400.0f);
    startPlayerVoice(VOICE_ROPE_CATCH, 0);
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
    static const int scHangWaitAnm[] = { PLAYER_ANIM_MONKEY_WAIT_R, PLAYER_ANIM_MONKEY_WAIT_L };
    mPyMdlMng.setAnm(scHangWaitAnm[m_8f0]);
}

void dAcPy_c::HangActionWait() {
    mSpeedF = 0.0f;
    if (mKey.buttonRight() || mKey.buttonLeft()) {
        setHangActionMove();
    }
}

void dAcPy_c::setHangActionMove() {
    mSubstate = HANG_ACTION_MOVE;
    static const int scHangMoveAnm[] = { PLAYER_ANIM_MONKEY_R_TO_L, PLAYER_ANIM_MONKEY_L_TO_R };
    mPyMdlMng.setAnm(scHangMoveAnm[m_8f0]);
    m_8f0 ^= 1;
    startSound(SE_PLY_MONKEY_BARS, false);
}

void dAcPy_c::HangActionMove() {
    if (mKey.buttonDush()) {
        mSpeedF = sc_DirSpeed[mDirection] * 1.125f;
        mPyMdlMng.mpMdl->setRate(1.5f);
    } else {
        mSpeedF = sc_DirSpeed[mDirection] * 0.875f;
        mPyMdlMng.mpMdl->setRate(1.0f);
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
    if (!isNowBgCross(BGC_HANG_ROPE) || mKey.buttonDown()) {
        changeState(StateID_Fall, false);
        startPlayerVoice(VOICE_ROPE_RELEASE, 0);
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
    offStatus(STATUS_HANG);
    offZPosSetNone();
}

void dAcPy_c::setPoleTurnAngle() {
    short target = getBesideMukiAngle(mDirection ^ 1);
    mAngle.y.chase(target, 0x2000);
}

bool dAcPy_c::setPoleAction() {
    if (isStatus(STATUS_9D)) {
        if (mPoleGrabCooldown != 0 || isCarry()) {
            return false;
        }
        if (mSpeed.y <= 3.0f && mKey.buttonUp() && isNowBgCross(BGC_CAN_CLIMB)) {
            changeState(StateID_Pole, 0);
            return true;
        }
    }
    return false;
}

void dAcPy_c::initializeState_Pole() {
    onStatus(STATUS_POLE);
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
    if (isNowBgCross(BGC_CAN_CLIMB)) {
        m_8f4.y = mPos.y;
    }
    if (isNowBgCross(BGC_FOOT)) {
        setPoleActionUp();
        mSpeed.y = 1.0f;
        offNowBgCross(BGC_FOOT);
        mDirection ^= 1;
        offStatus(STATUS_97);
    } else {
        mPyMdlMng.setAnm(PLAYER_ANIM_TREE_START);
        startSound(SE_PLY_POLE_CATCH, false);
        startPlayerVoice(VOICE_ROPE_CATCH, 0);
        mSubstate = 0;
    }
}

void dAcPy_c::PoleActionStart() {
    if (mKey.triggerJump()) {
        mSubstateValue = 1;
    }
    if (mPyMdlMng.mpMdl->mAnm.getFrame() >= 5.0f && mSubstateValue != 0) {
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
    mPyMdlMng.setAnm(PLAYER_ANIM_TREE_WAIT);
}

void dAcPy_c::PoleActionWait() {
    if (!isNowBgCross(BGC_CAN_CLIMB)) {
        mDirection ^= 1;
        changeState(StateID_Fall, false);
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
    mPyMdlMng.setAnm(PLAYER_ANIM_TREE_CLIMB);
}

void dAcPy_c::PoleActionUp() {
    setPoleTurnAngle();
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(0.0f)) {
        startSound(SE_PLY_POLE_CLIMB, false);
    }
    mSpeed.y = 0.0f;
    if (!isNowBgCross(BGC_CAN_CLIMB)) {
        mPos.y = m_8f4.y;
    }
    if (mKey.buttonDown()) {
        setPoleActionDown();
    } else if (!mKey.buttonUp()) {
        setPoleActionWait();
    } else if (mKey.buttonDush()) {
        mPyMdlMng.mpMdl->setRate(1.875f);
        mSpeed.y = 1.5f;
    } else {
        mPyMdlMng.mpMdl->setRate(1.5f);
        mSpeed.y = 1.0f;
    }
}

void dAcPy_c::setPoleActionDown() {
    mSubstate = 3;
    mPyMdlMng.setAnm(PLAYER_ANIM_TREE_POSE);
}

void dAcPy_c::PoleActionDown() {
    if (!isNowBgCross(BGC_CAN_CLIMB)) {
        mDirection ^= 1;
        changeState(StateID_Fall, false);
        return;
    }
    setPoleTurnAngle();
    mSpeed.y = 0.0f;
    if (mKey.buttonUp()) {
        setPoleActionUp();
    } else if (!mKey.buttonDown()) {
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
        holdSound(SE_PLY_POLE_SLIDE, false);
    }
}

void dAcPy_c::setPoleJump() {
    startPlayerVoice(VOICE_ROPE_RELEASE, 0);
    mPoleGrabCooldown = 5;
    changeState(StateID_Jump, nullptr);
    u8 dir = mDirection;
    float baseSpeed = getSpeedData()->mLowSpeed;
    mSpeedF = baseSpeed * sc_DirSpeed[dir];
}

bool dAcPy_c::setPoleShakeJump() {
    u8 dir = mDirection;
    float baseSpeed = getSpeedData()->mLowSpeed;
    if (setRideOffPlayerJump(sc_JumpSpeed, baseSpeed * sc_DirSpeed[dir])) {
        startPlayerVoice(VOICE_ROPE_RELEASE, 0);
        mPoleGrabCooldown = 5;
        return true;
    }
    return false;
}

void dAcPy_c::executeState_Pole() {
    if (isNowBgCross(BGC_FOOT)) {
        mDirection ^= 1;
        changeState(StateID_Walk, BLEND_DEFAULT);
    } else if (isNowBgCross(BgCross1_e(BGC_SIDE_LIMIT_L | BGC_SIDE_LIMIT_R))) {
        mDirection ^= 1;
        changeState(StateID_Fall, false);
    } else {
        if (!setPoleShakeJump()) {
            if (mSubstate != 0 && mKey.triggerJump()) {
                setPoleJump();
                return;
            }
            mPos.x = m_8f4.x;
            if (isNowBgCross(BGC_CAN_CLIMB)) {
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
            float yDelta = mCcRevTotalOffsY;
            mPos.y += yDelta;
        }
    }
}

void dAcPy_c::finalizeState_Pole() {
    offStatus(STATUS_POLE);
    offStatus(STATUS_97);
    offStatus(STATUS_9F);
    offZPosSetNone();
}

bool dAcPy_c::setKaniActionInitHangHand() {
    if (isCarry()) {
        return false;
    }
    if (
        !isNowBgCross(BGC_FOOT) &&
        mSpeed.y < 0.0f &&
        !isOldBgCross(BgCross2_e(BGC_CLIFF_ABOVE_1 | BGC_CLIFF_ABOVE_2)) &&
        isNowBgCross(BgCross2_e(BGC_CLIFF_ABOVE_1 | BGC_CLIFF_ABOVE_2))
    ) {
        mPos.y = mKaniHeight;
        changeState(StateID_Kani, KANI_ARG_HANG_HAND);
        return true;
    }
    return false;
}

bool dAcPy_c::setKaniHangAction() {
    if (isNowBgCross(BGC_CLIFF) && mSpeed.y <= 0.0f) {
        if (isStatus(STATUS_A1)) {
            changeState(StateID_Kani, KANI_ARG_JUMP_HANG);
            return true;
        }
        if (isStatus(STATUS_A2)) {
            changeState(StateID_Kani, KANI_ARG_WALK);
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
    if (isNowBgCross(BGC_LIFT)) {
        m_b70 = 1;
    }
    m_b74 = m_b70;
    m_b78 = mPos.y;
    m_b7c = 0;

    KaniArg_e arg = stateArg<KaniArg_e>();
    if (arg == KANI_ARG_WALK && mKey.buttonDown()) {
        arg = KANI_ARG_HANG;
    }
    switch (arg) {
        case KANI_ARG_WALK: setKaniAction_Walk(); break;
        case KANI_ARG_HANG: setKaniAction_Hang(); break;
        case KANI_ARG_JUMP_HANG: setKaniAction_JumpHang(); break;
        case KANI_ARG_WALK_FORCE: setKaniAction_Walk(); break;
        case KANI_ARG_HANG_UP_VINE: setKaniAction_HangUpVine(); break;
        case KANI_ARG_HANG_HAND: setKaniAction_HangHand(); break;
    }
}

void dAcPy_c::finalizeState_Kani() {
    offStatus(STATUS_9B);
    offStatus(STATUS_KANI_HANG_ANIMATION);
    offStatus(STATUS_KANI_HANG);
    offStatus(STATUS_KANI_WALK);
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

void dAcPy_c::setKaniHangStartEffect() {}

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
    mPyMdlMng.setAnm(PLAYER_ANIM_HANG_START);
    startPlayerVoice(VOICE_CLIFF_DOWN, 0);
    mSubstate = KANI_ACTION_HANG_INIT;
    onStatus(STATUS_KANI_HANG_ANIMATION);
}

void dAcPy_c::setKaniAction_JumpHang() {
    releaseCarryActor();
    mAngle.y = 0x8000;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP_HANG);
    setKaniCliffCatchEffect();
    startPlayerVoice(VOICE_CLIFF_DIVE, 0);
    mSubstate = KANI_ACTION_JUMP_HANG_INIT;
}

void dAcPy_c::setKaniAction_HangFall() {
    mPyMdlMng.setAnm(PLAYER_ANIM_HANG_WAIT);
    mSubstate = KANI_ACTION_HANG_FALL;
}

void dAcPy_c::setKaniAction_HangUp() {
    releaseCarryActor();
    mSpeedF = 0.0f;
    setKaniHangStartEffect();
    mPyMdlMng.setAnm(PLAYER_ANIM_HANG_UP);
    startPlayerVoice(VOICE_CLIFF_UP, 0);
    mSubstate = KANI_ACTION_HANG_UP;
}

void dAcPy_c::setKaniAction_HangHand() {
    if (isNowBgCross(BGC_CLIFF_ABOVE_2)) {
        m_b70 = 1;
    }
    releaseCarryActor();
    mAngle.y = 0x8000;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP_HANG, 0.0f, 0.0f);
    setKaniCliffCatchEffect();
    mSubstate = KANI_ACTION_JUMP_HANG_INIT;
}

void dAcPy_c::setKaniAction_HangUpVine() {
    releaseCarryActor();
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_HANG_WAIT, 0.5f, 0.0f, 0.0f);
    mSubstate = KANI_ACTION_HANG_UP_VINE;
}

void dAcPy_c::KaniAction_Walk() {
    onStatus(STATUS_KANI_WALK);
    onStatus(STATUS_9B);
    if (!isNowBgCross(BGC_CLIFF)) {
        changeState(StateID_Walk, BLEND_NONE);
        return;
    }
    if (mKey.triggerDown()) {
        setKaniAction_Hang();
    } else if (mKey.triggerJump()) {
        changeState(StateID_KaniJump);
    } else {
        setKaniMoveSpeed(true);
        if (!mSpeedF) {
            if (mPyMdlMng.getAnm() != PLAYER_ANIM_WALL_WAIT) {
                mPyMdlMng.setAnm(PLAYER_ANIM_WALL_WAIT);
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
            static const int sc_WallWalkAnm[] = { PLAYER_ANIM_WALL_WALK_L, PLAYER_ANIM_WALL_WALK_R };
            if (!mPyMdlMng.isAnm(sc_WallWalkAnm[mDirection])) {
                mPyMdlMng.setAnm(sc_WallWalkAnm[mDirection], rate, 10.0f, 0.0f);
            }
            mPyMdlMng.mpMdl->setRate(rate);
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f)) {
                startSound(SE_PLY_SHUFFLE, false);
            }
        }
    }
}

void dAcPy_c::KaniAction_HangInit() {
    onStatus(STATUS_KANI_HANG);
    if (mPyMdlMng.isAnmStop()) {
        setKaniHangStartEffect();
        mSubstate = KANI_ACTION_HANG;
        mPyMdlMng.setAnm(PLAYER_ANIM_HANG_WAIT, 0.0f, 0.0f);
        offStatus(STATUS_KANI_HANG_ANIMATION);
    }
}

void dAcPy_c::KaniAction_JumpHangInit() {
    onStatus(STATUS_KANI_HANG);
    mPyMdlMng.mpMdl->mAnm.checkFrame(3.0f);
    if (mPyMdlMng.mpMdl->mAnm.getFrame() > 9.0f) {
        mSubstate = KANI_ACTION_HANG;
    }
}

bool dAcPy_c::checkCliffHangFootGround() {
    float height;
    if (dBc_c::checkGround(&mPos, &height, mLayer, 1, -1)) {
        float currHeight = mPos.y - getHangBcOffsetY();
        if (height > currHeight && (u8) (dBc_c::getUnitKind(mPos.x, height, mLayer) >> 16) != 8) {
            mPos.y = currHeight;
            m_60 = 10;
            changeState(StateID_Walk, BLEND_NONE);
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
            onNowBgCross(BGC_WATER_SHALLOW);
            mPos.y = currHeight;
            m_60 = 10;
            changeState(StateID_Swim, SWIM_ARG_CLIFF_HANG);
            return true;
        }
    }
    return false;
}

void dAcPy_c::KaniAction_Hang() {
    onStatus(STATUS_KANI_HANG);
    if (!checkCliffHangFootGround() && !checkCliffHangWater() && !setKaniHangToVineAction()) {
        if (!isNowBgCross(BGC_FOOT)) {
            setKaniAction_HangFall();
        } else if (m_b74 == 1 && m_b70 == 0 && m_b7c == 1) {
            setKaniAction_JumpHang();
        } else {
            setKaniMoveSpeed(false);
            mSpeedF = mMaxSpeedF;
            if (!mSpeedF) {
                if (mPyMdlMng.isAnm(PLAYER_ANIM_JUMP_HANG)) {
                    if (mPyMdlMng.isAnmStop()) {
                        mPyMdlMng.setAnm(PLAYER_ANIM_HANG_WAIT);
                    }
                } else if (!mPyMdlMng.isAnm(PLAYER_ANIM_HANG_WAIT)) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_HANG_WAIT);
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
                static const int sc_WallHangAnm[] = { PLAYER_ANIM_HANG_WALK_R, PLAYER_ANIM_HANG_WALK_L };
                if (!mPyMdlMng.isAnm(sc_WallHangAnm[mDirection])) {
                    mPyMdlMng.setAnm(sc_WallHangAnm[mDirection], rate, 10.0f, 0.0f);
                }
                mPyMdlMng.mpMdl->setRate(rate);
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(1.0f)) {
                    startSound(SE_PLY_HANG_MOVE, false);
                }
            }
            int dir;
            if (
                !isNowBgCross(BGC_HEAD) &&
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
    onStatus(STATUS_KANI_HANG);
    if (!checkCliffHangFootGround() && !checkCliffHangWater()) {
        if (isNowBgCross(BGC_FOOT)) {
            setKaniAction_JumpHang();
        } else {
            setKaniMoveSpeed(false);
            mSpeedF = mMaxSpeedF;
        }
    }
}

void dAcPy_c::KaniAction_HangUp() {
    onStatus(STATUS_KANI_HANG);
    if (mPyMdlMng.isAnmStop()) {
        mPyMdlMng.setAnm(PLAYER_ANIM_WALL_WAIT, 0.0f, 0.0f);
        setKaniAction_Walk();
    }
}

void dAcPy_c::KaniAction_HangUpVine() {
    setKaniAction_HangUp();
}

void dAcPy_c::executeState_Kani() {
    if (checkEnableThrow()) {
        releaseCarryActor();
        return;
    }
    m_b74 = m_b70;
    m_b70 = 0;
    if (isNowBgCross(BGC_LIFT)) {
        m_b70 = 1;
        m_b7c = 0;
        if (m_b78 > mPos.y) {
            m_b7c = 1;
        }
    }
    offStatus(STATUS_9B);
    offStatus(STATUS_KANI_WALK);
    offStatus(STATUS_KANI_HANG);
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
            startSound(SE_PLY_CATCH_ROPE, false);
            break;
        default:
            startSound(SE_PLY_CATCH_KUSARI, false);
            break;
    }
    startPlayerVoice(VOICE_ROPE_CATCH, 0);
}

void dAcPy_c::setClimbRopeSE() {
    switch (m_914) {
        case 0:
        case 2:
            startSound(SE_PLY_CLIMB_ROPE, false);
            break;
        default:
            startSound(SE_PLY_CLIMB_KUSARI, false);
            break;
    }
}

void dAcPy_c::setSlideRopeSE() {
    switch (m_914) {
        case 0:
        case 2:
            holdSound(SE_PLY_SLIDE_ROPE, false);
            break;
        default:
            holdSound(SE_PLY_SLIDE_KUSARI, false);
            break;
    }
}

void dAcPy_c::setRopeHasigoSE() {
    if (mPyMdlMng.mpMdl->mAnm.checkFrame(10.0f) || mPyMdlMng.mpMdl->mAnm.checkFrame(30.0f)) {
        startSound(SE_PLY_FOOTNOTE_HASHIGO_NAWA, false);
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
    if (mTarzanRopeCooldown != 0) {
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
    switch ((int) mStateArg) {
        case 0:
            mAngle.y = getBesideMukiAngle(mDirection);
            mPyMdlMng.setAnm(PLAYER_ANIM_TREE_START);
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
            mPyMdlMng.setAnm(PLAYER_ANIM_HANG_START);
            startPlayerVoice(VOICE_CLIFF_DOWN, 0);
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
    if (!isNowBgCross(BGC_HEAD)) {
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
            h = sc_DirSpeed[mDirection] * getSpeedData()->mLowSpeed;
        }
        jumpSpeed = mPc.getPos().y;
    }
    if (setRideOffPlayerJump(jumpSpeed, h)) {
        startPlayerVoice(VOICE_ROPE_RELEASE, 0);
        if (h * sc_DirSpeed[mDirection] < 0.0f) {
            mDirection ^= 1;
        }
        mNoGravityTimer = 10;
        return true;
    } else {
        if (mKey.triggerJump()) {
            startPlayerVoice(VOICE_ROPE_RELEASE, 0);
            if (h * sc_DirSpeed[mDirection] < 0.0f) {
                mDirection ^= 1;
            }
            mNoGravityTimer = 10;
            mSpeedF = h;
            jmpInf_c jump(jumpSpeed, 1, BLEND_DEFAULT);
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
    mPyMdlMng.mpMdl->mCurrAnmID = PLAYER_ANIM_WALK;
    mPyMdlMng.setAnm(PLAYER_ANIM_ROPE_SWING, a, 10.0f, b);
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
            case ROPE_SWING_7:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(96.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_8;
                }
                // fallthrough
            case ROPE_SWING_8:
                if (m_900 * m_902 <= 0) {
                    setRopeSwingAnm(1.0f, 96.0f);
                    m_90c = ROPE_SWING_10;
                }
                break;
            case ROPE_SWING_10:
                if (mPyMdlMng.isAnmStop()) {
                    m_90c = ROPE_SWING_0;
                }
                checkStartSwingUp();
                break;
            case ROPE_SWING_5:
                if (mPyMdlMng.mpMdl->mAnm.checkFrame(86.0f)) {
                    mPyMdlMng.mpMdl->setRate(0.0f);
                    m_90c = ROPE_SWING_6;
                }
                // fallthrough
            case ROPE_SWING_6:
                if (m_900 * m_902 <= 0) {
                    setRopeSwingAnm(-0.5f, 86.0f);
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
        mPyMdlMng.setAnm(PLAYER_ANIM_TREE_WAIT, 10.0f, 0.0f);
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
    } else if (!(mPc.m_2a & 2) && mKey.buttonUp()) {
        setTarzanRopeActionUp();
    }
}

void dAcPy_c::setTarzanRopeActionUp() {
    mSubstate = 2;
    mPyMdlMng.setAnm(PLAYER_ANIM_TREE_CLIMB);
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
        mPyMdlMng.mpMdl->setRate(1.875f);
        m_918 = 1.5f;
    } else {
        mPyMdlMng.mpMdl->setRate(1.5f);
        m_918 = 1.0f;
    }
    if (!(mPc.m_2a & 4) && mKey.buttonDown()) {
        setTarzanRopeActionDown();
    } else if (!mKey.buttonUp()) {
        setTarzanRopeActionWait();
    }
}

void dAcPy_c::setTarzanRopeActionDown() {
    mSubstate = 3;
    mPyMdlMng.setAnm(PLAYER_ANIM_TREE_POSE);
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
    float f2 = mKey.buttonDush() ? 1.875f : 1.5f;
    if (mKey.buttonUp() && !(mPc.m_2a & 2)) {
        m_918 = f1;
        mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK1, f2, 0.0f, 0.0f);
        setRopeHasigoSE();
    } else if (mKey.buttonDown() && !(mPc.m_2a & 4)) {
        m_918 = -f1;
        mPyMdlMng.setAnm(PLAYER_ANIM_NET_WALK1, -f2, 0.0f, 0.0f);
        setRopeHasigoSE();
    } else {
        m_918 = 0.0f;
        mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT);
    }
}

void dAcPy_c::TarzanRopeActionHasigoHangDown() {
    if (!mPyMdlMng.isAnmStop()) {
        return;
    }
    mPyMdlMng.setAnm(PLAYER_ANIM_NET_WAIT, 0.0f, 0.0f);
    mPos.y -= getHangBcOffsetY();
    m_60 = 10;
    onStatus(STATUS_36);
    setTarzanRopeActionHasigoMove();
    if (!mPc.check(1)) {
        mTarzanRopeCooldown = 30;
        mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 0.0f, 0.0f);
        changeState(StateID_Fall, true);
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
    mPyMdlMng.setAnmOnlyRate(PLAYER_ANIM_HANG_WAIT, 0.0f, 0.0f);
    startPlayerVoice(VOICE_CLIFF_UP, 0);
    offStatus(STATUS_36);
    releasePoleCheck();
}

void dAcPy_c::TarzanRopeActionHasigoHangUp() {
    switch (m_91c) {
        case 0:
            mPyMdlMng.setAnm(PLAYER_ANIM_HANG_UP);
            m_91c = 1;
            break;
        case 1:
            if (mPyMdlMng.isAnmStop()) {
                mPyMdlMng.setAnmOnlyRate(PLAYER_ANIM_WALL_WAIT, 0.0f, 0.0f);
                mAngle.y = 0;
                m_91c = 2;
            }
            break;
        case 2:
            changeState(StateID_Walk, BLEND_DEFAULT);
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
        changeState(StateID_Fall, false);
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
            mPc.move(getCcRevOffsY() + m_918);
            if (mPc.m_2a & 1) {
                changeState(StateID_Fall, false);
            }
        }
    }
}

void dAcPy_c::startQuakeAction(QuakeMode_e mode) {
    setQuakeActionLocal(0, mode);
    mSubstateValue = 1;
}

void dAcPy_c::setQuakeAction(int i, QuakeMode_e mode) {
    setQuakeActionLocal(i, mode);
    mSubstateValue = 0;
}

void dAcPy_c::setQuakeActionLocal(int i, QuakeMode_e mode) {
    if (!isDemo() && isNowBgCross(BGC_FOOT) && !isState(StateID_Quake)) {
        changeState(StateID_Quake, (void *) mode);
        mSubstateTimer = i;
    }
}

void dAcPy_c::endQuakeAction() {
    if (isState(StateID_Quake)) {
        changeState(StateID_Walk, BLEND_DEFAULT);
    }
}

void dAcPy_c::initializeState_Quake() {
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_E_SHOCK);
    onStatus(STATUS_QUAKE);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
    startPlayerVoice(VOICE_QUAKE, 0);
    startQuakeShock(dQuake_c::TYPE_6);
    m_1040 = 8;
    mSubstate = (int) mStateArg;
}

void dAcPy_c::finalizeState_Quake() {
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_QUAKE);
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
    if (mSubstateValue == 0 && mSubstateTimer == 0) {
        changeState(StateID_Walk, BLEND_DEFAULT);
    }
}

void dAcPy_c::setQuakeNumbEffect() {
    mVec3_c center = getCenterPos();
    static const float sc_QuakeNumbScale[] = { 0.6f, 0.8f, 1.0f };
    u8 tallType = getTallType(-1);
    float scale = sc_QuakeNumbScale[tallType];
    mVec3_c size(scale, scale, scale);
    static const char *sc_QuakeEffectID[] = {
        "Wm_mr_brosquake",
        "Wm_mr_quakewait"
    };
    dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, sc_QuakeEffectID[mSubstate], 0, &center, nullptr, &size);
}

void dAcPy_c::initializeState_ElecShock() {
    initElecShock();
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = 0;
    onStatus(STATUS_STUNNED);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void dAcPy_c::finalizeState_ElecShock() {
    offStatus(STATUS_STUNNED);
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void dAcPy_c::executeState_ElecShock() {
    if (!executeElecShock()) {
        changeState(StateID_Walk, BLEND_DEFAULT);
        mPyMdlMng.setAnm(PLAYER_ANIM_E_SHOCK);
        setDamage2(nullptr, DAMAGE_BG);
    }
}

void dAcPy_c::initElecShock() {
    startPlayerVoice(VOICE_DAMAGE_ELEC, 0);
    startSound(SE_PLY_TOUCH_BIRI, false);
    mPyMdlMng.setAnm(PLAYER_ANIM_E_SHOCK);
    mSubstateTimer = 60;
}

bool dAcPy_c::executeElecShock() {
    setElecEffect();
    return mSubstateTimer != 0;
}

void dAcPy_c::setElecEffect() {
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
    float s = mPyMdlMng.getSomeScale();
    mVec3_c efScale(s, s, s);
    int shockType = 0;
    if (mPowerup == POWERUP_MINI_MUSHROOM || mPowerup == POWERUP_NONE) {
        shockType = 1;
    }
    static const char *sc_elecEffectID[] = {
        "Wm_mr_electricshock",
        "Wm_mr_electricshock_s"
    };
    dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, sc_elecEffectID[shockType], 0, &efPos, nullptr, &efScale);
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

void dAcPy_c::initializeState_FlyDamage() {
    static const u8 sc_DamageAnmID[] = {
        PLAYER_ANIM_DAM_F, PLAYER_ANIM_DAM_B,
        PLAYER_ANIM_DOW_F, PLAYER_ANIM_DOW_B,
        PLAYER_ANIM_DOW_F, PLAYER_ANIM_DOW_B,
        PLAYER_ANIM_DOW_F
    };
    m_103c = ((int) mStateArg) & 0xFF;
    u32 crash = ((int) mStateArg) & 0x100;
    mPyMdlMng.setAnm(sc_DamageAnmID[m_103c], 0.0f, 0.0f);
    if (crash == 0) {
        startPlayerVoice(VOICE_HIP_ATTACKED, 0);
        if (m_103c == 0 || m_103c == 1) {
            startSound(SE_PLY_CRASH_S, false);
        } else {
            startSound(SE_PLY_CRASH_L, false);
        }
    }
    if (m_103c == 0 || m_103c == 1) {
        float speed1 = sc_DirSpeed[(m_103c ^ 1) & 1];
        float speed2 = getDirSpeed();
        speed2 *= sGlobalData_c<dAcPy_c>::mData.f8;
        mSpeedF = speed1 * speed2;
        mSpeed.y = 0.0f;
        mSubstate = 5;
    } else {
        mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
        float speed = sc_DirSpeed[mDirection] * sc_DirSpeed[(m_103c ^ 1) & 1];
        mSubstate = 0;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            mPyMdlMng.setAnm(PLAYER_ANIM_STAR_ROLL_DUPLICATE);
            mSubstateTimer = 30;
            if (speed < 0.0f) {
                mDirection = DIR_LR_L;
            } else {
                mDirection = DIR_LR_R;
            }
        }
        switch (m_103c) {
            case 2:
            case 3:
                mSpeedF = speed * sGlobalData_c<dAcPy_c>::mData.f5;
                mSpeed.y = sGlobalData_c<dAcPy_c>::mData.f4;
                break;
            case 4:
            case 5:
                if (isNowBgCross(BGC_FOOT)) {
                    mSpeedF = speed * sGlobalData_c<dAcPy_c>::mData.f7;
                } else {
                    mSpeedF = speed;
                }
                mSpeed.y = sGlobalData_c<dAcPy_c>::mData.f6;
                break;
            case 6:
                mSpeedF = speed;
                mSpeed.y = 2.0f;
                break;
        }
    }
    mMaxSpeedF = mSpeedF;
    mAccelY = getGravityData()[0];
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAngle.y = getMukiAngle(mDirection);
}

void dAcPy_c::finalizeState_FlyDamage() {
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_A1);
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    offStatus(STATUS_PENGUIN_RECOIL);
}

void dAcPy_c::executeState_FlyDamage() {
    int dir;
    mAccelF = 0.08f;
    if (mSubstate == 5 && mKey.buttonWalk(&dir) && mSpeedF * sc_DirSpeed[dir] < 0.0f) {
        mAccelF = 0.15f;
    }

    icePowerChange(0);
    mMaxSpeedF = mSpeedF;
    if (isNowBgCross(BGC_FOOT)) {
        mMaxSpeedF = 0.0f;
    }
    static const u8 sc_damageStopFrame[] = { 0, 0, 9, 10, 9, 10, 9 };
    switch (mSubstate) {
        case 0:
            onStatus(STATUS_A1);
            if (mPowerup != POWERUP_MINI_MUSHROOM) {
                mSubstate = 2;
            } else {
                mSubstate = 1;
            }
            break;
        case 1:
            if (mSubstateTimer == 0 || mPowerup != POWERUP_MINI_MUSHROOM) {
                changeState(StateID_Fall, false);
            }
            break;
        case 2:
            if (mPyMdlMng.mpMdl->mAnm.getFrame() >= sc_damageStopFrame[m_103c]) {
                mPyMdlMng.mpMdl->setRate(0.0f);
                mSubstate = 3;
                mSubstateValue = 10;
            }
            break;
        case 3:
            if (isNowBgCross(BGC_FOOT)) {
                if (mSubstateValue != 0) {
                    mSubstateValue--;
                }
                if (mSubstateValue == 0) {
                    onStatus(STATUS_9B);
                    onStatus(STATUS_9C);
                    onStatus(STATUS_9D);
                    if (checkJumpTrigger()) {
                        break;
                    }
                }
                if (std::fabs(mSpeedF) < 0.5f) {
                    mPyMdlMng.mpMdl->setRate(1.0f);
                    mSubstate = 4;
                }
            }
            break;
        case 4:
            if (!isNowBgCross(BGC_FOOT)) {
                changeState(StateID_Fall, false);
            } else if (!checkJumpTrigger() && mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, BLEND_DEFAULT);
            }
            break;
        case 5:
            if (!isNowBgCross(BGC_FOOT)) {
                if (mPyMdlMng.mpMdl->mAnm.getFrame() > 17.0f) {
                    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 10.0f, 0.0f);
                    changeState(StateID_Fall, false);
                }
            } else if (mPyMdlMng.mpMdl->mAnm.getFrame() > 17.0f) {
                if (checkJumpTrigger()) {
                    break;
                }
                if (mKey.buttonWalk(nullptr)) {
                    changeState(StateID_Walk, BLEND_DEFAULT);
                    break;
                }
            }
            if (mPyMdlMng.isAnmStop()) {
                changeState(StateID_Walk, BLEND_DEFAULT);
            }
            break;
    }
}

void dAcPy_c::setBreakBalloonJump(u8 playerNo, short angle) {
    offStatus(STATUS_DISABLE_STATE_CHANGE);
    startPlayerVoice(VOICE_HUKKATSU, 0);
    startSound(SE_PLY_BALLOON_BRAKE, false);
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
    mJumpDaiFallTimer = 30;
    mTimer_ce8 = 30;
    mCcRevDisabledTimer = 30;
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        changeState(StateID_Swim, SWIM_ARG_INITIAL);
        float cos = mAng(angle).cos();
        float sin = mAng(angle).sin();
        setSwimSpeed(cos * 2.0f, sin * 2.0f);
    } else {
        float sin = mAng(angle).sin() * 4.0f;
        float cos = mAng(angle).cos() * sc_JumpSpeed;
        if (cos < 0.0f) {
            cos = 0.0f;
        }
        setJump(cos, sin, true, 1, 0);
    }
}

void dAcPy_c::setDrawBalloonInPlayer(mVec3_c &pos) {
    if (!isStatus(STATUS_53)) {
        return;
    }
    onStatus(STATUS_54);
    static const float scBalloonOffsetY[] = {
        0.0f, -4.0f, -4.0f, 4.0f, -6.0f, -2.0f, -4.0f
    };
    pos.y += scBalloonOffsetY[mPowerup];
    mPos = mLastPos = pos;
}

bool dAcPy_c::isNotBalloonCourse() {
    return daPyMng_c::mRest[daPyMng_c::mPlayerType[mPlayerNo]] <= 0;
}

void dAcPy_c::initializeState_Balloon() {
    clearJumpActionInfo(1);
    releaseCarryActor();
    endStar();
    mActorProperties |= BIT_FLAG(6);
    mPyMdlMng.setAnm(PLAYER_ANIM_BALLOON_WAIT);
    onStatus(STATUS_7E);
    onStatus(STATUS_7A);
    onStatus(STATUS_AA);
    onStatus(STATUS_INVISIBLE);
    onStatus(STATUS_OUT_OF_PLAY);
    onStatus(STATUS_PROPEL_NO_ROLL);
    onStatus(STATUS_DISABLE_STATE_CHANGE);
    mAccelY = 0.0f;
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAccelF = 0.0f;
    mSpeed.y = 0.0f;
    mSpeedF = 0.0f;
    mSubstate = (int) mStateArg;
    switch (mSubstate) {
        case 0:
            mSubstateTimer = 150;
            if (isNotBalloonCourse()) {
                mSubstate = 4;
            }
            break;
        case 3:
            startSound(SE_PLY_BALLOON_SELF, false);
            break;
    }
    m_12b8 = 0;
    daPyMng_c::decNum(mPlayerNo);
    if (daPyMng_c::mNum == 0) {
        onStatus(STATUS_57);
        mAllBalloonFadeTimer = 210;
        daPyMng_c::mAllBalloon = true;
    }
    if (mPlayerType == PLAYER_MARIO && mPyMdlMng.mpMdl->m_151 == 0) {
        ((dMarioMdl_c *) mPyMdlMng.mpMdl)->fn_800cab00(0);
    }
    if (mPowerup != POWERUP_NONE && mPowerup != POWERUP_MINI_MUSHROOM) {
        mAmiRelated2 = 0.9f;
    }
    setScrollMode(1);
    setZPosition(8000.0f);
    mPlayerOrchestra.init(mPlayerNo);
}

void dAcPy_c::finalizeState_Balloon() {
    mActorProperties &= ~BIT_FLAG(6);
    offStatus(STATUS_53);
    offStatus(STATUS_54);
    offStatus(STATUS_95);
    offStatus(STATUS_7E);
    offStatus(STATUS_7A);
    offStatus(STATUS_AA);
    offStatus(STATUS_INVISIBLE);
    offStatus(STATUS_OUT_OF_PLAY);
    offStatus(STATUS_PROPEL_NO_ROLL);
    mCc.mCcData.mStatus &= ~CC_STATUS_NO_PASS_INFO;
    daPyMng_c::addNum(mPlayerNo);
    mAmiRelated2 = 1.0f;
    setScrollMode(0);
    mDamageInvulnTimer = 127;
    offZPosSetNone();
    daPyMng_c::mPauseEnableInfo |= 1 << mPlayerNo;
}

void dAcPy_c::executeState_Balloon() {
    if (isStatus(STATUS_57) && mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
        SndSceneMgr::sInstance->moveMissFin();
    }
    sLib::calcTimer(&m_12b8);
    if (mAllBalloonFadeTimer != 0) {
        mAllBalloonFadeTimer--;
        if (mAllBalloonFadeTimer == 0) {
            checkAllBalloonFade();
        }
    }
    mAngle.y = 0;
    switch (mSubstate) {
        case 2:
            if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                mSubstate = 0;
                mSubstateTimer = dGameCom::rndInt(5) * 30;
            }
            break;
        case 0:
            if (mSubstateTimer != 0) {
                break;
            }
            if (!isNotBalloonCourse()) {
                onStatus(STATUS_53);
                onStatus(STATUS_95);
                m_12b8 = 600;
                offStatus(STATUS_OUT_OF_PLAY);
                mSubstate = 1;
                dEnemyMng_c::m_instance->createRevivalBallon(mPos, 0, mPlayerNo);
            } else {
                mSubstate = 4;
            }
            break;
        case 3: {
            onStatus(STATUS_53);
            onStatus(STATUS_95);
            m_12b8 = 180;
            offStatus(STATUS_OUT_OF_PLAY);
            mSubstate = 1;
            mVec3_c pos(mPos.x, mPos.y + 4.0f, mPos.z);
            dEnemyMng_c::m_instance->createRevivalBallon(mPos, 2, mPlayerNo);
            break;
        }
        case 1:
            onStatus(STATUS_INVISIBLE);
            if (isStatus(STATUS_54)) {
                offStatus(STATUS_INVISIBLE);
                offStatus(STATUS_54);
            }
            break;
        case 4:
            mPlayerOrchestra.update();
            daPyMng_c::mPauseEnableInfo &= ~(1 << mPlayerNo);
            if (isNotBalloonCourse()) {
                break;
            }
            startPlayerVoice(VOICE_CONTINUE_COURSE, 0);
            mSubstate = 0;
            mSubstateTimer = 60;
            daPyMng_c::mPauseEnableInfo |= 1 << mPlayerNo;
    }
}

void dAcPy_c::setBalloonHelpVoice() {
    if (isStatus(STATUS_53) && mSubstate == 1 && !isStatus(STATUS_DISPLAY_OUT_DEAD) && mBalloonHelpVoiceCooldown == 0) {
        startPlayerVoice(VOICE_BALLOON_HELP_2, 0);
        mBalloonHelpVoiceCooldown = 120;
    }
}

bool dAcPy_c::setBalloonInNextgoto() {
    if (daPyMng_c::isEntryNum1() || isStatus(STATUS_53)) {
        return false;
    }
    changeDemoState(StateID_DemoNone, true);
    changeState(StateID_Balloon, (void *) 2);
    return true;
}

bool dAcPy_c::setBalloonInDamage() {
    if (daPyMng_c::mNum == 0 || daPyMng_c::isEntryNum1() || isStatus(STATUS_53)) {
        return false;
    }
    changeDemoState(StateID_DemoNone, false);
    fn_801416c0(POWERUP_NONE);
    if (!isState(StateID_Balloon)) {
        changeState(StateID_Balloon, 0);
    }
    return true;
}

bool dAcPy_c::setBalloonButtonA() {
    if (!mKey.triggerA()) {
        return false;
    }
    if (dNext_c::m_instance->m_18) {
        return false;
    }
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_IS_COIN_BATTLE) {
        return false;
    }
    if (daPyMng_c::isEntryNum1()) {
        return false;
    }
    if (
        isDemoAll() ||
        isStatus(STATUS_CA) ||
        isStatus(STATUS_56) ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_50) ||
        isStatus(STATUS_QUAKE) ||
        isStatus(STATUS_53) ||
        isNotBalloonCourse() ||
        isStatus(STATUS_ENEMY_STAGE_CLEAR)
    ) {
        return false;
    }
    if (dEnemyMng_c::m_instance->m_15c == 1 || dEnemyMng_c::m_instance->m_15c == 2) {
        return false;
    }
    if (dScStage_c::m_miniGame) {
        return false;
    }
    int level = dScStage_c::m_instance->mCurrCourse;
    if (level == STAGE_TITLE || level == STAGE_PEACH_CASTLE || level == STAGE_STAFFROLL) {
        return false;
    }
    if (getFollowMameKuribo()) {
        startSound(SE_EMY_MAMEKURIBO_CLINGING_NOW, false);
        return false;
    }
    changeDemoState(StateID_DemoNone, false);
    if (!isState(StateID_Balloon)) {
        changeState(StateID_Balloon, (void *) 3);
    }
    return true;
}

bool dAcPy_c::checkAllBalloonFade() {
    if (!isStatus(STATUS_ALL_DOWN_FADE)) {
        onStatus(STATUS_ALL_DOWN_FADE);
        dScStage_c::setNextScene(3, 0, dScStage_c::EXIT_1, dFader_c::FADER_BOWSER);
        return true;
    }
    return false;
}

bool dAcPy_c::setBalloonInDispOutByYoshi(int i) {
    return setBalloonInDispOutBase(i, 1);
}

bool dAcPy_c::setBalloonInDispOut(int i) {
    return setBalloonInDispOutBase(i, 0);
}

bool dAcPy_c::setBalloonInDispOutBase(int i, int j) {
    if (!isDispOutCheckOn()) {
        return false;
    }
    if (i == 2 || i == 0) {
        if (isDemoAll() && !isStatus(STATUS_5A)) {
            return false;
        }
        if (isStatus(STATUS_B8)) {
            return false;
        }
    } else {
        if (isItemKinopio() && !dBg_c::m_bg_p->mAutoscrolls[0].mActive) {
            return false;
        }
    }
    if (getCarryPlayer() != nullptr) {
        getCarryPlayer()->setBalloonInDispOut(i);
    }
    if (j == 1) {
        onStatus(STATUS_B8);
    }
    if (isStatus(STATUS_DISPLAY_OUT_DEAD)) {
        setFallDownDemo();
    } else {
        setPressBgDamage(11, 0);
    }
    if (i == 2 || i == 0) {
        startPlayerVoice(VOICE_SCROLL_OUT, 0);
        dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
    }
    return true;
}

void dAcPy_c::setShakeIce(float f) {
    if (isStatus(STATUS_07)) {
        m_1594 = f;
    }
}

mVec3_c dAcPy_c::getIceDrawPos() {
    return mVec3_c(
        mPos.x + m_1594,
        mPos.y + m_1598 + m_159c,
        mPos.z
    );
}

void dAcPy_c::fn_801395a0() {
    offStatus(STATUS_DISABLE_STATE_CHANGE);
    setRideOffPlayerJump(sc_JumpSpeed, 0.0f);
}

void dAcPy_c::initializeState_IceDamage() {
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mSubstateTimer = 300;
    startPlayerVoice(VOICE_DAMAGE_FREEZE, 0);
    onStatus(STATUS_7E);
    onStatus(STATUS_7A);
    onStatus(STATUS_STUNNED);
    onStatus(STATUS_07);
    onStatus(STATUS_NO_ANIM);
    onStatus(STATUS_DISABLE_STATE_CHANGE);
    dActor_c *ice = dActor_c::construct(fProfile::PLAYER_ICE, this, 0,& mPos, nullptr, 0);
    mIceActorID = ice->mUniqueID;
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP, 0.0f, 0.0f, 0.0f);
    mPyMdlMng.mpMdl->setFrame(0.0f);
}

void dAcPy_c::finalizeState_IceDamage() {
    m_1594 = 0.0f;
    offStatus(STATUS_7E);
    offStatus(STATUS_7A);
    offStatus(STATUS_STUNNED);
    offStatus(STATUS_07);
    offStatus(STATUS_NO_ANIM);
    offStatus(STATUS_DISABLE_STATE_CHANGE);
    mDamageInvulnTimer = 127;
}

void dAcPy_c::executeState_IceDamage() {
    daPlyIce_c *ice = (daPlyIce_c *) fManager_c::searchBaseByID(mIceActorID);
    if (ice == nullptr) {
        offStatus(STATUS_DISABLE_STATE_CHANGE);
        changeState(StateID_Walk, BLEND_NONE);
    } else if (isNowBgCross(BgCross1_e(BGC_SIDE_LIMIT_L | BGC_SIDE_LIMIT_R))) {
        ice->setRevivalBreakIce();
    }
}

bool dAcPy_c::setRideOffPlayerJump(float a, float b) {
    if (mKey.triggerShakeJump()) {
        mVec3_c pos(mPos.x, mPos.y + 1.0f, mPos.z);
        float height;
        if (dBc_c::checkTenjou(&pos, &height, mLayer, 1)) {
            float tmp = getStandHeadBgPointY() + 1.0f;
            if (mPos.y > height - tmp) {
                mPos.y = height - tmp;
                mLastPos = mPos;
                a = 0.0f;
            }
        }
        calcDispSideLimit();
        return fn_80146e40(a, b, false);
    }
    return false;
}

bool dAcPy_c::setRideOffYoshiJump(daPlBase_c *yoshi) {
    if (!((daYoshi_c *) yoshi)->checkRideOffAble()) {
        return false;
    }

    if (mKey.triggerShakeJump()) {
        if (isDemo() || isStatus(STATUS_OUT_OF_PLAY)) {
            return false;
        }
        if (!yoshi->isNowBgCross(BGC_FOOT)) {
            m_1058 = 10;
            setJump(sc_JumpSpeed + 0.1f, yoshi->mSpeedF, true, 1, 1);
            startPlayerVoice(VOICE_YOSHI_JUMP, 0);
        } else {
            float speed = 0.0f;
            if (std::fabs(yoshi->mSpeedF) < 1.5f) {
                if (yoshi->mDirection == DIR_LR_L) {
                    speed = 1.0f;
                } else {
                    speed = -1.0f;
                }
            }
            mSpeedF = speed;
            mSpeed.y = sc_JumpSpeed - 0.2f;
            changeState(StateID_RideOffJump, 0);
            mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
        }
        calcDispSideLimit();
        return true;
    }
    return false;
}

daYoshi_c *dAcPy_c::getRideYoshi() {
    if (isStatus(STATUS_RIDE_YOSHI)) {
        return (daYoshi_c *) fManager_c::searchBaseByID(mRideActorID);
    }
    return nullptr;
}

void dAcPy_c::setRideOnYoshi(daYoshi_c *yoshi) {
    if (mRideActorID != 0) {
        return;
    }
    mRideActorID = yoshi->mUniqueID;
    onStatus(STATUS_RIDE_YOSHI);
    changeState(StateID_RideYoshi, 0);
}

void dAcPy_c::initializeState_RideYoshi() {
    daPyMng_c::startYoshiBGM();
    mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    onStatus(STATUS_7A);
    onStatus(STATUS_7E);
    onStatus(STATUS_93);
    onStatus(STATUS_81);
    mAccelY = 0.0f;
    mSubstateTimer = 10;
    m_60 = 10;
}

void dAcPy_c::finalizeState_RideYoshi() {
    mAngle.x = 0;
    daYoshi_c *yoshi = (daYoshi_c *) fManager_c::searchBaseByID(mRideActorID);
    if (yoshi != nullptr) {
        if (mIsBeingDeleted == 1) {
            yoshi->onStatus(STATUS_ABOUT_TO_BE_DELETED);
        }
        yoshi->setRideOffPlayer();
        mTreadCount = yoshi->mTreadCount;
    }
    mRideActorID = BASE_ID_NULL;
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    offStatus(STATUS_RIDE_YOSHI);
    offStatus(STATUS_7A);
    offStatus(STATUS_7E);
    offStatus(STATUS_93);
    offStatus(STATUS_81);
    mNoInteractTimer = 10;
    if (!isStatus(STATUS_C5)) {
        daPyMng_c::stopYoshiBGM();
    }
    calcDispSideLimit();
}

void dAcPy_c::executeState_RideYoshi() {
    offStatus(STATUS_DISABLE_STATE_CHANGE);
    daYoshi_c *yoshi = getRideYoshi();
    if (yoshi == nullptr) {
        changeState(StateID_Jump, nullptr);
        return;
    }
    turnAngle();
    if (yoshi->isStatus(STATUS_B3)) {
        onStatus(STATUS_DISABLE_STATE_CHANGE);
        return;
    }
    if (mSubstateTimer == 0 && setRideOffYoshiJump(yoshi)) {
        return;
    }
}

void dAcPy_c::initializeState_Cloud() {
    daPlBase_c::initializeState_Cloud();
    setScrollMode(6);
}

void dAcPy_c::finalizeState_Cloud() {
    daPlBase_c::finalizeState_Cloud();
    setScrollMode(0);
    if (mSubstate == 7) {
        releaseCarryActor();
    } else if (mSubstate == 4 && mIsBeingDeleted == 0) {
        createFireBall(0);
    }
}

float dAcPy_c::getCloudOffsetY() {
    return dPyMdlMng_c::m_hio.getValue(mPlayerType, 2, mPowerup);
}

void dAcPy_c::setCloudStateMove() {
    mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
    mSubstate = 1;
}

bool dAcPy_c::checkCloudStateCrouch() {
    if (!mKey.buttonCrouch()) {
        return false;
    }
    if (isCarry()) {
        return false;
    }
    onStatus(STATUS_51);
    mPyMdlMng.setAnm(PLAYER_ANIM_STOOP_START);
    mSubstate = 2;
    return true;
}

void dAcPy_c::setCloudStateFireCreate() {
    mPyMdlMng.setAnm(PLAYER_ANIM_FIRE_AT);
    mPyMdlMng.mpMdl->setFrame(0.0f);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
    mSubstate = 4;
}

void dAcPy_c::executeState_Cloud() {
    if (updateCloudMove()) {
        return;
    }
    switch (mSubstate) {
        case 0:
        case 5:
        case 8:
            if (mPyMdlMng.isAnmStop()) {
                finalizeThrowCommonBase();
                setCloudStateMove();
            }
            // fallthrough
        case 1:
            if (!setRideOffPlayerJump(sc_JumpSpeed - 0.2f, 0.0f)) {
                if (checkSetFireBall()) {
                    setCloudStateFireCreate();
                } else if (checkEnableThrow()) {
                    initializeThrowCommonBase();
                    mSubstate = 7;
                } else if (checkCloudStateCrouch()) {
                    return;
                }
            }
            break;
        case 2:
            if (mPyMdlMng.isAnmStop()) {
                mPyMdlMng.setAnm(PLAYER_ANIM_STOOP);
            }
            if (!mKey.buttonCrouch()) {
                mPyMdlMng.setAnm(PLAYER_ANIM_WAIT, 10.0f, 0.0f);
                mSubstateTimer = 10;
                offStatus(STATUS_51);
                mSubstate = 3;
            }
            break;
        case 3:
            if (checkSetFireBall()) {
                setCloudStateFireCreate();
            }
            if (mSubstateTimer == 0) {
                setCloudStateMove();
            }
            break;
        case 4:
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(4.0f)) {
                createFireBall(0);
                mSubstate = 5;
            }
            break;
        case 7:
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(5.0f)) {
                mSubstate = 8;
                setThrowActor();
            }
            break;
    }
}

bool dAcPy_c::checkCarryActor(dAcPy_c *player) {
    if (mCarryActorID == player->mUniqueID && player->mRideActorID == mUniqueID) {
        return true;
    }
    return false;
}

bool dAcPy_c::setDropCarryPlayer() {
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer == nullptr) {
        return false;
    }
    if (ridePlayer->isNowBgCross(BGC_FOOT) && !ridePlayer->isStatus(STATUS_JUMP)) {
        if (ridePlayer->isStatus(STATUS_46)) {
            if (ridePlayer->isNowBgCross(BGC_HEAD)) {
                setJump(0.0f, ridePlayer->mSpeedF / 2.0f, true, 0, 0);
                onStatus(STATUS_JUMP_DAI_COOLDOWN);
                return true;
            }
            mVec3_c pos(ridePlayer->mPos.x, ridePlayer->mPos.y + 4.0f, ridePlayer->mPos.z);
            mVec3_c pos2(mPos.x, mPos.y + 4.0f, mPos.z);
            if (!dBc_c::checkWall(&pos, &pos2, nullptr, mLayer, mAmiLayer, nullptr)) {
                float height;
                if (dBc_c::checkTenjou(&pos2, &height, mLayer, mAmiLayer)) {
                    const sBcPlayerPointData *p = getBgPointData_Powerup(mPowerup, 1);
                    if (mPos.y + p->mHead.mOffset / 4096.0f + 2.0f > height) {
                        setJump(0.0f, ridePlayer->mSpeedF / 2.0f, true, 0, 0);
                        onStatus(STATUS_JUMP_DAI_COOLDOWN);
                        return true;
                    }
                }
            }
        }
        if (!ridePlayer->isStatus(STATUS_46)) {
            if (ridePlayer->isNowBgCross(BGC_12) && !ridePlayer->isNowBgCross(BGC_13)) {
                float f;
                if (ridePlayer->isNowBgCross(BGC_WALL_TOUCH_R_2)) {
                    f = -1.0f;
                    mPos.x -= 3.0f;
                } else {
                    f = 1.0f;
                    mPos.x += 3.0f;
                }
                setJump(ridePlayer->mSpeed.y, f, true, 0, 0);
                onStatus(STATUS_JUMP_DAI_COOLDOWN);
                return true;
            }
            if (ridePlayer->isNowBgCross(BGC_SLOPE_AND_HEAD)) {
                setJump(0.0f, 0.0f, true, 0, 0);
                onStatus(STATUS_JUMP_DAI_COOLDOWN);
            }
        }
    }
    return false;
}

void dAcPy_c::initializeState_CarryPlayer() {
    onStatus(STATUS_45);
    onStatus(STATUS_97);
    onStatus(STATUS_7F);
    onStatus(STATUS_93);
    onStatus(STATUS_JUMP_DAI_COOLDOWN);
    onStatus(STATUS_9C);
    onStatus(STATUS_81);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
    mSubstateTimer = 55;
    mSubstate = 0;
    if (isItemKinopio()) {
        if (daPyMng_c::mKinopioCarryCount < 2) {
            daPyMng_c::mKinopioCarryCount++;
        }
        if (daPyMng_c::mKinopioCarryCount >= 2) {
            dGameCom::hideFukidashiForSession(mPlayerNo, 8);
        }
    }
}

void dAcPy_c::finalizeState_CarryPlayer() {
    mAngle.x = 0;
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer != nullptr) {
        ridePlayer->cancelCarry(this);
    }
    offStatus(STATUS_45);
    offStatus(STATUS_97);
    offStatus(STATUS_7F);
    offStatus(STATUS_THROW);
    offStatus(STATUS_93);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);
    offStatus(STATUS_51);
    offStatus(STATUS_9C);
    offStatus(STATUS_81);
    offZPosSetNone();
    mNoInteractTimer = 20;
    mPyMdlMng.mpMdl->m_17c &= ~BIT_FLAG(7);
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 0.0f, 0.0f);
    mRideActorID = BASE_ID_NULL;
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    calcDispSideLimit();
}

void dAcPy_c::executeState_CarryPlayer() {
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer == nullptr || fManager_c::searchBaseByID(ridePlayer->mCarryActorID) != this) {
        changeState(StateID_Fall, false);
        return;
    }
    setZPositionDirect(ridePlayer->mPos.z);
    if (setDropCarryPlayer()) {
        return;
    }
    if (isNowBgCross(BGC_VINE_TOUCH_FULL)) {
        mAmiLayer = ridePlayer->mAmiLayer;
    }
    if (!ridePlayer->isStatus(STATUS_46)) {
        if (setRideOffPlayerJump(sc_JumpSpeed - 0.2f, 0.0f)) {
            return;
        }
    }
    switch (mSubstate) {
        case 4:
        case 2:
            if (mPyMdlMng.isAnmStop()) {
                setCarryPlayerMode_Move(0.0f);
            }
            // fallthrough
        case 0:
            if (checkSetFireBall()) {
                setCarryPlayerMode_Fire();
            } else if (ridePlayer->isStatus(STATUS_47)) {
                setCarryPlayerMode_Crouch();
            }
            break;
        case 3:
            if (!mPyMdlMng.mpMdl->mAnm.checkFrame(dPyMdlBase_c::scFireShootFrame)) {
                break;
            }
            createFireBall(0);
            mSubstate = 4;
            break;
        case 1:
            if (mPyMdlMng.isAnmStop()) {
                mPyMdlMng.setAnm(PLAYER_ANIM_STOOP);
            }
            if (ridePlayer->isStatus(STATUS_47)) {
                break;
            }
            setCarryPlayerMode_Move(10.0f);
            offStatus(STATUS_51);
            break;
    }
}

void dAcPy_c::setCarryPlayerMode_Move(float f)  {
    mPyMdlMng.mpMdl->m_17c &= ~BIT_FLAG(7);
    mPyMdlMng.setAnmOnlyRate(PLAYER_ANIM_WAIT, f, 0.0f);
    mSubstate = 0;
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer != nullptr) {
        ridePlayer->mpMdlMng->mpMdl->copyLinkAnm(f);
    }
}

void dAcPy_c::setCarryPlayerMode_Fire() {
    mPyMdlMng.mpMdl->m_17c |= BIT_FLAG(7);
    mPyMdlMng.setAnmOnlyRate(PLAYER_ANIM_FIRE_AT2, 0.0f, 0.0f);
    mPyMdlMng.mpMdl->setFrame(0.0f);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
    mSubstate = 3;
}

void dAcPy_c::setCarryPlayerMode_Crouch() {
    mPyMdlMng.mpMdl->m_17c |= BIT_FLAG(7);
    mPyMdlMng.setAnm(PLAYER_ANIM_STOOP_START, 3.0f, 0.0f, 0.0f);
    mSubstate = 1;
    onStatus(STATUS_51);
}

void dAcPy_c::initRideSpinMove() {
    if (!isStatus(STATUS_51)) {
        onStatus(STATUS_SPIN);
    }
}

void dAcPy_c::endRideSpinMove() {
    offStatus(STATUS_SPIN);
}

dAcPy_c *dAcPy_c::getRidePlayer() {
    if (isStatus(STATUS_45)) {
        return (dAcPy_c *) fManager_c::searchBaseByID(mRideActorID);
    }
    return nullptr;
}

void dAcPy_c::setYoshiBackPos() {
    daYoshi_c *yoshi = getRideYoshi();
    if (yoshi == nullptr) {
        return;
    }

    mMtx_c mtx;
    mVec3_c pos;
    yoshi->getModel()->getJointMtx(&mtx, 8);
    mtx.concat(mMtx_c::createTrans(0.0f, -5.0f, 0.0f));
    mtx.multVecZero(pos);

    mPos.x = yoshi->mPos.x;
    mPos.y = pos.y - 8.0f;
    mDirection = yoshi->mDirection;
}

void dAcPy_c::setPlayerHandPos() {
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer == nullptr) {
        return;
    }

    mVec3_c liftPos = ridePlayer->getLiftUpPos();
    liftPos.z = mPos.z;
    mPos = mLastPos = liftPos;
    if (isStatus(STATUS_97)) {
        mDirection = ridePlayer->mDirection;
        int newAng = ridePlayer->mAngle.y.mAngle + ridePlayer->getMissSpinAngle();
        mAngle.y = newAng;
    }
}

void dAcPy_c::setEatTongue(dActor_c *eatingActor) {
    changeState(StateID_PlayerEat, 0);
}

void dAcPy_c::setEatTongueOff(dActor_c *eatingActor) {
    changeState(StateID_Fall, false);
}

void dAcPy_c::setEatMouth(dActor_c *eatingActor) {
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;
    if (isState(StateID_PlayerEat)) {
        onStatus(STATUS_INVISIBLE);
        onStatus(STATUS_C9);
        yoshi->setVirusStar(this);
    }
}

bool dAcPy_c::setEatSpitOut(dActor_c *eatingActor) {
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;
    if (isState(StateID_PlayerEat)) {
        setNoHitPlayer(yoshi, 10);
        dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
        mDirection = yoshi->mDirection;
        mAngle.y = getMukiAngle(mDirection);
        if (m_1298 == 1) {
            mSpeed.y = sGlobalData_c<dAcPy_c>::mData.mSpitRelated[2];
            float f = getDirSpeed();
            f *= sGlobalData_c<dAcPy_c>::mData.mSpitRelated[3];
            mSpeedF = 0.5f * yoshi->getSpeedF() + f;
            setRideOffPlayerJump(mSpeed.y, mSpeedF);
        } else {
            mSpeed.y = sGlobalData_c<dAcPy_c>::mData.mSpitRelated[0];
            float f = getDirSpeed();
            f *= sGlobalData_c<dAcPy_c>::mData.mSpitRelated[1];
            mSpeedF = 0.5f * yoshi->getSpeedF() + f;
            changeState(StateID_RollSlip, 0);
        }
        mMtx_c mtx;
        yoshi->getMouthMtx(&mtx);
        mtx.concat(mMtx_c::createTrans(10.0f, 0.0f, 0.0f));
        mPos.x = mtx.transX();
        mPos.y = mtx.transY();
        float f = yoshi->mPos.y + 4.0f;
        if (mPos.y < f) {
            mPos.y = f;
        }
        mVec3_c posCopy(yoshi->mPos.x, mPos.y, mPos.z);
        float offs = sc_DirSpeed[mDirection] * std::fabs(getWallBgPointData()->mOffset / 4096.0f);
        mVec3_c posCopy2(mPos.x + offs, mPos.y, mPos.z);
        float height;
        if (dBc_c::checkWall(&posCopy, &posCopy2, &height, mLayer, 1, nullptr)) {
            mSpeedF = 0.0f;
            mPos.x = height - offs;
        }
        float offs2 = getHeadBgPointData()->mOffset / 4096.0f;
        posCopy.set(mPos.x, offs2 + yoshi->mPos.y, mPos.z);
        if (dBc_c::checkTenjou(&posCopy, &height, mLayer, 1)) {
            if (mPos.y > height - offs2) {
                mPos.y = height - offs2;
                mSpeed.y = 0.0f;
            }
        }
        mLastPos = mPos;
        calcDispSideLimit();
    }
    return true;
}

void dAcPy_c::setPlayerEatPos(dActor_c *eatingActor) {
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;
    mMtx_c mtx;
    if (yoshi != nullptr && yoshi->getTongueTipMtx(&mtx)) {
        mPos.x = mtx.transX();
        mPos.y = mtx.transY();
        m_129c.x = (mPos.x - yoshi->mPos.x) / mAng(yoshi->mAngle.y).sin();
        m_129c.y = mPos.y - yoshi->mPos.y;
    }
}

void dAcPy_c::eatMove(dActor_c *eatingActor) {
    if (eatingActor != nullptr && isStatus(STATUS_C8)) {
        if (isStatus(STATUS_C9)) {
            mPos.x = eatingActor->mPos.x + m_129c.x * mAng(eatingActor->mAngle.y).sin();
            mPos.y = eatingActor->mPos.y + m_129c.y;
        } else {
            setPlayerEatPos(eatingActor);
        }
    }
}

void dAcPy_c::initializeState_PlayerEat() {
    releaseCarryActor();
    endPowerUpEffect();
    onStatus(STATUS_97);
    onStatus(STATUS_AA);
    onStatus(STATUS_7A);
    onStatus(STATUS_7E);
    onStatus(STATUS_81);
    onStatus(STATUS_C8);
    dActor_c *eatingActor = (dActor_c *) fManager_c::searchBaseByID(mEatenByID);
    if (eatingActor == nullptr) {
        setPlayerEatPos(eatingActor);
    }
    onStatus(STATUS_CA);
    mPyMdlMng.setAnm(PLAYER_ANIM_STAR_ROLL_DUPLICATE);
    mSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    m_1298 = 0;
}

void dAcPy_c::finalizeState_PlayerEat() {
    mEatState = EAT_STATE_NONE;
    mEatenByID = BASE_ID_NULL;
    offStatus(STATUS_97);
    offStatus(STATUS_AA);
    offStatus(STATUS_7A);
    offStatus(STATUS_7E);
    offStatus(STATUS_81);
    offStatus(STATUS_INVISIBLE);
    offStatus(STATUS_C8);
    offStatus(STATUS_C9);
    offStatus(STATUS_CA);
    calcDispSideLimit();
}

void dAcPy_c::executeState_PlayerEat() {
    daYoshi_c *yoshi = (daYoshi_c *) fManager_c::searchBaseByID(mEatenByID);
    if (yoshi == nullptr) {
        changeState(StateID_Jump, nullptr);
        return;
    }
    if (mSubstateValue != 0) {
        mSubstateValue--;
    }
    if (mKey.triggerShakeJump() && yoshi->fn_8014f030(this)) {
        m_1298 = 1;
    }
}

void dAcPy_c::setPlayerEatReact() {
    mEatBehavior = EAT_TYPE_NONE;
    if (
        isDemo() ||
        isChange() ||
        (mDamageInvulnTimer | mPowerupChangeInvulnTimer) != 0 || // [maybe an inline?]
        mNoInteractTimer != 0 ||
        isStatus(STATUS_DISPLAY_OUT_DEAD) ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_QUAKE)
    ) {
        return;
    }
    mEatBehavior = EAT_TYPE_EAT;
}

bool dAcPy_c::setRideJrCrown(const dActor_c *actor) {
    if (mRideActorID == 0) {
        mRideActorID = actor->mUniqueID;
        changeState(StateID_JrCrown, 0);
        return true;
    }
    return false;
}

bool dAcPy_c::isRideJrCrownOwn(const dActor_c *actor) {
    if (isStatus(STATUS_4F) && mRideActorID == actor->mUniqueID) {
        return true;
    }
    return false;
}

void dAcPy_c::setRideJrCrownMtx(const mMtx_c *mtx) {
    if (isStatus(STATUS_4F)) {
        mRideJrClownMtx = *mtx;
    }
}

void dAcPy_c::setJrCrownPos() {
    if (isStatus(STATUS_4F)) {
        mRideJrClownMtx.multVecZero(mPos);
    }
}

void dAcPy_c::setRideJrCrownAnm(int i) {
    if (isStatus(STATUS_4F) && !isStatus(STATUS_50)) {
        mPyMdlMng.setAnm(i);
    }
}

void dAcPy_c::initializeState_JrCrown() {
    releaseCarryActor();
    onStatus(STATUS_4F);
    onStatus(STATUS_7E);
    onStatus(STATUS_93);
    onStatus(STATUS_JUMP_DAI_COOLDOWN);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_RIDE_WAIT);
    mRc.mFlags |= BIT_FLAG(2);
}

void dAcPy_c::finalizeState_JrCrown() {
    offStatus(STATUS_4F);
    offStatus(STATUS_50);
    offStatus(STATUS_7E);
    offStatus(STATUS_93);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);
    mRideActorID = BASE_ID_NULL;
    mRc.mFlags &= ~BIT_FLAG(2);
}

void dAcPy_c::executeState_JrCrown() {
    switch (mSubstate) {
        case 0:
            offStatus(STATUS_50);
            break;
        case 1:
            if (!executeElecShock()) {
                setDamage2(nullptr, DAMAGE_BG);
                mSubstate = 2;
            }
            break;
        case 2:
            mSubstate = 0;
            mPyMdlMng.setAnm(PLAYER_ANIM_RIDE_WAIT);
            break;
    }
}

void dAcPy_c::setJrCrownElecDamage() {
    onStatus(STATUS_50);
    initElecShock();
    mSubstate = 1;
}

bool dAcPy_c::checkSetFireBall() {
    if (mKey.triggerFire()) {
        if (isCarry()) {
            return false;
        }
        if (mPowerup == POWERUP_FIRE_FLOWER) {
            if (daFireBall_Player_c::CheckFireBallLimit(mPlayerNo, 0)) {
                return true;
            }
        } else if (mPowerup == POWERUP_PENGUIN_SUIT || mPowerup == POWERUP_ICE_FLOWER) {
            if (daIceBall_c::CheckIceballLimit(mPlayerNo, 0)) {
                return true;
            }
        }
    }
    return false;
}

bool dAcPy_c::setFireBallAction() {
    if (isStatus(STATUS_9E) && checkSetFireBall()) {
        changeState(StateID_Fire, 0);
        return true;
    }
    return false;
}

bool dAcPy_c::checkFireJump() {
    if (isNowBgCross(BGC_FOOT) && mKey.triggerJump()) {
        if (isNowBgCross(BGC_WATER_SHALLOW)) {
            mSpeed.y = sc_WaterJumpSpeed;
        } else {
            fn_80145fd0(1);
            setJumpSpeed();
        }
        setJumpCommonBase();
        return true;
    }
    return false;
}

void dAcPy_c::createFireBall(int i) {
    mMtx_c mtx;
    mVec3_c pos;
    mVec3_c loopPos;
    mVec3_c pos2;
    mPyMdlMng.mpMdl->getJointMtx(&mtx, 11);
    mtx.concat(mMtx_c::createTrans(-1.0f, 4.0f, 4.0f));
    mtx.multVecZero(pos);
    mPyMdlMng.mpMdl->getJointMtx(&mtx, 0);
    mtx.multVecZero(pos2);
    mVec3_c diff = pos - pos2;
    float dist = diff.xzLen();
    float dirSpeed = sc_DirSpeed[m_12f4];
    loopPos.set(
        dScStage_c::getLoopPosX(pos2.x + dirSpeed * dist),
        pos.y,
        mPos.z
    );
    if (mPowerup == POWERUP_FIRE_FLOWER) {
        startSound(SE_PLY_THROW_FIRE, false);
        pos = loopPos;
        u32 param = (i << 16) | (mAmiLayer << 12) | (mLayer << 8) | (m_12f4 << 4) | mPlayerNo;
        dActor_c::construct(fProfile::PL_FIREBALL, param, &pos, nullptr, 0);
    } else if (mPowerup == POWERUP_PENGUIN_SUIT || mPowerup == POWERUP_ICE_FLOWER) {
        startSound(SE_PLY_THROW_ICEBALL, false);
        pos = loopPos;
        u32 param = (i << 16) | (mAmiLayer << 12) | (mLayer << 8) | (m_12f4 << 4) | mPlayerNo;
        dActor_c::construct(fProfile::ICEBALL, param, &pos, nullptr, 0);
    }
}

void dAcPy_c::initializeState_Fire() {
    mSubstate = 0;
    mPyMdlMng.setAnm(PLAYER_ANIM_FIRE_AT);
    mAngle.y = getMukiAngle(mDirection);
    m_12f4 = mDirection;
    onStatus(STATUS_A0);
    onStatus(STATUS_FIREBALL_PREPARE_SHOOT);
    onStatus(STATUS_9F);
}

void dAcPy_c::finalizeState_Fire() {
    offStatus(STATUS_9F);
    offStatus(STATUS_A0);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_FIREBALL_PREPARE_SHOOT);
    offStatus(STATUS_A1);
}

void dAcPy_c::executeState_Fire() {
    gravitySet();
    maxFallSpeedSet();
    simpleMoveSpeedSet();
    powerSet();
    if (isNowBgCross(BGC_FOOT)) {
        offStatus(STATUS_CAN_LAND);
        offStatus(STATUS_9B);
        offStatus(STATUS_9C);
        offStatus(STATUS_9D);
        offStatus(STATUS_A1);
    } else {
        onStatus(STATUS_CAN_LAND);
        onStatus(STATUS_9B);
        onStatus(STATUS_9C);
        onStatus(STATUS_9D);
        onStatus(STATUS_A1);
    }
    switch (mSubstate) {
        case 0:
            checkFireJump();
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(dPyMdlBase_c::scFireShootFrame)) {
                createFireBall(0);
                mSubstate = 1;
                offStatus(STATUS_FIREBALL_PREPARE_SHOOT);
            }
            break;
        case 1:
            if (checkSetFireBall()) {
                m_12f4 = getDirection();
                mAngle.y = getMukiAngle(mDirection);
                mSubstate = 0;
                mPyMdlMng.setAnm(PLAYER_ANIM_FIRE_AT);
                mPyMdlMng.mpMdl->setFrame(0.0f);
            } else if (isNowBgCross(BGC_FOOT)) {
                if (
                    !checkJumpTrigger() &&
                    (
                        mPyMdlMng.mpMdl->mAnm.getFrame() >= 10.0f && mSpeedF ||
                        mPyMdlMng.isAnmStop()
                    )
                ) {
                    changeState(StateID_Walk, BLEND_DEFAULT);
                }
            } else {
                if (
                    mPyMdlMng.mpMdl->mAnm.getFrame() >= 10.0f ||
                    (mPyMdlMng.isAnmStop() && mSpeed.y <= 0.0f)
                ) {
                    changeState(StateID_Fall, false);
                }
            }
            break;
    }
}

void dAcPy_c::setSpinFireBall() {
    if (isCarry()) {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr) {
            carryPlayer->m_12f4 = m_12f4;
            carryPlayer->setSpinFireBall();
        }
    } else if (mPowerup == POWERUP_FIRE_FLOWER) {
        if (daFireBall_Player_c::CheckFireBallLimit(mPlayerNo, 1)) {
            createFireBall(1);
        }
    } else if (mPowerup == POWERUP_PENGUIN_SUIT || mPowerup == POWERUP_ICE_FLOWER) {
        if (daIceBall_c::CheckIceballLimit(mPlayerNo, 1)) {
            createFireBall(1);
        }
    }
}

bool dAcPy_c::setPropelAction() {
    if (isStatus(STATUS_IS_SPIN_HOLD_REQ) || isOnSinkSand()) {
        changeState(StateID_SpinJump, 0);
        return false;
    }
    if (m_2f4 == 1) {
        if (!getCarryPropelBlock() && dScStage_c::m_miniGame == 0) {
            dGameCom::hideFukidashiForSession(mPlayerNo, 1);
        }
        if (isNowBgCross(BGC_FOOT)) {
            changeState(StateID_Propel, (void *) 2);
        } else {
            changeState(StateID_Propel, (void *) 0);
        }
        return true;
    }
    return false;
}

void dAcPy_c::resetPropelFlyTime() {
    if (m_2f4 == 0) {
        m_2f4 = 1;
    }
}

void dAcPy_c::clearPropelFlyUpTime() {
    if (isStatus(STATUS_PROPEL_UP) && m_2ec > 10) {
        m_2ec = 10;
    }
}

void dAcPy_c::calcPropelMoveSpeedF() {
    int dir;
    u8 wasWalk = mKey.buttonWalk(&dir);
    if (wasWalk) {
        mDirection = dir;
    }
    float f;
    if (!isStatus(STATUS_PROPEL_UP)) {
        mAccelF = data_802f5a0c[2];
        if (wasWalk && mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
            mAccelF = data_802f5a0c[3];
        }
        if (isStatus(STATUS_PROPEL_SLOW_FALL)) {
            f = data_802f5a0c[8];
        } else {
            f = data_802f5a0c[9];
        }
    } else {
        mAccelF = data_802f5a0c[1];
        f = data_802f5a0c[10];
    }
    if (!wasWalk) {
        float oldF = f;
        f = std::fabs(mSpeedF);
        mAccelF = 0.05f;
        if (f > oldF) {
            f = oldF;
        }
    }
    if (mDirection == DIR_LR_L) {
        f = -f;
    }
    mMaxSpeedF = f;
}

void dAcPy_c::setPropelActionFlyInit() {
    m_2f4 = 0;
    m_2f0 = 25;
    onStatus(STATUS_SPIN);
    mPropelRollSpeed = 12000;
    mIsPropelFall = 0;
    startPlayerVoice(VOICE_PRPL_JUMP, 0);
    startSound(SE_PLY_PRPL_JUMP, false);
    mPyMdlMng.setAnm(PLAYER_ANIM_PL_SPIN_JUMP);
    mAccelY = 0.0f;
    mMaxFallSpeed = data_802f5a0c[5];
}

void dAcPy_c::setPropelActionBigFly() {
    setPropelActionFlyInit();
    mSubstate = 0;
    mSpeed.y = 0.5f;
    m_2ec = 38;
    setStartPropelJumpEffect();
    startPatternRumble("****--****--**----**----**----**----**----**----**");
}

void dAcPy_c::setPropelActionFly() {
    setPropelActionFlyInit();
    mSubstate = 1;
    m_2ec = 50;
    startPatternRumble("***---***---*----**-----*----**-----*-----**-----*");
}

void dAcPy_c::PropelActionFly() {
    setCcAtSpin();
    setPropelSpinSmokeEffect();
    if (mPropelRollSpeed < 3000) {
        mPropelRollSpeed = 3000;
    }
    if (isNowBgCross(BGC_HEAD)) {
        mSpeed.y = data_802f5a0c[4];
    }
    if (isNowBgCross(BGC_FOOT)) {
        m_2ec = 0;
    }
    if (mSubstate == 0) {
        sLib::chase(&mSpeed.y, 3.0f, 0.7f);
    } else {
        sLib::chase(&mSpeed.y, data_802f5a0c[11], data_802f5a0c[12]);
    }
    if (m_2ec != 0) {
        m_2ec--;
    }
    if (m_2ec == 0) {
        setPropelActionFall();
        if (mSubstate == 0) {
            mSubstateTimer = 30;
        } else {
            mSubstateTimer = 10;
        }
    }
}

void dAcPy_c::setPropelActionFall() {
    offStatus(STATUS_SPIN);
    mPyMdlMng.setAnm(PLAYER_ANIM_PL_SPIN_JUMP, 30.0f, 0.0f);
    setAddLiftSpeedF();
    mSubstate = 2;
    mPropelRollSpeed = 3000;
    mIsPropelFall = 1;
    mAccelY = data_802f5a0c[7];
}

void dAcPy_c::PropelActionFall() {
    if (mSpeed.y < 0.0f) {
        offStatus(STATUS_PROPEL_UP);
    }
    if (isStatus(STATUS_PROPEL_SLOW_FALL)) {
        mMaxFallSpeed = data_802f5a0c[6];
    } else {
        mMaxFallSpeed = data_802f5a0c[5];
    }
    offStatus(STATUS_A8);
    onStatus(STATUS_A9);
    onStatus(STATUS_92);
    if (isNowBgCross(BGC_FOOT)) {
        setLandSE();
        mPyMdlMng.setAnm(PLAYER_ANIM_JUMPED);
        changeState(StateID_Land, false);
    } else {
        if (mPropelRollSpeed < 2000) {
            offStatus(STATUS_PROPEL_SLOW_FALL);
        } else {
            onStatus(STATUS_PROPEL_SLOW_FALL);
        }
        calcPropelFallSpinEffect();
        if (mPropelRollSpeed < 1000) {
            mPropelRollSpeed = 1000;
            if (mPyMdlMng.getAnm() != PLAYER_ANIM_SPIN_JUMP3) {
                mPyMdlMng.setAnm(PLAYER_ANIM_SPIN_JUMP3);
            }
        }
        if (
            mPyMdlMng.mpMdl->mCurrAnmID == PLAYER_ANIM_SPIN_JUMP3 &&
            (
                mPyMdlMng.mpMdl->mAnm.checkFrame(8.0f) ||
                mPyMdlMng.mpMdl->mAnm.checkFrame(19.0f)
            )
        ) {
            startSound(SE_PLY_PRPL_FLY, false);
        }
        if (mKey.triggerShakeJump()) {
            mPropelRollSpeed = 5000;
            mIsPropelFall = 1;
            mPyMdlMng.setAnm(PLAYER_ANIM_PL_SPIN_JUMP);
            setPropelFallSpinEffect();
        }
    }
}

void dAcPy_c::initializeState_Propel() {
    m_2ec = 0;
    switch ((int) mStateArg) {
        case 0:
            setPropelActionFly();
            break;
        case 1:
            setPropelActionFall();
            break;
        case 2:
            setPropelActionBigFly();
            break;
    }
    if (isNowBgCross(BGC_FOOT)) {
        offNowBgCross(BGC_FOOT);
    }
    onStatus(STATUS_PROPEL);
    onStatus(STATUS_PROPEL_UP);
    onStatus(STATUS_9B);
    onStatus(STATUS_9C);
    onStatus(STATUS_9D);
    onStatus(STATUS_A0);
    onStatus(STATUS_A8);
    onStatus(STATUS_CAN_LAND);
    mSubstateValue = 0;
    if (getCarryPropelActor() != nullptr) {
        mSubstateValue = 1;
    }
    setScrollMode(2);
}

void dAcPy_c::finalizeState_Propel() {
    endPropelFlyPartsMove();
    stopPropelFallSpinEffect();
    mPropelRollSpeed = 0;
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_PROPEL);
    offStatus(STATUS_PROPEL_UP);
    offStatus(STATUS_SPIN);
    offStatus(STATUS_9B);
    offStatus(STATUS_9C);
    offStatus(STATUS_9D);
    offStatus(STATUS_A0);
    offStatus(STATUS_A8);
    offStatus(STATUS_A9);
    offStatus(STATUS_CAN_LAND);
    offStatus(STATUS_PROPEL_SLOW_FALL);
    offStatus(STATUS_92);
    setScrollMode(0);
}

void dAcPy_c::executeState_Propel() {
    offStatus(STATUS_92);
    if (mSubstateValue == 0) {
        if (mPowerup != POWERUP_PROPELLER_SHROOM) {
            changeState(StateID_Fall, false);
            return;
        }
    } else if (getCarryPropelActor() == nullptr) {
        changeState(StateID_Fall, false);
        return;
    }
    if (mPropelRollSpeed != 0) {
        mAngle.y += mPropelRollSpeed;
        if (!mIsPropelFall) {
            mPropelRollSpeed -= 200;
        } else {
            mPropelRollSpeed -= 80;
        }
    }
    if (mPropelRollSpeed <= 0) {
        mPropelRollSpeed = 0;
        mAngle.y = getMukiAngle(mDirection);
        offStatus(STATUS_SPIN);
    }
    typedef void (dAcPy_c::*PropelActionProc)();
    static PropelActionProc l_PropelActionProc[] = {
        &dAcPy_c::PropelActionFly,
        &dAcPy_c::PropelActionFly,
        &dAcPy_c::PropelActionFall
    };
    (this->*l_PropelActionProc[mSubstate])();
    calcPropelMoveSpeedF();
    sLib::addCalcAngle(&mAngle.x.mAngle, 0, 0x100, 0x2000, 0x200);
    if (!checkCarryThrow() && setHipAttackAction()) {
        return;
    }
}

void dAcPy_c::updatePropelParts() {
    if (!isStatus(STATUS_45)) {
        if (isStatus(STATUS_PROPEL)) {
            updatePropelFlyPartsMove();
        } else if (isLiftUp()) {
            mpPropelParts->mMode = 3;
        } else if (isStatus(STATUS_70)) {
            mpPropelParts->mMode = 5;
        } else {
            mpPropelParts->mMode = 4;
        }
    }
    mMtx_c mtx;
    mPyMdlMng.mpMdl->getHeadPropelJointMtx(&mtx);
    mpPropelParts->update(mtx);
    if (!mpPropelParts->isMode(dPropelParts_c::PROPEL_MODE_3) && !isStatus(STATUS_PROPEL_NO_ROLL)) {
        mPyMdlMng.mpMdl->setPropelRollSpeed(mpPropelParts->mRollSpeed);
    } else {
        mPyMdlMng.mpMdl->setPropelRollSpeed(0);
        mPyMdlMng.mpMdl->setPropelRollAngle(0);
    }
    mPyMdlMng.mpMdl->setPropelScale(mpPropelParts->mScale);
}

void dAcPy_c::updatePropelFlyPartsMove() {
    int v = 4;
    if (isStatus(STATUS_PROPEL)) {
        switch (mSubstate) {
            case 0:
            case 1:
                v = 0;
                break;
            default:
                v = 2;
                if (mPropelRollSpeed < 3000) {
                    v = 1;
                }
                break;
        }
    }
    dActor_c *propelActor = getCarryPropelActor();
    if (propelActor == nullptr) {
        mpPropelParts->mMode = v;
    } else {
        mpPropelParts->mMode = 3;
        propelActor->mpPropelParts->mMode = v;
    }
}

void dAcPy_c::endPropelFlyPartsMove() {
    dActor_c *propelActor = getCarryPropelActor();
    if (propelActor != nullptr) {
        propelActor->mpPropelParts->finalizePropelFly(mPropelRollSpeed);
    }
    mpPropelParts->finalizePropelFly(mPropelRollSpeed);
}

void dAcPy_c::setPropelSpinSmokeEffect() {
    mVec3_c efPos = mPos;
    efPos.y += 10.0f;
    if (dMaskMng::isCaveMask() && mLayer == 0 && mAmiLayer == 1) {
        efPos.z = 2000.0f;
    }
    float height;
    if (dBc_c::checkGround(&efPos, &height, mLayer, 1, -1) && mPos.y < height + 64.0f) {
        efPos.y = height;
        dEf::createPlayerEffect(mPlayerNo, &mLevelEf3, "Wm_mr_spinsmoke", 0, &efPos, nullptr, nullptr);
    }
}

void dAcPy_c::setStartPropelJumpEffect() {
    if (isNowBgCross(BGC_FOOT)) {
        mVec3_c efPos = mPos;
        if (dMaskMng::isCaveMask() && mLayer == 0 && mAmiLayer == 1) {
            efPos.z = 2000.0f;
        }
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_spindepart", 0, &efPos, nullptr, nullptr);
    }
}

void dAcPy_c::setPropelFallSpinEffect() {
    startSound(SE_PLY_PRPL_LETDOWN_SPIN, false);
    if (m_538 == 0) {
        m_538 = 1;
    }
}

bool dAcPy_c::calcPropelFallSpinEffect() {
    if (m_53c != 0) {
        m_53c--;
    }
    switch (m_538) {
        case 1:
        case 2:
        case 3: {
            if (isStatus(STATUS_PROPEL_SLOW_FALL)) {
                dAcPy_c *carryPlayer = getCarryPlayer();
                mVec3_c efPos;
                if (carryPlayer != nullptr) {
                    carryPlayer->getModel()->getJointPos(&efPos, 1);
                } else {
                    mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
                }
                if (m_538 == 1) {
                    dEf::createPlayerEffect(mPlayerNo, &mFollowEf, "Wm_mr_spinjump_re", 0, &efPos, nullptr, nullptr);
                    if (carryPlayer != nullptr) {
                        m_538 = 3;
                    } else {
                        m_538 = 2;
                    }
                    return true;
                }
                if (m_538 == 3 && carryPlayer == nullptr) {
                    stopPropelFallSpinEffect();
                    return false;
                }
                if (mFollowEf.follow(&efPos, nullptr, nullptr)) {
                    return true;
                }
            }
            m_538 = 4;
            m_53c = 20;
            break;
        }
        case 4:
            if (m_53c == 0) {
                m_538 = 0;
            }
            break;
    }
    return false;
}

void dAcPy_c::stopPropelFallSpinEffect() {
    mFollowEf.kill();
    m_53c = 0;
    m_538 = 0;
}

void dAcPy_c::initDemo_DownPose() {
    if (mPyMdlMng.mpMdl->mFlags & 4) {
        mPos.y = getAnkleCenterPos().y;
    }
    mPyMdlMng.setAnm(PLAYER_ANIM_DEAD_POSE);
    mSpeedF = 0.0f;
    mAngle.set(0, 0, 0);
    mPos.y += 2.0f;
    setZPosition(5000.0f);
    mMaxFallSpeed = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAccelY = 0.0f;
    calcModel();
}

void dAcPy_c::initDemo_DownFall() {
    switch ((DemoDownArg_e) mDemoStateArg) {
        case DEMO_DOWN_ARG_HIT:
            startPlayerVoice(VOICE_DAMAGE_LAST_2, 0);
            break;
        case DEMO_DOWN_ARG_TIME_UP:
            if (daPyMng_c::mTimeUpPlayerNum >= 2) {
                startPlayerVoice(VOICE_TIMEUP_MULTI, 0);
            } else {
                startPlayerVoice(VOICE_TIMEUP, 0);
            }
            break;
        case DEMO_DOWN_ARG_POISON:
        case DEMO_DOWN_ARG_POISON_FOG:
            startPlayerVoice(VOICE_DAMAGE_POISON, 0);
            break;
    }
    mPyMdlMng.setAnm(PLAYER_ANIM_DEAD);
    mSpeed.x = 0.0f;
    mSpeed.y = 3.25f;
    mSpeed.z = 0.0f;
    mMaxFallSpeed = sc_MaxFallSpeed;
    mAccelY = -0.125f;
}

void dAcPy_c::exeDemo_DownFall() {
    if (mSpeed.y > 2.5f) {
        mAccelY = -0.06f;
    } else if (mSpeed.y > 0.5f) {
        mAccelY = -0.25f;
    } else if (mSpeed.y > -0.5f) {
        mAccelY = -0.047f;
    } else {
        mAccelY = -0.1f;
    }
    mPos.y += mSpeed.y;
    calcFallSpeed();
}

void dAcPy_c::checkRest() {
    if (isItemKinopio()) {
        deleteRequest();
        return;
    }
    if (!m_7c) {
        setBalloonInDamage();
        return;
    }
    if (isStatus(STATUS_ALL_DOWN_FADE)) {
        return;
    }
    onStatus(STATUS_ALL_DOWN_FADE);
    bool allDown = true;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (daPyMng_c::mPlayerEntry[i] && daPyMng_c::mRest[daPyMng_c::mPlayerType[i]] != 0) {
            allDown = false;
            break;
        }
    }
    if (!allDown) {
        dScStage_c::setNextScene(3, 0, dScStage_c::EXIT_1, dFader_c::FADER_BOWSER);
    } else {
        dScStage_c::setNextScene(9, 0, dScStage_c::EXIT_1, dFader_c::FADER_BOWSER);
    }
}

void dAcPy_c::initPlayerDownCommon() {
    m_7c = 0;
    if (daPyMng_c::decNum(mPlayerNo) && daPyMng_c::mNum == 0) {
        m_7c = 1;
    }
    changeState(StateID_None);
    releaseCarryActor();
    resetMissSpin();
    endStar();
    setScrollMode(3);
    onStatus(STATUS_OUT_OF_PLAY);
    mDamageInvulnTimer = 0;
    mStarTimer = 0;
    mVisible = true;
    if (!isItemKinopio()) {
        daPyMng_c::decRest(mPlayerNo);
        if (m_7c) {
            SndSceneMgr::sInstance->fn_8019be60(1);
        }
    }
    mDemoSubstateTimer = 30;
}

void dAcPy_c::stopOtherDownDemo() {
    if (isItemKinopio()) {
        return;
    }
    if (!m_7c) {
        stopOther();
        return;
    }
    daPyDemoMng_c::mspInstance->m_84 = mPlayerNo;
    mExecStopReq |= 0b1111;

    dAcPy_c *player;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        player = daPyMng_c::getPlayer(i);
        if (player != nullptr && !player->isStatus(STATUS_53) && !player->isItemKinopio()) {
            player->mExecStopMask &= ~BIT_FLAG(1);
        }
        daYoshi_c *yoshi = daPyMng_c::getYoshi(i);
        if (yoshi != nullptr) {
            yoshi->mExecStopMask &= ~BIT_FLAG(1);
        }
    }
    mExecStopReq |= 0b1101;
}

void dAcPy_c::playOtherDownDemo() {
    if (!isItemKinopio() && daPyDemoMng_c::mspInstance->m_84 == -1 && !m_7c) {
        playOther();
    }
}

bool dAcPy_c::setTimeOverDemo() {
    if (
        !isStatus(STATUS_53) &&
        !isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE) &&
        !isStatus(STATUS_OUT_OF_PLAY) &&
        !isStatus(STATUS_STUNNED)
    ) {
        changeDemoState(StateID_DemoDown, DEMO_DOWN_ARG_TIME_UP);
        return true;
    }
    return false;
}

void dAcPy_c::initializeState_DemoDown() {
    initPlayerDownCommon();
    initDemo_DownPose();
    startSound(SE_PLY_DOWN, false);
    switch ((DemoDownArg_e) mDemoStateArg) {
        case DEMO_DOWN_ARG_HIT:
        case DEMO_DOWN_ARG_POISON_FOG:
            stopOtherDownDemo();
            break;
        case DEMO_DOWN_ARG_POISON: {
            mVec3_c efPos(
                mPos.x,
                mWaterHeight,
                mPos.z
            );
            dEf::createPlayerEffect(mPlayerNo, "Wm_mr_poisonwave", nullptr, &efPos, nullptr, nullptr);
            startSound(SE_PLY_SPLASH_POISON, 0);
            dBg_c::m_bg_p->setWaterInWave(mPos.x, mWaterHeight, 20);
            stopOtherDownDemo();
            break;
        }
        case DEMO_DOWN_ARG_TIME_UP:
            daPyMng_c::mTimeUpPlayerNum++;
            stopOtherDownDemo();
            break;
    }
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
}

void dAcPy_c::finalizeState_DemoDown() {
    mMaxFallSpeed = sc_MaxFallSpeed;
}

void dAcPy_c::executeState_DemoDown() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0) {
                daPyMng_c::startMissBGM(mPlayerNo);
                mDemoSubstate = 1;
                initDemo_DownFall();
                if (dBg_c::m_bg_p->m_90009 == 3) {
                    mMaxFallSpeed = sc_MaxFallDownSpeed;
                }
                if ((int) mDemoStateArg != 1) {
                    playOtherDownDemo();
                    mDemoSubstateTimer = 80;
                } else {
                    mDemoSubstateTimer = 150;
                }
            }
            break;
        case 1: {
            exeDemo_DownFall();
            if (!m_7c) {
                dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
                dBg_c *bg = dBg_c::m_bg_p;
                float t = getVisTop();
                u8 val = bg->m_90009;
                float f = bgParam->yStart() - bgParam->ySize() - 24.0f;
                if (val == 3) {
                    f = bgParam->yStart() - bgParam->ySize() - 8.0f;
                }
                if (t < f) {
                    checkRest();
                    mDemoSubstate = 2;
                    break;
                }
                if (val != 3) {
                    break;
                }
                if (getVisBottom() > bgParam->yStart() + 64.0f) {
                    checkRest();
                    mDemoSubstate = 2;
                }
            } else if (mDemoSubstateTimer == 0) {
                checkRest();
                mDemoSubstate = 2;
            }
            break;
        }
        case 2:
            exeDemo_DownFall();
    }
    checkSideViewLemit();
}

void dAcPy_c::setFallDownDemo() {
    if (isDispOutCheckOn()) {
        mAccelY = 0.0f;
        mSpeedF = 0.0f;
        mBgPushForce.set(0.0f, 0.0f, 0.0f);
        mExtraPushForceX = 0.0f;
        mSpeed.y = 0.0f;
        changeDemoState(StateID_DemoFallDown, 0);
    }
}

void dAcPy_c::initializeState_DemoFallDown() {
    initPlayerDownCommon();
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP2, 1.0f, 0.0f, 0.0f);
    mAccelY = 0.0f;
    mSpeedF = 0.0f;
    mBgPushForce.set(0.0f, 0.0f, 0.0f);
    mExtraPushForceX = 0.0f;
    mSpeed.y = 0.0f;
    mAngle.y = 0;
    setZPosition(5000.0f);
    float f = dBgParameter_c::ms_Instance_p->getLoopScrollDispPosX(mPos.x);
    float f1 = f - 96.0f;
    float f2 = f + dBgParameter_c::ms_Instance_p->xSize() + 96.0f;
    if (f1 <= mPos.x && mPos.x < f2) {
        startSound(SE_PLY_DOWN, false);
    }
    stopOtherDownDemo();
    onStatus(STATUS_INVISIBLE);
    mVisible = false;
}

void dAcPy_c::finalizeState_DemoFallDown() {
    offStatus(STATUS_INVISIBLE);
}

void dAcPy_c::executeState_DemoFallDown() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0) {
                mDemoSubstate = 1;
                daPyMng_c::startMissBGM(mPlayerNo);
                if (!m_7c) {
                    playOtherDownDemo();
                    mDemoSubstateTimer = 50;
                } else {
                    mDemoSubstateTimer = 80;
                }
            }
            break;
        case 1:
            if (mDemoSubstateTimer == 0) {
                checkRest();
                mDemoSubstate = 2;
            }
    }
    checkSideViewLemit();
}

void dAcPy_c::initializeState_DemoFireDown() {
    dScStage_c *stage = dScStage_c::m_instance;
    initPlayerDownCommon();
    if (mBgDamageType == 7) {
        mVec3_c efPos(
            mPos.x,
            mWaterHeight,
            6500.0f
        );
        if (stage != nullptr) {
            if (
                stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_3 ||
                stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 3
            ) {
                dEf::createPlayerEffect(mPlayerNo, "Wm_en_cmnmagmawave", 0, &efPos, nullptr, nullptr);
            } else {
                dEf::createPlayerEffect(mPlayerNo, "Wm_mr_magmawave", 0, &efPos, nullptr, nullptr);
            }
        }
        dBg_c::m_bg_p->setWaterInWave(mPos.x, mWaterHeight, 13);
    }
    initDemo_DownPose();
    setZPosition(3000.0f);
    startSound(SE_PLY_SPLASH_LAVA, false);
    startSound(SE_PLY_DOWN, false);
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
    stopOtherDownDemo();
}

void dAcPy_c::finalizeState_DemoFireDown() {}

void dAcPy_c::executeState_DemoFireDown() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0) {
                daPyMng_c::startMissBGM(mPlayerNo);
                mDemoSubstateTimer = 80;
                startPlayerVoice(VOICE_DAMAGE_FIRE, 0);
                startSound(SE_PLY_BURN_DOWN, false);
                setZPosition(5000.0f);
                mPyMdlMng.setAnm(PLAYER_ANIM_FIREJMP);
                mSpeed.x = 0.0f;
                mSpeed.y = 3.25f;
                mSpeed.z = 0.0f;
                mMaxFallSpeed = sc_MaxFallSpeed;
                mAccelY = -0.125f;
                mAngle.y = getMukiAngle(mDirection);
                mDemoSubstate = 1;
                playOtherDownDemo();
            }
            break;
        case 1:
            exeDemo_DownFall();
            if (!m_7c) {
                dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
                float edgePos = getVisTop();
                float bgBottom = bgParam->yStart() - bgParam->ySize() - 24.0f;
                if (edgePos < bgBottom) {
                    checkRest();
                    mDemoSubstate = 2;
                }
            } else if (mDemoSubstateTimer == 0) {
                checkRest();
                mDemoSubstate = 2;
            }
            break;
        case 2:
            exeDemo_DownFall();
            break;
    }
    if (mDemoSubstate != 0) {
        mVec3_c efScale(1.0f, 1.0f, 1.0f);
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            efScale.set(0.6f, 0.6f, 0.6f);
        }
        dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, "Wm_mr_atitismoke", 0, &mPos, nullptr, &efScale);
    }
    checkSideViewLemit();
}

void dAcPy_c::setPlayerEatDie() {
    m_15a8 = 0;
    if ((int) mDemoStateArg == 12) {
        m_15a8 = 1;
    } else {
        if (!isStar() && (mPowerup == POWERUP_NONE || mPowerup == POWERUP_MINI_MUSHROOM)) {
            m_15a8 = 1;
        }
    }
}

void dAcPy_c::initializeState_DemoEatDie() {
    setPlayerEatDie();
    if (m_15a8 != 0) {
        initPlayerDownCommon();
    }
    mRelatedActorID = BASE_ID_NULL;
    mSpeedF = 0.0f;
    mMaxFallSpeed = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAccelY = 0.0f;
    mDemoSubstate = 0;
}

void dAcPy_c::finalizeState_DemoEatDie() {
    offStatus(STATUS_INVISIBLE);
    offStatus(STATUS_OUT_OF_PLAY);
}

void dAcPy_c::executeState_DemoEatDie() {
    switch (mDemoSubstate) {
        case 2:
            if (mDemoSubstateTimer == 0) {
                if (m_15a8 != 0) {
                    startSound(SE_PLY_DOWN, false);
                } else if (!isStar()) {
                    if (mPowerup == POWERUP_MUSHROOM) {
                        fn_801416c0(POWERUP_NONE);
                    } else {
                        fn_801416c0(POWERUP_MUSHROOM);
                    }
                    startSound(SE_PLY_CHANGE_SMALL, false);
                    mDamageInvulnTimer = 127;
                }
                mDemoSubstate = 1;
            }
            break;
        case 0:
        case 1:
            if (mRelatedActorID == 0 || fManager_c::searchBaseByID(mRelatedActorID) == nullptr) {
                releaseEatDie();
            }
            break;
        case 3:
            if (mDemoSubstateTimer == 0) {
                checkRest();
                mDemoSubstate = 4;
            }
            break;
    }
}

void dAcPy_c::releaseEatDie() {
    if (!isItemKinopio() && m_7c) {
        SndSceneMgr::sInstance->fn_8019bd90(1);
    }
    changeNormalAction();
    checkDisplayOutDead();
}

bool dAcPy_c::setEatDieHide() {
    if (isDemoState(StateID_DemoEatDie)) {
        mPyMdlMng.mpMdl->offStarEffect();
        onStatus(STATUS_INVISIBLE);
        mDemoSubstate = 1;
        return true;
    }
    return false;
}

bool dAcPy_c::setEatDieFadeOut() {
    if (isDemoState(StateID_DemoEatDie)) {
        daPyMng_c::startMissBGM(mPlayerNo);
        mDemoSubstate = 3;
        mDemoSubstateTimer = 80;
        return true;
    }
    return false;
}

bool dAcPy_c::setEatDieSpitOut() {
    if (isDemoState(StateID_DemoEatDie)) {
        releaseEatDie();
        return true;
    }
    return false;
}

void dAcPy_c::setEatDieScreamVoice() {
    if (isDemoState(StateID_DemoEatDie)) {
        startPlayerVoice(VOICE_DAMAGE_EATEN, 0);
        mDemoSubstate = 2;
        mDemoSubstateTimer = 30;
    }
}

bool dAcPy_c::searchDoorActor() {
    dActor_c *door = nullptr;
    while ((door = (dActor_c *) fManager_c::searchBaseByGroupType(ACTOR, door)) != nullptr) {
        if (
            door->mProfName == fProfile::EN_DOOR ||
            door->mProfName == fProfile::EN_OBAKEDOOR ||
            door->mProfName == fProfile::EN_TORIDEDOOR ||
            door->mProfName == fProfile::EN_CASTLEDOOR
        ) {
            if (std::fabs(door->mPos.x - mPos.x) < 16.0f && std::fabs(door->mPos.y - mPos.y) < 16.0f) {
                mRelatedActorID = door->mUniqueID;
                return true;
            }
        }
    }
    mRelatedActorID = BASE_ID_NULL;
    return false;
}

void dAcPy_c::initializeState_DemoInDoor() {
    onStatus(STATUS_INVISIBLE);
    m_74 = 255;
    changeState(StateID_Walk, BLEND_DEFAULT);
    mPyMdlMng.setAnm(PLAYER_ANIM_DOOR_WALK);
    mAngle.y = 0;
    if (mLayer == 0) {
        setZPosition(2800.0f);
    } else {
        setZPosition(-2000.0f);
    }
    mDamageInvulnTimer = 35;
    searchDoorActor();
    if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo) != 0) {
        mDemoSubstate = 0;
    } else {
        mDemoSubstate = 2;
    }
}

void dAcPy_c::finalizeState_DemoInDoor() {
    mDamageInvulnTimer = 0;
    mPowerupChangeInvulnTimer = 0;
    m_74 = 0;
    mRelatedActorID = BASE_ID_NULL;
}

void dAcPy_c::setDemoInDoor_Walk() {
    offStatus(STATUS_INVISIBLE);
    mDemoSubstate = 4;
    mDemoSubstateTimer = 10;
}

void dAcPy_c::executeState_DemoInDoor() {
    daEnDoor_c *door = nullptr;
    if (mRelatedActorID != BASE_ID_NULL) {
        door = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
    }
    if (door == nullptr) {
        if (!mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
            searchDoorActor();
        } else {
            changeNormalAction();
        }
        return;
    }
    switch (mDemoSubstate) {
        case 0:
            if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                door->m_5d4 = 1;
                if (door->m_5cc == 0) {
                    door->m_5cc = 2;
                }
                mDemoSubstate++;
            }
            break;
        case 1:
            if (door->m_5d0 != 0) {
                setDemoInDoor_Walk();
            }
            break;
        case 2:
            if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                setDemoInDoor_Walk();
            }
            break;
        case 4:
            sLib::chase(&m_74, 0, 15);
            onStatus(STATUS_79);
            if (mDemoSubstateTimer == 0) {
                if (daPyDemoMng_c::mspInstance->getNextDemoNo() == -1) {
                    mDemoSubstate = 5;
                    if (mRelatedActorID != BASE_ID_NULL) {
                        daEnDoor_c *tmpDoor = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
                        if (tmpDoor != nullptr && (tmpDoor->m_5cc == 1 || tmpDoor->m_5cc == 2)) {
                            tmpDoor->m_5cc = 3;
                        }
                    }
                } else {
                    mDemoSubstate = 6;
                }
            }
            break;
        case 5:
            sLib::chase(&m_74, 0, 15);
            onStatus(STATUS_79);
            if (door->isClosed()) {
                mDemoSubstate = 6;
            }
            break;
        case 6:
            sLib::chase(&m_74, 0, 15);
            onStatus(STATUS_79);
            short ang = getMukiAngle(mDirection);
            if (!addCalcAngleY(ang, 10)) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
                mAngle.y = ang;
                changeNormalAction();
            }
    }
}

void dAcPy_c::initializeState_DemoInJump() {
    offStatus(STATUS_INVISIBLE);
    mDemoSubstateTimer = 30;
    mPyMdlMng.setAnm(PLAYER_ANIM_JUMP, 0.0f, 0.0f);
    u8 jumpDir = (int) mDemoStateArg;
    mDirection = jumpDir;
    mSpeedF = sc_DirSpeed[jumpDir] * 0.5f;
    mMaxSpeedF = sc_DirSpeed[jumpDir] * 0.5f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        mSpeed.y = 4.2f;
    } else {
        mSpeed.y = 5.0f;
    }
    mMaxFallSpeed = sc_MaxFallSpeed;
    setButtonJumpGravity();
    mAngle.y = getMukiAngle(mDirection);
}

void dAcPy_c::finalizeState_DemoInJump() {}

void dAcPy_c::executeState_DemoInJump() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0 && daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                mDemoSubstateTimer = 30;
                mDemoSubstate++;
            }
            break;
        case 1:
            if (mDemoSubstateTimer == 0) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
                mDemoSubstateTimer = 95;
                mDemoSubstate++;
                int world = dScStage_c::m_instance->mCurrWorld;
                if (world == WORLD_4 || world == WORLD_6 || world == WORLD_8) {
                    if (dScStage_c::m_instance->mCurrCourse == STAGE_DOOMSHIP) {
                        startPlayerVoice(VOICE_JR_A_BATTLE_APP, 0);
                    }
                }
            }
            break;
        case 2:
            setButtonJumpGravity();
            calcSpeedXY();
            posMove();
            if (mSpeed.y < 0.0f) {
                changeNormalAction();
                changeState(StateID_Fall, false);
            }
            break;
    }
}

void dAcPy_c::initializeState_DemoInVine() {
    mPyMdlMng.setAnm(PLAYER_ANIM_PEA_PLANT, 0.0f, 0.0f);
    offStatus(STATUS_INVISIBLE);
    mDemoSubstateTimer = 30;
    mDirection = DIR_LR_R;
    mAngle.y = 0x8000;
    dEnemyMng_c::m_instance->demo_ivy_create(&mPos);
}

void dAcPy_c::finalizeState_DemoInVine() {}

void dAcPy_c::executeState_DemoInVine() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0 && daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo) != 0) {
                mDemoSubstateTimer = 30;
                mDemoSubstate++;
            }
            break;
        case 1:
            if (mDemoSubstateTimer == 0) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
                mDemoSubstateTimer = 95;
                mDemoSubstate++;
            }
            break;
        case 2:
            mPos.y += 1.0f;
            setIvyHangEffect();
            setVineWalkSE();
            mBc.checkHead(0);
            if (mDemoSubstateTimer == 0) {
                changeNormalAction();
                changeState(StateID_Vine, (void *) 1);
            }
            break;
    }
}

bool dAcPy_c::isDoorDemoEnable() {
    if (!isEnableDokanInStatus()) {
        return false;
    }
    if (mKey.buttonDoor()) {
        if (isDemoState(StateID_DemoNone) && isOldBgCross(BGC_FOOT) && ((int) mPos.y & 0xf) == 0) {
            return true;
        }
    }
    return false;
}

bool dAcPy_c::setDoorDemo(dActor_c *door) {
    if (isDoorDemoEnable()) {
        mVec3_c doorEnterPos(
            door->mPos.x,
            (int) mPos.y,
            mPos.z
        );
        int nextGoto;
        if (dNext_c::m_instance->searchNextNum(dScStage_c::m_instance->mCurrFile, doorEnterPos.x, doorEnterPos.y + 1.0f, &nextGoto)) {
            mDokanEnterNextGotoID = nextGoto;
            if (dNext_c::m_instance->fn_800cfed0(dScStage_c::m_instance->mCurrFile, mDokanEnterNextGotoID)) {
                return false;
            }
            if (daPyDemoMng_c::mspInstance->m_5c != 0) {
                return false;
            }
            dNext_c::m_instance->setChangeSceneNextDat(dScStage_c::m_instance->mCurrFile, mDokanEnterNextGotoID, dFader_c::FADER_CIRCLE_TARGET);
            mRelatedActorID = door->mUniqueID;
            mWarpPos = doorEnterPos;
            mDoorSize = 0;
            int profName = door->mProfName;
            if (
                profName == fProfile::EN_TORIDEDOOR ||
                profName == fProfile::EN_CASTLEDOOR
                // || profName == fProfile::EN_KOOPADOOR
            ) {
                mDoorSize = 1;
            }
            changeDemoState(StateID_DemoOutDoor, 0);
            changeState(StateID_None);
            return true;
        }
    }
    return false;
}

void dAcPy_c::initializeState_DemoOutDoor() {
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        releaseCarryActor();
    }
    mDemoSubstate = DEMO_OUT_DOOR_OPEN_DOOR;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    float z = daPyDemoMng_c::mspInstance->get_88() * 64.0f;
    daPyDemoMng_c::mspInstance->inc_88();
    if (mLayer == 0) {
        setZPosition(2800.0f + z);
    } else {
        setZPosition(-2000.0f + z);
    }
    mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
    mAngle.x = 0;
    if (daPyMng_c::mNum == 1) {
        stopOther();
    }
    mVec3_c pos(mWarpPos.x, mWarpPos.y, 0.0f);
    if (mRelatedActorID != 0) {
        daEnDoor_c *door = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
        if (door != nullptr && door->m_5cc == 0) {
            door->m_5cc = 1;
        }
    }
}

void dAcPy_c::finalizeState_DemoOutDoor() {
    mRelatedActorID = BASE_ID_NULL;
}

void dAcPy_c::exeDemoOutDoor_OpenDoor() {
    addCalcAngleY(0x8000, 2);
    if (mRelatedActorID != 0) {
        daEnDoor_c *door = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
        if (door != nullptr && door->m_5d0 != 0) {
            static const float scDoorEnterWidth[] = { 8.0f, 10.0f };
            if (std::fabs(mPos.x - mWarpPos.x) > scDoorEnterWidth[mDoorSize]) {
                initDemoOutDoor_MoveCenter();
            } else {
                initDemoOutDoor_MoveInter();
            }
        }
    }
}

void dAcPy_c::initDemoOutDoor_MoveCenter() {
    mDemoSubstateTimer = 10;
    float speed = std::fabs(mPos.x - mWarpPos.x) / mDemoSubstateTimer;
    if (speed > 1.0f) {
        speed = 1.0f;
    }
    mSpeedF = speed;
    mDemoSubstate = DEMO_OUT_DOOR_MOVE_CENTER;

    float animSpeed = (speed * 2.0f < 2.0f) ? 2.0f : speed * 2.0f;
    mPyMdlMng.setAnm(PLAYER_ANIM_RUN, animSpeed, 0.0f, 0.0f);
}

void dAcPy_c::exeDemoOutDoor_MoveCenter() {
    if (std::fabs(mPos.x - mWarpPos.x) < 1.0f) {
        initDemoOutDoor_MoveInter();
        return;
    }
    if (mPos.x < mWarpPos.x) {
        addCalcAngleY(getMukiAngle(DIR_LR_R), 2);
    } else {
        addCalcAngleY(getMukiAngle(DIR_LR_L), 2);
    }
    sLib::chase(&mPos.x, mWarpPos.x, mSpeedF);
}

void dAcPy_c::initDemoOutDoor_MoveInter() {
    mDemoSubstateTimer = 30;
    mDemoSubstate = DEMO_OUT_DOOR_MOVE_INTER;
    mPyMdlMng.setAnm(PLAYER_ANIM_DOOR_WALK);
    endPowerUpEffect();
}

void dAcPy_c::exeDemoOutDoor_MoveInter() {
    addCalcAngleY(0x8000, 2);
    sLib::chase(&m_1598, 2.0f, 0.2f);
    sLib::chase(&m_74, 255, 15);
    if (mDoorSize == 0) {
        sLib::chase(&mPos.x, mWarpPos.x, 0.4f);
    }
    if (mDemoSubstateTimer != 0) {
        return;
    }
    bool hasNonEnteredPlayer = false;
    int count = 0;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getCtrlPlayer(i);
        if (player == nullptr || player == this) {
            continue;
        }
        if (daPyMng_c::mActPlayerInfo & (1 << (u8) i)) {
            if (!player->isStatus(STATUS_64)) {
                hasNonEnteredPlayer = true;
            }
        } else {
            if (player->isStatus(STATUS_53)) {
                count++;
            }
        }
    }
    if (!hasNonEnteredPlayer && count == 0) {
        if (mRelatedActorID != BASE_ID_NULL) {
            daEnDoor_c *door = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
            if (door != nullptr) {
                if ((door->m_5cc == 1 || door->m_5cc == 2)) {
                    door->m_5cc = 3;
                }
                mDemoSubstate = DEMO_OUT_DOOR_WAIT_CLOSE;
            }
        }
    } else {
        mDemoSubstate = DEMO_OUT_DOOR_WAIT_ENTER;
    }
    m_1598 = 0.0f;
    onStatus(STATUS_INVISIBLE);
    mDemoSubstateTimer = 20;
}

void dAcPy_c::exeDemoOutDoor_WaitClose() {
    addCalcAngleY(0x8000, 2);
    if (mDemoSubstateTimer != 0) {
        return;
    }
    if (mRelatedActorID != nullptr) {
        daEnDoor_c *door = (daEnDoor_c *) fManager_c::searchBaseByID(mRelatedActorID);
        if (door != nullptr && door->isClosed()) {
            changeNextScene(1);
            mDemoSubstate = DEMO_OUT_DOOR_FINISHED;
        }
    }
}

void dAcPy_c::exeDemoOutDoor_WaitEnter() {
    if (mDemoSubstateTimer == 0) {
        changeNextScene(1);
        mDemoSubstate = DEMO_OUT_DOOR_FINISHED;
    }
}

void dAcPy_c::executeState_DemoOutDoor() {
    typedef void (dAcPy_c::*DemoOutDoorProc)();
    static DemoOutDoorProc sDemoOutDoorProc[] = {
        &dAcPy_c::exeDemoOutDoor_OpenDoor,
        &dAcPy_c::exeDemoOutDoor_MoveCenter,
        &dAcPy_c::exeDemoOutDoor_MoveInter,
        &dAcPy_c::exeDemoOutDoor_WaitClose,
        &dAcPy_c::exeDemoOutDoor_WaitEnter,
    };
    if (mDemoSubstate != DEMO_OUT_DOOR_FINISHED) {
        (this->*sDemoOutDoorProc[mDemoSubstate])();
    }
}

bool dAcPy_c::initDemoDokanCannon(mVec3_c &pos, int cannonMode) {
    if (isEnableDokanInStatus()) {
        m_68 = cannonMode;
        mWarpPos = pos;
        changeDemoState(StateID_DemoDokanCannon, 0);
        changeState(StateID_None);
        return true;
    }
    return false;
}

void dAcPy_c::endDemoDokanCannon(mVec3_c &pos) {
    mPos.x = pos.x;
    mPos.y = pos.y;
    changeDemoState(StateID_DemoNone, false);
}

bool dAcPy_c::isDokanCannonIn() {
    if (isDemoState(StateID_DemoDokanCannon) && isStatus(STATUS_INVISIBLE)) {
        return true;
    }
    return false;
}

void dAcPy_c::initializeState_DemoDokanCannon() {
    onStatus(STATUS_5A);
    mpMdlMng->setAnm(PLAYER_ANIM_WAIT, 0.0f, 5.0f, scDokanWaitAnmFixFrame);
    initDemoOutDokan();
}

void dAcPy_c::finalizeState_DemoDokanCannon() {
    offStatus(STATUS_5A);
    offStatus(STATUS_INVISIBLE);
}

void dAcPy_c::executeState_DemoDokanCannon() {
    switch (mDemoSubstate) {
        case 0:
            sLib::chase(&mPos.x, mWarpPos.x, scDokanInSpeedX);
            if (std::fabs(mPos.x - mWarpPos.x) <= scDokanInWidthX) {
                if (m_68 != 2) {
                    setScrollMode(4);
                }
                mDemoSubstate++;
            }
            break;
        case 1:
            if (sLib::chase(&mPos.y, mWarpPos.y - 32.0f, scDokanInMoveSpeed)) {
                onStatus(STATUS_INVISIBLE);
                mDemoSubstate++;
            }
            break;
        case 2:
            checkDisplayOutDead();
            break;
    }
}

int dAcPy_c::setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType) {
    daYoshi_c *yoshi = getRideYoshi();
    if (yoshi != nullptr) {
        return yoshi->setDemoGoal(landPos, goalCastleX, goalType);
    } else {
        return daPlBase_c::setDemoGoal(landPos, goalCastleX, goalType);
    }
}

void dAcPy_c::setOffYoshiInGoal(daPlBase_c *yoshi) {
    changeState(StateID_RideOffJump, 0);
    changeDemoState(StateID_DemoGoal, true);
    mGoalDemoIndex = yoshi->mGoalDemoIndex;
    mGoalTouchOrder = yoshi->mGoalTouchOrder;
    mWarpPos = yoshi->mWarpPos;
}

bool dAcPy_c::setHideNotGoalPlayer() {
    if (isStatus(STATUS_OUT_OF_PLAY) || isStatus(STATUS_53) || isItemKinopio()) {
        return false;
    }
    return daPlBase_c::setHideNotGoalPlayer();
}

void dAcPy_c::initDemoGoalBase() {
    mDirection = DIR_LR_R;
    endPowerUpEffect();
    dActor_c *carryActor = (dActor_c *) fManager_c::searchBaseByID(mCarryActorID);
    if (carryActor != nullptr) {
        carryActor->mPos.x = mPos.x;
    }
    releaseCarryActor();
    daPlBase_c::initDemoGoalBase();
    if ((int) mDemoStateArg == 1) {
        setDemoGoalMode(3, 6);
        mPyMdlMng.setAnm(PLAYER_ANIM_GET_DOWN, 0.0f, 0.0f);
        mSpeed.y = sc_JumpSpeed - 0.2f;
        mSpeedF = 1.0f;
        mDemoState = CONTROL_DEMO_KINOPIO_SWIM;
        mDirection = DIR_LR_L;
        mAngle.y = getMukiAngle(DIR_LR_L);
    }
}

void dAcPy_c::executeDemoGoal_Run() {
    switch (mDemoState) {
        case 0:
            if (mGoalTouchOrder >= 3 && dGameCom::rnd() < 0.5f) {
                mDemoSubstateTimer = 60;
                mDemoState = 2;
            } else {
                mDemoSubstateTimer = mGoalTouchOrder * 5 + 10;
                mDemoState = 1;
            }
            for (int i = 0; i < PLAYER_COUNT; i++) {
                daYoshi_c *yoshi = daPyMng_c::getYoshi(i);
                if (yoshi != nullptr && yoshi->isStatus(STATUS_GOAL_POLE_TOUCHED)) {
                    mDemoSubstateTimer += 14;
                    break;
                }
            }
        case 1:
        case 2:
            if (mDemoSubstateTimer == 0) {
                daPyDemoMng_c::mspInstance->mFlags |= 0x20;
                if (mGoalTouchOrder == 0) {
                    daPyDemoMng_c::mspInstance->setDemoMode(daPyDemoMng_c::MODE_2, 0);
                }
                mDirection = DIR_LR_R;
                mSpeed.set(0.0f, 0.0f, 0.0f);
                mDemoSubstateTimer = 60;
                mSpeedF = 1.9499999f;
                float f = 2.6f;
                if (mPowerup == POWERUP_MINI_MUSHROOM) {
                    f = 2.0f * f;
                } else if (mDemoState == 2) {
                    mSpeedF = 2.25f;
                    f = 3.0f;
                }
                mPyMdlMng.setAnm(PLAYER_ANIM_RUN, f, 5.0f, 0.0f);
                mDemoState = 3;
            }
            break;
        case 3:
            turnAngle();
            if (mPos.x > mWarpPos.x) {
                float height;
                if (dBc_c::checkGround(&mPos, &height, 0, 1, -1)) {
                    if (mPos.y < height + 1.0f) {
                        mPos.y = height + 1.0f;
                    }
                }
                mPos.z = -1800.0f;
            }
            if (sLib::chase(&mPos.x, mWarpPos.x + 32.0f, mSpeedF)) {
                mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
                mDemoState = 4;
                onStatus(STATUS_6E);
            }
            break;
        case 4:
            mPos.z = -1800.0f;
            break;
        case 6:
            setNormalJumpGravity();
            mSpeed.y += mAccelY;
            if (mSpeed.y < sc_MaxFallSpeed) {
                mSpeed.y = sc_MaxFallSpeed;
            }
            mPos.y += mSpeed.y;
            mPos.x += mSpeedF;
            if (isNowBgCross(BGC_FOOT)) {
                mSpeedF = 0.0f;
                mPyMdlMng.setAnm(PLAYER_ANIM_JUMPED);
                setLandSE();
                mDemoState = 7;
            }
            break;
        case 7:
            if (mPyMdlMng.isAnmStop()) {
                mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
                mDemoState = 1;
                mDemoSubstateTimer = mGoalTouchOrder * 5;
            }
            break;
    }
}

bool dAcPy_c::updateDemoKimePose(ClearType_e clearType) {
    switch (mKimePoseMode) {
        case KIME_POSE_NONE:
            mPyMdlMng.setAnm(PLAYER_ANIM_WAIT);
            if (mPowerup == POWERUP_PROPELLER_SHROOM) {
                mKimePoseMode = KIME_POSE_PROPELLER;
                mPyMdlMng.setAnm(PLAYER_ANIM_PL_GOAL_PUTON_CAP);
            } else if (mPowerup == POWERUP_PENGUIN_SUIT) {
                mKimePoseMode = KIME_POSE_PENGUIN;
                mPyMdlMng.setAnm(PLAYER_ANIM_P_GOAL_PUTON_CAP);
            } else {
                if (
                    mPyMdlMng.mpMdl->m_151 == mPyMdlMng.mpMdl->get151CheckVal() ||
                    mPyMdlMng.mpMdl->m_151 == 3 ||
                    mPyMdlMng.mpMdl->m_151 == 4
                ) {
                    mKimePoseMode = KIME_POSE_NO_HAT;
                } else {
                    mKimePoseMode = KIME_POSE_WITH_HAT;
                }
                mPyMdlMng.setAnm(PLAYER_ANIM_GOAL_PUTON_CAP);
            }
            startKimePoseVoice(clearType);
            // fallthrough
        case KIME_POSE_WITH_HAT: {
            dMarioMdl_c *mdl = (dMarioMdl_c *) mPyMdlMng.mpMdl;
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(41.0f)) {
                mdl->fn_800cab00(1);
                break;
            }
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(107.0f)) {
                mdl->fn_800cab00(0);
                break;
            }
            if (mPyMdlMng.isAnmStop()) {
                offStatus(STATUS_6C);
                return true;
            }
            break;
        }
        case KIME_POSE_PROPELLER:
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(5.0f)) {
                onStatus(STATUS_70);
                break;
            }
            if (mPyMdlMng.mpMdl->mAnm.checkFrame(12.0f)) {
                offStatus(STATUS_70);
                break;
            }
            if (mPyMdlMng.isAnmStop()) {
                offStatus(STATUS_6C);
                return true;
            }
            break;
        case KIME_POSE_PENGUIN:
        case KIME_POSE_NO_HAT:
            if (mPyMdlMng.isAnmStop()) {
                offStatus(STATUS_6C);
                return true;
            }
            break;
    }
    return false;
}

void dAcPy_c::initializeDemoControl() {
    releaseCarryActor();
    if (isStatus(STATUS_72)) {
        clearPropelFlyUpTime();
    }
}

bool dAcPy_c::setDemoCannonWarp(int delay, short angX, short angY) {
    if (!isDemo()) {
        changeDemoState(StateID_DemoCannonWarp, 0);
        mAngle.x = angX + 0x2000;
        mAngle.y = angY + 0x6000;
        mDemoSubstateTimer = delay;
        return true;
    }
    return false;
}

void dAcPy_c::initializeState_DemoCannonWarp() {
    mPyMdlMng.setAnm(PLAYER_ANIM_SHOOT);
    mAccelY = 0.0f;
    mMaxFallSpeed = sc_MaxFallSpeed;
    m_78 = 60;
}

void dAcPy_c::finalizeState_DemoCannonWarp() {}

void dAcPy_c::executeState_DemoCannonWarp() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0) {
                mDemoSubstate = 1;
                mAccelF = 0.0f;
                mSpeed.y = mAng(mAngle.x).sin() * 4.0f;
                mSpeedF = mAng(mAngle.x).cos() * 4.0f;
                mSpeed.x = mSpeedF * mAng(mAngle.y).sin();
                mSpeed.z = mSpeedF * mAng(mAngle.y).cos();
                mAmiRelated2 = 1.0f;
                dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_5, 0, false);
                startPlayerVoice(VOICE_CANNON_SHOT, 0);
            }
            break;
        case 1: {
            int ang = cM::atan2s(std::fabs(mSpeedF), mSpeed.y);
            mAngle.x = 0x4000 - ang;
            if (m_78 != 0) {
                m_78--;
                mAccelY = 0.0f;
            } else {
                if (mSpeed.y < 1.0f) {
                    mAccelY = -0.0625f;
                } else {
                    mAccelY = -0.125f;
                }
            }
            mSpeed.y += mAccelY;
            if (mSpeed.y < sc_MaxFallSpeed) {
                mSpeed.y = sc_MaxFallSpeed;
            }
            posMove();
            mVec3_c efPos;
            mPyMdlMng.mpMdl->getJointPos(&efPos, 1);
            static const float sc_slipEffectScale[] = { 0.5f, 0.8f, 1.0f };
            float s = mAmiRelated2 * sc_slipEffectScale[getTallType(-1)];
            mVec3_c efScale(s, s, s);
            dEf::createPlayerEffect(mPlayerNo, &mSlipSmokeEffect, "Wm_mr_slipsmoke", 0, &efPos, nullptr, &efScale);
            if (sLib::chase(&mAmiRelated2, 0.0f, 0.008f)) {
                onStatus(STATUS_INVISIBLE);
                mDemoSubstate = 2;
                mDemoSubstateTimer = 30;
            }
            break;
        }
        case 2:
            if (mDemoSubstateTimer == 0) {
                mDemoSubstate = 3;
                mDemoSubstateTimer = 90;
                dEf::createPlayerEffect(mPlayerNo, "Wm_ob_warpcannonkira", 0, &mPos, nullptr, nullptr);
                dAudio::g_pSndObjMap->startSound(SE_OBJ_WARP_CANNON_SHINE, mPos, 0);
            }
            break;
        case 3:
            if (mDemoSubstateTimer == 0) {
                mDemoSubstate = 4;
                dScStage_c::setNextScene(3, 0, dScStage_c::EXIT_0, dFader_c::FADER_CIRCLE_TARGET);
                if (daPyDemoMng_c::mspInstance->mPlayerNo < 0) {
                    daPyDemoMng_c::mspInstance->mPlayerNo = mPlayerNo;
                }
            }
            break;
    }
}

bool dAcPy_c::isEnableDokanInStatus() {
    if (!daPlBase_c::isEnableDokanInStatus()) {
        return false;
    }

    if (isCarry() && getCarryPlayer() == nullptr) {
        return false;
    }

    if (getRidePlayer() != nullptr) {
        return false;
    }

    if (isChange()) {
        return false;
    }

    return !isStatus(STATUS_RIDE_YOSHI);
}

bool dAcPy_c::setDokanIn(DokanDir_e dir) {
    if (isStatus(STATUS_7E)) {
        return false;
    }

    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        int nextGotoNum;
        switch (dir) {
            case DOKAN_D:
                if (mKey.buttonDown() && mBc.checkDokanDown(&mWarpPos, &nextGotoNum)) {
                    if (setDemoOutDokanAction(nextGotoNum, dir)) {
                        return true;
                    }
                }
                break;
            case DOKAN_U:
                if (carryPlayer->mKey.buttonUp() && mBc.checkDokanUp(&mWarpPos, &nextGotoNum)) {
                    carryPlayer->mWarpPos = mWarpPos;
                    if (carryPlayer->setDemoOutDokanAction(nextGotoNum, dir)) {
                        return true;
                    }
                }
                break;
            default:
                break;
        }
        return false;
    }

    return daPlBase_c::setDokanIn(dir);
}

void dAcPy_c::initDemoOutDokan() {
    daPlBase_c::initDemoOutDokan();

    if (getCarryPlayer() != nullptr) {
        releaseCarryActor();
    }

    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer != nullptr) {
        ridePlayer->cancelCarry(this);
    }
}

void dAcPy_c::initialDoor() {
    changeDemoState(StateID_DemoInDoor, 0);
}

void dAcPy_c::initialJumpRight() {
    changeDemoState(StateID_DemoInJump, 0);
}

void dAcPy_c::initialJumpLeft() {
    changeDemoState(StateID_DemoInJump, 1);
}

void dAcPy_c::initialVine() {
    changeDemoState(StateID_DemoInVine, 0);
}

void dAcPy_c::initialSwim() {
    changeState(StateID_Swim, SWIM_ARG_INITIAL);
    changeDemoState(StateID_DemoNone, false);
    onDemo();
    mSpeed.y = -1.0f;
    bgCheck(0);
}

void dAcPy_c::initialBlockJump() {
    playOther();
    changeState(StateID_BlockJump, 0);
    changeDemoState(StateID_DemoControl, CONTROL_DEMO_WAIT);
}

void dAcPy_c::initialBlockJumpBelow() {
    playOther();
    changeState(StateID_BlockJump, (void *) 1); // [This might have initially meant to be StateID_HipAttack].
    changeDemoState(StateID_DemoControl, CONTROL_DEMO_WAIT);
}

void dAcPy_c::setCreateAction(int action) {
    mExecStopReq |= BIT_FLAG(0) | BIT_FLAG(3);
    if (daPyMng_c::isCreateBalloon(mPlayerNo) && setBalloonInNextgoto()) {
        return;
    }
    if (daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] & 2) {
        daYoshi_c *yoshi = daPyMng_c::createYoshi(mPos, daPyMng_c::getYoshiColor(mPlayerNo), this);
        if (yoshi != nullptr) {
            yoshi->mFruitCount = daPyMng_c::getYoshiFruit(mPlayerNo);
        }
    } else {
        daPlBase_c::setCreateAction(action);
    }
}

void dAcPy_c::initChangeInit() {
    if (!isStatus(STATUS_GOAL_POLE_TOUCHED)) {
        playOther();
    } else {
        playGoalOther();
    }
    dAudio::pauseOffMove(mPlayerNo);
}

bool dAcPy_c::executeChangeInit() {
    if (mPowerup == mPowerupCopy2) {
        return false;
    }
    setPowerup(mPowerupCopy2, 0);
    stopOther();
    dAudio::pauseMove(mPlayerNo);
    setChange(1);
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        float jumpSpeed = getJumpSpeed();
        if (mSpeed.y > jumpSpeed) {
            mSpeed.y = jumpSpeed;
        }
        releaseCarryActor();
    }
    return isPlayerGameStop();
}

int dAcPy_c::change_reverse_scale_set() {
    int res = 6 - mChangeTimer / 6;
    if (res < 0) {
        res = 0;
    }
    /// @unofficial
    static const float ratios[] = {
        0.4f, 0.2f, 0.6f, 0.4f, 0.8f, 0.6f, 1.0f
    };
    /// @unofficial
    struct Keyframe {
        float startScaleY, endScaleY;
        float startScaleX, endScaleX;
        float unk;
    };
    /// @unofficial
    static const Keyframe data[] = {
        {
                0.6f, 1.0f,
                0.6f, 1.0f,
                6.0f
            },
        {
                0.36f, 1.0f,
                0.6f, 1.0f,
                20.0f
            },
        {
                1.0f, 0.6f,
                1.0f, 0.6f,
                -10.0f
            },
        {
                0.6f, 1.0f,
                1.0f, 1.0f,
                10.0f
            },
        {
                1.0f, 0.36f,
                1.0f, 0.6f,
                -20.0f
            },
        {
                1.0f, 0.6f,
                1.0f, 1.0f,
                -10.0f
            },
        {
                1.0f, 1.0f,
                1.0f, 1.0f,
                0.0f
            },
        {
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f
            }
    };
    if (mChangeTimer != 0) {
        if (m_67 != 7) {
            float ratioA = 1.0f - ratios[res];
            float ratioB = ratios[res];
            Keyframe curr = data[m_67];
            float x = curr.startScaleX * ratioA + curr.endScaleX * ratioB;
            float y = curr.startScaleY * ratioA + curr.endScaleY * ratioB;
            mScale.set(x, y, x);
            m_159c = 0.0f;
            if (isStatus(STATUS_HANG)) {
                m_159c = curr.unk * ratioA;
            }
        }
    } else {
        mScale.set(1.0f, 1.0f, 1.0f);
        m_159c = 0.0f;
    }
    return res;
}

void dAcPy_c::initChangeNormal() {
    mChangeTimer = 30;
    int prevTallType = getTallType(mPowerupCopy);
    int currTallType = getTallType(-1);
    if (isStatus(STATUS_HANG)) {
        static const float offsets[3] = { 21.0f, 11.0f, 0.0f };
        mPos.y += offsets[currTallType] - offsets[prevTallType];
    }
    static const int tallTypeChange[3][3] = {
        {7, 7, 1},
        {2, 7, 3},
        {4, 5, 6}
    };
    m_67 = tallTypeChange[prevTallType][currTallType];
    switch (m_67) {
        case 0:
        case 1:
        case 3:
            if (mPowerup == POWERUP_PROPELLER_SHROOM) {
                startSound(SE_PLY_CHANGE_PRPL, false);
            } else if (mPowerup == POWERUP_PENGUIN_SUIT) {
                startSound(SE_PLY_CHANGE_PNGN, false);
            } else {
                startSound(SE_PLY_CHANGE_BIG, false);
            }
            break;
        case 2:
        case 4:
            startSound(SE_PLY_CHANGE_MAME, false);
            break;
        case 5:
        case 6:
            if (mPowerup == POWERUP_PROPELLER_SHROOM) {
                startSound(SE_PLY_CHANGE_PRPL, false);
            } else if (mPowerup == POWERUP_PENGUIN_SUIT) {
                startSound(SE_PLY_CHANGE_PNGN, false);
            } else if (mPowerup == POWERUP_FIRE_FLOWER || mPowerup == POWERUP_ICE_FLOWER) {
                startSound(SE_PLY_CHANGE_BIG, false);
            } else {
                startSound(SE_PLY_CHANGE_SMALL, false);
            }
            break;
    }
}

bool dAcPy_c::executeChangeNormal() {
    int frame = change_reverse_scale_set();
    PLAYER_POWERUP_e powerup = mPowerup;
    if (mChangeTimer != 0) {
        if ((frame & 1) != 0) {
            powerup = mPowerupCopy;
        }
        if (powerup == POWERUP_NONE || powerup == POWERUP_MINI_MUSHROOM) {
            powerup = mPowerup;
            if (
                mPowerup == POWERUP_NONE && mPowerupCopy == POWERUP_MINI_MUSHROOM ||
                mPowerup == POWERUP_MINI_MUSHROOM && mPowerupCopy == POWERUP_NONE
            ) {
                powerup = POWERUP_NONE;
            } else {
                powerup = POWERUP_MUSHROOM;
            }
        }
    } else {
        setChange(0);
        mPowerupChangeInvulnTimer = 8;
    }
    mPyMdlMng.mpMdl->setPlayerMode(powerup);
    switch (powerup) {
        case POWERUP_FIRE_FLOWER:
            mPyMdlMng.mpMdl->setColorType(1);
            break;
        case POWERUP_ICE_FLOWER:
            mPyMdlMng.mpMdl->setColorType(2);
            break;
        default:
            mPyMdlMng.mpMdl->setColorType(0);
            break;
    }
    return isPlayerGameStop();
}

void dAcPy_c::setChange(int mode) {
    mChangeType = mode;
    typedef void (dAcPy_c::*ChangeInitProc)();
    static ChangeInitProc l_changeInitProc[] = {
        &dAcPy_c::initChangeInit,
        &dAcPy_c::initChangeNormal
    };
    (this->*l_changeInitProc[mChangeType])();
    updateChange();
    setCcData();
}

bool dAcPy_c::updateChange() {
    typedef bool (dAcPy_c::*ChangeActionProc)();
    static ChangeActionProc l_changeActionProc[] = {
        &dAcPy_c::executeChangeInit,
        &dAcPy_c::executeChangeNormal
    };
    if (mChangeType >= 2) {
        setChange(0);
        return false;
    }
    if (mChangeTimer != 0) {
        mChangeTimer--;
    }
    if ((this->*l_changeActionProc[mChangeType])()) {
        mVisible = true;
        setBcData(0);
        bgCheck(1);
        mBgPressActive = 0;
        return true;
    }
    return false;
}

void dAcPy_c::fn_801416c0(PLAYER_POWERUP_e powerup) {
    mPowerupCopy2 = powerup;
    setPowerup(powerup, 0);
}

void dAcPy_c::initializeState_DemoStartWait() {
    daPlBase_c::initializeState_DemoStartWait();
}

void dAcPy_c::executeState_DemoStartWait() {
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
        bgCheck(0);
        if (isNowBgCross(BGC_WATER_SHALLOW)) {
            onOldBgCross(BGC_WATER_SHALLOW);
            changeState(daPlBase_c::StateID_Swim, SWIM_ARG_INITIAL); // [why daPlBase_c?]
        }
    }
    daPlBase_c::executeState_DemoStartWait();
}

void dAcPy_c::finalizeState_DemoStartWait() {
    daPlBase_c::finalizeState_DemoStartWait();
}

void dAcPy_c::setFireBallDamage(u8 type) {
    switch (type) {
        case 1:
            startPlayerVoice(VOICE_DAMAGE_FIRE, 0);
            break;
        case 2:
            startPlayerVoice(VOICE_DAMAGE_FREEZE, 0);
            break;
        default:
            startPlayerVoice(VOICE_HIP_ATTACKED, 0);
            break;
    }
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_7, 0, false);
}

///< @unofficial
const int l_tall_type_offsets[] = { 0x8000, 0xF800, 0x1F800 };

const sCcDatNew l_cc_data_mame = {
    { 0.0f, 4.0f },
    { 2.0f, 4.0f }
};

const sCcDatNew l_cc_data_normal = {
    { 0.0f, 7.0f },
    { 4.0f, 7.0f }
};

const sCcDatNew l_cc_data_super = {
    { 0.0f, 12.0f },
    { 6.0f, 12.0f }
};

const sCcDatNew l_cc_data_propel = {
    { 0.0f, 12.0f },
    { 6.0f, 12.0f }
};

const sCcDatNew l_cc_data_normal_sit = {
    { 0.0f, 3.5f },
    { 4.0f, 3.5f }
};

const sCcDatNew l_cc_data_super_sit = {
    { 0.0f, 7.0f },
    { 6.0f, 7.0f }
};

const sCcDatNew l_cc_data_normal_swim = {
    { 0.0f, 8.0f },
    { 4.0f, 6.0f }
};

const sCcDatNew l_cc_data_super_swim = {
    { 0.0f, 15.0f },
    { 6.0f, 9.0f }
};

const sCcDatNew l_cc_data_normal_waterwalk = {
    { 0.0f, 7.5f },
    { 4.0f, 7.5f }
};

const sCcDatNew l_cc_data_super_waterwalk = {
    { 0.0f, 14.0f },
    { 6.0f, 14.0f }
};

const sCcDatNew l_cc_data_penguin_slid = {
    { 6.0f, 8.0f },
    { 12.0f, 8.0f }
};

enum CcSizeDataIndex_e {
    CC_SIZE_DATA_MAME,
    CC_SIZE_DATA_NORMAL,
    CC_SIZE_DATA_SUPER,
    CC_SIZE_DATA_PROPEL,
    CC_SIZE_DATA_NORMAL_SIT,
    CC_SIZE_DATA_SUPER_SIT,
    CC_SIZE_DATA_NORMAL_SWIM,
    CC_SIZE_DATA_SUPER_SWIM,
    CC_SIZE_DATA_NORMAL_WATERWALK,
    CC_SIZE_DATA_SUPER_WATERWALK,
    CC_SIZE_DATA_PENGUIN_SLID
};

const sCcDatNew *l_cc_size_data[] = {
    &l_cc_data_mame,
    &l_cc_data_normal,
    &l_cc_data_super,
    &l_cc_data_propel,
    &l_cc_data_normal_sit,
    &l_cc_data_super_sit,
    &l_cc_data_normal_swim,
    &l_cc_data_super_swim,
    &l_cc_data_normal_waterwalk,
    &l_cc_data_super_waterwalk,
    &l_cc_data_penguin_slid
};

const CcSizeDataIndex_e l_mode_cc_size_data[] = {
    CC_SIZE_DATA_NORMAL, CC_SIZE_DATA_SUPER, CC_SIZE_DATA_SUPER, CC_SIZE_DATA_MAME, CC_SIZE_DATA_PROPEL, CC_SIZE_DATA_SUPER, CC_SIZE_DATA_SUPER
};

const sCcDatNewF scCcData = {
    {
        { 0.0f, 8.0f },
        { 3.0f, 8.0f }
    },
    CC_KIND_PLAYER,
    CC_ATTACK_NONE,
    (u8) ~BIT_FLAG(CC_KIND_YOSHI),
    (u32) ~(BIT_FLAG(CC_ATTACK_NONE) | BIT_FLAG(CC_ATTACK_YOSHI_MOUTH) | BIT_FLAG(CC_ATTACK_SAND_PILLAR)),
    CC_STATUS_NONE,
    dAcPy_c::ccCallBack
};

const sCcDatNewF scAtCcData = {
    {
        { 0.0f, 8.0f },
        { 6.0f, 6.0f }
    },
    CC_KIND_PLAYER_ATTACK,
    CC_ATTACK_NONE,
    (u8) ~(BIT_FLAG(CC_KIND_YOSHI) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_TAMA)),
    0,
    CC_STATUS_NONE,
    dAcPy_c::atCcCallBack
};

bool dAcPy_c::ccCheckAttack(dCc_c *self, dCc_c *other) {
    dAcPy_c *selfPlayer = (dAcPy_c *) self->getOwner();
    dActor_c *otherActor = (dActor_c *) other->getOwner();

    if (other->mCcData.mKind == CC_KIND_PLAYER || other->mCcData.mKind == CC_KIND_PLAYER_ATTACK) {
        if (selfPlayer->mNoInteractTimer != 0) {
            return false;
        }
        if (otherActor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *otherPlayer = (dAcPy_c *) otherActor;
            if (otherPlayer->mNoInteractTimer != 0) {
                return false;
            }
            if (selfPlayer->checkRideActor(otherPlayer)) {
                return false;
            }
        }
    }

    if (other->mCcData.mAttack == CC_ATTACK_SPIN) {
        if (otherActor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *otherPlayer = (dAcPy_c *) otherActor;
            if (!otherPlayer->checkCarryActor(selfPlayer)) {
                selfPlayer->checkCarryActor(otherPlayer);
            }
            return true;
        }
        return true;
    }

    if (selfPlayer->isNoDamage()) {
        return false;
    }

    dAcPy_c *otherPlayer = (dAcPy_c *) otherActor;

    switch (other->mCcData.mAttack) {
        case CC_ATTACK_HIP_ATTACK:
        case CC_ATTACK_SPIN_FALL: {
            DamageType_e damageType = DAMAGE_HIP_ATTACK;
            if (selfPlayer->isClimbing()) {
                damageType = DAMAGE_CLIMB;
            }
            if (selfPlayer->setDamage(otherActor, damageType)) {
                if (other->mCcData.mAttack == CC_ATTACK_HIP_ATTACK) {
                    otherPlayer->setVsPlHipAttackEffect();
                }

                selfPlayer->setHipAttackDamagePlayer(otherPlayer);
            }
            return true;
        }
        case CC_ATTACK_WIRE_NET:
            selfPlayer->setDamage(other->getOwner(), DAMAGE_CLIMB);
            return true;
        case CC_ATTACK_FIREBALL:
        case CC_ATTACK_ICEBALL:
            if (selfPlayer->getPlrNo() != otherActor->getPlrNo() && !selfPlayer->isStatus(STATUS_IS_SPIN_HOLD_REQ)) {
                selfPlayer->setFireBallDamage(other->mCcData.mAttack);
            }
            return true;
        case CC_ATTACK_YOSHI_BULLET:
        case CC_ATTACK_YOSHI_FIRE:
        case CC_ATTACK_ICE_2:
            if (selfPlayer->getPlrNo() != otherActor->getPlrNo() && !selfPlayer->isStatus(STATUS_IS_SPIN_HOLD_REQ)) {
                if (selfPlayer->isStatus(STATUS_51)) {
                    selfPlayer->mSpeedF = daPlBase_c::sc_DirSpeed[other->getOwner()->mDirection];
                    selfPlayer->mNoInteractTimer = 0;
                } else {
                    selfPlayer->mNoInteractTimer = 30;
                    if (!selfPlayer->setDamage(other->getOwner(), DAMAGE_6)) {
                        return false;
                    }
                }
                selfPlayer->setFireBallDamage(other->mCcData.mAttack);
            }
            return true;
        default:
            return false;
    }
}

bool dAcPy_c::ccCheckStamp(dCc_c *self, dCc_c *other) {
    dAcPy_c *selfPlayer = (dAcPy_c *) self->getOwner();
    daPlBase_c *otherActor = (daPlBase_c *) other->getOwner();

    if (
        selfPlayer->mSpeed.y <= 0.0f &&
        !selfPlayer->isStatus(STATUS_JUMP_DAI_COOLDOWN) &&
        !selfPlayer->isDemoType(DEMO_PLAYER) &&
        selfPlayer->isStatus(STATUS_CAN_LAND)
    ) {
        if (otherActor->mKind == STAGE_ACTOR_YOSHI) {
            daYoshi_c *yoshi = (daYoshi_c *) otherActor;
            if (
                self->getCenterPosY() > other->getUnderPos() &&
                (!selfPlayer->isNowBgCross(BGC_FOOT) || selfPlayer->isOnSinkSand()) &&
                yoshi->m_94 == 0
            ) {
                return yoshi->fn_8014eb70(selfPlayer, 0);
            }
        }
        if (selfPlayer->isEnableStampPlayerJump(self, other)) {
            if (otherActor->isStatus(STATUS_JUMP) && otherActor->mSpeed.y > 0.0f) {
                selfPlayer->setStampPlayerJump(true, self->mCollOffsetY[CC_KIND_PLAYER]);
                otherActor->setStampReduction();
                return true;
            }
            if (selfPlayer->setPlayerJumpDai(otherActor)) {
                return true;
            }
        }
    }
    return false;
}

bool dAcPy_c::ccCheckSideHit(dCc_c *self, dCc_c *other) {
    dAcPy_c *selfPlayer = (dAcPy_c *) self->getOwner();
    daPlBase_c *otherActor = (daPlBase_c *) other->getOwner();

    if (otherActor->isDemo()) {
        selfPlayer->setCcPlayerRev(self, other, 1.0f, CC_KIND_PLAYER);
        return true;
    }

    static const float sCcRevRate[3][3] = {
        { 0.5f, 1.0f, 1.0f },
        { 0.0f, 0.5f, 0.5f },
        { 0.0f, 0.5f, 0.5f },
    };
    u8 selfTallType = selfPlayer->getTallType(-1);
    u8 otherTallType = otherActor->getTallType(-1);
    selfPlayer->setCcPlayerRev(self, other, sCcRevRate[selfTallType][otherTallType], CC_KIND_PLAYER);

    return true;
}

void dAcPy_c::ccCallBack(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor;
    dAcPy_c *selfPlayer;

    selfPlayer = (dAcPy_c *) self->getOwner();
    otherActor = (dActor_c *) other->getOwner();

    if (other->mCcData.mKind == CC_KIND_PLAYER || other->mCcData.mKind == CC_KIND_YOSHI) {
        daPlBase_c *otherPlayer = (daPlBase_c *) otherActor;
        if (selfPlayer == otherPlayer->mpNoHitPlayer || otherPlayer == selfPlayer->mpNoHitPlayer) {
            return;
        }
    }

    if (ccCheckAttack(self, other)) {
        return;
    }

    if (other->mCcData.mKind == CC_KIND_PLAYER && otherActor->mKind == STAGE_ACTOR_PLAYER) {
        daPlBase_c *otherPlayer = (daPlBase_c *) otherActor;
        if (selfPlayer->checkRideActor(otherPlayer)) {
            return;
        }
        if (ccCheckStamp(self, other)) {
            return;
        }
        if (ccCheckSideHit(self, other)) {
            return;
        }
    }

    if (other->mCcData.mKind == CC_KIND_YOSHI) {
        daYoshi_c *yoshi = (daYoshi_c *) otherActor;
        if (selfPlayer->checkRideActor(yoshi)) {
            return;
        }
        if (ccCheckStamp(self, other)) {
            return;
        }
        if (yoshi->m_94 == 0) {
            return;
        }

        static const float sCcRevRate[] = { 1.0f, 0.8f, 0.6f };
        selfPlayer->setCcPlayerRev(self, other, sCcRevRate[selfPlayer->getTallType(-1)], CC_KIND_YOSHI);
    }
}

void dAcPy_c::setCcDataDirect() {
    const sCcDatNew *ccData = l_cc_size_data[l_mode_cc_size_data[mPowerup]];
    m_0c.x = ccData->mOffset.x;
    m_0c.y = ccData->mOffset.y + m_1598 + m_159c;
    mCc.mCcData.mBase.mOffset.set(m_0c.x, m_0c.y);
    mCc.mCcData.mBase.mSize.set(ccData->mSize.x, ccData->mSize.y);
}

void dAcPy_c::setCcAtNetPunch() {
    float s = mPyMdlMng.getSomeScale();
    mAttCc1.mCcData.mAttack = CC_ATTACK_WIRE_NET;
    mAttCc1.mCcData.mBase.mOffset.set(s * 8.0f, s * 16.0f);
    mAttCc1.mCcData.mBase.mSize.set(s * 12.0f, s * 12.0f);
}

void dAcPy_c::setCcAtSpin() {
    float y = (getModelHeight() + 4.0f) / 2.0f;
    mAttCc1.mCcData.mBase.mOffset.set(mCc.mCcData.mBase.mOffset.x, y);
    float x = mCc.mCcData.mBase.mSize.x;
    mAttCc1.mCcData.mBase.mSize.set(x * 3.5f, y);
    mAttCc1.mCcData.mVsKind |= BIT_FLAG(CC_KIND_PLAYER_ATTACK);
    mAttCc1.mCcData.mAttack = CC_ATTACK_SPIN;
    mAttCc1.mCcData.mVsDamage = BIT_FLAG(CC_ATTACK_SPIN);
    mCc.mCcData.mVsDamage &= ~BIT_FLAG(CC_ATTACK_SPIN);
}

void dAcPy_c::setCcAtSpinFall() {
    mAttCc1.mCcData.mBase.mOffset.set(0.0f, 10.0f);
    mAttCc1.mCcData.mBase.mSize.set(12.0f, 10.0f);
    mAttCc1.mCcData.mAttack = CC_ATTACK_SPIN_FALL;
    setCcAtBody(CC_ATTACK_SPIN_FALL);
}

void dAcPy_c::getCcBounds(sRangeDataF *bounds) {
    float f = 18.0f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        f = 8.0f;
    } else if (mPowerup == POWERUP_NONE) {
        f = 16.0f;
    }
    if (bounds == nullptr) {
        return;
    }
    float h = (getModelHeight() + 4.0f) / 2.0f;
    bounds->mOffset.x = 0.0f;
    bounds->mSize.x = f;
    bounds->mOffset.y = h;
    bounds->mSize.y = h;
}

void dAcPy_c::setCcAtSpinLiftUp() {
    sRangeDataF bounds;
    getCcBounds(&bounds);
    mAttCc1.mCcData.mBase.mOffset.x = bounds.mOffset.x;
    mAttCc1.mCcData.mBase.mOffset.y = bounds.mOffset.y;
    mAttCc1.mCcData.mBase.mSize.x = bounds.mSize.x;
    mAttCc1.mCcData.mBase.mSize.y = bounds.mSize.y;
    mAttCc1.mCcData.mAttack = CC_ATTACK_SPIN_LIFT_UP;
    checkSpinLiftUpRoofHeight();
}

void dAcPy_c::setCcData() {
    int sizeIndex = l_mode_cc_size_data[mPowerup];
    if (mPyMdlMng.getFlags() & 1) {
        switch (getPowerup()) {
            case POWERUP_MINI_MUSHROOM:
                break;
            case POWERUP_NONE:
                sizeIndex = CC_SIZE_DATA_NORMAL_SIT;
                break;
            default:
                sizeIndex = CC_SIZE_DATA_SUPER_SIT;
                break;
        }
    } else if (mPyMdlMng.getFlags() & 0x10) {
        switch (getPowerup()) {
            case POWERUP_MINI_MUSHROOM:
                break;
            case POWERUP_NONE:
                sizeIndex = CC_SIZE_DATA_NORMAL_SWIM;
                break;
            default:
                sizeIndex = CC_SIZE_DATA_SUPER_SWIM;
                break;
        }
    } else if (isStatus(STATUS_PENGUIN_SLIDE)) {
        sizeIndex = CC_SIZE_DATA_PENGUIN_SLID;
    } else if (isStatus(STATUS_SWIM)) {
        switch (getPowerup()) {
            case POWERUP_MINI_MUSHROOM:
                break;
            case POWERUP_NONE:
                sizeIndex = CC_SIZE_DATA_NORMAL_WATERWALK;
                break;
            default:
                sizeIndex = CC_SIZE_DATA_SUPER_WATERWALK;
                break;
        }
    }

    if (isStatus(STATUS_PENGUIN_SWIM)) {
        onStatus(STATUS_78);
        mMtx_c mtx;
        mVec3_c pos;
        mPyMdlMng.mpMdl->getJointMtx(&mtx, 1);
        mtx.concat(mMtx_c::createTrans(2.0f, 0.0f, 0.0f));
        mtx.multVecZero(pos);
        mCc.mCcData.mBase.mSize.set(6.0f, 6.0f);
        mCc.mCcData.mBase.mOffset.set(pos.x - mPos.x, pos.y - mPos.y);
        mPyMdlMng.mpMdl->getJointMtx(&mtx, mPyMdlMng.mpMdl->getFaceJointIdx());
        mtx.concat(mMtx_c::createTrans(0.0f, -4.0f, 0.0f));
        mtx.multVecZero(pos);
        mCc1.mCcData.mBase.mSize.set(6.0f, 6.0f);
        mCc1.mCcData.mBase.mOffset.set(pos.x - mPos.x, pos.y - mPos.y);
        return;
    }

    const sCcDatNew *ccData = l_cc_size_data[sizeIndex];
    float x = ccData->mOffset.x;
    if (mDirection == DIR_LR_L) {
        x = -x;
    }
    float sx = ccData->mSize.x;
    float sy = ccData->mSize.y;
    m_0c.set(x, ccData->mOffset.y + m_1598 + m_159c);
    mCc.mCcData.mBase.mSize.set(sx, sy);
    mCc.mCcData.mBase.mOffset.setX(m_0c.x);
    if (mPyMdlMng.mpMdl->mFlags & 4) {
        mMtx_c mtx;
        mVec3_c pos;
        mVec3_c anklePos = getAnkleCenterPos();
        float tmp = anklePos.y - 2.0f;
        if (tmp > mPos.y) {
            tmp = mPos.y;
        }
        mPyMdlMng.mpMdl->getJointMtx(&mtx, mPyMdlMng.mpMdl->getFaceJointIdx());
        mtx.concat(mMtx_c::createTrans(0.0f, -4.0f, 0.0f));
        mtx.multVecZero(pos);
        float tmp1 = (pos.y + tmp) / 2.0f;
        mCc.mCcData.mBase.mOffset.y = tmp1 - mPos.y;
        float tmp2 = (pos.y - tmp) / 2.0f;
        mCc.mCcData.mBase.mSize.y = std::fabs(tmp2);
    } else {
        mCc.mCcData.mBase.mOffset.y = m_0c.y;
    }
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer != nullptr && ridePlayer->mPowerup == POWERUP_NONE) {
        if (sizeIndex == 2) {
            mCc.mCcData.mBase.mOffset.y = 16.0f;
            mCc.mCcData.mBase.mSize.y = 8.0f;
            mCc.mCcData.mBase.mSize.x = 4.0f;
        } else if (sizeIndex == 1) {
            mCc.mCcData.mBase.mOffset.y = 9.0f;
            mCc.mCcData.mBase.mSize.y = 5.0f;
            mCc.mCcData.mBase.mSize.x = 4.0f;
        }
    }
    if (isStatus(STATUS_8E) || isStatus(STATUS_4F)) {
        mCc.mCcData.mVsKind &= ~(BIT_FLAG(CC_KIND_YOSHI) | BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK));
    } else {
        mCc.mCcData.mVsKind |= (BIT_FLAG(CC_KIND_YOSHI) | BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK));
    }
}

void dAcPy_c::atCcCallBack(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor;
    dAcPy_c *selfPlayer;

    selfPlayer = (dAcPy_c *) self->getOwner();
    otherActor = (dActor_c *) other->getOwner();

    if (self->mCcData.mAttack == CC_ATTACK_SPIN_LIFT_UP) {
        selfPlayer->checkSpinLiftUpReserve(other);
        return;
    }

    if (selfPlayer->isNoDamage()) {
        return;
    }

    if (other->mCcData.mKind == CC_KIND_PLAYER) {
        dAcPy_c *otherPlayer = (dAcPy_c *) otherActor;
        if (selfPlayer->mNoInteractTimer != 0 || otherPlayer->mNoInteractTimer != 0) {
            return;
        }
    }

    if (self->mCcData.mAttack != CC_ATTACK_PENGUIN_SLIDE) {
        return;
    }

    if (other->mCcData.mAttack != CC_ATTACK_PENGUIN_SLIDE) {
        return;
    }

    selfPlayer->setDamage(otherActor, DAMAGE_CLIMB);
}

const sBcPlayerPointData scBgPointData_Mame[5] = {
    {
        { 0xC8001, -0x1800, 0x1000, 0x0 },
        { 0x5C0001, -0x1000, 0x0, 0x7A00 },
        { 0xC0001, 0x3000, 0x4000, 0x5000 },
        { 0xC0001, 0x4000, 0x1000, 0x0 }
    },
    {
        { 0xC8001, -0x1800, 0x1000, 0x0 },
        { 0x5C0001, -0x1000, 0x0, 0x7A00 },
        { 0xC0001, 0x2800, 0x3800, 0x5000 },
        { 0xC0001, 0x4000, 0x1000, 0x0 }
    },
    {
        { 0xC8001, -0x2000, 0x1000, 0x0 },
        { 0x5C0001, -0x1000, 0x0, 0x1000 },
        { 0xC0001, -0x4000, -0x4000, 0x3000 },
        { 0xC0001, 0x4000, 0x1000, 0x0 }
    },
    {
        { 0xC8001, -0x1800, 0x1000, 0x0 },
        { 0x5C0001, -0x1000, 0x0, 0x7800 },
        { 0xC0001, 0x3000, 0x4000, 0x5000 },
        { 0xC0001, 0x4000, 0x1000, 0x0 }
    },
    {
        { 0xC8001, -0x1800, 0x1000, 0x0 },
        { 0x5C0001, -0x1000, 0x0, 0x7800 },
        { 0xC0001, 0x2800, 0x3800, 0x5000 },
        { 0xC0001, 0x4000, 0x1000, 0x0 }
    }
};

const sBcPlayerPointData scBgPointData_Normal[5] = {
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x5C0001, -0x2000, 0x1000, 0x10000 },
        { 0xC0001, 0x4000, 0xB000, 0x8000 },
        { 0xC0001, 0x8000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x5C0001, -0x2000, 0x1000, 0xC000 },
        { 0xC0001, 0x4000, 0x8800, 0x8000 },
        { 0xC0001, 0x8000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x5C0001, -0x2000, 0x1000, 0x1000 },
        { 0xC0001, -0x4000, -0xE000, 0x8000 },
        { 0xC0001, 0x8000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x5C0001, -0x2000, 0x1000, 0x10000 },
        { 0xC0001, 0x4000, 0xA000, 0x8000 },
        { 0xC0001, 0x8000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x5C0001, -0x2000, 0x1000, 0xC000 },
        { 0xC0001, 0x4000, 0xA000, 0x8000 },
        { 0xC0001, 0x8000, 0x1000, 0x2000 }
    }
};

const sBcPlayerPointData scBgPointData_Super[5] = {
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x9C0001, -0x2000, 0x1000, 0x1B000 },
        { 0xC0001, 0x4000, 0x13000, 0x8000 },
        { 0xC0001, 0x10000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x9C0001, -0x2000, 0x1000, 0x10000 },
        { 0xC0001, 0x4000, 0xB000, 0x8000 },
        { 0xC0001, 0xC000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x9C0001, -0x2000, 0x1000, 0x1000 },
        { 0xC0001, -0x4000, -0x18000, 0x8000 },
        { 0xC0001, 0x10000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x9C0001, -0x2000, 0x1000, 0x1B000 },
        { 0xC0001, 0x4000, 0x13000, 0x8000 },
        { 0xC0001, 0x10000, 0x1000, 0x2000 }
    },
    {
        { 0xC8001, -0x5000, 0x4000, 0x0 },
        { 0x9C0001, -0x2000, 0x1000, 0x12000 },
        { 0xC0001, 0x4000, 0x10000, 0x8000 },
        { 0xC0001, 0x10000, 0x1000, 0x2000 }
    }
};

const sBcPlayerPointData scBgPointData_PenguinSlide = {
    { 0xC8101, -0xA000, 0x9000, 0x0 },
    { 0x9C0101, -0x7000, 0x6000, 0x12000 },
    { 0x129C0101, 0x4000, 0xD000, 0x10000 },
    { 0x0, 0x0, 0x0, 0x0 }
};

const sBcPlayerPointData scBgPointData_PenguinSwim = {
    { 0xC8001, -0x5000, 0x4000, 0x4000 },
    { 0x9C0001, -0x2000, 0x1000, 0x1B000 },
    { 0xC0001, 0x8000, 0x17000, 0x8000 },
    { 0x0, 0x0, 0x0, 0x0 }
};

const sBcPlayerPointData *scBgPointData[POWERUP_COUNT] = {
    scBgPointData_Normal,
    scBgPointData_Super,
    scBgPointData_Super,
    scBgPointData_Mame,
    scBgPointData_Super,
    scBgPointData_Super,
    scBgPointData_Super,
};

const sBcPlayerPointData *dAcPy_c::getBgPointData_Powerup(PLAYER_POWERUP_e powerup, int index) {
    return &scBgPointData[powerup][index];
}

float dAcPy_c::getStandHeadBgPointY() {
    return getBgPointData_Powerup(mPowerup, 0)->mHead.mOffset / 4096.0f;
}

const sBcPlayerPointData *dAcPy_c::getBgPointData() {
    if (isStatus(STATUS_PENGUIN_SLIDE) && !isStatus(STATUS_INITIAL_SLIDE)) {
        return &scBgPointData_PenguinSlide;
    }
    if (isStatus(STATUS_PENGUIN_SWIM)) {
        return &scBgPointData_PenguinSwim;
    }
    u32 flags = mPyMdlMng.mpMdl->mFlags;
    int bgDataIndex = 0;
    if (flags & BIT_FLAG(0)) {
        if (mPowerup != POWERUP_MINI_MUSHROOM) {
            bgDataIndex = 1;
        }
    } else if (flags & BIT_FLAG(2)) {
        bgDataIndex = 2;
    } else if (flags & BIT_FLAG(4)) {
        bgDataIndex = 3;
    } else if (mPyMdlMng.mpMdl->mCurrAnmID == PLAYER_ANIM_STAR_ROLL_DUPLICATE) {
        bgDataIndex = 4;
    }
    return getBgPointData_Powerup(mPowerup, bgDataIndex);
}

void dAcPy_c::setBcData(int setInstant) {
    offStatus(STATUS_B8);
    if (isItemKinopio()) {
        onStatus(STATUS_B8);
    }
    const sBcPlayerPointData *data = getBgPointData();
    sBcPointData footData = data->mFoot;
    sBcPointData headData = data->mHead;
    sBcPointData wallData = data->mWall;
    sBcPointData vineData = data->mVine;
    if (!isMameAction() && !isStatus(STATUS_4E) && !isStatus(STATUS_ENEMY_STAGE_CLEAR)) {
        footData.mFlags |= 0x200000;
    }
    if (isStatus(STATUS_BD)) {
        footData.mFlags |= 0x10900100;
        wallData.mFlags |= 0x10900100;
        headData.mFlags |= 0x10900100;
        m_15b8 = 5;
    } else if (isStatus(STATUS_HANG)) {
        footData.mFlags |= 0x80000000;
    } else if (isStatus(STATUS_VINE)) {
        footData.mFlags |= 0x2000;
        wallData.mSupMargin = headData.mOffset - 0x2000;
    } else if (isStatus(STATUS_SPIN_HIP_ATTACK_FALL)) {
        footData.mFlags |= 0xd00000;
    } else if (isState(StateID_HipAttack)) {
        switch (mPyMdlMng.mpMdl->mCurrAnmID) {
            case PLAYER_ANIM_HIPAT:
                if (mSpeed.y < 0.0f) {
                    footData.mFlags |= 0x500000;
                    if (isStatus(STATUS_C4)) {
                        footData.mFlags |= 0x800000;
                    }
                }
                break;
            case PLAYER_ANIM_HIPED:
                if (isStatus(STATUS_C4)) {
                    footData.mFlags |= 0x900000;
                }
                break;
        }
    } else {
        if (isStatus(STATUS_52)) {
            headData.mFlags |= 0x4000000;
            wallData = getBgPointData_Powerup(mPowerup, 1)->mWall;
            float y = mPos.y + getStandHeadBgPointY() - 1.0f;
            mVec3_c pos(mPos.x, mPos.y + 4.0f, mPos.z);
            float height;
            if (dBc_c::checkTenjou(&pos, &height, mLayer, mAmiLayer)) {
                if (height < y && !dBc_c::checkBg(pos.x, height + 4.0f, mLayer, mAmiLayer, 0x10)) {
                    footData.mFlags |= 0x800000;
                }
            }
        }
    }
    if (isChange()) {
        headData.mFlags |= 0x4000000;
    }
    if (isStatus(STATUS_30)) {
        footData.mFlags |= 0x10000;
    }
    if (isStatus(STATUS_A4)) {
        footData.mFlags |= 8;
    }
    if (isStatus(STATUS_7F)) {
        footData.mFlags |= 0x80000000;
        wallData.mFlags |= 0x80000000;
        headData.mFlags |= 0x80000000;
    }
    if (m_15b8 != 0) {
        footData.mFlags |= 0x100;
        wallData.mFlags |= 0x100;
        headData.mFlags |= 0x100;
    }
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        footData.mFlags |= 0x10;
    }
    if (isStatus(STATUS_EXTRA_PUSH_FORCE)) {
        footData.mFlags |= 0x20;
    }
    if (isStar()) {
        wallData.mFlags |= 0x2000000;
        headData.mFlags |= 0x2000000;
    }
    if (isStatus(STATUS_82)) {
        headData.mFlags |= 0x4000000;
    }
    reviseBcDataCarryHardBlock(wallData, headData);
    reviseBcDataCarryPlayer(wallData, headData);
    if (isStatus(STATUS_PENGUIN_SWIM)) {
        float angY = mAngle.y.sin() * 19456.0f;
        mAng x = mAngle.x;
        if (x > 0x2000) {
            x -= 0x2000;
            angY -= x.sin() * mAngle.y.sin() * 81920.0f;
        }
        headData.mInfMargin += angY;
        headData.mSupMargin += angY;
    }
    if (setInstant == 1) {
        mFootBcData = footData;
        mHeadBcData = headData;
        mWallBcData = wallData;
        mVineBcData = vineData;
    } else {
        sLib::chase(&mFootBcData.mInfMargin, footData.mInfMargin, 0x1000);
        sLib::chase(&mFootBcData.mSupMargin, footData.mSupMargin, 0x1000);
        sLib::chase(&mFootBcData.mOffset, footData.mOffset, 0x1000);

        sLib::chase(&mHeadBcData.mInfMargin, headData.mInfMargin, 0x1000);
        sLib::chase(&mHeadBcData.mSupMargin, headData.mSupMargin, 0x1000);

        sLib::chase(&mWallBcData.mInfMargin, wallData.mInfMargin, 0x1000);
        sLib::chase(&mWallBcData.mSupMargin, wallData.mSupMargin, 0x1000);
        sLib::chase(&mWallBcData.mOffset, wallData.mOffset, 0x1000);

        sLib::chase(&mVineBcData.mInfMargin, vineData.mInfMargin, 0x1000);
        sLib::chase(&mVineBcData.mSupMargin, vineData.mSupMargin, 0x1000);
        sLib::chase(&mVineBcData.mOffset, vineData.mOffset, 0x1000);

        if (mHeadBcData.mOffset < headData.mOffset) {
            mVec3_c pos(mPos.x, mPos.y + mHeadBcData.mOffset / 4096.0f + 1.0f, mPos.z);
            float height;
            if (dBc_c::checkTenjou(&pos, &height, mLayer, mAmiLayer)) {
                if (height < mPos.y + headData.mOffset / 4096.0f) {
                    float tmp = height - mPos.y;
                    if (tmp < 0.0f) {
                        tmp = 0.0f;
                    }
                    headData.mOffset = (int) (tmp * 4096.0f);
                }
            }
        }
        sLib::chase(&mHeadBcData.mOffset, headData.mOffset, 0x1000);
        mFootBcData.mFlags = footData.mFlags;
        mHeadBcData.mFlags = headData.mFlags;
        mWallBcData.mFlags = wallData.mFlags;
        mVineBcData.mFlags = vineData.mFlags;
    }
    mBc.set(this, mFootBcData, mHeadBcData, mWallBcData);
    mBc.mAmiLine = getCcLineKind();
    mRc.mLineKind = getCcLineKind();
    mBc.mLayer = mLayer;
    mRc.mLayer = mLayer;
    if (m_15b2 != 0) {
        mRc.mFlags |= BIT_FLAG(1);
    } else {
        mRc.mFlags &= ~BIT_FLAG(1);
    }
}

void dAcPy_c::fn_80143060(sBcPointData &data1, sBcPointData &data2, bool mode) {
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer == nullptr) {
        return;
    }

    const sBcPlayerPointData *data;
    if (mode) {
        data = getBgPointData_Powerup(carryPlayer->mPowerup, 1);
    } else {
        data = getBgPointData_Powerup(carryPlayer->mPowerup, 0);
    }

    if (data1.mOffset < data->mWall.mOffset) {
        data1.mOffset = data->mWall.mOffset;
    }

    int headOffset = data->mHead.mOffset + l_tall_type_offsets[getTallType(-1)];
    int wallLMargin = data->mWall.mSupMargin + l_tall_type_offsets[getTallType(-1)];

    if (isStatus(STATUS_46)) {
        int thing = (int) (carryPlayer->mPos.y * 4096.0f) - (int) (mPos.y * 4096.0f);
        int tmp = data->mHead.mOffset + thing;
        if (headOffset > tmp) {
            headOffset = tmp;
        }
        if (headOffset < data2.mOffset) {
            headOffset = data2.mOffset;
        }
        int tmp2 = data->mWall.mSupMargin + thing;
        if (wallLMargin > tmp2) {
            wallLMargin = tmp2;
        }
        if (wallLMargin < data1.mSupMargin) {
            wallLMargin = data1.mSupMargin;
        }
    }

    data2.mFlags = data->mHead.mFlags;
    data2.mInfMargin = data->mHead.mInfMargin;
    data2.mSupMargin = data->mHead.mSupMargin;
    data2.mOffset = headOffset;
    data1.mSupMargin = wallLMargin;
    if (!isMameAction()) {
        data2.mFlags = 0x9c0001;
    }
}

bool dAcPy_c::fn_80143220(sBcPointData &data1, sBcPointData &data2) {
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer == nullptr) {
        return false;
    }
    mVec3_c pos(mPos.x, mPos.y + data2.mOffset / 4096.0f - 3.0f, mPos.z);
    PLAYER_POWERUP_e powerup = carryPlayer->mPowerup;
    float tmp = getBgPointData_Powerup(powerup, 0)->mHead.mOffset - getBgPointData_Powerup(powerup, 1)->mHead.mOffset;
    float val = mPos.y + data2.mOffset / 4096.0f + tmp / 4096.0f;
    float someOffset = data1.mOffset / 4096.0f - 1.0f;
    pos.x = mPos.x + someOffset;
    float height;
    if (dBc_c::checkTenjou(&pos, &height, mLayer, 1)) {
        if (height > pos.y && height <= val) {
            return true;
        }
    }
    if (!isStatus(STATUS_47)) {
        pos.x = mPos.x - (someOffset + 1.0f);
        if (dBc_c::checkTenjou(&pos, &height, mLayer, 1)) {
            if (height > pos.y && height <= val) {
                return true;
            }
        }
    }
    return false;
}

void dAcPy_c::reviseBcDataCarryPlayer(sBcPointData &data1, sBcPointData &data2) {
    dAcPy_c *ridePlayer = getRidePlayer();
    if (ridePlayer != nullptr && !ridePlayer->isMameAction()) {
        data2.mFlags = 0x9c0001;
    }
    mBc.mRidePlrNo = -1;
    offStatus(STATUS_47);
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        mBc.mRidePlrNo = carryPlayer->getPlrNo();
        if (isStatus(STATUS_JUMP) && !isStatus(STATUS_48)) {
            fn_80143060(data1, data2, false);
        } else {
            fn_80143060(data1, data2, true);
            if (fn_80143220(data1, data2)) {
                onStatus(STATUS_47);
            }
        }
    }
}

void dAcPy_c::reviseBcDataCarryHardBlock(sBcPointData &data1, sBcPointData &data2) {
    if (getCarryHardBlock()) {
        const sBcPlayerPointData *data = getBgPointData_Powerup(mPowerup, 0);
        if (data1.mOffset < data->mWall.mOffset) {
            data1.mOffset = data->mWall.mOffset;
        }
        data2.mInfMargin = data->mHead.mInfMargin;
        data2.mSupMargin = data->mHead.mSupMargin;
        data2.mOffset += 0xc000;
        data2.mFlags = 0x9c0001;
        data1.mSupMargin += 0xc000;
    }
}

void dAcPy_c::initCcData() {
    initCollision((sCcDatNewF *) &scCcData, (sCcDatNewF *) &scAtCcData);
}

void dAcPy_c::initBcData() {
    mBc.init();
    setBcData(1);
    clearNowBgCross();
    clearOldBgCross();
}

void dAcPy_c::maxFallSpeedSet() {
    if (isMameAction()) {
        mMaxFallSpeed = -2.5f;
    } else {
        mMaxFallSpeed = sc_MaxFallSpeed;
    }
}

void dAcPy_c::set1UpKinokoEffect() {
    setFollowEffect(1);
}

void dAcPy_c::setFlagGetEffect() {
    startPlayerVoice(VOICE_TYUKAN, 0);
    setFollowEffect(2);
}

void dAcPy_c::setFollowEffect(int followType) {
    mFollowType = followType;
    if (mFollowType == 0) {
        return;
    }
    int efSizeType = 0;
    if (mPowerup == POWERUP_NONE) {
        efSizeType = 1;
    } else if (mPowerup == POWERUP_MINI_MUSHROOM) {
        efSizeType = 2;
    }
    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 0);
    float s = mPyMdlMng.getSomeScale();
    mVec3_c efScale(s, s, s);
    if (mFollowType == 1) {
        static const char *sc_oneUpEffectID[] = {
            "Wm_mr_1upkira",
            "Wm_mr_1upkira_s",
            "Wm_mr_1upkira_ss"
        };
        dEf::createPlayerEffect_change(mPlayerNo, &mFollowEf4, sc_oneUpEffectID[efSizeType], 0, &efPos, nullptr, &efScale);
    } else if (mFollowType == 2) {
        static const char *sc_flagGetEffectID[] = {
            "Wm_mr_flaggetkira",
            "Wm_mr_flaggetkira_s",
            "Wm_mr_flaggetkira_ss"
        };
        dEf::createPlayerEffect(mPlayerNo, &mFollowEf4, sc_flagGetEffectID[efSizeType], 0, &efPos, nullptr, &efScale);
    }
}

void dAcPy_c::updateFollowEffect() {
    if (mFollowType == 0) {
        return;
    }

    mVec3_c efPos;
    mPyMdlMng.mpMdl->getJointPos(&efPos, 0);
    float s = mPyMdlMng.getSomeScale();
    mVec3_c efScale(s, s, s);
    if (!mFollowEf4.follow(&efPos, nullptr, &efScale)) {
        mFollowType = 0;
    }
}

void dAcPy_c::calcTimerProc() {
    if (isStatus(STATUS_OUT_OF_PLAY)) {
        return;
    }

    daPlBase_c::calcTimerProc();
    calcInvalidKeyTimer();

    sLib::calcTimer(&mStarTimer);
    sLib::calcTimer(&mNoInteractTimer);
    sLib::calcTimer(&mJumpComboTimer);
    sLib::calcTimer(&mWallSlideCooldown);
    sLib::calcTimer(&mPoleGrabCooldown);
    sLib::calcTimer(&m_15b2);
    sLib::calcTimer(&m_15ae);
    sLib::calcTimer(&m_2f0);
    sLib::calcTimer(&mSpinCooldown);
    sLib::calcTimer(&m_15b6);
    sLib::calcTimer(&m_60);
    sLib::calcTimer(&m_15b8);
    sLib::calcTimer(&mBalloonHelpVoiceCooldown);
    sLib::calcTimer(&mPowerUpEffectTimer);
    sLib::calcTimer(&m_b98);
    sLib::calcTimer(&mTarzanRopeCooldown);
    if (m_15b0 != 0 && !sLib::calcTimer(&m_15b0)) {
        mKey.clearShakeJump();
    }
}

void dAcPy_c::executeMain() {
    mPc.chkTimer();
    setCenterOffset();
    updateItemGetEffect();
    managePropelFukidashi();
    if (updateChange()) {
        return;
    }
    calcReductionScale();
    if (!isStatus(STATUS_NO_ANIM)) {
        mPyMdlMng.play();
    }
    setFootSound();
    updatePropelParts();
    updateMissSpin();
    updateFollowEffect();
    calcStarTimer();
    daYoshi_c *yoshi = getRideYoshi();
    if (yoshi != nullptr) {
        if (yoshi->isDemo()) {
            return;
        }
    } else {
        if (executeDemoState()) {
            setCcDataDirect();
            return;
        }
    }
    calcTimerProc();
    selectAction();
    if (mSpeed.y < 0.75f) {
        offStatus(STATUS_96);
    }
    mPrevDirection = mDirection;
    int dir;
    if (!isStatus(STATUS_96) && !isStatus(STATUS_97) && mKey.buttonWalk(&dir)) {
        mDirection = dir;
    }
    executeState();
    if (mSquishNoMoveTimer == 0) {
        calcPlayerSpeedXY();
    }
    updateRideNat();
    setCcData();
    if (isStar()) {
        setCcAtStar();
    }
    setBcData(1);
    bgCheck(1);
    checkDispOver();
    onStatus(STATUS_77);
    if (mKey.mActionTriggered) {
        mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
    }
}

void dAcPy_c::checkBgCrossSub() {
    if (mBc.checkCollision(&mVineBcData)) {
        onNowBgCross(BGC_CAN_CLIMB);
    }
    u32 resFlags = mBc.checkCollision2(&mVineBcData);
    if (resFlags & 0x0F) {
        onNowBgCross(BGC_VINE_TOUCH_FULL);
        if (resFlags & 0b1100) {
            onNowBgCross(BGC_VINE_TOUCH_D);
        }
        if (resFlags & 0b0011) {
            onNowBgCross(BGC_VINE_TOUCH_U);
        }
    }
    if (resFlags & 0x30) {
        onNowBgCross(BGC_VINE_TOUCH_2);
    }
    if (isAmiRollAction()) {
        onNowBgCross(BGC_VINE_TOUCH_L);
        onNowBgCross(BGC_VINE_TOUCH_R);
        return;
    }
    sBcPointData vineData = mVineBcData;
    int v = mWallBcData.mOffset * 0.7f;
    if (vineData.mOffset < v) {
        vineData.mOffset = v;
    }
    u32 resFlags2 = mBc.checkCollision2(&vineData);
    if (resFlags2 & 1 || resFlags2 & 2) {
        onNowBgCross(BGC_VINE_TOUCH);
    }
    if (resFlags2 & 4 || resFlags2 & 8) {
        onNowBgCross(BGC_VINE_TOUCH);
    }
    if (resFlags2 & 1 || resFlags2 & 4) {
        onNowBgCross(BGC_VINE_TOUCH_L);
    }
    if (resFlags2 & 2 || resFlags2 & 8) {
        onNowBgCross(BGC_VINE_TOUCH_R);
    }
}

void dAcPy_c::postBgCross() {
    if (!isMameAction()) {
        offStatus(STATUS_CAN_WATER_WALK);
    }
    if (isNowBgCross(BGC_WATER_TOUCH) && !isNowBgCross(BGC_WATER_BUBBLE)) {
        if (isNowBgCross(BGC_HEAD)) {
            m_15b6 = 10;
            offStatus(STATUS_CAN_WATER_WALK);
        } else if (isStatus(STATUS_CAN_WATER_WALK) || isStatus(STATUS_CAN_WATER_SLIDE)) {
            if (mSpeed.y < 0.0f && m_15b6 == 0) {
                mPos.y = mWaterHeight;
                onNowBgCross(BGC_ON_WATER_MOVE);
                onNowBgCross(BGC_FOOT);
                if (isStatus(STATUS_PENGUIN_SLIDE)) {
                    onNowBgCross(BGC_ON_ICE);
                }
                offNowBgCross(BGC_WATER_SHALLOW);
                offNowBgCross(BGC_WATER_SUBMERGED);
                mWaterDepth = 0;
                mBc.mpCtrHead = nullptr;
                mRc.clrLink();
            }
        }
    }

    if (isNowBgCross(BGC_ON_SINK_SAND)) {
        if (isStatus(STATUS_PENGUIN_SLIDE) && mSpeed.y < 0.0f) {
            mPos.y = mSinkSandHeight;
            onNowBgCross(BGC_FOOT);
            offNowBgCross(BGC_ON_SINK_SAND);
            offNowBgCross(BGC_IN_SINK_SAND);
            offNowBgCross(BGC_INSIDE_SINK_SAND);
        }
    }

    daPlBase_c::postBgCross();

    if (!isNowBgCross(BGC_61)) {
        if (!isStatus(STATUS_1A) && !isStatus(STATUS_1B)) {
            if (
                isNowBgCross(BGC_WALL_TOUCH_L_2) && mSpeedF < 0.0f ||
                isNowBgCross(BGC_WALL_TOUCH_R_2) && mSpeedF > 0.0f
            ) {
                mSpeedF = 0.0f;
                mMaxSpeedF = 0.0f;
                mFinalAirPushForceX = 0;
                m_1138 = 0;
            }
            if (
                isNowBgCross(BGC_FOOT) &&
                isNowBgCross(BGC_ON_ICE) &&
                (isNowBgCross(BGC_OBJBG_TOUCH_L) || isNowBgCross(BGC_OBJBG_TOUCH_R)) &&
                mBc.mpCtrWall != nullptr
            ) {
                float f = mBc.mpCtrWall->mpActor->mPos.x - mBc.mpCtrWall->mpActor->mLastPos.x;
                if (f > 1.0f) {
                    f = 1.0f;
                }
                if (f < -1.0f) {
                    f = -1.0f;
                }
                if (isNowBgCross(BGC_OBJBG_TOUCH_L)) {
                    if (mSpeedF < f) {
                        mSpeedF = f;
                    }
                } else {
                    if (mSpeedF > f) {
                        mSpeedF = f;
                    }
                }
            }
        }
    }

    if (isNowBgCross(BGC_FOOT)) {
        if (isNowBgCross(BGC_ON_WATER_MOVE)) {
            onStatus(STATUS_ON_WATER_MOVE);
        } else {
            offStatus(STATUS_ON_WATER_MOVE);
        }
    }

    if (
        isNowBgCross(BGC_FOOT) ||
        isNowBgCross(BGC_WATER_SHALLOW) ||
        (isStatus(STATUS_VINE) && mBc.mFenceType == 1) ||
        isStatus(STATUS_53) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_RIDE_YOSHI)
    ) {
        if (!isNowBgCross(BGC_62)) {
            clearTreadCount();
        }
    }

    if (
        isNowBgCross(BGC_FOOT) ||
        isNowBgCross(BGC_WATER_SHALLOW) ||
        isStatus(STATUS_RIDE_YOSHI) ||
        isStatus(STATUS_VINE) ||
        isStatus(STATUS_HANG) ||
        isStatus(STATUS_POLE) ||
        isStatus(STATUS_KANI_HANG) ||
        isStatus(STATUS_36)
    ) {
        setEnemyStageClearDemo();
        clearJumpActionInfo(1);
        resetPropelFlyTime();
        mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
        mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    }
}

void dAcPy_c::clearJumpActionInfo(int) {
    offStatus(STATUS_A7);
    offStatus(STATUS_SINK_SAND_JUMP);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);

    bool checkRes = false;
    if (mBc.mpCtrHead != nullptr && mBc.mpCtrHead->mpActor != nullptr) {
        if (mBc.mpCtrHead->mpActor->mProfName == fProfile::EN_GOALPOLE) {
            checkRes = true;
        }
        if (mBc.mpCtrHead->mpActor->mActorProperties & 0x10 || mBc.mpCtrHead->mpActor->mActorProperties & 0x20) {
            checkRes = true;
        }
    }
    if (!checkRes) {
        onStatus(STATUS_B7);
    }
    if (mScrollMode == 4 || mScrollMode == 2) {
        setScrollMode(0);
    }
    resetMissSpin();
}

float dAcPy_c::getSandSinkRate() {
    return sGlobalData_c<dAcPy_c>::mData.mSandSinkRate;
}

bool dAcPy_c::setPressBgDamage(int a, int b) {
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        if (b == 0) {
            cancelCarry(getCarryPlayer());
            return false;
        }
        if (mPowerup == POWERUP_MINI_MUSHROOM || carryPlayer->mPowerup == POWERUP_MINI_MUSHROOM) {
            carryPlayer->setPressBgDamage(a, b);
        } else {
            mVec3_c pos = carryPlayer->mPos;
            float height;
            if (dBc_c::checkTenjou(&pos, &height, mLayer, mAmiLayer)) {
                const sBcPlayerPointData *data = getBgPointData_Powerup(mPowerup, 0);
                pos.y = height - data->mHead.mOffset / 4096.0f - 2.0f;
                carryPlayer->mPos = pos;
            }
            cancelCarry(carryPlayer);
            return false;
        }
    }
    return daPlBase_c::setPressBgDamage(a, b);
}

const sPowerChangeData daPlayerData_c::smc_POWER_CHANGE_DATA = {
    {
        // no star
        0.0f, 0.08f,
        0.03f, 0.06f,
        0.029f, 0.021f,
        0.09f,
        // star
        0.0f, 0.11f,
        0.04f, 0.078f,
        0.039f, 0.027f,
        0.12f
    },
    {
        // no star
        0.12f, 0.12f, 0.094f, 0.25f,
        // star
        0.12f, 0.12f, 0.094f, 0.25f
    },
    {
        // no star
        0.028f, 0.028f, 0.032f, 0.25f,
        // star
        0.038f, 0.038f, 0.042f, 0.25f
    },
    {
        // no star
        0.07f, 0.07f, 0.07f, 0.25f,
        // star
        0.07f, 0.07f, 0.07f, 0.25f
    },
    // gravity normal
    -0.34f,
    2.5f,
    1.5f,
    0.3f,
    -0.12f,
    -3.0f,
    -0.34f,
    -0.34f,
    -0.34f,
    -0.25f,
    -0.34f,
    -0.34f,
    -0.06f,
    -0.25f,
    -0.34f,
    -0.08f,
    -0.31f,
    -0.34f,
    // gravity mini
    -0.09f,
    2.5f,
    1.5f,
    0.0f,
    -2.0f,
    -2.0f,
    -0.09f,
    -0.09f,
    -0.09f,
    -0.06f,
    -0.09f,
    -0.09f,
    -0.04f,
    -0.06f,
    -0.09f,
    -0.06f,
    -0.09f,
    -0.09f,
    -0.34f,
    2.5f,
    1.5f,
    0.3f,
    -0.12f,
    -3.0f,
    -0.34f,
    -0.34f,
    -0.34f,
    -0.25f,
    -0.34f,
    -0.34f,
    -0.06f,
    -0.25f,
    -0.34f,
    -0.1f,
    -0.1f,
    -0.1f,
    { 0.7f, 1.5f, 2.8f },
    { 0.0f, 0.18f, 0.24f, 0.3f }
};

const float dAcPy_c::data_802f5a0c[] = {
    0.0f, 0.08f, 0.07f, 0.11f, -1.5f, -1.25f,
    -1.0f, -0.1f, 1.5f, 1.5f, 2.5f, 2.5f,
    1.0f, -6.0f, 3.214242E-39 // [-> not just floats?]
};

const float dAcPy_c::data_802f5a48[] = {
    25.0f, 3.627f, 3.0f, 0.6f, 2.3509887E-38, // [-> not just floats?]
    1.2f, 2.0f, 1.5f, 2.4f, 0.02f, 0.01f,
    0.05f, 0.2f, 0.03f, 0.03f, 0.8f, 0.7f,
    0.8f, 0.14f, 4.0f, 0.07f, 40.0f
};

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
// STATE_VIRTUAL_DEFINE(dAcPy_c, DemoStartWait); // [TODO: intentionally left out?]

ACTOR_PROFILE(PLAYER, dAcPy_c, 0);

template <>
const dAcPy_c::GlobalData_t sGlobalData_c<dAcPy_c>::mData = {
    0.0f, 0.0f,
    1.5f, 4.5f, 0.5f, 4.0f, 1.4f, 1.0f,
    {
        mVec3_c(0.0f, 10.0f, 0.0f),
        mVec3_c(0.0f, 16.0f, 0.0f),
        mVec3_c(0.0f, 28.0f, 0.0f)
    },
    -3.0f, 3.0f, 3.2f,
    0.5f, 3.2f, 3.0f, 1.2f,
    0.5f, 0.625f, 0.5f, 0.625f,
    0.8f, 1.0f, 1.3f, 2.0f
};

dAcPy_HIO_Speed_c dAcPy_c::m_speed_hio[2];

dAcPy_c::dAcPy_c() : mPyMdlMng(daPyMng_c::getCourseInPlayerModelType(ACTOR_PARAM(PlayerNo))) {
    setKind(STAGE_ACTOR_PLAYER);
    mExecStopMask = BIT_FLAG(STAGE_ACTOR_PLAYER);
    mpMdlMng = &mPyMdlMng;
    mPyMdlMng.mpMdl->mpOwner = this;
    mPlayerNo = ACTOR_PARAM(PlayerNo);
    mPlayerType = daPyMng_c::getPlayerType(mPlayerNo);
    mPowerup = daPyMng_c::mPlayerMode[daPyMng_c::getPlayerType(mPlayerNo)];
    mIsRescueKinopio = 0;
    if (daPyMng_c::mCreateItem[daPyMng_c::getPlayerType(mPlayerNo)] & 8) {
        mIsRescueKinopio = 1;
        dInfo_c::m_instance->m_6c = true;
        mPlayerType = PLAYER_BLUE_TOAD;
        mPowerup = daPyMng_c::mKinopioMode;
        daPyMng_c::mPlayerEntry[mPlayerNo] = 0;
        daPyMng_c::mCreateItem[daPyMng_c::getPlayerType(mPlayerNo)] &= ~CREATE_ITEM_RESCUE_TOAD;
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
                !ctrlPlayer->isStatus(STATUS_GOAL_POLE_TOUCHED) &&
                !isStatus(STATUS_OUT_OF_PLAY) &&
                !isStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT)
            ) {
                daPyMng_c::fn_8005f570(mPowerup, mPlayerNo);
            }
        }
        return;
    }

    PLAYER_POWERUP_e powerup = mPowerup;
    u32 createItem = CREATE_ITEM_NONE;

    switch (dScStage_c::getExitMode()) {
        case dScStage_c::EXIT_3: {
            if (!isStatus(STATUS_55) || isNotBalloonCourse()) {
                if (isStatus(STATUS_53)) {
                    createItem = CREATE_ITEM_BUBBLE;
                } else if (isStatus(STATUS_OUT_OF_PLAY)) {
                    powerup = POWERUP_NONE;
                    createItem = CREATE_ITEM_BUBBLE;
                }
            }
            daYoshi_c *yoshi = getRideYoshi();
            if (yoshi != nullptr) {
                createItem = CREATE_ITEM_YOSHI;
                int fruitCount = yoshi->getFruitCount();
                u8 yoshiColor = yoshi->getModel()->getColor();
                daPyMng_c::setCarryOverYoshiInfo(mPlayerNo, yoshiColor, fruitCount);
                onStatus(STATUS_C5);
            }
            break;
        }
        case dScStage_c::EXIT_2:
            powerup = daPyMng_c::mPlayerMode[daPyMng_c::getPlayerType(mPlayerNo)];
            createItem = daPyMng_c::mCreateItem[daPyMng_c::getPlayerType(mPlayerNo)];
            break;
        case dScStage_c::EXIT_1:
            powerup = POWERUP_NONE;
            break;
        default:
            break;
    }

    daPyMng_c::mPlayerEntry[mPlayerNo] = 1;
    daPyMng_c::mPlayerMode[daPyMng_c::getPlayerType(mPlayerNo)] = powerup;
    daPyMng_c::mCreateItem[daPyMng_c::getPlayerType(mPlayerNo)] = createItem;
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
    setStatus(STATUS_CREATED);
    mCarryActorID = BASE_ID_NULL;
    mRideActorID = BASE_ID_NULL;
    mRelatedActorID = BASE_ID_NULL;
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
    if (dScStage_c::m_miniGame == 0 && dScStage_c::m_instance->mCurrCourse != STAGE_PEACH_CASTLE) {
        if ((daPyMng_c::mCreateItem[daPyMng_c::mPlayerType[mPlayerNo]] & CREATE_ITEM_STAR_POWER)) {
            setStar(STAR_SET_2, 660);
        } else {
            if (daPyMng_c::m_star_time[mPlayerNo] != 0) {
                setStar(STAR_SET_2, daPyMng_c::m_star_time[mPlayerNo]);
            }
            mStarCount = daPyMng_c::m_star_count[mPlayerNo];
        }
    }
    if (!isItemKinopio() && dScStage_c::m_gameMode == dScStage_c::GAME_MODE_NORMAL) {
        mSndObj.m_58 = dAudio::getRemotePlayer(mPlayerNo);
    }
    int x = 0;
    switch (mPlayerType) {
        case PLAYER_MARIO: x = 0; break;
        case PLAYER_LUIGI: x = 1; break;
        case PLAYER_YELLOW_TOAD: x = 2; break;
        case PLAYER_BLUE_TOAD: x = 3; break;
        default: break;
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
    int res = daPlBase_c::preExecute();
    mExecStopMask = oldExecStopMask;
    return res;
}

void dAcPy_c::postExecute(MAIN_STATE_e status) {
    if (status == SUCCESS) {
        mVec3_c pos = mPos;
        dAcPy_c *ridePlayer = getRidePlayer();
        if (ridePlayer != nullptr) {
            pos.x = ridePlayer->mPos.x;
        }
        if (mScrollMode != 3) {
            if (m_60 != 0) {
                bool notDone = false;
                if (!sLib::chase(&mPc.m_30.x, pos.x, 3.0f)) {
                    notDone = true;
                }
                if (!sLib::chase(&mPc.m_30.y, pos.y, 3.0f)) {
                    notDone = true;
                }
                if (!notDone) {
                    m_60 = 0;
                }
            } else {
                sLib::addCalc(&mPc.m_30.x, pos.x, 0.25f, 6.0f, 0.1f);
                sLib::addCalc(&mPc.m_30.y, pos.y, 0.25f, 6.0f, 0.1f);
            }
            if (isStatus(STATUS_5C)) {
                setScrollMode(5);
            } else {
                if (mScrollMode == 5) {
                    setScrollMode(0);
                }
            }
        }
    }
    daPlBase_c::postExecute(status);
}

void dAcPy_c::selectAction() {
    offStatus(STATUS_CAN_PENGUIN_SLIDE);
    if (std::fabs(mSpeedF) >= 1.0f) {
        mFastRunFrames++;
        if (mFastRunFrames >= 0) {
            onStatus(STATUS_CAN_PENGUIN_SLIDE);
        }
    } else {
        mFastRunFrames = 0;
    }

    if (isStatus(STATUS_07) && setSpinActionReq()) {
        return;
    }
    if (isDemo() || isStatus(STATUS_53)) {
        return;
    }
    if (setBalloonButtonA()) {
        return;
    }
    if (isStatus(STATUS_QUAKE_BIG)) {
        offStatus(STATUS_QUAKE_BIG);
        setQuakeAction(60, QUAKE_MODE_0);
    } else if (isStatus(STATUS_QUAKE_SMALL)) {
        offStatus(STATUS_QUAKE_SMALL);
        if (isNowBgCross(BGC_FOOT)) {
            setWaitJump(4.0f);
        }
    } else if (isStatus(STATUS_RIDE_YOSHI) && !isState(StateID_RideYoshi)) {
        changeState(StateID_RideYoshi, 0);
    } else if (setVineAction()) {
        return;
    } else if (setHangAction()) {
        return;
    } else if (setPoleAction()) {
        return;
    } else if (setKaniHangAction()) {
        return;
    } else if (checkWallJump()) {
        return;
    } else if (setSpinActionReq()) {
        return;
    } else if (setSwimAction()) {
        return;
    } else if (setFireBallAction()) {
        return;
    } else if (setTarzanRopeAction()) {
        return;
    }
}

void dAcPy_c::executeLastPlayer() {
    if (!isStatus(STATUS_CAN_EXECUTE)) {
        return;
    }
    clearHipAttackDamagePlayer();
    if (isStatus(STATUS_RIDE_YOSHI)) {
        setYoshiBackPos();
    }
    if (isStatus(STATUS_PLAYER_JUMP)) {
        setPlayerJumoDaiPos();
    }
    if (isStatus(STATUS_4F)) {
        setJrCrownPos();
    }
    calcModel();
    if (isStatus(STATUS_77)) {
        setPlayerEatReact();
        entryCollision();
        offStatus(STATUS_77);
        offStatus(STATUS_78);
    }
}

void dAcPy_c::executeLastAll() {
    if (!isStatus(STATUS_CAN_EXECUTE)) {
        return;
    }
    setSpinLiftUpReserve();
    setSpinAction();
    offStatus(STATUS_8A);
    if (isStatus(STATUS_45)) {
        setPlayerHandPos();
        mAng3_c ang = mAngle;
        ang.y += getMissSpinAngle();
        mPyMdlMng.calc(
            mPos,
            ang,
            getModelScale()
        );
    }
    if (!isChange()) {
        calcHeadAttentionAngle();
    }
    setSpinJumpEffect(0);
    setMissSpinJumpEffect(0);
    mPyMdlMng.calc2();
}

void dAcPy_c::calcModel() {
    mPyMdlMng.mpMdl->setDark(m_74);

    if (isNowBgCross(BGC_36)) {
        mPyMdlMng.mpMdl->m_17c |= 0x400;
    } else {
        mPyMdlMng.mpMdl->m_17c &= ~0x400;
    }

    daYoshi_c *yoshi = getRideYoshi();

    if (yoshi != nullptr) {
        mMtx_c mtx;
        yoshi->getModel()->getJointMtx(&mtx, 1);

        mtx.concat(mMtx_c::createTrans(
            dPyMdlMng_c::m_hio.getValue(mPlayerType, 0, mPowerup),
            dPyMdlMng_c::m_hio.getValue(mPlayerType, 1, mPowerup),
            0.0f
        ));

        mtx.ZrotM(-0x58E3);
        mtx.YrotM(0xC000);

        mtx.concat(mMtx_c::createTrans(0.0f, 0.0f, 15.0f));
        mtx.concat(mMtx_c::createScale(daPlBase_c::getReductionModelScale()));
        mtx.concat(mMtx_c::createTrans(0.0f, 0.0f, -15.0f));
        mtx.concat(mMtx_c::createScale(getModelScaleBase()));

        mPyMdlMng.calc(mtx);
        return;
    }

    if (daPlBase_c::isStatus(STATUS_4F)) {
        mPyMdlMng.calc(mRideJrClownMtx);
        return;
    }

    mVec3_c pos(
        mPos.x + m_1594,
        mPos.y + m_1598 + m_159c,
        mPos.z
    );

    mAng3_c rot(
        mAngle.x.mAngle,
        mAngle.y.mAngle + getMissSpinAngle(),
        mAngle.z.mAngle
    );

    mPyMdlMng.calc(pos, rot, getModelScale());
}

mVec3_c dAcPy_c::getModelScaleBase() {
    float scale = mAmiRelated2;
    mVec3_c res(
        mScale.x * scale,
        mScale.y * scale,
        mScale.z * scale
    );
    return res;
}

mVec3_c dAcPy_c::getModelScale() {
    return mVec3_c(
        getModelScaleBase().x * getReductionModelScale().x,
        getModelScaleBase().y * getReductionModelScale().y,
        getModelScaleBase().z * getReductionModelScale().z
    );
}

int dAcPy_c::draw() {
    if (isStatus(STATUS_INVULNERABLILITY_BLINK)) {
        offStatus(STATUS_INVULNERABLILITY_BLINK);
        return SUCCEEDED;
    }

    mPyMdlMng.draw();
    return SUCCEEDED;
}

bool dAcPy_c::fn_80145c00(PLAYER_POWERUP_e powerup) {
    if (isStatus(STATUS_OUT_OF_PLAY) || isStatus(STATUS_STUNNED)) {
        return false;
    }

    if (mPowerupCopy2 == powerup) {
        return false;
    }

    mPowerupCopy2 = powerup;
    return mPowerup != powerup;
}

void dAcPy_c::setStar(StarSet_e starSet, int timer) {
    setStarBase(starSet, timer);
    if (starSet == STAR_SET_0) {
        setVirusStar(getRidePlayer());
        setVirusStar(getCarryPlayer());
    }
}

void dAcPy_c::setStarBase(StarSet_e starSet, int timer) {
    if (starSet == STAR_SET_0) {
        startPlayerVoice(VOICE_GET_STAR, 0);
    }
    if (timer != 0) {
        mStarTimer = timer;
        if (timer >= 60) {
            daPyMng_c::startStarBGM();
        }
        mPyMdlMng.mpMdl->onStarAnm();
    }
}

void dAcPy_c::endStar() {
    if (mStarTimer != 0) {
        mStarTimer = 0;
        mPyMdlMng.mpMdl->offStarAnm();
        mPyMdlMng.mpMdl->offStarEffect();
        daPyMng_c::stopStarBGM();
    }
}

void dAcPy_c::calcStarTimer() {
    mPyMdlMng.mpMdl->offStarEffect();
    if (mStarTimer != 0) {
        mPyMdlMng.mpMdl->mScale = mVec3_c(mAmiRelated2, mAmiRelated2, mAmiRelated2);
        if (!isStatus(STATUS_INVISIBLE) && !isStatus(STATUS_5E) && mStarTimer > 25) {
            mPyMdlMng.mpMdl->onStarEffect();
        }
        if (mStarTimer < 10) {
            mPyMdlMng.mpMdl->offStarAnm();
        }
        if (mStarTimer == 120) {
            mStarTimer--;
            startSound(SE_SYS_STAR_FINISH, false);
            startSound(SE_SYS_STAR_FINISH_RC, false);
        }
        if (mStarTimer < 60) {
            daPyMng_c::stopStarBGM();
        }
    } else {
        clearStarCount();
    }
}

void dAcPy_c::setVirusStar(daPlBase_c *otherPlayer) {
    if (otherPlayer == nullptr) {
        return;
    }

    int selfTimer = mStarTimer;
    int otherTimer = otherPlayer->mStarTimer;

    if (selfTimer == otherTimer) {
        return;
    }

    if (selfTimer > otherTimer) {
        otherPlayer->setStar(STAR_SET_1, selfTimer);
    } else {
        setStar(STAR_SET_1, otherTimer);
    }
}

void dAcPy_c::setWaterWalkFlag() {
    if (isMameAction() && !isNowBgCross(BGC_WATER_SHALLOW)) {
        onStatus(STATUS_CAN_WATER_WALK);
    }
}

void dAcPy_c::fn_80145fd0(int jumpType) {
    static const dAudio::SoundEffectID_t sc_JumpSoundNormal[] = { SE_PLY_JUMP, SE_PLY_2NDJUMP, SE_PLY_3RDJUMP };
    static const dAudio::SoundEffectID_t sc_JumpSoundSuper[] = { SE_PLY_JUMP_S, SE_PLY_2NDJUMP_S, SE_PLY_3RDJUMP_S };
    static const dAudio::SoundEffectID_t sc_JumpSoundMame[] = { SE_PLY_JUMP_SS, SE_PLY_2NDJUMP_SS, SE_PLY_3RDJUMP_SS };
    if (jumpType == 2 && !mKey.buttonJump()) {
        jumpType = 0;
    }
    switch (jumpType) {
        case 1:
            switch (mPowerup) {
                case POWERUP_NONE:
                    startSound(sc_JumpSoundSuper[mJumpCounter], true);
                    break;
                case POWERUP_MINI_MUSHROOM:
                    startSound(sc_JumpSoundMame[mJumpCounter], true);
                    break;
                default:
                    startSound(sc_JumpSoundNormal[mJumpCounter], true);
                    break;
            }
            break;
        case 2:
            switch (mPowerup) {
                case POWERUP_NONE:
                    startSound(SE_PLY_JUMP_SS_HIGH, false);
                    break;
                case POWERUP_MINI_MUSHROOM:
                    startSound(SE_PLY_JUMP_S_HIGH, false);
                    break;
                default:
                    startSound(SE_PLY_JUMP_HIGH, false);
                    break;
            }
    }
}

bool dAcPy_c::isNoDamage() {
    if (
        (mDamageInvulnTimer | mPowerupChangeInvulnTimer) != 0 ||
        isDemo() ||
        isDemoType(DEMO_PLAYER) ||
        isStatus(STATUS_DISPLAY_OUT_NO_DAMAGE) ||
        isStatus(STATUS_84)
    ) {
        return true;
    }
    return false;
}

bool dAcPy_c::setDamage(dActor_c *actor, daPlBase_c::DamageType_e damageType) {
    if (mPowerup != mPowerupCopy2 || isChange()) {
        return false;
    }

    if (isNoDamage()) {
        return false;
    }

    if (setDamage2(actor, damageType)) {
        dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
        return true;
    }

    return false;
}

bool dAcPy_c::setForcedDamage(dActor_c *actor, daPlBase_c::DamageType_e damageType) {
    if (isDemo() || mPowerup != mPowerupCopy2 || isChange()) {
        return false;
    }

    if (setDamage2(actor, damageType)) {
        dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
        return true;
    }

    return false;
}

bool dAcPy_c::setDamage2(dActor_c *actor, daPlBase_c::DamageType_e damageType) {
    if (isStatus(STATUS_OUT_OF_PLAY) || isStatus(STATUS_STUNNED) || isStatus(STATUS_53)) {
        return false;
    }
    releaseCarryActor();
    if (damageType == DAMAGE_DEFAULT && isStatus(STATUS_PENGUIN_SLIDE) && actor != nullptr && actor->mActorProperties & 0x200) {
        if (setFlyDamageAction(3, actor)) {
            onStatus(STATUS_PENGUIN_RECOIL);
            return true;
        }
        return false;
    }
    switch (damageType) {
        case DAMAGE_YOGAN:
            changeDemoState(StateID_DemoFireDown, 0);
            break;
        case DAMAGE_ELEC_SHOCK:
            if (isStatus(STATUS_4F)) {
                setJrCrownElecDamage();
            } else {
                changeState(StateID_ElecShock, 0);
            }
            break;
        case DAMAGE_POISON:
            changeDemoState(StateID_DemoDown, DEMO_DOWN_ARG_POISON);
            break;
        case DAMAGE_SQUISH:
            changeDemoState(StateID_DemoDown, DEMO_DOWN_ARG_HIT);
            break;
        case DAMAGE_POISON_FOG:
            changeDemoState(StateID_DemoDown, DEMO_DOWN_ARG_POISON_FOG);
            break;
        case DAMAGE_EAT_DIE:
        case DAMAGE_D:
            changeDemoState(StateID_DemoEatDie, damageType);
            mRelatedActorID = actor->mUniqueID;
            break;
        case DAMAGE_HIP_ATTACK:
        case DAMAGE_4:
        case DAMAGE_CLIMB:
        case DAMAGE_6:
            return setFlyDamageAction(damageType, actor);
        case DAMAGE_11:
            setReductionScale();
            break;
        case DAMAGE_10:
            changeState(StateID_IceDamage, 0);
            break;
        default:
            if (mPowerup == POWERUP_NONE || mPowerup == POWERUP_MINI_MUSHROOM) {
                if (damageType == DAMAGE_8) {
                    changeDemoState(StateID_DemoFireDown, 0);
                } else {
                    changeDemoState(StateID_DemoDown, DEMO_DOWN_ARG_HIT);
                }
            } else {
                if (mPowerup == POWERUP_MUSHROOM) {
                    fn_80145c00(POWERUP_NONE);
                } else {
                    fn_80145c00(POWERUP_MUSHROOM);
                }
                if (damageType == DAMAGE_2) {
                    mDamageInvulnTimer = 127;
                    return setFlyDamageAction(damageType, actor);
                }
                if (damageType == DAMAGE_FREEZE) {
                    changeState(StateID_IceDamage, 0);
                } else {
                    mDamageInvulnTimer = 127;
                }
            }
            break;
    }
    return true;
}

u8 dAcPy_c::getTallType(s8 powerup) {
    const u8 l_tall_type[] = {
        1, 2, 2, 0, 2, 2, 2
    };

    if (powerup == -1) {
        powerup = mPowerup;
    }
    return l_tall_type[powerup];
}

void dAcPy_c::setCenterOffset() {
    float l_powerup_offsets[] = {
        16.0f, 31.0f, 31.0f, 6.0f, 34.0f, 31.0f, 31.0f
    };

    mModelHeight = l_powerup_offsets[mPowerup];

    mVec3_c offset(0.0f, 0.0f, 0.0f);
    if (mPyMdlMng.mpMdl->mFlags & 1) {
        offset.y = mModelHeight / 4.0f;
    } else if (mPyMdlMng.mpMdl->mFlags & 4) {
        offset.y = -(mModelHeight / 2.0f);
    } else {
        offset.y = mModelHeight / 2.0f;
    }
    mCenterOffs = offset;

    float target = 16.0f;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        target = 8.0f;
    }
    sLib::chase(&mViewLimitPadding, target, 0.1f);
}

void dAcPy_c::setPlayerData() {
    static const mVec2_POD_c l_player_cull_data[] = {
        {0.0f, 20.0f},
        {15.0f, 22.0f}
    };
    setVisibleArea(l_player_cull_data[0], l_player_cull_data[1]);
}

void dAcPy_c::setSpeedData() {
    static const int scSpeedHioData[] = { 1, 1, 1, 0, 1, 1, 1 };
    u8 index = scSpeedHioData[mPowerup];
    mSpeedDataNormal = &m_speed_hio[index].mDataNormal;
    mSpeedDataStar = &m_speed_hio[index].mDataStar;
}

void dAcPy_c::setModeGravity() {
    int index = 0;
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        index = 1;
    }
    mGravityData = daPlayerData_c::smc_POWER_CHANGE_DATA.mGravityData[index];
}

const float *dAcPy_c::getGravityData() {
    if (isMameAction()) {
        return daPlayerData_c::smc_POWER_CHANGE_DATA.mGravityData[1];
    } else {
        return daPlayerData_c::smc_POWER_CHANGE_DATA.mGravityData[0];
    }
}

void dAcPy_c::setPowerup(PLAYER_POWERUP_e powerup, int) {
    mPowerupCopy = mPowerup;
    mPowerup = powerup;
    mPyMdlMng.mpMdl->setPlayerMode(mPowerup);
    setPlayerData();
    setCenterOffset();
    setSpeedData();
    mAccelF = getSpeedData()->mPowerChangeNormal.mSlowAccel;
    setModeGravity();
    offStatus(STATUS_IS_PENGUIN);
    switch (mPowerup) {
        case POWERUP_PROPELLER_SHROOM:
            if (!isItemKinopio()) {
                if (fn_801477c0()) {
                    dGameCom::fn_800b37b0(mPlayerNo, 1);
                }
                m_1580 = 0;
            }
            resetPropelFlyTime();
            break;
        case POWERUP_PENGUIN_SUIT:
            onStatus(STATUS_IS_PENGUIN);
            break;
        default:
            break;
    }
    mBc.mPlayerFlags &= ~BIT_FLAG(3);
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        mBc.mPlayerFlags |= BIT_FLAG(3);
    }
    setSoundPlyMode();
}

bool dAcPy_c::setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    if (mPowerup != POWERUP_MINI_MUSHROOM && isClimbing()) {
        return false;
    }
    return _setJump(jumpSpeed, speedF, allowSteer, keyMode, jumpMode);
}

bool dAcPy_c::_setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    if (isDemo() || isStatus(STATUS_OUT_OF_PLAY)) {
        return false;
    }
    if (isStatus(STATUS_DISABLE_STATE_CHANGE) && isStatus(STATUS_53)) {
        return false;
    }
    if (setSwimSpeed(jumpSpeed * 0.5f, speedF * 0.5f)) {
        return true;
    }
    clearJumpActionInfo(0);
    if (isStatus(STATUS_KANI_WALK)) {
        changeState(StateID_KaniJump);
    } else {
        bool res = false;
        mSpeedF = speedF;
        if (jumpSpeed <= 0.0f) {
            if (isStatus(STATUS_80)) {
                if (isOldBgCross(BGC_FOOT)) {
                    res = true;
                }
            } else {
                if (isNowBgCross(BGC_FOOT)) {
                    res = false;
                }
            }
        }
        if (!res) {
            bool isSpinJump = false;
            bool isSitJump = false;
            if (isStatus(STATUS_SPIN) && isStatus(STATUS_JUMP)) {
                isSpinJump = true;
            } else if (isStatus(STATUS_SIT_JUMP)) {
                isSitJump = true;
            }
            if (jumpSpeed == 0.0f) {
                changeState(StateID_Fall, false);
            } else {
                jmpInf_c jump(jumpSpeed, jumpMode, BLEND_DEFAULT);
                changeState(StateID_Jump, &jump);
            }
            onStatus(STATUS_AB);
            onStatus(STATUS_88);
            if (isSpinJump) {
                int param = 2;
                if (jumpMode == 1) {
                    param = 1;
                }
                changeState(StateID_SpinJump, (void *) param);
            } else if (isSitJump) {
                changeState(StateID_SitJump, true);
            }
        }
    }
    offStatus(STATUS_A7);
    offNowBgCross(BGC_FOOT);
    mWallSlideCooldown = 4;
    if (!allowSteer) {
        mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
        onStatus(STATUS_A7);
    }
    if (keyMode == 1) {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    } else if (keyMode == 2) {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    }

    return true;
}

bool dAcPy_c::fn_80146e40(float a, float b, bool c) {
    if (isDemo() || isStatus(STATUS_OUT_OF_PLAY) || isNowBgCross(BGC_WATER_SHALLOW)) {
        return false;
    }
    checkSinkSand();
    if (getSpinActionMode() == 0 || isOnSinkSand()) {
        mSpeed.y = a;
        mSpeedF = b;
        changeState(StateID_SpinJump, (void *) 1);
        setJumpCommonBase();
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
        offNowBgCross(BGC_FOOT);
        mWallSlideCooldown = 4;
        offStatus(STATUS_A7);
    } else {
        mSpeedF = b;
        changeState(StateID_Propel, (void *) 2);
    }
    return true;
}

bool dAcPy_c::setCannonJump(float speedY, float speedF, int jumpType) {
    mSpeedF = speedF;
    mMaxSpeedF = speedF;
    mSpeed.y = speedY;
    if (mSpeedF > 0.0f) {
        mDirection = DIR_LR_R;
    } else {
        mDirection = DIR_LR_L;
    }
    changeState(StateID_CannonJump, (void *) jumpType);
    return true;
}

bool dAcPy_c::setSwimSpeed(float speedY, float speedF) {
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        setUzuSwimAction();
        mSpeed.y = speedY;
        mSpeedF = speedF;
        m_b88 = 20;
        return true;
    }
    return false;
}

bool dAcPy_c::isEnablePropelJump() {
    if (mSpinCooldown != 0 || isNowBgCross(BGC_WATER_SHALLOW) || !isStatus(STATUS_9F)) {
        return false;
    }

    if (isDemo() || isStatus(STATUS_AUTO_BOUNCE)) {
        return false;
    }

    return getSpinActionMode() == 1;
}

int dAcPy_c::getSpinActionMode() {
    s8 tmp = getPowerup();
    if (getCarryPropelActor() != nullptr) {
        tmp = POWERUP_PROPELLER_SHROOM;
    } else {
        dAcPy_c *carryPlayer = getCarryPlayer();
        if (carryPlayer != nullptr) {
            tmp = carryPlayer->getPowerup();
        } else {
            if (tmp == POWERUP_PROPELLER_SHROOM && isLiftUp()) {
                tmp = POWERUP_MUSHROOM;
            }
        }
    }
    return tmp == POWERUP_PROPELLER_SHROOM ? 1 : 0;
}

bool dAcPy_c::setSpinActionReq() {
    mKey.offStatus(dAcPyKey_c::STATUS_2);
    if (mPowerup == POWERUP_PROPELLER_SHROOM) {
        mKey.onStatus(dAcPyKey_c::STATUS_2);
    }

    if (
        mSpinCooldown != 0 ||
        !mKey.triggerShakeJump() ||
        isNowBgCross(BGC_WATER_SHALLOW) ||
        !isStatus(STATUS_9F)
    ) {
        return false;
    }

    if (isNowBgCross(BGC_FOOT)) {
        setCcAtSpinLiftUp();
        clearSpinLiftUpReserve();
    }
    onStatus(STATUS_8A);
    return true;
}

bool dAcPy_c::setSpinAction() {
    if (
        isDemo() ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_QUAKE) ||
        isStatus(STATUS_AUTO_BOUNCE) ||
        !isStatus(STATUS_8A)
    ) {
        return false;
    }

    if (checkStandUpRoof()) {
        return false;
    }

    if (isNowBgCross(BGC_HEAD)) {
        return false;
    }

    mSpinCooldown = 30;
    resetMissSpin();
    if (getSpinActionMode() == 0) {
        if (isNowBgCross(BGC_FOOT)) {
            changeState(StateID_SpinJump, 0);
            return true;
        }
    } else {
        if (setPropelAction()) {
            return true;
        }
    }

    setMissSpin();
    return true;
}

void dAcPy_c::setScrollMode(s8 mode) {
    if (!isItemKinopio()) {
        mScrollMode = mode;
    } else {
        mScrollMode = 1;
    }
}

void dAcPy_c::setInvalidKeyTimer(int p1, int p2) {
    m_1318[0] = p1;
    m_1310[0] = p2;
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void dAcPy_c::setInvalidKeyTimer_LR(int p1, int p2) {
    m_1318[1] = p1;
    m_1310[1] = p2;
    mKey.onStatus(dAcPyKey_c::STATUS_DISABLE_LR);
}

void dAcPy_c::calcInvalidKeyTimer() {
    for (int i = 0; i < 2; i++) {
        if (m_1318[i] != 0) {
            if (m_1310[i] == 2) {
                if (mSpeed.y <= 0.0f || isNowBgCross(BGC_FOOT)) {
                    m_1318[i] = 0;
                }
            } else if (m_1310[i] == 1) {
                if (isNowBgCross(BGC_FOOT)) {
                    m_1318[i] = 0;
                }
            }
            sLib::calcTimer(&m_1318[i]);
            if (m_1318[i] == 0) {
                if (i == 0) {
                    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
                } else {
                    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
                }
            }
        }
    }
}

void dAcPy_c::startQuakeShock(dQuake_c::TYPE_SHOCK_e type) {
    daPlBase_c::startQuakeShock(type);
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        carryPlayer->startQuakeShock(type);
    }
}

void dAcPy_c::startPatternRumble(const char *pattern) {
    daPlBase_c::startPatternRumble(pattern);
    dAcPy_c *carryPlayer = getCarryPlayer();
    if (carryPlayer != nullptr) {
        carryPlayer->startPatternRumble(pattern);
    }
}

void dAcPy_c::setItemGetEffect() {
    mPowerUpEffectTimer = 90;
    mPowerUpType = 0;
}

void dAcPy_c::setChukanPowerUpEffect() {
    mPowerUpEffectTimer = 90;
    mPowerUpType = 1;
}

void dAcPy_c::endPowerUpEffect() {
    if (mPowerUpEffectTimer != 0) {
        mItemGetEffect1.fade();
        if (mPowerUpType == 0) {
            mItemGetEffect2.fade();
        }
    }
    mPowerUpEffectTimer = 0;
}

void dAcPy_c::updateItemGetEffect() {
    if (mPowerUpEffectTimer == 0) {
        return;
    }
    mVec3_c efPos = getCenterPos();
    mVec3_c efScale(1.0f, 1.0f, 1.0f);
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        efScale.set(0.7f, 0.7f, 0.7f);
    }
    mItemGetEffect1.createEffect("Wm_mr_itemget01", 0, &efPos, nullptr, &efScale);
    if (mPowerUpType == 0) {
        int alpha = mPowerUpEffectTimer * 8;
        if (alpha > 255) {
            alpha = 255;
        }
        mItemGetEffect1.setColor(255, 255, 255, alpha, EGG::Effect::RECURSIVE_3);
        mItemGetEffect1.copyExEffectParam();
        mItemGetEffect2.createEffect("Wm_mr_itemget02", 0, &efPos, nullptr, &efScale);
    }
}

bool dAcPy_c::fn_801477c0() {
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_MULTIPLAYER_MODE) {
        return false;
    }
    if (
        dScStage_c::m_instance->mCurrWorld == WORLD_1 &
        dScStage_c::m_instance->mCurrCourse == STAGE_1 &&
        !dSaveMng_c::m_instance->getSaveGame(-1)->isCourseDataFlag(0, 0, 0x10)
    ) {
        return true;
    }
    return false;
}

bool dAcPy_c::searchKinopioNearPlayer(float *dist) {
    dAcPy_c *player;
    float closest = 1600.0f;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (!(daPyMng_c::mActPlayerInfo & (1 << (u8) i))) {
            continue;
        }
        player = daPyMng_c::getPlayer(i);
        if (player == nullptr) {
            continue;
        }
        if (player == this || !player->isDrawingCarryFukidashi()) {
            continue;
        }
        mVec2_c diff(
            player->mPos.x - mPos.x,
            player->mPos.y - mPos.y
        );
        if (closest > diff.length()) {
            closest = diff.length();
        }
    }
    if (closest >= 1600.0f) {
        return false;
    }
    if (dist != nullptr) {
        *dist = closest;
    }
    return true;
}

void dAcPy_c::managePropelFukidashi() {
    if (m_157c != 0) {
        m_157c--;
    }
    if (m_1584 != 0) {
        m_1584--;
    }
    if (isItemKinopio()) {
        int check = false;
        float dist;
        if (isSpinLiftUpEnable() && searchKinopioNearPlayer(&dist)) {
            if (m_1580 == 0) {
                if (dist < 48.0f) {
                    check = true;
                }
            } else if (dist < 80.0f) {
                check = true;
            }
        }
        if (check == true) {
            if (m_157c != 0) {
                check = false;
            }
        } else {
            m_157c = 3;
        }
        if (m_1580 == 0) {
            if (check == true) {
                m_1580 = 1;
                dGameCom::showFukidashi(mPlayerNo, 8);
                m_1584 = 240;
            }
        } else {
            if (m_1584 == 0) {
                dGameCom::hideFukidashiTemporarily(mPlayerNo, 8, 0);
            }
            if (!check) {
                m_1580 = 0;
                dGameCom::hideFukidashiTemporarily(mPlayerNo, 8, 0);
            }
        }
    } else {
        int check = false;
        if (isDrawingFukidashi() && isEnablePropelJump() && getCarryPropelBlock() == nullptr) {
            if (m_157c == 0) {
                check = true;
            }
        } else {
            m_157c = 3;
        }
        if (m_1580 == 0) {
            if (check == true) {
                m_1580 = 1;
                m_1584 = 480;
            }
        } else if (m_1584 == 0 && !fn_801477c0()) {
            dGameCom::hideFukidashiTemporarily(mPlayerNo, 1, 0);
        } else if (!check) {
            dGameCom::hideFukidashiTemporarily(mPlayerNo, 1, 0);
        } else {
            dGameCom::showFukidashi(mPlayerNo, 1);
        }
    }
}

bool dAcPy_c::isDrawingFukidashi() {
    if (
        isItemKinopio() ||
        !isStatus(STATUS_9F) ||
        isDemoAll() ||
        mSpinCooldown != 0 ||
        isNowBgCross(BGC_WATER_SHALLOW) ||
        isStatus(STATUS_GOAL_POLE_TOUCHED) ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        dScStage_c::m_miniGame != 0
    ) {
        return false;
    }
    int level = dScStage_c::m_instance->mCurrCourse;
    return level != STAGE_TITLE &&
           level != STAGE_PEACH_CASTLE &&
           level != STAGE_STAFFROLL;
}

bool dAcPy_c::isDrawingCarryFukidashi() {
    if (
        !isDrawingFukidashi() ||
        !mBc.isFoot() ||
        fManager_c::searchBaseByID(mCarryActorID) != nullptr
    ) {
        return false;
    }
    return true;
}

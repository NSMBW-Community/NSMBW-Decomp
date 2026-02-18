#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_cd.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_next.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_stage_timer.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_attention.hpp>
#include <game/bases/d_mask_manager.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_quake.hpp>
#include <game/bases/d_rail.hpp>
#include <game/bases/d_remocon_manager.hpp>
#include <game/mLib/m_fader.hpp>
#include <game/cLib/c_math.hpp>
#include <game/sLib/s_math.hpp>
#include <constants/sound_list.h>

const float daPlBase_c::sc_DirSpeed[] = { 1.0f, -1.0f };
const float daPlBase_c::sc_JumpSpeed = 3.628f;
const float daPlBase_c::sc_JumpSpeedNuma1 = 3.5f;
const float daPlBase_c::sc_JumpSpeedNuma2 = 2.3f;
const float daPlBase_c::sc_WaterWalkSpeed = 0.5625f;
const float daPlBase_c::sc_WaterSwimSpeed = 1.125f;
const float daPlBase_c::sc_WaterJumpSpeed = 1.25f;
const float daPlBase_c::sc_WaterMaxFallSpeed = -3.0f;
const float daPlBase_c::sc_MaxFallSpeed = -4.0f;
const float daPlBase_c::sc_MaxFallSpeed_Foot = -2.0f;
const float daPlBase_c::sc_MaxFallDownSpeed = -8.5f;
const float daPlBase_c::scTurnPowerUpRate = 3.0f;

namespace {
    const float l_sakaMaxSpeedRatio[][3] = {
        { 1.0f, 1.0f, 1.0f },
        { 1.25f, 0.6875f, 1.0f },
        { 1.25f, 0.6875f, 1.0f },
        { 1.44f, 0.5f, 1.0f },
        { 1.62f, 0.3f, 1.0f },
    };
    const float l_sakaStopAccele[][3] = {
        {  0.035f, 0.035f, 0.035f },
        {  0.0315f, 0.046f, 0.035f },
        {  0.028f, 0.06f, 0.035f },
        {  0.021f, 0.123f, 0.035f },
        {  0.014f, 0.175f, 0.035f },
    };
    const float l_sakaMoveAccele[][3] = {
        { 0.1f, 0.03f, 0.03f },
        { 0.04f, 0.03f, 0.03f },
        { 0.05f, 0.045f, 0.03f },
        { 0.07f, 0.12f, 0.03f },
        { 0.17f, 0.17, 0.03f },
    };
    const float l_sakaSlipOffSpeed[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.2f, -0.2f, 0.0f },
        { 0.4f, -0.4f, 0.0f },
        { 0.4f, -0.4f, 0.0f },
    };
}

daPlBase_c::daPlBase_c() :
    mpMdlMng(nullptr),
    mDemoStateMgr(*this, sStateID::null),
    mStateMgr(*this, sStateID::null)
{
    mAreaNo = dScStage_c::m_instance->mCurrAreaNo;
    mLayer = dScStage_c::m_instance->mCurrLayer;
    mAmiLayer = 1;
    mBc.mOwningPlrNo = mPlayerNo;
    changeDemoState(StateID_DemoNone, false);
    changeState(StateID_None);
    mAttentionMode = 3;
    mViewLimitPadding = 16.0f;
    mEatBehaviour = EAT_TYPE_NONE;
}

daPlBase_c::~daPlBase_c() {
    releaseCcData();
    mpMdlMng = nullptr;
}

int daPlBase_c::create() {
    mKey.init();
    if (dScStage_c::m_gameMode == dScStage_c::GAME_MODE_TITLE_SCREEN) {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_RIGHT);
    }
    setZPosition();
    return SUCCEEDED;
}

int daPlBase_c::preExecute() {
    offStatus(STATUS_CAN_EXECUTE);
    if (dActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    if (isStatus(STATUS_STOP_EXECUTE)) {
        return NOT_READY;
    }
    onStatus(STATUS_CAN_EXECUTE);
    if (isNowBgCross(BGC_LIFT)) {
        mLastPos = mLiftRelatedPos;
    }
    mBgPressFlags = 0;
    offStatus(STATUS_84);
    offStatus(STATUS_B7);
    offStatus(STATUS_40);
    offStatus(STATUS_61);
    if (isStatus(STATUS_83)) {
        onStatus(STATUS_82);
        offStatus(STATUS_83);
    }
    return SUCCEEDED;
}

int daPlBase_c::execute() {
    updateEndingDance();
    if (isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE)) {
        mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
    }
    mKey.update();
    setZPosition();
    releaseCcData();
    executeMain();
    mKey.updateEnd();
    mSndObj.calculate(dAudio::cvtSndObjctPos(getCenterPos()));
    return SUCCEEDED;
}

void daPlBase_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        mLastPosDelta = mLastPos - mPos;
        mLiftRelatedPos = mPos;
        offStatus(STATUS_1A);
        offStatus(STATUS_1B);
        offStatus(STATUS_BE);
        offStatus(STATUS_RIDE_NUT);
        offStatus(STATUS_62);
        offStatus(STATUS_5B);
        offStatus(STATUS_5C);
        offStatus(STATUS_89);
        offStatus(STATUS_QUAKE_BIG);
        offStatus(STATUS_QUAKE_SMALL);
        offStatus(STATUS_SWIM_AGAINST_JET_H);
        offStatus(STATUS_SWIM_AGAINST_JET_V);
        offStatus(STATUS_82);
        if (isStatus(STATUS_8D)) {
            offStatus(STATUS_8D);
            if (isStatus(STATUS_DISPLAY_OUT_DEAD)) {
                onStatus(STATUS_STOP_EXECUTE);
            }
        }
        offStatus(STATUS_WAS_TWIRL);
        if (isStatus(STATUS_TWIRL)) {
            offStatus(STATUS_TWIRL);
            onStatus(STATUS_WAS_TWIRL);
        }
        offStatus(STATUS_A4);
        clearFollowMameKuribo();
        clearCcPlayerRev();
        clearCcData();
    }
    dActor_c::postExecute(status);
}

int daPlBase_c::doDelete() {
    mIsBeingDeleted = true;
    onStatus(STATUS_ABOUT_TO_BE_DELETED);
    mStateMgr.finalizeState();
    return SUCCEEDED;
}

int daPlBase_c::preDraw() {
    if (isStatus(STATUS_INVISIBLE)) {
        mVisible = false;
    } else {
        mVisible = true;
    }
    if (dBaseActor_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

int daPlBase_c::draw() {
    return SUCCEEDED;
}

STATE_VIRTUAL_DEFINE(daPlBase_c, None);
STATE_VIRTUAL_DEFINE(daPlBase_c, Walk);
STATE_VIRTUAL_DEFINE(daPlBase_c, Jump);
STATE_VIRTUAL_DEFINE(daPlBase_c, SitJump);
STATE_VIRTUAL_DEFINE(daPlBase_c, Fall);
STATE_VIRTUAL_DEFINE(daPlBase_c, Land);
STATE_VIRTUAL_DEFINE(daPlBase_c, Crouch);
STATE_VIRTUAL_DEFINE(daPlBase_c, Slip);
STATE_VIRTUAL_DEFINE(daPlBase_c, Turn);
STATE_VIRTUAL_DEFINE(daPlBase_c, HipAttack);
STATE_VIRTUAL_DEFINE(daPlBase_c, Swim);
STATE_VIRTUAL_DEFINE(daPlBase_c, JumpDai);
STATE_VIRTUAL_DEFINE(daPlBase_c, PlayerJumpDai);
STATE_VIRTUAL_DEFINE(daPlBase_c, Funsui);
STATE_VIRTUAL_DEFINE(daPlBase_c, Kani);
STATE_VIRTUAL_DEFINE(daPlBase_c, Cloud);
STATE_VIRTUAL_DEFINE(daPlBase_c, AnimePlay);
STATE_VIRTUAL_DEFINE(daPlBase_c, WaitJump);

void daPlBase_c::changeState(const sStateIDIf_c &stateID, void *arg) {
    if (isStatus(STATUS_DISABLE_STATE_CHANGE)) {
        return;
    }
    clearComboCount();
    mSubstate = 0;
    mSubstateTimer = 0;
    mSubstateValue = 0;
    offStatus(STATUS_97);
    offStatus(STATUS_98);
    mStateArg = arg;
    mStateMgr.changeState(stateID);
}

void daPlBase_c::executeState() {
    if (mSubstateTimer != 0) {
        mSubstateTimer--;
    }
    mStateMgr.executeState();
}

short daPlBase_c::getMukiAngle(u8 direction) {
    static const short scMukiAngle[] = { 0x3000, -0x3000 };
    return scMukiAngle[direction];
}

int daPlBase_c::turnAngle() {
    return mAngle.y.chase(getMukiAngle(mDirection), 0x2000);
}

int daPlBase_c::addCalcAngleY(short p1, short p2) {
    return sLib::addCalcAngle(&mAngle.y.mAngle, p1, p2, 0x2000, 0x400);
}

short daPlBase_c::getBesideMukiAngle(u8 direction) {
    static const short scMukiAngle[] = { 0x4000, -0x4000 };
    return scMukiAngle[direction];
}

void daPlBase_c::turnBesideAngle() {
    mAngle.y.chase(getBesideMukiAngle(mDirection), 0x2000);
}

bool daPlBase_c::checkRideActor(daPlBase_c *other) {
    if (mRideActorID == other->mUniqueID || other->mRideActorID == mUniqueID) {
        return true;
    }
    return false;
}

void daPlBase_c::initializeState_None() {}
void daPlBase_c::finalizeState_None() {}
void daPlBase_c::executeState_None() {}

void daPlBase_c::setWaitActionAnm(AnmBlend_e blendMode) {}
void daPlBase_c::setWalkActionAnm(AnmBlend_e blendMode) {}

void daPlBase_c::walkActionInit_Wait(AnmBlend_e blendMode) {}
void daPlBase_c::walkAction_Wait() {}

void daPlBase_c::walkActionInit_Move(AnmBlend_e blendMode) {}
void daPlBase_c::walkAction_Move() {
    AnmBlend_e blendMode = BLEND_DEFAULT;
    if (!mSpeedF) {
        if (isNowBgCross(BGC_11) || mTimer_f4 != 0 || mWindSpeed) {
            if (mKey.buttonWalk(nullptr)) {
                blendMode = BLEND_NONE;
            } else {
                walkActionInit_Wait(BLEND_DEFAULT);
                return;
            }
        } else {
            if (isNowBgCross(BGC_WALL)) {
                if (mPowerup == POWERUP_MINI_MUSHROOM && mBc.getSakaUpDown(mDirection) == 1) {
                    blendMode = BLEND_NONE;
                } else {
                    walkActionInit_Wait(BLEND_DEFAULT);
                    return;
                }
            }
            if (isNowBgCross(BGC_SLOPE_AND_HEAD) || !mKey.buttonWalk(nullptr)) {
                walkActionInit_Wait(BLEND_DEFAULT);
                return;
            }
        }
    } else if (isNowBgCross(BGC_LIFT)) {
        mTimer_f4 = 3;
    }
    setWalkActionAnm(blendMode);
    setRunFootEffect();
}

void daPlBase_c::initializeState_Walk() {}

void daPlBase_c::finalizeState_Walk() {
    offStatus(STATUS_52);
}

void daPlBase_c::executeState_Walk() {
    if (isStatus(STATUS_52)) {
        offStatus(STATUS_52);
    }
}

void daPlBase_c::calcSpeedOnIceLift() {
    /// @unofficial
    static const float csSpeedMult[] = { 0.5f, 0.8f };
    /// @unofficial
    static const float csSpeedMultNoMove[] = { 0.3f, 1.0f };
    if (
        mPowerup != POWERUP_PENGUIN_SUIT &&
        isNowBgCross(BGC_LIFT) &&
        isNowBgCross(BGC_ON_ICE) &&
        std::fabs(mBc.mIceSpeed) > 1.0f
    ) {
        u8 idx = 0;
        if (mBc.mIceSpeed < 0.0f) {
            idx = 1;
        }
        if (mKey.buttonWalk(nullptr)) {
            mSpeedF *= csSpeedMult[idx];
        } else {
            mSpeedF *= csSpeedMultNoMove[idx];
        }

        float newSpeedF = std::fabs(mSpeedF);
        if (newSpeedF < 0.3f) {
            newSpeedF = 0.3f;
        } else if (newSpeedF > 1.5f) {
            newSpeedF = 1.5f;
        }

        if (mSpeedF < 0.0f) {
            mSpeedF = -newSpeedF;
        } else {
            mSpeedF = newSpeedF;
        }
    }
}

void daPlBase_c::calcAccOnIceLift() {
    if (
        isNowBgCross(BGC_LIFT) &&
        isNowBgCross(BGC_ON_ICE) &&
        !mKey.buttonWalk(nullptr) &&
        std::fabs(mBc.mIceSpeed) > 1.0f
    ) {
        mAccelF = calcSomeAccel(mAccelF);
    }
}

void daPlBase_c::initializeState_Jump() {
    onStatus(STATUS_A0);
    onStatus(STATUS_A5);
    offStatus(STATUS_88);
    if (mJumpDaiFallTimer != 0) {
        onStatus(STATUS_JUMP_DAI_COOLDOWN);
    }
}
void daPlBase_c::finalizeState_Jump() {
    mAngle.x = 0;
    if (isStatus(STATUS_A5)) {
        mSndObj.stopPlyJumpSound();
    }
    offStatus(STATUS_JUMP);
    offStatus(STATUS_88);
    offStatus(STATUS_A5);
    offStatus(STATUS_AB);
    offStatus(STATUS_A0);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);
    mJumpDaiFallTimer = 0;
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
}
void daPlBase_c::executeState_Jump() {
    if (mJumpDaiFallTimer == 0) {
        offStatus(STATUS_JUMP_DAI_COOLDOWN);
    }
    if (isStatus(STATUS_A5)) {
        if (!mKey.buttonJump() || isNowBgCross(BGC_HEAD)) {
            mSndObj.stopPlyJumpSound();
            offStatus(STATUS_A5);
        }
        if (mSpeed.y <= 1.0f) {
            offStatus(STATUS_A5);
        }
    }
    onStatus(STATUS_JUMP);
}

bool daPlBase_c::setCrouchJump() {
    if (mKey.triggerJump()) {
        changeState(StateID_SitJump, false);
        return true;
    }
    return false;
}

void daPlBase_c::initializeState_SitJump() {}
void daPlBase_c::finalizeState_SitJump() {}
void daPlBase_c::executeState_SitJump() {}

void daPlBase_c::setFallAction() {
    if (isNowBgCross(BGC_FOOT)) {
        return;
    }
    changeState(StateID_Fall, false);
}

void daPlBase_c::initializeState_Fall() {
    onStatus(STATUS_A0);
    if (mJumpDaiFallTimer != 0) {
        onStatus(STATUS_JUMP_DAI_COOLDOWN);
    }
    if (!stateArg<bool>()) {
        mpMdlMng->setAnm(PLAYER_ANIM_JUMP2, 10.0f, 0.0f);
    }
    if (isOldBgCross(BGC_LIFT)) {
        if (m_1134 * mSpeedF > 0.0f) {
            m_1138 = m_1134;
            m_1134 = 0.0f;
            m_113c = std::fabs(m_1138 / 10.0f);
        }
    }
}
void daPlBase_c::finalizeState_Fall() {
    offStatus(STATUS_A0);
    offStatus(STATUS_88);
    offStatus(STATUS_JUMP_DAI_COOLDOWN);
    mJumpDaiFallTimer = 0;
}
void daPlBase_c::executeState_Fall() {
    if (mJumpDaiFallTimer == 0) {
        offStatus(STATUS_JUMP_DAI_COOLDOWN);
    }
}

void daPlBase_c::initializeState_Land() {}
void daPlBase_c::finalizeState_Land() {}
void daPlBase_c::executeState_Land() {}

bool daPlBase_c::checkCrouch() {
    return false;
}

bool daPlBase_c::setCancelCrouch() {
    if (checkStandUpRoofOnLift()) {
        return false;
    }
    if (mpMdlMng->getAnm() != PLAYER_ANIM_STOOP_START) {
        mpMdlMng->setAnm(PLAYER_ANIM_STOOP_START);
        mpMdlMng->mpMdl->setFrame(mpMdlMng->getLastFrame());
    }
    if (!isNowBgCross(BGC_WATER_SHALLOW)) {
        mpMdlMng->mpMdl->setRate(-1.0f);
    } else {
        mpMdlMng->mpMdl->setRate(-dPyMdlBase_c::scWaterCrouchAnmSpeed);
    }
    onStatus(STATUS_52);
    changeState(StateID_Walk, BLEND_DEFAULT);
    return true;
}

bool daPlBase_c::checkStandUpRoofOnLift() {
    if (isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_LIFT)) {
        return false;
    }
    return checkStandUpRoof();
}

bool daPlBase_c::checkStandUpRoof() {
    const sBcPointData *headBgP = getHeadBgPointData();
    if (headBgP == nullptr) {
        return false;
    }
    float standHeadBgY = getStandHeadBgPointY();
    float tmpCalc = mPos.y + standHeadBgY - 1.0f;
    mVec3_c p = mVec3_c(
        mPos.x + headBgP->mInfMargin / 4096.0f,
        mPos.y + 4.0f,
        mPos.z
    );
    float res;
    if (dBc_c::checkTenjou(&p, &res, mLayer, mAmiLayer) && res < tmpCalc) {
        return true;
    }
    p.x = mPos.x + headBgP->mSupMargin / 4096.0f;
    if (dBc_c::checkTenjou(&p, &res, mLayer, mAmiLayer) && res < tmpCalc) {
        return true;
    }
    return false;
}

void daPlBase_c::initializeState_Crouch() {}
void daPlBase_c::finalizeState_Crouch() {}
void daPlBase_c::executeState_Crouch() {}

void daPlBase_c::initializeState_Slip() {
    mSubstate = SLIP_ACTION_NONE;
    mpMdlMng->setAnm(PLAYER_ANIM_SLIP);
    mMaxSpeedF = getSlipMaxSpeedF();
    mSubstateTimer = 8;
    mAutoSlipTimer = 0;
    onStatus(STATUS_30);
    onStatus(STATUS_97);
}
void daPlBase_c::finalizeState_Slip() {
    offStatus(STATUS_30);
    offStatus(STATUS_97);
    mAngle.x = 0;
}
void daPlBase_c::executeState_Slip() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    sLib::calcTimer(&mAutoSlipTimer);
    mAng ang = 0;
    if (isNowBgCross(BGC_FOOT)) {
        ang = mBc.getSakaAngle(mDirection) * 0.3f;
    }
    sLib::addCalcAngle(&mAngle.x.mAngle, ang.mAngle, 4, 0x2000, 0x100);
    int angle = turnAngle();
    if (isNowBgCross(BGC_AUTOSLIP)) {
        mAutoSlipTimer = 9;
    } else if (checkJumpTrigger()) {
        return;
    } else if (angle && mAutoSlipTimer == 0 && checkSlipEndKey()) {
        changeActionSlipEnd(BLEND_NONE);
        return;
    }
    switch ((SlipSubstate_e) mSubstate) {
        case SLIP_ACTION_NONE:
            if (!isNowBgCross(BGC_FOOT) && (mKey.triggerJump() || mKey.triggerCross())) {
                changeState(StateID_Fall, false);
            } else {
                slipActionMove(angle);
            }
            break;
        case SLIP_ACTION_STOOP:
            mMaxSpeedF = 0.0f;
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                changeState(StateID_Crouch, CROUCH_ARG_FROM_OTHER);
            }
            break;
        case SLIP_ACTION_END:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                changeActionSlipEnd(BLEND_DEFAULT);
            }
        default:
            break;
    }
}

void daPlBase_c::setSlipAction_ToStoop() {
    if (checkStandUpRoofOnLift()) {
        return;
    }
    offStatus(STATUS_30);
    if (isStatus(STATUS_PENGUIN_SLIDE)) {
        offStatus(STATUS_PENGUIN_SLIDE);
    }
    mSubstate = SLIP_ACTION_STOOP;
    mpMdlMng->setAnm(PLAYER_ANIM_SLIP_TO_STOOP);
}

void daPlBase_c::setSlipAction_ToEnd() {
    if (checkStandUpRoofOnLift()) {
        return;
    }
    offStatus(STATUS_30);
    if (isStatus(STATUS_PENGUIN_SLIDE)) {
        offStatus(STATUS_PENGUIN_SLIDE);
    }
    mSubstate = SLIP_ACTION_END;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mpMdlMng->setAnm(PLAYER_ANIM_SLIPED);
}

void daPlBase_c::setSlipActionEnd() {
    offStatus(STATUS_30);
    if (!mKey.buttonCrouch()) {
        setSlipAction_ToEnd();
    }
}

void daPlBase_c::setSlipActionViewLimitEnd() {
    offStatus(STATUS_30);
    if (!mKey.buttonCrouch()) {
        setSlipAction_ToEnd();
    } else if (dScStage_c::m_instance->mCurrWorld == WORLD_1 && dScStage_c::m_instance->mCurrCourse == STAGE_6 && dScStage_c::m_instance->mCurrFile == 1) {
        if (isNowBgCross(BGC_LIFT)) {
            setSlipAction_ToEnd();
        }
    }
}

void daPlBase_c::slipActionMove(int param) {
    onStatus(STATUS_30);
    if (mSpeedF) {
        if (mPowerup != POWERUP_PENGUIN_SUIT) {
            setCcAtSlip();
        } else {
            setCcAtPenguinSlip();
        }
    }
    if (isOldBgCross(BGC_FOOT) && mPrevMoveSakaAngle > 0 && mStillSakaAngle * mPrevStillSakaAngle <= 0) {
        offNowBgCross(BGC_FOOT);
        float baseSpeed = mSpeedF;
        float cos = mAng(mPrevMoveSakaAngle).cos();
        mMaxSpeedF = baseSpeed;
        mSpeedF = baseSpeed * cos;
        mSpeed.y = std::fabs(baseSpeed) * mAng(mPrevMoveSakaAngle).sin();
    }
    if (isNowBgCross(BGC_FOOT)) {
        if (mSpeedF <= 0.0f && isNowBgCross(BGC_SIDE_LIMIT_L)) {
            setSlipActionViewLimitEnd();
        } else if (checkSakaReverse()) {
            setSlipActionEnd();
        } else if (isNowBgCross(BGC_SLOPE)) {
            mSlipEndTimer = 3;
            mSubstateTimer = 8;
            mMaxSpeedF = getSlipMaxSpeedF();
            if (!mBc.getSakaUpDown(mDirection)) {
                if (param == 0) {
                    mAccelF = 0.5f * mAccelF;
                }
                if (std::fabs(mSpeedF) < 1.0f &&
                    ((mCc.getXOffset(0) < 0.0f && mSpeedF >= 0.0f) ||
                    (mCc.getXOffset(0) > 0.0f && mSpeedF <= 0.0f)))
                {
                    setSlipActionEnd();
                }
                if (!mSpeedF && checkBGCrossWall(mDirection)) {
                    setSlipActionEnd();
                }
            }
        } else {
            mSubstateTimer = 0;
            mMaxSpeedF = 0.0f;
            if (mSlipEndTimer == 0 && std::fabs(mSpeedF) < 1.1f) {
                if (!mKey.buttonCrouch()) {
                    setSlipAction_ToEnd();
                } else {
                    setSlipAction_ToStoop();
                }
            }
        }
    } else {
        if (mSubstateTimer != 0) {
            mAccelY = 0.0f;
            onStatus(STATUS_98);
        } else {
            offStatus(STATUS_98);
        }
    }
}

void daPlBase_c::changeActionSlipEnd(AnmBlend_e param) {
    if (
        (mKey.buttonLeft() && mDirection == 0 && mSpeedF > 0.0f) ||
        (mKey.buttonRight() &&mDirection == 1 && mSpeedF < 0.0f)
    ) {
        mAngle.y = getMukiAngle(mDirection);
        mDirection ^= 1;
        changeState(StateID_Turn);
        return;
    }
    if (
        (mKey.buttonLeft() && mDirection == 0 && mSpeedF < 0.0f) ||
        (mKey.buttonRight() && mDirection == 1 && mSpeedF > 0.0f)
    ) {
        mAngle.y = getMukiAngle(mDirection);
        mDirection ^= 1;
    }
    changeState(StateID_Walk, param);
}

bool daPlBase_c::checkTurn() {
    if (!isCarry() && !isStatus(STATUS_74) && std::fabs(mSpeedF) >= 2.5f) {
        if (isNowBgCross(BGC_ON_ICE)) {
            if (
                (mSpeedF < 0.0f && mKey.buttonRight() && mDirection == 0) ||
                (mSpeedF > 0.0f && mKey.buttonLeft() && mDirection == 1)
            ) {
                changeState(StateID_Turn);
                return true;
            }
        } else {
            changeState(StateID_Turn);
            return true;
        }
    }
    return false;
}

void daPlBase_c::setTurnEnd() {
    mAngle.y = getMukiAngle(mDirection);
    changeState(StateID_Walk, BLEND_NONE);
}

void daPlBase_c::initializeState_Turn() {}
void daPlBase_c::finalizeState_Turn() {}
void daPlBase_c::executeState_Turn() {}

bool daPlBase_c::setHipAttackOnEnemy(mVec3_c *) {
    return 0;
}

void daPlBase_c::setVsPlHipAttackEffect() {
    dEf::createPlayerEffect(mPlayerNo, "Wm_mr_misshit", 0, &mPos, nullptr, nullptr);
}

void daPlBase_c::setHipAttackEffect() {
    if (mRideActorID) {
        return;
    }
    if (!isOnSinkSand()) {
        setLandSmokeEffect(getTallType(-1));
    }
    if (mGroundType == GROUND_TYPE_WATER) {
        startSound(SE_PLY_HPDP_SPLASH, false);
    }
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        startSound(SE_PLY_HIP_ATTACK_M, false);
        return;
    }

    if (
        !(GROUND_TYPE_DEFAULT <= mGroundType && mGroundType <= GROUND_TYPE_CLOUD) &&
        !(GROUND_TYPE_MANTA <= mGroundType && mGroundType <= GROUND_TYPE_CARPET)
    ) {
        switch (mGroundType) {
            case GROUND_TYPE_FUNSUI:
            case GROUND_TYPE_LEAF:
                startSound(SE_PLY_HIP_ATTACK_SOFT, false);
            default:
                return;
            case GROUND_TYPE_WOOD:
                break;
        }
    }
    startSound(SE_PLY_HIP_ATTACK, false);
}

void daPlBase_c::setHipAttackDropEffect() {
    static const float sc_dropEffectScale[] = { 0.7f, 0.8f, 1.0f };
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    float scale = sc_dropEffectScale[getTallType(-1)];
    mVec3_c scaleVec(scale, scale, scale);
    dEf::createPlayerEffect(mPlayerNo, &mHitAttackDropEffect, "Wm_mr_drop", 0, &pos, nullptr, &scaleVec);
}

void daPlBase_c::setHipBlockBreak() {
    if (mPowerup == POWERUP_MINI_MUSHROOM || mPowerup == POWERUP_NONE) {
        offStatus(STATUS_C4);
    } else {
        onStatus(STATUS_C4);
    }
}

void daPlBase_c::setHipAttack_Ready() {
    mSubstate = HIP_ACTION_READY;
    mpMdlMng->setAnm(PLAYER_ANIM_HIPSR);
    mSpeed.y = 1.0f;
}

void daPlBase_c::setHipAttack_KinopioStart() {
    setHipAttack_AttackStart();
    onStatus(STATUS_7F);
    onStatus(STATUS_7A);
    if (isItemKinopio()) {
        startSound(SE_VOC_ITEM_KO_FOUND, false);
    }
}

void daPlBase_c::setHipAttack_AttackStart() {
    mSubstate = HIP_ACTION_ATTACK_START;
    mpMdlMng->setAnm(PLAYER_ANIM_HIPAT);
    mSubstateTimer = 5;
    mSpeed.y = 0.0f;
}

void daPlBase_c::setHipAttack_AttackFall() {
    mSubstate = HIP_ACTION_ATTACK_FALL;
    mAccelF = 0.1f;
    mMaxSpeedF = 0.0f;
    mAccelY = *getGravityData();
    mMaxFallSpeed = -6.0f;
    if (stateArg<HipAttackArg_e>() == HIP_ATTACK_ARG_ITEM_KINOPIO) {
        mSpeed.y = -2.0f;
    } else {
        mSpeed.y = -6.0f;
    }
    onStatus(STATUS_HIP_ATTACK_FALL);
    setHipBlockBreak();
    mSubstateTimer = 5;
}

void daPlBase_c::setHipAttack_StandNormal() {
    mSubstateTimer = 20;
    mSubstate = HIP_ACTION_STAND_NORMAL;
    mpMdlMng->setAnm(PLAYER_ANIM_HIPED);
    onStatus(STATUS_HIP_ATTACK_STAND_UP);
    offStatus(STATUS_HIP_ATTACK_FALL);
    onStatus(STATUS_9F);
    mAccelY = *getGravityData();
    mMaxFallSpeed = -6.0f;
    mSpeed.y = 0.0f;
}

void daPlBase_c::setHipAttack_StandNormalEnd() {
    mSubstate = HIP_ACTION_STAND_NORMAL_END;
    mpMdlMng->setAnm(PLAYER_ANIM_SLIPED);
    offStatus(STATUS_HIP_ATTACK_STAND_UP);
}

void daPlBase_c::setHipAttack_ToStoop() {
    mSubstate = HIP_ACTION_TO_STOOP;
    mpMdlMng->setAnm(PLAYER_ANIM_HIP_TO_STOOP);
    offStatus(STATUS_HIP_ATTACK_STAND_UP);
}

void daPlBase_c::HipAction_Ready() {
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        setHipAttack_AttackStart();
    }
}

void daPlBase_c::HipAction_AttackStart() {
    if (mSubstateTimer == 0) {
        setHipAttack_AttackFall();
    }
}

void daPlBase_c::HipAction_AttackFall() {
    setHipAttackDropEffect();
    if (mSubstateTimer == 0 || isNowBgCross(BGC_FOOT)) {
        offStatus(STATUS_7F);
        offStatus(STATUS_7A);
    }
    if (!isNowBgCross(BGC_FOOT)) {
        if (isNowBgCross(BGC_WATER_SHALLOW)) {
            mMaxFallSpeed = -3.0f;
        } else {
            mMaxFallSpeed = -6.0f;
        }
        mAccelY = *getGravityData();
        int dir;
        if (mKey.buttonWalk(&dir)) {
            mMaxSpeedF = sc_DirSpeed[dir] * 0.3f;
        }
        if (mKey.buttonDown() && mSpeed.y < 0.0f) {
            setHipBlockBreak();
        }
        if (!isStatus(STATUS_PRESS_ATTACH) && mKey.buttonUp()) {
            changeState(StateID_Fall, false);
        }
    } else {
        if (!mKey.buttonDown()) {
            offStatus(STATUS_C4);
        }
        setHipAttackEffect();
        int quakeType = 0;
        if (mPowerup == POWERUP_MINI_MUSHROOM) {
            quakeType = 2;
        }
        daPyMng_c::setHipAttackQuake(quakeType, mPlayerNo);
        onStatus(STATUS_HIP_ATTACK_LAND);
        offStatus(STATUS_91);
        if (isNowBgCross(BGC_CLIFF)) {
            changeState(StateID_Kani, KANI_ARG_JUMP_HANG);
        } else if (isSlipSaka()) {
            if (!isNowBgCross(BGC_WATER_TOUCH)) {
                if (mBc.getSakaType() >= 2) {
                    mSpeedF = getSlipMaxSpeedF();
                } else {
                    mSpeedF = getSlipMaxSpeedF() * 0.5f;
                }
            }
            setSlipAction();
        } else {
            mMaxSpeedF = 0.0f;
            mSpeedF = 0.0f;
            mSpeed.y = 0.0f;
            mAccelY = 0.0f;
            mSubstate = HIP_ACTION_GROUND;
        }
    }
}

void daPlBase_c::HipAction_Ground() {
    if (!mKey.buttonDown() && !isNowBgCross(BGC_FOOT)) {
        changeState(StateID_Fall, false);
    } else {
        setHipAttack_StandNormal();
    }
}

void daPlBase_c::HipAction_StandNormal() {
    if (mpMdlMng->getAnm() != PLAYER_ANIM_HIPED) {
        mpMdlMng->setAnm(PLAYER_ANIM_HIPED);
    }
    if (isNowBgCross(BGC_FOOT)) {
        if (mpMdlMng->isAnmStop()) {
            if (!mKey.buttonDown()) {
                offStatus(STATUS_HIP_ATTACK);
                setHipAttack_StandNormalEnd();
            } else {
                setHipBlockBreak();
                if (m_344 == 0) {
                    if (isNowBgCross(BGC_54)) {
                        mSubstateTimer = 15;
                    }
                    if (dScStage_c::m_isStaffCredit && isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_LIFT)) {
                        m_344 = 1;
                    }
                }
                if (mSubstateTimer == 0) {
                    offStatus(STATUS_HIP_ATTACK);
                    setHipAttack_ToStoop();
                }
            }
        } else if (isSlipSaka()) {
            mSpeedF = getSlipMaxSpeedF();
            setSlipAction();
        } else {
            turnAngle();
        }
    } else {
        if (mKey.buttonCrouch()) {
            setHipAttack_AttackFall();
        }
    }
}

void daPlBase_c::HipAction_StandNormalEnd() {
    if (!checkCrouch()) {
        if (mpMdlMng->mpMdl->mAnm.isStop()) {
            changeState(StateID_Walk, BLEND_DEFAULT);
        } else {
            turnAngle();
        }
    }
}

void daPlBase_c::HipAction_ToStoop() {
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        changeState(StateID_Crouch, CROUCH_ARG_FROM_OTHER);
    } else {
        turnAngle();
    }
}

void daPlBase_c::initializeState_HipAttack() {
    mAngle.y = getMukiAngle(mDirection);
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    switch (stateArg<HipAttackArg_e>()) {
        case HIP_ATTACK_ARG_PLAYER:
            setHipAttack_Ready();
            break;
        case HIP_ATTACK_ARG_ITEM_KINOPIO:
            setHipAttack_KinopioStart();
            break;
    }
    onStatus(STATUS_A8);
    onStatus(STATUS_AA);
    onStatus(STATUS_HIP_ATTACK);
}

void daPlBase_c::finalizeState_HipAttack() {
    mMaxFallSpeed = -4.0f;
    offStatus(STATUS_HIP_ATTACK_FALL);
    offStatus(STATUS_A8);
    offStatus(STATUS_AA);
    offStatus(STATUS_C4);
    offStatus(STATUS_PRESS_ATTACH);
    offStatus(STATUS_HIP_ATTACK_LAND);
    offStatus(STATUS_HIP_ATTACK_STAND_UP);
    offStatus(STATUS_9F);
    offStatus(STATUS_HIP_ATTACK);
    offStatus(STATUS_7F);
    offStatus(STATUS_7A);
    m_344 = 0;
}

void daPlBase_c::executeState_HipAttack() {
    static ProcFunc l_HipActionProc[] = {
        &daPlBase_c::HipAction_Ready,
        &daPlBase_c::HipAction_AttackStart,
        &daPlBase_c::HipAction_AttackFall,
        &daPlBase_c::HipAction_Ground,
        &daPlBase_c::HipAction_StandNormal,
        &daPlBase_c::HipAction_StandNormalEnd,
        &daPlBase_c::HipAction_ToStoop
    };

    offStatus(STATUS_HIP_ATTACK_LAND);
    if (isStatus(STATUS_HIP_ATTACK_FALL)) {
        setCcAtHipAttack();
    }
    if (isNowBgCross(BGC_WATER_SUBMERGED)) {
        offStatus(STATUS_AA);
    }
    if (mSubstate < HIP_ACTION_STAND_NORMAL || !checkJumpTrigger()) {
        (this->*l_HipActionProc[mSubstate])();
        if (isStatus(STATUS_PRESS_ATTACH)) {
            offStatus(STATUS_PRESS_ATTACH);
            mPos.x = mPressAttachPos.x;
            mPos.y = mPressAttachPos.y;
        }
    }
}

void daPlBase_c::initializeState_Swim() {}
void daPlBase_c::finalizeState_Swim() {}
void daPlBase_c::executeState_Swim() {}

bool daPlBase_c::setJumpDaiRide() {
    if (isStatus(STATUS_45) || isNowBgCross(BGC_HEAD)) {
        return false;
    }
    changeState(StateID_JumpDai);
    return true;
}

void daPlBase_c::initializeState_JumpDai() {
    onStatus(STATUS_SPRING_JUMP);
    onStatus(STATUS_86);
    onStatus(STATUS_BIG_JUMP);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
    mSpeed.y = 0.0f;
    mSpeedF = 0.0f;
    if (mpMdlMng->getAnm() == PLAYER_ANIM_STOOP) {
        mSubstate = JUMP_DAI_HIGH_JUMP;
    } else {
        mpMdlMng->setAnm(PLAYER_ANIM_JUMPED);
    }
}

void daPlBase_c::finalizeState_JumpDai() {
    offStatus(STATUS_BIG_JUMP);
    offStatus(STATUS_SPRING_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
}

void daPlBase_c::executeState_JumpDai() {
    if (!isStatus(STATUS_86)) {
        changeState(StateID_Fall, false);
    } else {
        turnAngle();
        switch ((JumpDaiSubstate_e) mSubstate) {
            case JUMP_DAI_MOVE_DOWN:
                if (mpMdlMng->mpMdl->mAnm.isStop()) {
                    setWaitActionAnm(BLEND_DEFAULT);
                    mSubstate = JUMP_DAI_HIGH_JUMP;
                }
                break;
            case JUMP_DAI_HIGH_JUMP:
                break;
        }
    }
}

bool daPlBase_c::setPlayerJumpDai(daPlBase_c *other) {
    if (mRideActorID == 0) {
        float topPos = other->getHeadTopPosP()->y - 4.0f;
        mVec3_c pos = mVec3_c(
            mPos.x,
            topPos,
            mPos.z
        );
        float f;
        if (mBc.checkRoofPlayer(&pos, &f) && f < topPos + getModelHeight()) {
            return false;
        }
        mRideActorID = other->mUniqueID;
        changeState(StateID_PlayerJumpDai);
        return true;
    }
    return false;
}

void daPlBase_c::setPlayerJumoDaiPos() {
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor == nullptr) {
        return;
    }
    mPos.set(
        mJumpDaiOffset.x + rideActor->mPos.x,
        rideActor->getHeadTopPosP()->y - 4.0f,
        mJumpDaiOffset.z + rideActor->mPos.z
    );
}

void daPlBase_c::initializeState_PlayerJumpDai() {
    onStatus(STATUS_PLAYER_JUMP);
    onStatus(STATUS_BIG_JUMP);
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor != nullptr) {
        rideActor->initStampReduction();
        setNoHitPlayer(rideActor, 5);
        mJumpDaiOffset = mPos - rideActor->mPos;
    }
    mJumpDaiSpeedF = mSpeedF;
    mSpeedF = 0.0f;
    mSpeed.y = 0.0f;
    if (mpMdlMng->getAnm() != PLAYER_ANIM_STOOP) {
        mpMdlMng->setAnm(PLAYER_ANIM_JUMPED);
    }
    if (mKey.triggerJumpBuf(5)) {
        mSubstate = JUMP_DAI_HIGH_JUMP;
    } else {
        mSubstate = JUMP_DAI_MOVE_DOWN;
    }
    mSubstateValue = 0;
}

void daPlBase_c::finalizeState_PlayerJumpDai() {
    offStatus(STATUS_PLAYER_JUMP);
    offStatus(STATUS_BIG_JUMP);
    mRideActorID = (fBaseID_e) 0;
}

void daPlBase_c::executeState_PlayerJumpDai() {
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor == nullptr) {
        changeState(StateID_Fall, false);
    } else if (isNowBgCross(BGC_HEAD)) {
        changeState(StateID_Fall, false);
        mJumpDaiFallTimer = 30;
    } else {
        setNoHitPlayer(rideActor, 5);
        turnAngle();
        if (mpMdlMng->mpMdl->mCurrAnmID == PLAYER_ANIM_JUMPED && mpMdlMng->mpMdl->mAnm.isStop()) {
            setWaitActionAnm(BLEND_DEFAULT);
        }
        switch ((JumpDaiSubstate_e) mSubstate) {
            case JUMP_DAI_MOVE_DOWN:
                if (mSubstateValue < 5) {
                    if (mKey.triggerJump()) {
                        mSubstate = JUMP_DAI_HIGH_JUMP;
                    }
                } else if (isMameAction()) {
                    setJump(daPlBase_c::sc_JumpSpeed - 0.35f, mJumpDaiSpeedF, true, 0, 0);
                    return;
                } else if (mKey.buttonJump()) {
                    setJump(daPlBase_c::sc_JumpSpeed + 0.2f, mJumpDaiSpeedF, true, 0, 2);
                    return;
                } else {
                    setJump(daPlBase_c::sc_JumpSpeed + 1.0f, mJumpDaiSpeedF, true, 2, 0);
                    return;
                }
                break;
            case JUMP_DAI_HIGH_JUMP:
                if (mSubstateValue > 5) {
                    float jumpSpeed = daPlBase_c::sc_JumpSpeed + 0.8f;
                    if (isMameAction()) {
                        jumpSpeed = daPlBase_c::sc_JumpSpeed + 0.2f;
                    }
                    setJump(jumpSpeed, 0.0f, true, 1, 2);
                    return;
                }
                break;
        }
        rideActor->calcJumpDaiReductionScale(mSubstateValue, 5);
        mSubstateValue++;
    }
}

bool daPlBase_c::setFunsui() {
    if (isDemo()) {
        return false;
    }
    if (!isState(StateID_Funsui)) {
        changeState(StateID_Funsui);
    }
    return true;
}

bool daPlBase_c::updateFunsuiPos(float x, float y) {
    if (isState(StateID_Funsui)) {
        mPos.x = x;
        mPos.y = y;
        return true;
    }
    return false;
}

bool daPlBase_c::releaseFunsui(float f) {
    if (isState(StateID_Funsui)) {
        mSpeed.y = f;
        mSubstate = FUNSUI_ACTION_START;
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
        offStatus(STATUS_7F);
    }
    return true;
}

void daPlBase_c::releaseFunsuiAction() {
    changeState(StateID_Fall, false);
}

void daPlBase_c::initializeState_Funsui() {
    onStatus(STATUS_7F);
    mAccelY = 0.0f;
    mSpeedF *= 0.7f;
    mSpeed.y = 0.0f;
    mSubstate = FUNSUI_ACTION_NONE;
    mpMdlMng->setAnm(PLAYER_ANIM_BLOW_UP);
    if (mPlayerNo >= 0) {
        startPlayerVoice(VOICE_INTO_SANDPILLAR, 0);
        dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_7, 0, false);
        mSubstateTimer = 8;
    }
}

void daPlBase_c::finalizeState_Funsui() {
    offStatus(STATUS_7F);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

void daPlBase_c::executeState_Funsui() {
    if (mPlayerNo >= 0) {
        dEf::createPlayerEffect(mPlayerNo, &mSmokeEffect, "Wm_mr_sprisesmoke", 0, &mPos, nullptr, nullptr);
        if (mSubstateTimer == 0) {
            mSubstateTimer = 8;
            dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_8, 0, false);
        }
    }
    turnAngle();
    if (mSubstate == FUNSUI_ACTION_START) {
        gravitySet();
        maxFallSpeedSet();
        if (mSpeed.y <= 0.0f || isNowBgCross(BGC_HEAD)) {
            releaseFunsuiAction();
            return;
        }
    }
    int dir;
    if (mKey.buttonWalk(&dir)) {
        mMaxSpeedF = sc_DirSpeed[dir] * 0.35f;
    } else {
        mMaxSpeedF = 0.0f;
    }
    if (std::fabs(mSpeedF) > 0.35f) {
        mAccelF = 0.1f;
    } else {
        mAccelF = 0.04f;
    }
}

void daPlBase_c::initializeState_Kani() {}
void daPlBase_c::finalizeState_Kani() {}
void daPlBase_c::executeState_Kani() {}

bool daPlBase_c::setCloudOn(dActor_c *cloudActor) {
    if (isNowBgCross(BGC_FOOT) || isStatus(STATUS_7E) || isStatus(STATUS_VINE)) {
        return false;
    }
    if (cloudActor->mPos.y + getCloudOffsetY() >= mPos.y && mSpeed.y < 0.0f && mRideActorID == 0) {
        mRideActorID = cloudActor->mUniqueID;
        changeState(StateID_Cloud);
        return true;
    }
    return false;
}

void daPlBase_c::cancelCloudOn() {
    mRideActorID = (fBaseID_e) 0;
}

float daPlBase_c::getCloudOffsetY() {
    return 0.0f;
}

mVec3_c daPlBase_c::getCloudPos() {
    return mVec3_c(
        mPos.x,
        mPos.y - getCloudOffsetY(),
        mPos.z
    );
}

void daPlBase_c::initializeState_Cloud() {
    startSound(SE_PLY_RIDE_CLOUD, 0);
    onStatus(STATUS_4E);
    mAccelY = 0.0f;
    mSpeed.y = 0.1f;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mpMdlMng->setAnm(PLAYER_ANIM_JUMPED);
    mRc.mFlags |= 4;
}

void daPlBase_c::finalizeState_Cloud() {
    mAngle.x = 0;
    cancelCloudOn();
    offStatus(STATUS_4E);
    offStatus(STATUS_51);
    mRc.mFlags &= ~4;
}

bool daPlBase_c::updateCloudMove() {
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor == nullptr) {
        changeState(StateID_Fall, false);
        return true;
    }
    if (mKey.triggerJump()) {
        changeState(StateID_Jump, nullptr);
        return true;
    }
    onStatus(STATUS_82);
    mPos = rideActor->mPos;
    mPos.y += getCloudOffsetY();
    mSpeed.y = 0.0f;
    if (!isStatus(STATUS_SPIN)) {
        mAngle.set(rideActor->mAngle);
        mDirection = rideActor->mDirection;
    }
    return false;
}

void daPlBase_c::executeState_Cloud() {}

void daPlBase_c::initializeState_AnimePlay() {
    mDemoAnime = stateArg<AnimePlayArg_e>();
    if (mDemoAnime != DEMO_ANIME_NORMAL) {
        onStatus(STATUS_DISABLE_STATE_CHANGE);
    }
}

void daPlBase_c::finalizeState_AnimePlay() {
    offStatus(STATUS_DISABLE_STATE_CHANGE);
    offStatus(STATUS_24);
}

void daPlBase_c::executeState_AnimePlay() {
    static const ProcFunc scDemoAnmFunc[] = {
        &daPlBase_c::DemoAnmNormal,
        &daPlBase_c::DemoAnmBossSetUp,
        &daPlBase_c::DemoAnmBossGlad,
        &daPlBase_c::DemoAnmBossAttention,
        &daPlBase_c::DemoAnmBossKeyGet,
        &daPlBase_c::DemoAnmBossGlad
    };

    mAccelY = *getGravityData();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    if (isStatus(STATUS_DISABLE_STATE_CHANGE) || !checkWalkNextAction()) {
        (this->*scDemoAnmFunc[mDemoAnime])();
    }
}

void daPlBase_c::DemoAnmNormal() {
    if (isStatus(STATUS_24)) {
        addCalcAngleY(0, 10);
    } else {
        turnAngle();
    }
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        setControlDemoWait();
    }
}

void daPlBase_c::DemoAnmBossSetUp() {
    if (mSubstate == ANIME_PLAY_ACTION_0) {
        mpMdlMng->setAnm(PLAYER_ANIM_WAIT_L3);
        mSubstate++;
        offStatus(STATUS_DISABLE_STATE_CHANGE);
    }
}

void daPlBase_c::DemoAnmBossGlad() {
    addCalcAngleY(0, 10);
    switch (mSubstate) {
        case 0:
            mSubstateTimer = 15;
            mSubstate++;
            // fallthrough
        case 1:
            if (mSubstateTimer == 0) {
                initDemoKimePose();
                mSubstate++;
            }
            break;
        case 2: {
            ClearType_e clearType = CLEAR_TYPE_BOSS;
            if (mDemoAnime == DEMO_ANIME_BOSS_GLAD_2) {
                clearType = CLEAR_TYPE_FINAL_BOSS;
            }
            if (!updateDemoKimePose(clearType)) {
                break;
            }
            mpMdlMng->setAnm(PLAYER_ANIM_WAIT, 5.0f, 0.0f);
            mSubstate++;
            offStatus(STATUS_DISABLE_STATE_CHANGE);
            break;
        }
        default:
            break;
    }
}

void daPlBase_c::DemoAnmBossAttention() {
    daPlBase_c *boss = (daPlBase_c *) dAttention_c::mspInstance->search(mpMdlMng->mpMdl->mHatPosMaybe);
    if (boss != nullptr) {
        if (boss->getLookatPos().x > mPos.x) {
            mDirection = DIR_LR_R;
        } else {
            mDirection = DIR_LR_L;
        }
    }
    switch (mSubstate) {
        default:
            break;
        case HIP_ACTION_READY:
            if (mDirection == DIR_LR_R) {
                mpMdlMng->setAnm(PLAYER_ANIM_WAIT_L_DUPLICATE);
            } else {
                mpMdlMng->setAnm(PLAYER_ANIM_WAIT_R_DUPLICATE);
            }
            mSubstate++;
            break;
        case HIP_ACTION_ATTACK_START:
            if (mAngle.y.chase(getMukiAngle(mDirection), 0x800)) {
                mSubstate++;
                offStatus(STATUS_DISABLE_STATE_CHANGE);
            }
            break;
    }
}

void daPlBase_c::DemoAnmBossKeyGet() {
    addCalcAngleY(0, 10);
    switch (mSubstate) {
        case 0:
            mSubstateTimer = 15;
            mSubstate++;
            // [Fallthrough]
        case 1:
            if (mSubstateTimer == 0) {
                mpMdlMng->setAnm(PLAYER_ANIM_BOSS_KEY_GET);
                startKimePoseVoice(CLEAR_TYPE_BOSS);
                mSubstate++;
                offStatus(STATUS_DISABLE_STATE_CHANGE);
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::initializeState_WaitJump() {
    startQuakeShock(dQuake_c::TYPE_5);
    onStatus(STATUS_WAIT_JUMP);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);

    mSpeedF = 0.0f;
    mpMdlMng->setAnm(PLAYER_ANIM_WAIT);
    mAngle.y = getMukiAngle(mDirection);
}

void daPlBase_c::finalizeState_WaitJump() {
    offStatus(STATUS_WAIT_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

void daPlBase_c::executeState_WaitJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    if (mSpeed.y <= 0.0f) {
        changeState(StateID_Fall, false);
    }
}

bool daPlBase_c::checkSakaReverse() {
    if (std::fabs(mSpeedF) < 0.5f && mBc.mPlayerFlags & 4) {
        return true;
    }
    return false;
}

bool daPlBase_c::isSaka() {
    if (isNowBgCross(BGC_SLOPE) ? 1 : 0) {
        return true;
    }
    return false;
}

bool daPlBase_c::isSlipSaka() {
    if (isSaka() && mBc.getFootAttr() != 10) {
        return true;
    }
    return false;
}

void daPlBase_c::setSlipAction() {
    mMoveSakaAngle = mBc.getSakaAngle(mSpeedF);
    changeState(StateID_Slip);
}

bool daPlBase_c::checkSlip() {
    if (
        (mSpeedF > 0.0f || !isNowBgCross(BGC_SIDE_LIMIT_L)) &&
        isNowBgCross(BGC_AUTOSLIP)
    ) {
        setSlipAction();
        return true;
    }
    return false;
}

bool daPlBase_c::checkCrouchSlip() {
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        return false;
    }
    if (
        (mSpeedF > 0.0f || !isNowBgCross(BGC_SIDE_LIMIT_L)) &&
        (isNowBgCross(BGC_AUTOSLIP) || isSlipSaka())
    ) {
        setSlipAction();
        return true;
    }
    return false;
}

bool daPlBase_c::checkSlipEndKey() {
    if (isNowBgCross(BGC_FOOT)) {
        if (!mKey.buttonDown()) {
            if (mBc.getSakaType() == 0) {
                if (mKey.buttonCross()) {
                    return true;
                }
            } else if (mBc.getSakaType() != 4) {
                if (
                    mKey.buttonLeft() && mBc.getSakaDir() == 0 ||
                    mKey.buttonRight() && mBc.getSakaDir() == 1
                ) {
                    return true;
                }
            }
        }
        if (mKey.buttonUp()) {
            return true;
        }
    }
    return false;
}

float daPlBase_c::getSlipMaxSpeedF() {
    if (isSaka()) {
        return sc_DirSpeed[mBc.getSakaDir()] * 4.0f;
    }
    return 0.0f;
}

float daPlBase_c::getSakaMaxSpeedRatio(u8 direction) {
    return l_sakaMaxSpeedRatio[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::getSakaStopAccele(u8 direction) {
    return l_sakaStopAccele[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::getSakaMoveAccele(u8 direction) {
    return l_sakaMoveAccele[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::getIceSakaSlipOffSpeed() {
    return l_sakaSlipOffSpeed[mBc.getSakaType()][mBc.getSakaDir()];
}

bool daPlBase_c::setSandMoveSpeed() {
    if (isOnSinkSand()) {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            if (isStatus(STATUS_SPIN)) {
                mMaxSpeedF = sc_DirSpeed[dir];
            } else {
                mMaxSpeedF = sc_DirSpeed[dir] * 0.5f;
            }
        } else {
            mMaxSpeedF = 0.0f;
        }
        if (mSpeedF * mMaxSpeedF >= 0.0f) {
            if (std::fabs(mSpeedF) > std::fabs(mMaxSpeedF)) {
                mSpeedF = mMaxSpeedF;
            }
        } else {
            mSpeedF = 0.0f;
        }
        return true;
    }
    return false;
}

void daPlBase_c::moveSpeedSet() {
    if (!setSandMoveSpeed() && !isStatus(STATUS_96)) {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            if (!isNowBgCross(BGC_FOOT)) {
                float absSpeed = std::fabs(mSpeedF);
                float speed1 = sc_DirSpeed[dir] * *(getSpeedData() + 0);
                float speed2 = sc_DirSpeed[dir] * *(getSpeedData() + 2);
                float tmp = sc_DirSpeed[dir] * absSpeed;
                if (absSpeed >= std::fabs(speed2) || mKey.buttonDush()) {
                    mMaxSpeedF = speed2;
                } else if (absSpeed > std::fabs(speed1)) {
                    mMaxSpeedF = tmp;
                } else {
                    mMaxSpeedF = speed1;
                }
            } else {
                float speed;
                if (mKey.buttonDush()) {
                    speed = sc_DirSpeed[dir] * *(getSpeedData() + 2);
                } else {
                    speed = sc_DirSpeed[dir] * *(getSpeedData() + 0);
                }
                mMaxSpeedF = speed  * getSakaMaxSpeedRatio(dir);
            }
        } else {
            mMaxSpeedF = 0.0f;
            if (checkSakaReverse()) {
                mSpeedF = 0.0f;
            } else if (getPowerChangeType(false) == POWER_CHANGE_ICE) {
                mMaxSpeedF = getIceSakaSlipOffSpeed();
            }
        }
        if (!isNowBgCross(BGC_FOOT) && !isStatus(STATUS_88) && std::fabs(mSpeedF) > *(getSpeedData() + 2)) {
            if (mSpeedF < 0.0f) {
                mSpeedF = -*(getSpeedData() + 2);
            } else {
                mSpeedF = *(getSpeedData() + 2);
            }
        }
    }
}

void daPlBase_c::simpleMoveSpeedSet() {
    if (!setSandMoveSpeed()) {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            if (mKey.buttonDush()) {
                mMaxSpeedF = sc_DirSpeed[dir] * *(getSpeedData() + 2);
            } else {
                mMaxSpeedF = sc_DirSpeed[dir] * *(getSpeedData() + 0);
            }
        } else {
            mMaxSpeedF = 0.0f;
        }
    }
}

void daPlBase_c::grandPowerSet() {
    if (isOnSinkSand()) {
        mAccelF = *(getSpeedData() + 7);
    } else if (isStatus(STATUS_30)) {
        slipPowerSet(1);
    } else {
        slipPowerSet(0);
    }
}

void daPlBase_c::slipPowerSet(int mode) {
    /// @unofficial
    static const float sc_data[] = { 1.1f, 0.9f, 1.0f };
    if (isSaka()) {
        int dir = 0;
        if (mSpeedF < 0.0f) {
            dir = 1;
        }
        mAccelF = getSakaMoveAccele(dir);
        int dir2;
        if (mKey.buttonWalk(&dir2)) {
            mAccelF *= sc_data[mBc.getSakaUpDown(dir2)];
        }
        if (mode == 0) {
            if (!mKey.buttonWalk(&dir)) {
                mAccelF = getSakaStopAccele(dir);
                if (isStatus(STATUS_89)) {
                    mAccelF = calcSomeAccel(mAccelF);
                }
            } else if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
                mAccelF = *(getSpeedData() + 6);
                if (isStatus(STATUS_89)) {
                    mAccelF = calcSomeAccel(mAccelF);
                }
                return;
            }
        }
        icePowerChange(mode);
    } else {
        if (mode == 1) {
            if (isStatus(STATUS_31)) {
                mAccelF = 0.05f;
            } else {
                mAccelF = 0.09f;
            }
            icePowerChange(1);
        } else {
            normalPowerSet();
        }
    }
}

void daPlBase_c::normalPowerSet() {
    if (std::fabs(mSpeedF) > *(getSpeedData() + 2)) {
        mAccelF = 0.75f;
        return;
    }
    PowerChangeType_e powerChangeType = getPowerChangeType(false);
    SpeedData_t data;
    getPowerChangeSpeedData(&data);
    if (!mKey.buttonWalk(nullptr)) {
        if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
            mAccelF = data.data[2];
        } else if (std::fabs(mSpeedF) < *getSpeedData()) {
            mAccelF = data.data[1];
        } else {
            mAccelF = data.data[0];
        }
        if (powerChangeType == POWER_CHANGE_ICE && std::fabs(mSpeedF) < 0.5f) {
            mAccelF = 0.004f;
        }
        if (isStatus(STATUS_89)) {
            mAccelF = calcSomeAccel(mAccelF);
        }
        return;
    }
    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
        mAccelF = data.data[3];
        if (isStatus(STATUS_89)) {
            mAccelF = calcSomeAccel(mAccelF);
        }
        return;
    }

    float absMaxSpeed = std::fabs(mMaxSpeedF);
    float absSpeed = std::fabs(mSpeedF);

    if (absSpeed < 0.5f) {
        mAccelF = data.data[4];
    } else if (absSpeed < *(getSpeedData() + 0)) {
        if (mKey.buttonDush()) {
            mAccelF = data.data[6];
        } else {
            mAccelF = data.data[5];
        }
    } else if (absSpeed < *(getSpeedData() + 1)) {
        if (absMaxSpeed < *(getSpeedData() + 1)) {
            mAccelF = data.data[0];
        } else {
            mAccelF = data.data[7];
        }
    } else {
        if (absMaxSpeed < *(getSpeedData() + 1)) {
            mAccelF = data.data[0];
        } else {
            mAccelF = data.data[8];
        }
    }
}

daPlBase_c::PowerChangeType_e daPlBase_c::getPowerChangeType(bool affectPenguin) {
    if (!affectPenguin && isStatus(STATUS_IS_PENGUIN)) {
        return POWER_CHANGE_NORMAL;
    }
    if (isNowBgCross(BGC_ON_ICE)) {
        return POWER_CHANGE_ICE;
    }
    if (isNowBgCross(BGC_ON_ICE_LOW_SLIP)) {
        return POWER_CHANGE_ICE_LOW_SLIP;
    }
    if (isNowBgCross(BGC_ON_SNOW) ? 1 : 0) {
        return POWER_CHANGE_ICE_LOW_SLIP;
    }
    return POWER_CHANGE_NORMAL;
}

void daPlBase_c::getPowerChangeSpeedData(SpeedData_t *data) {
    switch (getPowerChangeType(false)) {
        case POWER_CHANGE_NORMAL:
            *data = *(SpeedData_t *) (getSpeedData() + 3);
            break;
        case POWER_CHANGE_ICE:
            *data = *(SpeedData_t *) (getSpeedData() + 12);
            break;
        case POWER_CHANGE_ICE_LOW_SLIP:
            *data = *(SpeedData_t *) (getSpeedData() + 21);
            break;
    }
}

void daPlBase_c::getTurnPower(sTurnPowerData &bb) {
    const sPowerChangeData *data = &daPlayerData_c::smc_POWER_CHANGE_DATA;
    int idx = isStar() ? 1 : 0;
    switch (getPowerChangeType(false)) {
        case POWER_CHANGE_NORMAL:
            bb = data->mRangeType0[idx];
            break;
        case POWER_CHANGE_ICE:
            bb = data->mRangeType1[idx];
            break;
        case POWER_CHANGE_ICE_LOW_SLIP:
            bb = data->mRangeType2[idx];
            break;
    }
}

void daPlBase_c::icePowerChange(int mode) {
    PowerChangeType_e powerChangeType = getPowerChangeType(false);
    if (powerChangeType == POWER_CHANGE_ICE || (powerChangeType == POWER_CHANGE_ICE_LOW_SLIP && mode == 1)) {
        if (mMaxSpeedF) {
            if (mSpeedF * mMaxSpeedF < 0.0f) {
                mAccelF = mAccelF * 0.375f;
            } else if (mode == 0) {
                if (isSaka()) {
                    mAccelF = mAccelF * 0.375f;
                } else if (std::fabs(mSpeedF) < 0.5f) {
                    mAccelF = mAccelF * 0.25f;
                }
            }
        } else if (!isSaka() && std::fabs(mSpeedF) < 0.5f) {
            mAccelF = 0.004;
        } else {
            mAccelF = mAccelF * 0.375f;
        }
    }
}

void daPlBase_c::airPowerSet() {
    const sPowerChangeData *data = &daPlayerData_c::smc_POWER_CHANGE_DATA;
    u8 idx = isStar() ? 1 : 0;
    const sAirTurnPowerData &airPowerData = data->mAirPower[idx];
    int dir;
    if (mKey.buttonWalk(&dir)) {
        if (mSpeedF * sc_DirSpeed[dir] < 0.0f) {
            mAccelF = airPowerData.mTurnAround;
            return;
        } else if (std::fabs(mSpeedF) < 0.5f) {
            mAccelF = airPowerData.mStand;
            return;
        }

        float absSpeed = std::fabs(mSpeedF);

        if (absSpeed < getSpeedData()[0]) {
            if (mKey.buttonDush()) {
                mAccelF = airPowerData.mSlowDash;
            } else {
                mAccelF = airPowerData.mSlowNoDash;
            }
        } else if (absSpeed < getSpeedData()[1]) {
            mAccelF = airPowerData.mMedium;
        } else {
            mAccelF = airPowerData.mFast;
        }
    } else {
        mAccelF = airPowerData.mNoButton;
    }
}

void daPlBase_c::powerSet() {
    if (isNowBgCross(BGC_FOOT)) {
        grandPowerSet();
    } else {
        airPowerSet();
    }
}

void daPlBase_c::maxFallSpeedSet() {
    mMaxFallSpeed = -4.0f;
}

void daPlBase_c::gravitySet() {
    if (isStatus(STATUS_98)) {
        return;
    }
    if (isNowBgCross(BGC_FOOT)) {
        mAccelY = *getGravityData();
    } else {
        setJumpGravity();
    }
}

void daPlBase_c::setJumpGravity() {
    if (mNoGravityTimer != 0) {
        mAccelY = 0.0f;
    } else if (mKey.buttonJump()) {
        setButtonJumpGravity();
    } else {
        setNormalJumpGravity();
    }
}

void daPlBase_c::setButtonJumpGravity() {
    mAccelY = *(getGravityData() + 17);
    for (int i = 0; i < 5; i++) {
        if (mSpeed.y > *(getGravityData() + i + 1)) {
            mAccelY = *(getGravityData() + i + 12);
            break;
        }
    }
}

void daPlBase_c::setNormalJumpGravity() {
    if (mSpeed.y <= 1.5f) {
        mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    }
    mAccelY = *(getGravityData() + 11);
    for (int i = 0; i < 5; i++) {
        if (mSpeed.y > *(getGravityData() + i + 1)) {
            mAccelY = *(getGravityData() + i + 6);
            break;
        }
    }
}

float fn_8004c700(float v) {
    bool isNeg = false;
    if (v < 0.0f) {
        v = -v;
        isNeg = true;
    }
    if (v < 0.1f) {
        v = 0.0f;
    } else {
        v = (v - 0.1f) / 0.9f;
    }
    float tmp;
    float lim2 = 0.1f;
    if (v < lim2) {
        v = 0.0f;
    } else {
        tmp = 1.0f;
        v = (v - 0.1f) / (tmp - 0.1f);
    }
    if (isNeg == true) {
        v = -v;
    }
    v *= 1.11111f;
    return v;
}

class daTagWind_c : public dActor_c {
public:
    float m_00;
};

void daPlBase_c::calcWindSpeed() {
    daTagWind_c *windActor = (daTagWind_c *) fManager_c::searchBaseByProfName(fProfile::TAG_WIND, nullptr);
    if (windActor == nullptr) {
        mWindSpeed = 0.0f;
        return;
    }
    if (isStatus(STATUS_A0) && !isOnSinkSand()) {
        float windSpeed = fn_8004c700(windActor->m_00);
        float halfWindSpeed = windSpeed * 0.5f;
        windSpeed = std::fabs(windSpeed) * 3.0f;
        if (
            mWindSpeed > 0.0f && isNowBgCross(BGC_WALL_TOUCH_R_2) ||
            mWindSpeed < 0.0f && isNowBgCross(BGC_WALL_TOUCH_L_2)
        ) {
            mWindSpeed = 0.0f;
        }
        if (isNowBgCross(BGC_FOOT)) {
            if (isStatus(STATUS_43)) {
                offStatus(STATUS_43);
                mSpeedF += mWindSpeed;
                mWindSpeed = 0.0f;
            }
            float scale;
            if (isStatus(STATUS_5B)) {
                windSpeed = 0.45f * windSpeed;
            } else {
                windSpeed = 0.55f * windSpeed;
            }
            mWindGroundTimer = 3;
            mWindSpeed += halfWindSpeed;
        } else {
            if (!isStatus(STATUS_43)) {
                onStatus(STATUS_43);
            }
            if (mWindGroundTimer != 0) {
                mWindGroundTimer--;
            } else if (mSpeedF * mWindSpeed < 0.0f) {
                mSpeedF += halfWindSpeed;
            } else {
                mWindSpeed += halfWindSpeed;
            }
        }
        if (mWindSpeed > windSpeed) {
            mWindSpeed = windSpeed;
        } else if (mWindSpeed < -windSpeed) {
            mWindSpeed = -windSpeed;
        }
    } else {
        mWindSpeed = 0.0f;
        return;
    }
}

void daPlBase_c::setLandSE() {
    if (mPowerup == POWERUP_PENGUIN_SUIT) {
        startFootSoundPlayer(SE_PLY_LAND_PNGN);
        return;
    }
    static const dAudio::SoundEffectID_t scLandSeID[] = {
        SE_PLY_LAND_ROCK,
        SE_PLY_LAND_SNOW,
        SE_PLY_LAND_SAND,
        SE_PLY_LAND_ROCK,
        SE_PLY_LAND_DIRT,
        SE_PLY_LAND_WATER,
        SE_PLY_LAND_CLOUD,
        SE_PLY_LAND_BLOWSAND,
        SE_PLY_LAND_MANTA,
        SE_PLY_LAND_SAND,
        SE_PLY_LAND_ROCK,
        SE_PLY_LAND_LEAF,
        SE_PLY_LAND_ROCK
    };
    startFootSoundPlayer(scLandSeID[mGroundType]);
}

void daPlBase_c::setSlipSE() {
    if (mGroundType == GROUND_TYPE_WATER) {
        holdSound(SE_PLY_PNGN_SLIP_SEA, std::fabs(mSpeedF), false);
        return;
    }
    static const dAudio::SoundEffectID_t scSlipSeID[] = {
        SE_PLY_SLIP,
        SE_PLY_SLIP_SNOW,
        SE_PLY_SLIP_SAND,
        SE_PLY_SLIP_ICE,
        SE_PLY_SLIP,
        SE_PLY_SLIP,
        SE_PLY_SLIP,
        SE_PLY_SLIP_SAND,
        SE_PLY_SLIP,
        SE_PLY_SLIP_SAND,
        SE_PLY_SLIP,
        SE_PLY_SLIP,
        SE_PLY_SLIP
    };
    holdSound(scSlipSeID[mGroundType], false);
}

void daPlBase_c::setLandSmokeEffect(int param1) {
    static const char *sc_landSmokeEffectID[][3] = {
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_sndlandsmk_ss", "Wm_mr_sndlandsmk_s", "Wm_mr_sndlandsmk" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_watersplash", "Wm_mr_watersplash", "Wm_mr_watersplash" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_sndlandsmk_ss", "Wm_mr_sndlandsmk_s", "Wm_mr_sndlandsmk" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_beachlandsmk_ss", "Wm_mr_beachlandsmk_s", "Wm_mr_beachlandsmk" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" },
        {"Wm_mr_landsmoke_ss", "Wm_mr_landsmoke_s", "Wm_mr_landsmoke" }
    };
    if (mGroundType == GROUND_TYPE_FUNSUI) {
        setSandFunsuiLandEffect();
    } else if (mGroundType == GROUND_TYPE_WATER) {
        PLAYER_POWERUP_e powerup = mPowerup;
        float sz = 1.0f;
        if (powerup == POWERUP_MINI_MUSHROOM) {
            sz = 0.6f;
        } else if (powerup == POWERUP_NONE) {
            sz = 0.8f;
        }
        mVec3_c size(sz, sz, sz);
        dEf::createPlayerEffect(mPlayerNo, sc_landSmokeEffectID[mGroundType][param1], 0, &mPos, nullptr, &size);
    } else {
        dEf::createPlayerEffect_change(mPlayerNo, sc_landSmokeEffectID[mGroundType][param1], 0, &mPos, nullptr, nullptr);
    }
}

void daPlBase_c::setLandSmokeEffectLight() {
    mVec3_c pos = mPos;
    if (dMaskMng::isCaveMask() && mLayer == 0) {
        if (mAmiLayer == 1) {
            pos.z = 3700.0f;
        }
    }
    float sz = dPyMdlMng_c::m_hio.m_08[mpMdlMng->mpMdl->m_152];
    mVec3_c size(sz, sz, sz);
    if (isNowBgCross(BGC_ON_SAND)) {
        if (mGroundType == GROUND_TYPE_FUNSUI) {
            setSandFunsuiLandEffect();
        } else if (!isOnSinkSand()) {
            dEf::createPlayerEffect(mPlayerNo, "Wm_mr_cmnsndlandsmk", 0, &pos, nullptr, &size);
        }
    } else {
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_cmnlandsmoke", 0, &pos, nullptr, &size);
    }
}

bool daPlBase_c::setSandFunsuiLandEffect() {
    if (mGroundType == GROUND_TYPE_FUNSUI) {
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_spsmoke", 0, &mPos, nullptr, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::setStartJumpEffect(int param1) {
    if (isNowBgCross(BGC_FOOT)) {
        if (isOnSinkSand()) {
            setSandJumpEffect();
            onStatus(STATUS_SINK_SAND_JUMP);
        } else if (!setSandFunsuiLandEffect() && param1 == 1) {
            setLandSmokeEffectLight();
        }
    }
}

void daPlBase_c::setLandJumpEffect(int param1) {
    setLandSE();
    if (!setSandFunsuiLandEffect() && param1 == 1) {
        setLandSmokeEffectLight();
    }
}

void daPlBase_c::setSlipOnWaterEffect(mEf::levelEffect_c *effect) {
    static const float sc_runFootScale[] = { 0.5f, 0.8f, 1.0f };
    float sz = sc_runFootScale[getTallType(-1)];
    mVec3_c size(sz, sz, sz);
    mVec3_c pos(
        mPos.x,
        mPos.y + sz * 10.0f,
        mPos.z
    );
    dEf::createPlayerEffect(mPlayerNo, effect, "Wm_mr_foot_water", 0, &pos, nullptr, &size);
}

void daPlBase_c::setSlipSmokeEffect() {
    static const char *sc_slipSmokeEffectID[][2] = {
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_sndslipsmk_ss", "Wm_mr_sndslipsmk" },
        { "Wm_mr_iceslipsmk_ss", "Wm_mr_iceslipsmk" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_beachslipsmk_ss", "Wm_mr_beachslipsmk" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" },
        { "Wm_mr_slipsmoke_ss", "Wm_mr_slipsmoke" }
    };

    if (mGroundType == GROUND_TYPE_WATER) {
        setSlipOnWaterEffect(&mSlipSmokeEffect);
        return;
    }
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    int idx = 0;
    if (mPowerup != POWERUP_MINI_MUSHROOM) {
        idx = 1;
    }
    dEf::createPlayerEffect_change(mPlayerNo, &mSlipSmokeEffect, sc_slipSmokeEffectID[mGroundType][idx], 0, &pos, nullptr, nullptr);
}

void daPlBase_c::setBrakeSmokeEffect(mVec3_c &offset) {
    static const char *sc_brakeSmokeEffectID[][2] = {
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_sndbrakesmk_ss", "Wm_mr_sndbrakesmk" },
        { "Wm_mr_icebrakesmk_ss", "Wm_mr_icebrakesmk" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_beachbrakesmk_ss", "Wm_mr_beachbrakesmk" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" },
        { "Wm_mr_brakesmoke_ss", "Wm_mr_brakesmoke" }
    };

    if (mGroundType == GROUND_TYPE_WATER) {
        setSlipOnWaterEffect(&mBrakeSmokeEffect);
        return;
    }
    int idx = 0;
    if (mPowerup != POWERUP_MINI_MUSHROOM) {
        idx = 1;
    }
    dEf::createPlayerEffect_change(mPlayerNo, &mBrakeSmokeEffect, sc_brakeSmokeEffectID[mGroundType][idx], 0, &offset, nullptr, nullptr);
}

void daPlBase_c::setTurnSmokeEffect() {
    if (mSpeedF) {
        static const dAudio::SoundEffectID_t scTurnSeID[] = {
            SE_PLY_BRAKE,
            SE_PLY_BRAKE_SNOW,
            SE_PLY_BRAKE_SAND,
            SE_PLY_BRAKE_ICE,
            SE_PLY_BRAKE,
            SE_PLY_BRAKE_WATER,
            SE_PLY_BRAKE,
            SE_PLY_BRAKE_SAND,
            SE_PLY_BRAKE,
            SE_PLY_BRAKE_SAND,
            SE_PLY_BRAKE,
            SE_PLY_BRAKE,
            SE_PLY_BRAKE
        };
        holdSound(scTurnSeID[mGroundType], 0);
    }
    static const char *sc_turnSmokeEffectID[][2] = {
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_snow_r", "Wm_mr_turn_snow_l" },
        { "Wm_mr_turn_sand_r", "Wm_mr_turn_sand_l" },
        { "Wm_mr_turn_ice_r", "Wm_mr_turn_ice_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_water_r", "Wm_mr_turn_water_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_sand_r", "Wm_mr_turn_sand_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_beach_r", "Wm_mr_turn_beach_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" },
        { "Wm_mr_turn_usual_r", "Wm_mr_turn_usual_l" }
    };
    static const float sc_turnSmokeScale[] = { 0.5f, 0.8f, 1.0f };
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    if (mGroundType == GROUND_TYPE_WATER) {
        if (mPos.y < mWaterHeight - 4.0f) {
            fadeOutTurnEffect();
            return;
        }
        pos.y = mWaterHeight;
    }
    float sz = sc_turnSmokeScale[getTallType(-1)];
    mVec3_c size(sz, sz, sz);
    if (mTurnEffectFade == 1 && mTurnGroundType == mGroundType) {
        mTurnSmokeEffect.follow(&pos, 0, 0);
    } else {
        dEf::createPlayerEffect(mPlayerNo, &mTurnSmokeEffect, sc_turnSmokeEffectID[mGroundType][mDirection], 0, &pos, nullptr, &size);
        mTurnGroundType = mGroundType;
        mTurnEffectFade = 1;
    }
}

void daPlBase_c::fadeOutTurnEffect() {
    if (mTurnEffectFade != 1) {
        return;
    }
    mTurnSmokeEffect.followFade();
    mTurnEffectFade = 0;
}

void daPlBase_c::setRunFootEffect() {
    static const char *sc_runFootEffectID[] = {
        nullptr,
        "Wm_mr_foot_snow",
        "Wm_mr_foot_sand",
        "Wm_mr_foot_ice",
        nullptr,
        "Wm_mr_foot_water",
        nullptr,
        "Wm_mr_foot_sand",
        nullptr,
        "Wm_mr_foot_beach",
        nullptr,
        nullptr,
        nullptr
    };
    if (!isOnSinkSand() && isStatus(STATUS_62)) {
        if (
            GROUND_TYPE_SNOW <= mGroundType && mGroundType <= GROUND_TYPE_ICE ||
            mGroundType == GROUND_TYPE_WATER ||
            mGroundType == GROUND_TYPE_FUNSUI ||
            mGroundType == GROUND_TYPE_BEACH
        ) {
            mVec3_c pos;
            mpMdlMng->mpMdl->getJointPos(&pos, 1);
            static const float sc_runFootScale[] = { 0.5f, 0.8f, 1.0f };
            float sz = sc_runFootScale[getTallType(-1)];
            mVec3_c size(sz, sz, sz);
            dEf::createPlayerEffect(mPlayerNo, &mRunEffect, sc_runFootEffectID[mGroundType], 0, &pos, nullptr, &size);
        }
    }
}

void daPlBase_c::setSandEffect() {
    if (isStatus(STATUS_4E) || isStatus(STATUS_RIDE_YOSHI)) {
        return;
    }
    if (isOnSinkSand() && !isNowBgCross(BGC_INSIDE_SINK_SAND) || isStatus(STATUS_SINK_SAND_JUMP)) {
        mVec3_c pos;
        mpMdlMng->mpMdl->getJointPos(&pos, 8);
        dEf::createPlayerEffect(mPlayerNo, &mQuicksandSplashEffect, "Wm_mr_sandsplash", 0, &pos, nullptr, nullptr);
    }
    if (isNowBgCross(BGC_IN_SINK_SAND)) {
        mVec3_c pos = getCenterPos();
        dEf::createPlayerEffect(mPlayerNo, &mQuicksandSinkEffect, "Wm_mr_quicksand", 0, &pos, nullptr, nullptr);
    }
    if (isOnSinkSand() && !wasOnSinkSand()) {
        if (mPos.y > mSinkSandHeight - 8.0f && mPrevSpeedY < 0.0f) {
            int idx = 2;
            if ((mPrevSpeedY < -4.0f || mAirTopHeight > mSinkSandHeight + 58.0f) && mPowerup != POWERUP_MINI_MUSHROOM) {
                mSpeedF = 0.0f;
                idx = (mPowerup == POWERUP_NONE) ? 1 : 0;
            }
            mVec3_c pos(
                mPos.x,
                mSinkSandHeight,
                mPos.z
            );
            static const char *scSandDiveEffectID[] = {
                "Wm_mr_sanddive",
                "Wm_mr_sanddive_m",
                "Wm_mr_sanddive_s"
            };
            dEf::createPlayerEffect(mPlayerNo, scSandDiveEffectID[idx], 0, &pos, nullptr, nullptr);
        }
    }
}

bool daPlBase_c::setSandJumpEffect() {
    if (isOnSinkSand() && mPos.y + getModelHeight() + 16.0f > mSinkSandHeight) {
        mVec3_c pos = mPos;
        pos.y = mSinkSandHeight;
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_sanddive_s", 0, &pos, nullptr, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::setSoundPlyMode() {
    static const int scPlayerSound[] = {
        0, 1, 2, 3, 4, 5, 1
    };
    mSndObj.m_b0 = scPlayerSound[mPowerup];
}

void daPlBase_c::setFootSound() {
    if (
        (isDemo() || isNowBgCross(BGC_FOOT)) &&
        dScStage_c::m_gameMode != dScStage_c::GAME_MODE_TITLE_SCREEN &&
        mpMdlMng->mpMdl->isFootStepTiming()
    ) {
        if (mPowerup == POWERUP_PENGUIN_SUIT) {
            startFootSoundPlayer(SE_PLY_FOOTNOTE_PNGN);
            return;
        }
        static const dAudio::SoundEffectID_t scFootSeID[] = {
            SE_PLY_FOOTNOTE_ROCK,
            SE_PLY_FOOTNOTE_SNOW,
            SE_PLY_FOOTNOTE_SAND,
            SE_PLY_FOOTNOTE_ROCK,
            SE_PLY_FOOTNOTE_DIRT,
            SE_PLY_FOOTNOTE_WATER,
            SE_PLY_FOOTNOTE_CLOUD,
            SE_PLY_FOOTNOTE_BLOWSAND,
            SE_PLY_FOOTNOTE_MANTA,
            SE_PLY_FOOTNOTE_SAND,
            SE_PLY_FOOTNOTE_CARPET,
            SE_PLY_FOOTNOTE_LEAF,
            SE_PLY_FOOTNOTE_WOOD
        };
        startFootSoundPlayer(scFootSeID[mGroundType]);
    }
}

STATE_VIRTUAL_DEFINE(daPlBase_c, DemoNone);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoStartWait);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoWait);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanU);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanD);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanR);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanL);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanU);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanD);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanR);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanL);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanRoll);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoInWaterTank);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoOutWaterTank);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoRailDokan);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoDown);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoNextGotoBlock);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoGoal);
STATE_VIRTUAL_DEFINE(daPlBase_c, DemoControl);

void daPlBase_c::initialDokanUnder() {
    changeDemoState(StateID_DemoInDokanD, DOKAN_TYPE_NORMAL);
}
void daPlBase_c::initialDokanUper() {
    changeDemoState(StateID_DemoInDokanU, DOKAN_TYPE_NORMAL);
}
void daPlBase_c::initialDokanRight() {
    changeDemoState(StateID_DemoInDokanR, DOKAN_TYPE_NORMAL);
}
void daPlBase_c::initialDokanLeft() {
    changeDemoState(StateID_DemoInDokanL, DOKAN_TYPE_NORMAL);
}

void daPlBase_c::initialDokanUnderM() {
    changeDemoState(StateID_DemoInDokanD, DOKAN_TYPE_MINI);
}
void daPlBase_c::initialDokanUperM() {
    changeDemoState(StateID_DemoInDokanU, DOKAN_TYPE_MINI);
}
void daPlBase_c::initialDokanRightM() {
    changeDemoState(StateID_DemoInDokanR, DOKAN_TYPE_MINI);
}
void daPlBase_c::initialDokanLeftM() {
    changeDemoState(StateID_DemoInDokanL, DOKAN_TYPE_MINI);
}

void daPlBase_c::initialDokanDepth() {
    changeDemoState(StateID_DemoInWaterTank, DOKAN_TYPE_NORMAL);
}

void daPlBase_c::initialDoor() {}
void daPlBase_c::initialJumpRight() {}
void daPlBase_c::initialJumpLeft() {}
void daPlBase_c::initialVine() {}

void daPlBase_c::initialFall() {
    changeState(StateID_Fall, false);
    changeDemoState(StateID_DemoNone, false);
    mSpeed.y = -1.0f;
    bgCheck(0);
}

void daPlBase_c::initialHipAttack() {
    changeState(StateID_HipAttack, HIP_ATTACK_ARG_PLAYER);
    changeDemoState(StateID_DemoWait, DEMO_WAIT_ARG_TO_NONE);
}

void daPlBase_c::initialSlip() {
    onStatus(STATUS_INITIAL_SLIDE);
    setSlipAction();
    changeDemoState(StateID_DemoWait, DEMO_WAIT_ARG_TO_CONTROL);
}

void daPlBase_c::initialSwim() {}

void daPlBase_c::initialTorideBoss() {
    initialNormal();
    startControlDemo();
}

void daPlBase_c::initialBlockJump() {}
void daPlBase_c::initialBlockJumpBelow() {}

void daPlBase_c::initialNormal() {
    changeState(StateID_Walk, 0);
    changeDemoState(StateID_DemoStartWait);
    mSpeed.y = -1.0f;
    bgCheck(0);
}

void daPlBase_c::setCreateAction(int action) {
    static ProcFunc l_createActionProc[] = {
        &daPlBase_c::initialNormal,
        &daPlBase_c::initialNormal,
        &daPlBase_c::initialDoor,
        &daPlBase_c::initialDokanUnder,
        &daPlBase_c::initialDokanUper,
        &daPlBase_c::initialDokanRight,
        &daPlBase_c::initialDokanLeft,
        &daPlBase_c::initialFall,
        &daPlBase_c::initialHipAttack,
        &daPlBase_c::initialSlip,
        &daPlBase_c::initialSwim,
        &daPlBase_c::initialNormal,
        &daPlBase_c::initialNormal,
        &daPlBase_c::initialNormal,
        &daPlBase_c::initialDoor,
        &daPlBase_c::initialDoor,
        &daPlBase_c::initialDokanUnderM,
        &daPlBase_c::initialDokanUperM,
        &daPlBase_c::initialDokanRightM,
        &daPlBase_c::initialDokanLeftM,
        &daPlBase_c::initialJumpRight,
        &daPlBase_c::initialVine,
        &daPlBase_c::initialDokanDepth,
        &daPlBase_c::initialTorideBoss,
        &daPlBase_c::initialJumpLeft,
        &daPlBase_c::initialBlockJump,
        &daPlBase_c::initialBlockJumpBelow,
        &daPlBase_c::initialNormal
    };
    (this->*l_createActionProc[action])();
}

bool daPlBase_c::isDemoType(DemoType_e type) {
    switch (type) {
        case DEMO_1:
            if (isDemoMode() && isStatus(STATUS_OUT_OF_PLAY)) {
                return true;
            }
            break;
        case DEMO_2:
            if (isChange()) {
                return true;
            }
            break;
        case DEMO_3:
            if (isStatus(STATUS_DEMO_NEXT_GOTO_BLOCK) || isDemoMode()) {
                return true;
            }
            break;
        case DEMO_PLAYER:
            if (!isItemKinopio() && (isStatus(STATUS_72) || isStatus(STATUS_71)) && mDemoState >= CONTROL_DEMO_WAIT && mDemoState <= CONTROL_DEMO_4) {
                return true;
            }
            break;
        case DEMO_KINOPIO:
            if (isItemKinopio() && (isStatus(STATUS_72) || isStatus(STATUS_71)) && mDemoState >= CONTROL_DEMO_WAIT && mDemoState <= CONTROL_DEMO_KINOPIO_SINK_SAND) {
                return true;
            }
            break;
        case DEMO_ENDING_DANCE:
            if ((isStatus(STATUS_72) || isStatus(STATUS_71)) && mDemoState == CONTROL_DEMO_ENDING_DANCE) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool daPlBase_c::isDemo() {
    if (isDemoType(DEMO_1) || isDemoType(DEMO_2) || isDemoType(DEMO_3)) {
        return true;
    }
    return false;
}

bool daPlBase_c::isControlDemoAll() {
    if (isDemoType(DEMO_PLAYER) || isDemoType(DEMO_KINOPIO) || isDemoType(DEMO_ENDING_DANCE)) {
        return true;
    }
    return false;
}

bool daPlBase_c::isDemoAll() {
    if (isDemo() || isControlDemoAll()) {
        return true;
    }
    return false;
}

bool daPlBase_c::isDemoMode() const {
    return mIsDemoMode != false;
}

void daPlBase_c::onDemo() {
    if (mPlayerNo == -1) {
        return;
    }
    daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
    if (pl != nullptr && !pl->isItemKinopio()) {
        daPyMng_c::mPauseEnableInfo &= ~(1 << mPlayerNo);
        daPyMng_c::mStopTimerInfo |= (1 << mPlayerNo);
    }
    mIsDemoMode = true;
}

void daPlBase_c::offDemo() {
    if (mPlayerNo == -1) {
        return;
    }
    daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
    if (pl != nullptr && !pl->isItemKinopio()) {
        daPyMng_c::mPauseEnableInfo |= (1 << mPlayerNo);
        daPyMng_c::mStopTimerInfo &= ~(1 << mPlayerNo);
    }
    mIsDemoMode = false;
}

void daPlBase_c::changeNextScene(int param1) {
    onStatus(STATUS_64);
    daPyMng_c::mPauseDisable = true;
    daPyDemoMng_c::mspInstance->setCourseOutList(mPlayerNo);
    if (mPlayerNo == daPyDemoMng_c::mspInstance->m_70) {
        dNext_c::m_instance->m_19 = true;
        daPyDemoMng_c::mspInstance->mPlayerNo = mPlayerNo;
    }
    if (param1 == 1) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            pl->onStatus(STATUS_STOP_EXECUTE);
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->onStatus(STATUS_STOP_EXECUTE);
        }
    }
}

bool daPlBase_c::isPlayerGameStop() {
    return (dInfo_c::mGameFlag >> 1) & 1;
}

void daPlBase_c::stopOther() {
    if (!isPlayerGameStop()) {
        return;
    }
    dActor_c::mExecStopReq |= 0xf;
    if (mPlayerNo != -1) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            pl->mExecStopMask &= ~2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->mExecStopMask &= ~2;
        }
    } else if (mKind == STAGE_ACTOR_YOSHI) {
        mExecStopMask &= ~4;
    }
}

void daPlBase_c::playOther() {
    dActor_c::mExecStopReq &= ~0xf;
    if (mPlayerNo != -1) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            pl->mExecStopMask |= 2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->mExecStopMask |= 2;
        }
    } else if (mKind == STAGE_ACTOR_YOSHI) {
        mExecStopMask |= 4;
    }
}

void daPlBase_c::changeNormalAction() {
    bgCheck(0);
    offZPosSetNone();
    changeState(StateID_Walk, BLEND_DEFAULT);
    changeDemoState(StateID_DemoNone, false);
}

bool daPlBase_c::checkTimeOut() {
    if (dStageTimer_c::m_instance->convertToIGT() == 0) {
        if (setTimeOverDemo()) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::changeDemoState(const sStateIDIf_c &stateID, int arg) {
    if (stateID == StateID_DemoNone && isItemKinopio()) {
        changeDemoState(StateID_DemoControl, CONTROL_DEMO_KINOPIO_WALK);
    } else {
        onDemo();
        mDemoSubstate = 0;
        mDemoStateArg = arg;
        mDemoStateMgr.changeState(stateID);
    }
}

bool daPlBase_c::executeDemoState() {
    offStatus(STATUS_79);
    if (mDemoSubstateTimer != 0) {
        mDemoSubstateTimer--;
    }
    mDemoStateMgr.executeState();
    if (isStatus(STATUS_71)) {
        if (isDemoState(StateID_DemoNone)) {
            changeDemoState(StateID_DemoControl, CONTROL_DEMO_WAIT);
        }
    }
    if (!isDemoMode()) {
        return false;
    }
    if (isStatus(STATUS_79)) {
        onStatus(STATUS_77);
    }
    return true;
}

void daPlBase_c::initializeState_DemoNone() {
    mDemoSubstateTimer = 15;
    daPyDemoMng_c::mspInstance->clearDemoNo(mPlayerNo);
    offDemo();
    if (mDemoStateArg != 1) {
        playOther();
    }
    offStatus(STATUS_INVISIBLE);
}

void daPlBase_c::finalizeState_DemoNone() {}

void daPlBase_c::executeState_DemoNone() {
    if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
        if (isDemoMode()) {
            offDemo();
        }
        if (mPlayerNo == daPyDemoMng_c::mspInstance->getPlrNo()) {
            daPyDemoMng_c::mspInstance->setPlrNo(-1);
        }
    }
    if (checkTimeOut()) {
        return;
    }
    if (isNowBgCross(BGC_FOOT) || isStatus(STATUS_SWIM)) {
        if (mKey.buttonRight()) {
            mDokanCounterR++;
            if (mDokanCounterR > sc_DokanEnterThreshold) {
                mDokanCounterR = sc_DokanEnterThreshold;
            }
        } else {
            mDokanCounterR = 0;
        }
        if (mKey.buttonLeft()) {
            mDokanCounterL++;
            if (mDokanCounterL > sc_DokanEnterThreshold) {
                mDokanCounterL = sc_DokanEnterThreshold;
            }
        } else {
            mDokanCounterL = 0;
        }
    } else {
        mDokanCounterR = 0;
        mDokanCounterL = 0;
    }
    if (mDemoSubstateTimer == 0 && isEnableDokanInStatus()) {
        if (setDokanIn(DOKAN_D)) {
            return;
        }
        if (setDokanIn(DOKAN_U)) {
            return;
        }
        if (mDokanCounterR >= sc_DokanEnterThreshold && mDirection == DIR_LR_R) {
            if (setDokanIn(DOKAN_R)) {
                return;
            }
        }
        if (mDokanCounterL >= sc_DokanEnterThreshold && mDirection == DIR_LR_L) {
            if (setDokanIn(DOKAN_L)) {
                return;
            }
        }
        if (mKey.buttonDown()) {
            onStatus(STATUS_A4);
        }
    }
}

void daPlBase_c::initializeState_DemoStartWait() {}

void daPlBase_c::executeState_DemoStartWait() {
    if (dScStage_c::m_gameMode == dScStage_c::GAME_MODE_TITLE_SCREEN) {
        changeDemoState(StateID_DemoNone, false);
    } else {
        if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
            bgCheck(0);
            changeDemoState(StateID_DemoNone, false);
        }
    }
}

void daPlBase_c::finalizeState_DemoStartWait() {}

void daPlBase_c::initializeState_DemoWait() {
    if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
        mDemoSubstate = DEMO_WAIT_TRANSITION;
        mDemoSubstateTimer = 0;
    } else {
        mDemoSubstate = DEMO_WAIT_DELAY;
    }
}

void daPlBase_c::finalizeState_DemoWait() {}

void daPlBase_c::executeState_DemoWait() {
    if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
        switch ((DemoWaitSubstate_e) mDemoSubstate) {
            case DEMO_WAIT_DELAY:
                if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                    mDemoSubstate = DEMO_WAIT_TRANSITION;
                    mDemoSubstateTimer = sc_DemoWaitDuration;
                }
                break;
            case DEMO_WAIT_TRANSITION:
                if (mDemoSubstateTimer == 0) {
                    daPyDemoMng_c::mspInstance->turnNextDemoNo();
                    switch ((DemoWaitArg_e) mDemoStateArg) {
                        case DEMO_WAIT_ARG_TO_NONE:
                            changeDemoState(StateID_DemoNone, false);
                            onDemo();
                            bgCheck(0);
                            break;
                        case DEMO_WAIT_ARG_TO_CONTROL:
                            changeDemoState(StateID_DemoControl, CONTROL_DEMO_4);
                            break;
                    }
                }
                break;
        }
    }
}

const float daPlBase_c::scDokanInSpeedX = 1.0f;
const float daPlBase_c::scDokanInWidthX = 0.0f;
const float daPlBase_c::scDokanInMoveSpeed = 0.75f;
const float daPlBase_c::scDokanWaitAnmFixFrame = 85.0f;

namespace {
    const float scDokanOutTurnSpeed[] = { 2048.0f };
}

float daPlBase_c::getWaterDokanCenterOffset(float param1) {
    /// @unofficial
    static const float l_maxOffsets[] = { 12.0f, 15.0f, 13.0f, 13.0f };
    float max = 16.0f;
    if (mKind == 1) {
        max = l_maxOffsets[mPlayerType];
    }
    float pos = getCenterY() - mPos.y;
    if (pos > max) {
        pos = max;
    }
    return param1 + 16.0f - pos;
}

void daPlBase_c::initDemoInDokan() {
    onStatus(STATUS_5E);
    mSpeedF = 0.0f;
    mSpeed.y = 0.0f;
    setZPosition(-1800.0f);
    if (mDemoStateArg == DOKAN_TYPE_CONNECTED) {
        mDamageInvulnTimer = 0;
    } else {
        mDamageInvulnTimer = 35;
        if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
            stopOther();
        }
    }
    mDemoSubstate = 0;
}

void daPlBase_c::endDemoInDokan() {
    mDamageInvulnTimer = 0;
    mPowerupChangeInvulnTimer = 0;
    offStatus(STATUS_CAN_WATER_WALK);
    offStatus(STATUS_PROPEL_NO_ROLL);
    offStatus(STATUS_5E);
}

void daPlBase_c::executeDemoInDokan(u8 dir) {
    // 0: Play the pipe sound effect
    // 1: Wait for a bit
    // 2: Move the player out of the pipe
    // 3: Finalize the transition
    switch (mDemoSubstate) {
        case 0:
            if (!mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                break;
            }
            if (!daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                break;
            }
            offStatus(STATUS_INVISIBLE);
            mDemoSubstate = 1;
            mDemoSubstateTimer = 40;
            if (mBc.checkWater(mWarpPos.x, mWarpPos.y, mLayer, nullptr)) {
                onNowBgCross(BGC_WATER_SHALLOW);
            }
            if (isNowBgCross(BGC_WATER_SHALLOW)) {
                if (isDemoState(StateID_DemoInDokanL) || isDemoState(StateID_DemoInDokanR)) {
                    mpMdlMng->setAnm(PLAYER_ANIM_SWIM_PIPE);
                    if (mDemoStateArg != DOKAN_TYPE_CONNECTED) {
                        mPos.y = getWaterDokanCenterOffset(mPos.y);
                        mWarpPos.y = mPos.y;
                    }
                }
                startSound(SE_PLY_WATER_DOKAN_IN_OUT, false);
            } else {
                startSound(SE_PLY_DOKAN_IN_OUT, false);
            }
            break;
        case 1:
            if (mDemoSubstateTimer == 0) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
                mDemoSubstate = 2;
            }
        case 2:
            onStatus(STATUS_79);
            if (isStatus(STATUS_PROPEL_NO_ROLL) && (dir == 2 || dir == 3) && std::fabs(mPos.x - mWarpPos.x) <= 20.0f) {
                offStatus(STATUS_PROPEL_NO_ROLL);
            }
            if (isStatus(STATUS_5E) && std::fabs(mPos.x - mWarpPos.x) <= 20.0f) {
                offStatus(STATUS_5E);
            }
            if (!demo_dokan_move_x(0.75f, 0.0f)) {
                break;
            }
            if (!demo_dokan_move_y(0.75f, 0.0f)) {
                break;
            }
            if (mDemoSubstate == 1) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
            }
            mDemoSubstate = 3;
            break;
        case 3:
            if (!mKey.buttonWalk(nullptr)) {
                if (!mAngle.y.chase(getMukiAngle(mDirection), scDokanOutTurnSpeed[0])) {
                    break;
                }
            }
            changeNormalAction();
            if (isNowBgCross(BGC_WATER_SHALLOW)) {
                onOldBgCross(BGC_WATER_SHALLOW);
            }
            break;
    }
}

void daPlBase_c::initDemoInDokanUD(u8 dir) {
    /// @unofficial
    static const float tmps[] = { 34.0f, 36.0f, 38.0f, 38.0f };
    /// @unofficial
    static const float tmps_big[] = { 40.0f, 42.0f, 44.0f, 44.0f };
    mpMdlMng->setAnm(PLAYER_ANIM_WAIT);
    mWarpPos = mPos;
    if (dir == 1) {
        if (mDemoStateArg == DOKAN_TYPE_MINI) {
            mWarpPos.y = mPos.y + 16.0f;
        } else {
            mWarpPos.y = mPos.y + 32.0f;
        }
        if (mKind == STAGE_ACTOR_YOSHI) {
            mPos.y -= 16.0f;
        } else {
            mPos.y -= 8.0f;
        }
    } else {
        onStatus(STATUS_PROPEL_NO_ROLL);
        float tmp;
        if (mKind == STAGE_ACTOR_YOSHI) {
            tmp = 30.0f;
            daPlBase_c *pl = ((daYoshi_c *) this)->getPlayerRideOn();
            if (pl != nullptr) {
                int t = pl->mPlayerType;
                switch (pl->mPowerup) {
                    case POWERUP_NONE:
                        tmp = tmps[t];
                        break;
                    case POWERUP_MINI_MUSHROOM:
                        tmp = 30.0f;
                        break;
                    default:
                        tmp = tmps_big[t];
                        break;
                }
            }
        } else {
            switch (mPowerup) {
                case POWERUP_NONE:
                    tmp = 20.0f;
                    break;
                case POWERUP_MINI_MUSHROOM:
                    tmp = 12.0f;
                    break;
                case POWERUP_PROPELLER_SHROOM:
                    tmp = getModelHeight() + 8.0f;
                    break;
                default:
                    tmp = getModelHeight();
                    break;
            }
        }
        mWarpPos.y = -tmp + mPos.y;
        mPos.y += 2.0f;
    }
    if (mKind == STAGE_ACTOR_YOSHI) {
        mAngle.y = 0;
    }
    initDemoInDokan();
}

void daPlBase_c::initDemoInDokanLR(u8 dir) {
    static const float l_dokanOffset[] = { 32.0f, 32.0f, 20.0f };
    mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK);
    onStatus(STATUS_PROPEL_NO_ROLL);
    if (dir == DOKAN_R) {
        mPos.x += 8.0f;
    } else {
        mPos.x -= 8.0f;
    }
    float tmp;
    if (mKind == STAGE_ACTOR_YOSHI) {
        tmp = 32.0f;
    } else {
        tmp = l_dokanOffset[mDemoStateArg];
    }
    if (dir == 3) {
        tmp = -tmp;
    }
    mWarpPos.set(mPos.x + tmp, mPos.y, mPos.z);
    if (dir == DOKAN_R) {
        mDirection = DIR_LR_L;
    } else {
        mDirection = DIR_LR_R;
    }
    mAngle.y = getMukiAngle(mDirection);
    initDemoInDokan();
}

void daPlBase_c::initializeState_DemoInDokanU() { initDemoInDokanUD(DOKAN_U); }
void daPlBase_c::finalizeState_DemoInDokanU() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanU() { executeDemoInDokan(DOKAN_U); }

void daPlBase_c::initializeState_DemoInDokanD() { initDemoInDokanUD(DOKAN_D); }
void daPlBase_c::finalizeState_DemoInDokanD() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanD() { executeDemoInDokan(DOKAN_D); }

void daPlBase_c::initializeState_DemoInDokanL() { initDemoInDokanLR(DOKAN_L); }
void daPlBase_c::finalizeState_DemoInDokanL() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanL() { executeDemoInDokan(DOKAN_L); }

void daPlBase_c::initializeState_DemoInDokanR() { initDemoInDokanLR(DOKAN_R); }
void daPlBase_c::finalizeState_DemoInDokanR() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanR() { executeDemoInDokan(DOKAN_R); }

bool daPlBase_c::demo_dokan_move_x(float moveStep, float epsilon) {
    sLib::chase(&mPos.x, mWarpPos.x, moveStep);
    return std::fabs(mPos.x - mWarpPos.x) <= epsilon;
}

bool daPlBase_c::demo_dokan_move_y(float moveStep, float offset) {
    return sLib::chase(&mPos.y, mWarpPos.y + offset, moveStep);
}

bool daPlBase_c::isEnableDokanInStatus() {
    if (isDemo()) {
        return false;
    }
    if (
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_QUAKE) ||
        isStatus(STATUS_53)
    ) {
        return false;
    }
    return true;
}

bool daPlBase_c::setDokanIn(DokanDir_e dir) {
    if (isStatus(STATUS_7E)) {
        return false;
    }
    int res = 0;
    int entranceNextGotoID;
    switch (dir) {
        case DOKAN_D:
            if (mKey.buttonDown()) {
                res = mBc.checkDokanDown(&mWarpPos, &entranceNextGotoID);
            }
            break;
        case DOKAN_U:
            if (mKey.buttonUp()) {
                res = mBc.checkDokanUp(&mWarpPos, &entranceNextGotoID);
            }
            break;
        case DOKAN_L:
        case DOKAN_R: {
            float x = 0.0f;
            float y = x;
            if (isStatus(STATUS_SWIM)) {
                x = 2.0f;
                y = -2.0f;
            } else {
                if (isStatus(STATUS_17)) {
                    x = 8.0f;
                    y = 8.0f;
                }
            }
            res = mBc.checkDokanLR(&mWarpPos, mDirection, &entranceNextGotoID, x, y);
            break;
        }
        case DOKAN_ROLL:
            break;
    }
    if (res == 1 && setDemoOutDokanAction(entranceNextGotoID, dir)) {
        return true;
    }
    return false;
}

bool daPlBase_c::setDemoOutDokanAction(int entranceNextGotoID, DokanDir_e dir) {
    mDokanEnterNextGotoID = entranceNextGotoID;
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sNextGotoData *nextGoto = cdFile->getNextGotoP(mDokanEnterNextGotoID);

    mDokanMode = DEMO_DOKAN_NORMAL;
    if (nextGoto->mFlags & NEXT_GOTO_RAIL) {
        mDokanMode = DEMO_DOKAN_RAIL;
    } else if (nextGoto->mFlags & NEXT_GOTO_WATER_TANK) {
        mDokanMode = DEMO_DOKAN_WATER_TANK;
    }

    static sStateIDIf_c *l_dokanInAction[] = {
        &StateID_DemoOutDokanU,
        &StateID_DemoOutDokanD,
        &StateID_DemoOutDokanL,
        &StateID_DemoOutDokanR,
        &StateID_DemoOutDokanRoll,
    };
    switch (mDokanMode) {
        case DEMO_DOKAN_NONE:
            break;
        case DEMO_DOKAN_NORMAL:
            if (dNext_c::m_instance->fn_800cfed0(dScStage_c::m_instance->mCurrFile, mDokanEnterNextGotoID)) {
                return false;
            }
            if (daPyDemoMng_c::mspInstance->m_5c) {
                return false;
            }
            dNext_c::m_instance->setChangeSceneNextDat(dScStage_c::m_instance->mCurrFile, mDokanEnterNextGotoID, dFader_c::FADER_CIRCLE_TARGET);
            if (nextGoto->mType == 22) {
                changeDemoState(StateID_DemoOutWaterTank, DOKAN_U);
            } else {
                changeDemoState(*l_dokanInAction[dir]);
            }
            return true;
        case DEMO_DOKAN_RAIL:
            dRail_c::getRailInfoP(nextGoto->mRailID); // [Unused return value]
            changeDemoState(*l_dokanInAction[dir]);
            return true;
        case DEMO_DOKAN_WATER_TANK:
            if (nextGoto->mType == 22) {
                changeDemoState(StateID_DemoOutWaterTank, dir);
            } else {
                changeDemoState(*l_dokanInAction[dir]);
            }
            return true;
    }
    return false;
}

void daPlBase_c::initDemoOutDokan() {
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0.0f;
    setZPosition(-1800.0f);
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        startSound(SE_PLY_WATER_DOKAN_IN_OUT, false);
    } else {
        startSound(SE_PLY_DOKAN_IN_OUT, false);
    }
}

void daPlBase_c::endDemoOutDokan() {
    offStatus(STATUS_PROPEL_NO_ROLL);
    offStatus(STATUS_5E);
}

void daPlBase_c::initDemoOutDokanUD(u8 dir) {
    mDokanDir = dir;
    changeState(StateID_Walk, nullptr);
    mpMdlMng->setAnm(PLAYER_ANIM_WAIT, 0.0f, 5.0f, 85.0f);
    if (mDokanMode == DEMO_DOKAN_RAIL) {
        if (dir == DOKAN_U) {
            mDokanOffsetY = 0.0f;
        } else {
            mDokanOffsetY = -34.0f;
        }
    } else if (dir == DOKAN_U) {
        mDokanOffsetY = 2.0f;
    } else if (mKind == STAGE_ACTOR_YOSHI) {
        mDokanOffsetY = -16.0f;
    } else {
        mDokanOffsetY = -10.0f;
    }
    mDokanShiftXEpsilon = 0.0f;
    if (mDokanMode == DEMO_DOKAN_NORMAL && daPyMng_c::mNum == 1) {
        stopOther();
    }
    initDemoOutDokan();
}

void daPlBase_c::executeDemoOutDokanUD() {
    // 0: Move to the center of the pipe
    // 1: Move into the pipe
    // 2: Water tank only: move to Y target, then transition to in-tank state
    // 3: Done
    switch (mDemoSubstate) {
        case 0: {
            int cond = 0;
            if (mKind == STAGE_ACTOR_YOSHI) {
                if (mAngle.y.chase(0, scDokanOutTurnSpeed[0])) {
                    cond = 1;
                }
            } else {
                if (mAngle.y.chase(getMukiAngle(mDirection), 0x2000)) {
                    cond = 1;
                }
            }
            if (demo_dokan_move_x(1.0f, mDokanShiftXEpsilon) && cond == 1) {
                mDemoSubstate = 1;
                mDemoSubstateTimer = 10;
            }
            break;
        }
        case 1:
            if (demo_dokan_move_y(0.75f, mDokanOffsetY)) {
                onStatus(STATUS_5E);
                switch (mDokanMode) {
                    case DEMO_DOKAN_RAIL:
                        changeDemoState(StateID_DemoRailDokan);
                        break;
                    case DEMO_DOKAN_WATER_TANK:
                        onStatus(STATUS_INVISIBLE);
                        mLayer = 0;
                        if (mDokanDir == DOKAN_U) {
                            mWarpPos.y = mPos.y + 80.0f;
                        } else {
                            mWarpPos.y = mPos.y - 80.0f;
                        }
                        mDemoSubstate = 2;
                        break;
                    default:
                        changeNextScene(1);
                        mDemoSubstate = 3;
                        break;
                }
            }
            break;
        case 2:
            if (sLib::chase(&mPos.y, mWarpPos.y, 1.0f)) {
                changeState(StateID_Walk, nullptr);
                changeDemoState(StateID_DemoInWaterTank, DOKAN_TYPE_CONNECTED);
            }
            break;
    }
}

void daPlBase_c::initDemoOutDokanLR(u8 dir) {
    mDokanDir = dir;
    if (isStatus(STATUS_SWIM)) {
        mpMdlMng->setAnm(PLAYER_ANIM_SWIM_PIPE);
    } else {
        mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK_START);
    }
    onStatus(STATUS_PROPEL_NO_ROLL);
    if (mDokanMode == DEMO_DOKAN_NORMAL && daPyMng_c::mNum == 1) {
        stopOther();
    }
    if (isStatus(STATUS_SWIM) || mKind == STAGE_ACTOR_YOSHI) {
        if (dir == DOKAN_R) {
            mWarpPos.x += 8.0f;
        } else {
            mWarpPos.x -= 8.0f;
        }
    }
    initDemoOutDokan();
}

void daPlBase_c::executeDemoOutDokanLR() {
    // 0: Move into the pipe, possibly shifting in the Y direction if needed
    // 1: Water tank only: move to X target
    // 2: Water tank only: move down slightly
    // 3: Water tank only: move to final X target, then transition to in-tank state
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK);
    }
    switch (mDemoSubstate) {
        case 0: {
            bool yTargetReached = false;
            if (!addCalcAngleY(getMukiAngle(mDirection), 10)) {
                float offset = 0.0f;
                if (isStatus(STATUS_SWIM)) {
                    offset = getWaterDokanCenterOffset(mWarpPos.y) - mWarpPos.y;
                }
                if (demo_dokan_move_y(0.75f, offset)) {
                    yTargetReached = true;
                }
            }
            if (yTargetReached && demo_dokan_move_x(0.75f, 0.0f)) {
                onStatus(STATUS_5E);
                switch (mDokanMode) {
                    case DEMO_DOKAN_RAIL:
                        changeDemoState(StateID_DemoRailDokan);
                        break;
                    case DEMO_DOKAN_WATER_TANK:
                        onStatus(STATUS_INVISIBLE);
                        mLayer = LAYER_1;
                        mWarpPos.x = mPos.x + sc_DirSpeed[mDirection] * 48.0f;
                        mDemoSubstate = 1;
                        break;
                    default:
                        changeNextScene(1);
                        break;
                }
            }
            break;
        }
        case 1:
            if (sLib::chase(&mPos.x, mWarpPos.x, 1.0f)) {
                mWarpPos.y -= 16.0f;
                mDemoSubstate = 2;
            }
            break;
        case 2:
            if (sLib::chase(&mPos.y, mWarpPos.y, 1.0f)) {
                mWarpPos.x = mPos.x + sc_DirSpeed[mDirection] * 32.0f;
                mDemoSubstate = 3;
            }
            break;
        case 3:
            if (sLib::chase(&mPos.x, mWarpPos.x, 1.0f)) {
                changeState(StateID_Walk, nullptr);
                changeDemoState(StateID_DemoInWaterTank, DOKAN_TYPE_CONNECTED);
            }
            break;
    }
}

void daPlBase_c::initializeState_DemoOutDokanU() { initDemoOutDokanUD(DOKAN_U); }
void daPlBase_c::finalizeState_DemoOutDokanU() { endDemoOutDokan(); }
void daPlBase_c::executeState_DemoOutDokanU() { executeDemoOutDokanUD(); }

void daPlBase_c::initializeState_DemoOutDokanD() { initDemoOutDokanUD(DOKAN_D); }
void daPlBase_c::finalizeState_DemoOutDokanD() { endDemoOutDokan(); }
void daPlBase_c::executeState_DemoOutDokanD() { executeDemoOutDokanUD(); }

void daPlBase_c::initializeState_DemoOutDokanL() { initDemoOutDokanLR(DOKAN_L); }
void daPlBase_c::finalizeState_DemoOutDokanL() { endDemoOutDokan(); }
void daPlBase_c::executeState_DemoOutDokanL() { executeDemoOutDokanLR(); }

void daPlBase_c::initializeState_DemoOutDokanR() { initDemoOutDokanLR(DOKAN_R); }
void daPlBase_c::finalizeState_DemoOutDokanR() { endDemoOutDokan(); }
void daPlBase_c::executeState_DemoOutDokanR() { executeDemoOutDokanLR(); }

void daPlBase_c::initializeState_DemoOutDokanRoll() {
    initDemoOutDokanUD(DOKAN_ROLL);
    mDokanMode = DEMO_DOKAN_NORMAL;
    mDokanMoveSpeed.x = 0.0f;
    mDokanMoveSpeed.y = 0.0f;
    mBc.setRideOnObjBg(mpDokanBgCtr, mPos);
    mpDokanBgCtr->addDokanMoveDiff(&mWarpPos);
}

void daPlBase_c::finalizeState_DemoOutDokanRoll() {}

void daPlBase_c::executeState_DemoOutDokanRoll() {
    mBc.setRideOnObjBg(mpDokanBgCtr, mPos);
    mpDokanBgCtr->addDokanMoveDiff(&mWarpPos);
    mVec3_c warpPos(mWarpPos.x, mWarpPos.y, mPos.z);
    switch (mDemoSubstate) {
        case 0: {
            int cond = 0;
            if (mKind == STAGE_ACTOR_YOSHI) {
                if (mAngle.y.chase(0, scDokanOutTurnSpeed[0])) {
                    cond = 1;
                }
            } else {
                if (mAngle.y.chase(getMukiAngle(mDirection), 0x2000)) {
                    cond = 1;
                }
            }
            mVec3_c copy;
            mVec3_c diff1 = warpPos - mPos;
            copy.set(diff1.x, diff1.y, diff1.z);
            copy.normalize();
            mPos += 1.0f * copy;

            if ((mPos - warpPos).isSmallerThan1()) {
                if (cond == 1) {
                    mPos = warpPos;
                    mDemoSubstate = 1;
                }
            }
            break;
        }
        case 1: {
            mAng v = *mpDokanBgCtr->mRotation;
            if (mDirection == DIR_LR_L) {
                v = -v;
            }
            sLib::addCalcAngle(&mAngle.x.mAngle, v.mAngle, 4, 0x1000, 0x100);
            float tmp = mDokanOffsetY + -32.0f;
            sLib::chase(&mDokanMoveSpeed.y, tmp, 0.75f);
            mMtx_c m1, m2;
            m1.trans(warpPos);
            m1.ZrotM(*mpDokanBgCtr->mRotation);
            m2.trans(0.0f, mDokanMoveSpeed.y, 0.0f);
            m1.concat(m2);
            m1.multVecZero(mPos);
            if (mDokanMoveSpeed.y <= tmp) {
                changeNextScene(1);
                mDemoSubstate = 3;
                onStatus(STATUS_INVISIBLE);
            }
            break;
        }
    }
}

void daPlBase_c::initializeState_DemoInWaterTank() {
    onStatus(STATUS_INVISIBLE);
    mDirection = mDemoStateArg;
    mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK);
    mAngle.y = 0;
    if (mDemoStateArg == 1) {
        mDamageInvulnTimer = 0;
        mLayer = 0;
        setZPosition(3000.0f);
    } else {
        mDamageInvulnTimer = 35;
        if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
            stopOther();
        }
    }
    mDemoSubstate = 0;
}

void daPlBase_c::finalizeState_DemoInWaterTank() {}

void daPlBase_c::executeState_DemoInWaterTank() {
    // 0: Animation in
    // 1: Wait for animation end, then change to normal action
    switch (mDemoSubstate) {
        case 0:
            if (!mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                break;
            }
            if (!daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                break;
            }
            offStatus(STATUS_INVISIBLE);
            mDemoSubstate = 1;
            mDemoSubstateTimer = 60;
            clearNowBgCross();
            checkWater();
            if (isNowBgCross(BGC_WATER_SHALLOW)) {
                mpMdlMng->setAnm(PLAYER_ANIM_SWIM_PIPE);
                mPos.y = getWaterDokanCenterOffset(mPos.y);
                startSound(SE_PLY_WATER_DOKAN_IN_OUT, false);
            } else {
                startSound(SE_PLY_DOKAN_IN_OUT, false);
            }
            break;
        case 1:
            if (mDemoSubstateTimer == 0) {
                changeNormalAction();
            }
            break;
    }
}

void daPlBase_c::initializeState_DemoOutWaterTank() {
    mpMdlMng->setAnm(PLAYER_ANIM_WAIT);
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0.0f;
    mAngle.y = 0x8000;
}

void daPlBase_c::finalizeState_DemoOutWaterTank() {}

void daPlBase_c::executeState_DemoOutWaterTank() {
    // 0: Shift towards the center of the pipe
    // 1: Wait for animation to finish
    // 2: Move inside of the pipe horizontally
    // 3: Move down the pipe vertically
    // 4: Move to the final target position, then switch to the pipe exit state
    switch (mDemoSubstate) {
        case 0:
            if (sLib::chase(&mPos.x, mWarpPos.x, 1.0f)) {
                mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK_START);
                startSound(SE_PLY_DOKAN_IN_OUT, false);
                mDemoSubstateTimer = 60;
                mDemoSubstate = 1;
            }
            break;
        case 1:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                mpMdlMng->setAnm(PLAYER_ANIM_LOW_WALK);
            }
            if (mDemoSubstateTimer == 0) {
                onStatus(STATUS_INVISIBLE);
                if (mDokanMode == DEMO_DOKAN_WATER_TANK) {
                    setZPosition(-1800.0f);
                    if (mDemoStateArg <= 1) {
                        mWarpPos.x = mPos.x + sc_DirSpeed[mDemoStateArg] * 32.0f;
                        mDemoSubstate = 2;
                    } else {
                        mWarpPos.y = mPos.y - sc_DirSpeed[mDemoStateArg & 1] * 80.0f;
                        mDemoSubstate = 4;
                    }
                } else {
                    changeNextScene(1);
                }
                break;
            }
            break;
        case 2:
            if (sLib::chase(&mPos.x, mWarpPos.x, 1.0f)) {
                mWarpPos.y = getWaterDokanCenterOffset(mWarpPos.y + 16.0f);
                mDemoSubstate = 3;
            }
            break;
        case 3:
            if (sLib::chase(&mPos.y, mWarpPos.y, 1.0f)) {
                mWarpPos.x = mPos.x + sc_DirSpeed[mDemoStateArg] * 48.0f;
                mDemoSubstate = 4;
            }
            break;
        case 4:
            if (
                sLib::chase(&mPos.x, mWarpPos.x, 1.0f) &&
                sLib::chase(&mPos.y, mWarpPos.y, 1.0f)
            ) {
                mLayer = LAYER_2;
                mAngle.y = 0;
                switch (mDemoStateArg) {
                    case 0:
                        changeDemoState(StateID_DemoInDokanL, DOKAN_TYPE_CONNECTED);
                        break;
                    case 1:
                        changeDemoState(StateID_DemoInDokanR, DOKAN_TYPE_CONNECTED);
                        break;
                    case 2:
                        changeDemoState(StateID_DemoInDokanU, DOKAN_TYPE_CONNECTED);
                        break;
                    case 3:
                        changeDemoState(StateID_DemoInDokanD, DOKAN_TYPE_CONNECTED);
                        break;
                }
                setZPosition(-1800.0f);
            }
            break;
    }
}

void daPlBase_c::initializeState_DemoRailDokan() {
    onStatus(STATUS_INVISIBLE);
    sNextGotoData *nextGoto = dCd_c::m_instance->
        getFileP(dScStage_c::m_instance->mCurrFile)->
        getNextGotoP(mDokanEnterNextGotoID);

    sRailInfoData *rail = dRail_c::getRailInfoP(nextGoto->mRailID);

    // @bug This line should appear after setting mRailDokanRailIndex, not before.
    sRailNodeData *node = &dCd_c::m_instance->
        getFileP(dScStage_c::m_instance->mCurrFile)->
        mpRailNodes[rail->mNodeIdx + mRailDokanRailIndex];

    if (nextGoto->mFlags & NEXT_GOTO_RAIL_REVERSE) {
        mRailDokanRailIndex = rail->mCount - 2;
    } else {
        mRailDokanRailIndex = 1;
    }

    mVec2_c delta(node->mX - mPos.x, -node->mY - mPos.y);

    float size = delta.length();
    mRailDokanNextNodeTimer = size / 2.0f;
    mDokanMoveSpeed.set(delta.x / size * 2.0f, delta.y / size * 2.0f);
}

void daPlBase_c::finalizeState_DemoRailDokan() {
    offStatus(STATUS_INVISIBLE);
}

void daPlBase_c::setExitRailDokan() {
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sNextGotoData *nextGoto = cdFile->getNextGotoP(mDokanEnterNextGotoID);

    // @bug This will the entered next-goto, not the one the player will exit from.
    // To fix this, we'd need to do
    // nextGoto = cdFile->getNextGotoP(nextGoto->mDestID);
    // here.

    mLayer = nextGoto->mLayer;

    switch (nextGoto->mType) {
        case 3:
            changeDemoState(StateID_DemoInDokanU, DOKAN_TYPE_CONNECTED);
            break;
        case 4:
            // @bug This should be StateID_DemoInDokanD.
            changeDemoState(StateID_DemoInDokanU, DOKAN_TYPE_CONNECTED);
            break;
        case 5:
            changeDemoState(StateID_DemoInDokanR, DOKAN_TYPE_CONNECTED);
            break;
        case 6:
            changeDemoState(StateID_DemoInDokanL, DOKAN_TYPE_CONNECTED);
            break;
    }
}

void daPlBase_c::executeState_DemoRailDokan() {
    if (--mRailDokanNextNodeTimer < 0) {
        sNextGotoData *ngt = dCd_c::m_instance->
            getFileP(dScStage_c::m_instance->mCurrFile)->
            getNextGotoP(mDokanEnterNextGotoID);
        sRailInfoData *rail = dRail_c::getRailInfoP(ngt->mRailID);

        sRailNodeData *currNode = &dCd_c::m_instance->
            getFileP(dScStage_c::m_instance->mCurrFile)->
            mpRailNodes[rail->mNodeIdx + mRailDokanRailIndex];
        mPos.x = currNode->mX;
        mPos.y = -currNode->mY;

        int done = 0;
        if (ngt->mFlags & 1) {
            mRailDokanRailIndex--;
            if (mRailDokanRailIndex < 0) {
                done = 1;
            }
        } else {
            mRailDokanRailIndex++;
            if (mRailDokanRailIndex >= rail->mCount) {
                done = 1;
            }
        }

        if (done == 1) {
            setExitRailDokan();
            return;
        }

        sRailNodeData *nextNode = &dCd_c::m_instance->
            getFileP(dScStage_c::m_instance->mCurrFile)->
            mpRailNodes[rail->mNodeIdx + mRailDokanRailIndex];

        mVec2_c distToNext(nextNode->mX - mPos.x, -nextNode->mY - mPos.y);

        float distLen = distToNext.length();
        mRailDokanNextNodeTimer = distLen / 2.0f;
        mDokanMoveSpeed.set(distToNext.x / distLen * 2.0f, distToNext.y / distLen * 2.0f);
    } else {
        mPos.x += mDokanMoveSpeed.x;
        mPos.y += mDokanMoveSpeed.y;
    }
}

void daPlBase_c::setObjDokanIn(dBg_ctr_c *bgCtr, mVec3_c &pos, int nextGotoID) {
    mpDokanBgCtr = bgCtr;
    mWarpPos.set(pos.x, pos.y, mPos.z);
    mRollDokanAngle = *bgCtr->mRotation;
    setDemoOutDokanAction(nextGotoID, DOKAN_ROLL);
}

bool daPlBase_c::isDispOutCheckOn() {
    if (isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_53) ||
        isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE) ||
        isStatus(STATUS_DEMO_NEXT_GOTO_BLOCK) ||
        isStatus(STATUS_8D)
    ) {
        return false;
    }
    return true;
}

void daPlBase_c::initializeState_DemoDown() {}
void daPlBase_c::finalizeState_DemoDown() {}
void daPlBase_c::executeState_DemoDown() {}

int daPlBase_c::setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType) {
    if (daPyDemoMng_c::mspInstance->mFlags & 4) {
        return -1;
    }

    mPos.x = landPos.x;
    changeState(StateID_None);
    changeDemoState(StateID_DemoGoal, false);

    mWarpPos.x = goalCastleX;
    mWarpPos.y = landPos.y;
    mWarpPos.z = landPos.x + 80.0f;

    mGoalDemoIndex = daPyDemoMng_c::mspInstance->setGoalDemoList(mPlayerNo);
    if (mGoalDemoIndex == 0) {
        // Only set once, when the first player reaches the goal
        daPyDemoMng_c::mspInstance->setDemoMode(daPyDemoMng_c::MODE_1, 0);
        daPyDemoMng_c::mspInstance->mGoalType = goalType;
        daPyDemoMng_c::mspInstance->mPlayerNo = mPlayerNo;

        mVec3_c fireworkPos(goalCastleX - 112.0f, landPos.y, 5500.0f);
        float height;
        if (dBc_c::checkGround(&fireworkPos, &height, mLayer, 1, -1)) {
            fireworkPos.y = height + 112.0f;
        }
        daPyDemoMng_c::mspInstance->mFireworkPos = fireworkPos;
    }

    if (!daPyMng_c::isItemKinopio(mPlayerNo)) {
        if (mGoalDemoIndex != 0 && mGoalDemoIndex + 1 == daPyMng_c::getNumInGame()) {
            // All players have reached the goal
            dMultiMng_c::mspInstance->setClapSE();
        }
    } else {
        daPyDemoMng_c::mspInstance->m_42 = 1;
        dGameCom::hideFukidashiForSession(mPlayerNo, 8);
    }

    int totalPlayers = daPyMng_c::getNumInGame() + daPyMng_c::getItemKinopioNum();
    if (totalPlayers == mGoalDemoIndex + 1) {
        daPyDemoMng_c::mspInstance->stopBgmGoalDemo();
    }

    return -1;
}

bool daPlBase_c::setHideNotGoalPlayer() {
    if (!isStatus(STATUS_GOAL_POLE_TOUCHED)) {
        mSpeedF = 0.0f;
        mSpeed.y = 0.0f;
        onStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE);
        setFallAction();
        return true;
    }
    return false;
}

void daPlBase_c::stopGoalOther() {
    if (!isPlayerGameStop()) {
        return;
    }
    dActor_c::mExecStopReq |= 0xf;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        daPlBase_c *ctrlPl = daPyMng_c::getCtrlPlayer(i);
        if (ctrlPl == nullptr || !ctrlPl->isStatus(STATUS_GOAL_POLE_TOUCHED)) {
            continue;
        }

        daPlBase_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr) {
            pl->mExecStopMask &= ~2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(i);
        if (yoshi != nullptr) {
            yoshi->mExecStopMask &= ~2;
        }
    }
}

void daPlBase_c::playGoalOther() {
    dActor_c::mExecStopReq &= ~0xf;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        daPlBase_c *ctrlPl = daPyMng_c::getCtrlPlayer(i);
        if (ctrlPl == nullptr || ctrlPl->isStatus(STATUS_GOAL_POLE_TOUCHED)) {
            continue;
        }

        daPlBase_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr) {
            pl->mExecStopMask |= 2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(i);
        if (yoshi != nullptr) {
            yoshi->mExecStopMask |= 2;
        }
    }
}

void daPlBase_c::initDemoGoalBase() {
    onStatus(STATUS_GOAL_POLE_TOUCHED);
    if (!mDemoStateArg) {
        startPlayerVoice(VOICE_GOAL_POLE_CATCH, 0);
        onStatus(STATUS_7E);
    }
    clearJumpActionInfo(0);
    endStar();
    setDemoGoalMode(0, 0);
    mpMdlMng->setAnm(PLAYER_ANIM_RTREE_START);
    mAngle.x = 0;
    setZPositionDirect(3000.0f);
    mSpeed.x = 0.0f;
    mSpeedF = 0.0f;
    mSpeed.y = 0.0f;
    mAccelY = 0.0f;
    mMaxFallSpeed = -4.0f;
}

void daPlBase_c::finalizeDemoGoalBase() {
    offStatus(STATUS_GOAL_POLE_TOUCHED);
    offStatus(STATUS_GOAL_POLE_WAIT_BELOW_PLAYER);
    offStatus(STATUS_GOAL_POLE_FINISHED_SLIDE_DOWN);
    offStatus(STATUS_GOAL_POLE_READY_FOR_JUMP_OFF);
    offStatus(STATUS_GOAL_POLE_TURN);
    offStatus(STATUS_6C);
    offStatus(STATUS_6D);
    offStatus(STATUS_6E);
    offStatus(STATUS_7E);
}

void daPlBase_c::initializeState_DemoGoal() { initDemoGoalBase(); }
void daPlBase_c::finalizeState_DemoGoal() { finalizeDemoGoalBase(); }

float daPlBase_c::getDemoGoalLandPos() {
    float pos = mWarpPos.z + daPyDemoMng_c::mspInstance->m_1c * 16.0f;
    if (daPyDemoMng_c::mspInstance->m_1c > 1) {
        pos -= mGoalTouchOrder * 32.0f;
    }
    return pos;
}

void daPlBase_c::setDemoGoal_MultiJump() {
    offStatus(STATUS_7E);
    mVec3_c pos(
        getDemoGoalLandPos(),
        mPos.y,
        mPos.z
    );
    dBc_c::checkGround(&pos, &pos.y, mLayer, 1, -1);
    mAngle.y = 0x4000;
    mDemoState = GOAL_DEMO_POLE_JUMP;
    mpMdlMng->setAnm(PLAYER_ANIM_GOAL_JUMP);
    if (daPyDemoMng_c::mspInstance->m_1c > 1) {
        initGoalJump(pos, daPlBase_c::sc_JumpSpeed + 1.5f);
    } else {
        initGoalJump(pos, daPlBase_c::sc_JumpSpeed + 1.3f);
    }
}

void daPlBase_c::executeDemoGoal_Pole() {
    switch ((DemoGoalState_Pole_e) mDemoState) {
        case GOAL_DEMO_POLE_SWING:
            if (!addCalcAngleY(-0x4000, 10) && mpMdlMng->mpMdl->mAnm.isStop()) {
                mDemoState = GOAL_DEMO_WAIT_BELOW_PLAYER;
                mpMdlMng->setAnm(PLAYER_ANIM_RTREE_WAIT);
                onStatus(STATUS_GOAL_POLE_WAIT_BELOW_PLAYER);
            }
            break;
        case GOAL_DEMO_WAIT_BELOW_PLAYER:
            if (isStatus(STATUS_67)) {
                int polePlayer = daPyDemoMng_c::mspInstance->getPoleBelowPlayer(mPlayerNo);
                if (polePlayer != -1) {
                    daPlBase_c *ctrlPl = daPyMng_c::getCtrlPlayer(polePlayer);
                    if (ctrlPl != nullptr) {
                        float l = 0.7f * ctrlPl->getModelHeight() + ctrlPl->mPos.y;
                        if (!(mPos.y > l || ctrlPl->isStatus(STATUS_GOAL_POLE_FINISHED_SLIDE_DOWN))) {
                            break;
                        }
                    }
                }
                mDemoState = GOAL_DEMO_POLE_SLIDE;
                offStatus(STATUS_GOAL_POLE_WAIT_BELOW_PLAYER);
                daPyDemoMng_c::mspInstance->mFlags |= 1;
                mSpeed.y = -1.9f;
            }
            break;
        case GOAL_DEMO_POLE_SLIDE: {
            mPos.y += mSpeed.y;
            if (mPos.y < mGoalPoleEndY) {
                mPos.y = mGoalPoleEndY;
                onStatus(STATUS_GOAL_POLE_FINISHED_SLIDE_DOWN);
                mSpeed.y = 0.0f;
                mDemoState = GOAL_DEMO_POLE_WAIT_JUMP;
                mpMdlMng->setAnm(PLAYER_ANIM_RTREE_POSE);
            }
            break;
        }
        case GOAL_DEMO_POLE_WAIT_JUMP:
            if (isStatus(STATUS_GOAL_POLE_READY_FOR_JUMP_OFF)) {
                setDemoGoal_MultiJump();
            }
            break;
        case GOAL_DEMO_POLE_JUMP:
            if (calcGoalJump()) {
                mDemoState = GOAL_DEMO_POLE_LAND;
                mpMdlMng->setAnm(PLAYER_ANIM_GOAL_JUMP_ED);
                setLandSE();
            }
            break;
        case GOAL_DEMO_POLE_LAND:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                mpMdlMng->setAnm(PLAYER_ANIM_STAMP);
                mDemoState = GOAL_DEMO_POLE_WAIT_TURN;
                mDemoSubstateTimer = sc_DemoPoleWaitTurn;
            }
            break;
        case GOAL_DEMO_POLE_WAIT_TURN:
            if (mDemoSubstateTimer == 0) {
                onStatus(STATUS_GOAL_POLE_TURN);
                mDemoState = GOAL_DEMO_POLE_TURN;
            }
            break;
        case GOAL_DEMO_POLE_TURN:
            if (daPyDemoMng_c::mspInstance->mFlags & 8 && !addCalcAngleY(0, 10)) {
                mDemoSubstateTimer = sc_DemoPoleWaitEnd;
                mDemoState = GOAL_DEMO_POLE_WAIT_END;
            }
            break;
        case GOAL_DEMO_POLE_WAIT_END:
            if (mDemoSubstateTimer == 0) {
                setDemoGoalMode(GOAL_DEMO_ACTION_WAIT, 0);
            }
            break;
    }
}

void daPlBase_c::executeDemoGoal_Wait() {
    if (isStatus(STATUS_6B)) {
        offStatus(STATUS_6B);
        onStatus(STATUS_6C);
        setDemoGoalMode(GOAL_DEMO_ACTION_KIME_POSE, 0);
    }
    if (isStatus(STATUS_6D)) {
        setDemoGoalMode(GOAL_DEMO_ACTION_RUN, 0);
    }
}

void daPlBase_c::executeDemoGoal_KimePose() {
    if (mKimePoseMode != KIME_POSE_NONE && mpMdlMng->mpMdl->mAnm.checkFrame(107.0f)) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            if (pl->isItemKinopio()) {
                if (mKimePoseMode == KIME_POSE_PENGUIN || mKimePoseMode == KIME_POSE_NO_HAT) {
                    if (pl->mPowerup == POWERUP_NONE) {
                        dScoreMng_c::m_instance->fn_800e25a0(8, mPlayerNo, 1);
                    } else {
                        dScoreMng_c::m_instance->fn_800e25a0(9, mPlayerNo, 1);
                    }
                }
            } else {
                if (dInfo_c::mGameFlag & 0x10 && dInfo_c::mGameFlag & 0x40) {
                    if (mGoalDemoIndex == 0) {
                        SndAudioMgr::sInstance->startSystemSe(SE_OBJ_GOAL_GET_COIN_BONUS, 1);
                    }
                    static const int scGoalCoin[] = {20, 15, 10, 5 };
                    dMultiMng_c::mspInstance->setBattleCoin(mPlayerNo, scGoalCoin[mGoalTouchOrder]);
                }
            }
        }
    }
    if (updateDemoKimePose(CLEAR_TYPE_GOAL)) {
        setDemoGoalMode(GOAL_DEMO_ACTION_WAIT, 0);
    }
}

void daPlBase_c::executeDemoGoal_Run() {}

void daPlBase_c::setDemoGoalMode(int mode, int param) {
    mDemoSubstate = mode;
    mDemoState = param;
}

void daPlBase_c::executeState_DemoGoal() {
    switch ((DemoGoalSubstate_e) mDemoSubstate) {
        case GOAL_DEMO_ACTION_POLE:
            executeDemoGoal_Pole();
            break;
        case GOAL_DEMO_ACTION_WAIT:
            executeDemoGoal_Wait();
            break;
        case GOAL_DEMO_ACTION_KIME_POSE:
            executeDemoGoal_KimePose();
            break;
        case GOAL_DEMO_ACTION_RUN:
            executeDemoGoal_Run();
            break;
    }
    bgCheck(0);
}

void daPlBase_c::initGoalJump(mVec3_c &pos, float ySpeed) {
    mSpeed.y = ySpeed;
    int count = 0;
    float yPos = mPos.y;
    while (true) {
        mAccelY = -0.28f;
        mSpeed.y += mAccelY;
        if (mSpeed.y < -4.0f) {
            mSpeed.y = -4.0f;
        }
        yPos += mSpeed.y;
        if (mSpeed.y < 0.0f && yPos <= pos.y) {
            break;
        }
        count++;
    }
    mGoalJumpTarget = pos;
    mGoalJumpFrameCount = count;
    mSpeed.y = ySpeed;
}

bool daPlBase_c::calcGoalJump() {
    if (mGoalJumpFrameCount != 0) {
        sLib::chase(&mPos.x, mGoalJumpTarget.x, (mGoalJumpTarget.x - mPos.x) / mGoalJumpFrameCount);
        mGoalJumpFrameCount--;
    }
    mAccelY = -0.28f;
    mSpeed.y += mAccelY;
    if (mSpeed.y < -4.0f) {
        mSpeed.y = -4.0f;
    }
    mPos.y += mSpeed.y;
    if (mSpeed.y < 0.0f && isNowBgCross(BGC_FOOT)) {
        mPos = mGoalJumpTarget;
        return true;
    }
    return false;
}

void daPlBase_c::initDemoKimePose() {
    onStatus(STATUS_6C);
    mKimePoseMode = KIME_POSE_NONE;
}

bool daPlBase_c::updateDemoKimePose(ClearType_e clearType) {
    return false;
}

void daPlBase_c::startKimePoseVoice(ClearType_e clearType) {
    int playerCount;
    if (clearType == CLEAR_TYPE_GOAL) {
        playerCount = daPyDemoMng_c::mspInstance->m_1c;
    } else {
        playerCount = daPyDemoMng_c::mspInstance->getControlDemoPlayerNum();
    }
    if (dInfo_c::m_startGameInfo.mScreenType == 1) {
        if (playerCount >= 2) {
            startPlayerVoice(VOICE_CLEAR_MULTI, 0);
        } else {
            startPlayerVoice(VOICE_CLEAR_HELPED, 0);
        }
    } else if (clearType == CLEAR_TYPE_FINAL_BOSS) {
        startPlayerVoice(VOICE_CLEAR_LAST_BOSS, 0);
    } else if (playerCount >= 2) {
        startPlayerVoice(VOICE_CLEAR_MULTI, 0);
    } else if (clearType == CLEAR_TYPE_GOAL) {
        if (daPyDemoMng_c::mspInstance->mGoalType == 0) {
            startPlayerVoice(VOICE_CLEAR_NORMAL, 0);
        } else {
            startPlayerVoice(VOICE_CLEAR_ANOTHER, 0);
        }
    } else {
        startPlayerVoice(VOICE_CLEAR_BOSS, 0);
    }
}

bool daPlBase_c::startControlDemo() {
    if (isDemoType(DEMO_PLAYER)) {
        return true;
    } else if (isDemoType(DEMO_ENDING_DANCE)) {
        changeDemoState(StateID_DemoControl, CONTROL_DEMO_WAIT);
        return true;
    } else {
        onStatus(STATUS_71);
        return false;
    }
}

void daPlBase_c::endControlDemo(int p) {
    if (isStatus(STATUS_72)) {
        changeNormalAction();
    }
}

void daPlBase_c::setControlDemoDir(u8 dir) {
    if (isStatus(STATUS_72)) {
        mDirection = dir;
        if (!isState(StateID_Walk)) {
            changeState(StateID_Walk, (void *) 1);
        }
    }
}

void daPlBase_c::setControlDemoWait() {
    if (isStatus(STATUS_72)) {
        mDemoState = CONTROL_DEMO_WAIT;
        changeState(StateID_Walk, (void *) 1);
    }
}

bool daPlBase_c::isControlDemoWait() {
    if (isStatus(STATUS_72)) {
        if (mDemoState == CONTROL_DEMO_WAIT) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::setControlDemoWalk(const float &f1, const float &f2) {
    if (isStatus(STATUS_72)) {
        mControlDemoTargetPos.x = f1;
        mDemoState = CONTROL_DEMO_WALK;
        mControlDemoSpeedF = std::fabs(f2);
        if (mControlDemoSpeedF > getSpeedData()[2]) {
            mControlDemoSpeedF = getSpeedData()[2];
        }
    }
}

bool daPlBase_c::isControlDemoWalk() {
    if (isStatus(STATUS_72)) {
        if (mDemoState == CONTROL_DEMO_WALK) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::setControlDemoAnm(int anmNo) {
    if (isStatus(STATUS_72)) {
        mDemoState = CONTROL_DEMO_REGULAR_ANIM;
        changeState(StateID_AnimePlay, DEMO_ANIME_NORMAL);
        mpMdlMng->setAnm(anmNo);
    }
}
bool daPlBase_c::isControlDemoAnm(int anmNo) {
    if (isStatus(STATUS_72) && mDemoState == CONTROL_DEMO_REGULAR_ANIM && anmNo == mpMdlMng->mpMdl->mCurrAnmID) {
        return true;
    }
    return false;
}

void daPlBase_c::setControlDemoCutscene(AnimePlayArg_e animID) {
    if (isStatus(STATUS_72)) {
        mDemoState = CONTROL_DEMO_CUTSCENE_ANIM;
        changeState(StateID_AnimePlay, animID);
    }
}

void daPlBase_c::setControlDemoKinopioWalk() {
    if (isDemoState(StateID_DemoNone) || (isDemoType(DEMO_KINOPIO) && mDemoState != CONTROL_DEMO_KINOPIO_WALK)) {
        changeDemoState(StateID_DemoControl, CONTROL_DEMO_KINOPIO_WALK);
    }
}

void daPlBase_c::setControlDemoKinopioSwim() {
    if (isDemoState(StateID_DemoNone) || (isDemoType(DEMO_KINOPIO) && mDemoState != CONTROL_DEMO_KINOPIO_SWIM)) {
        changeDemoState(StateID_DemoControl, CONTROL_DEMO_KINOPIO_SWIM);
    }
}

void daPlBase_c::setControlDemoEndingDance() {
    if (!isControlDemoAll()) {
        changeDemoState(StateID_DemoControl, CONTROL_DEMO_ENDING_DANCE);
    }
}

bool daPlBase_c::isBossDemoLand() {
    if (!isNowBgCross(BGC_FOOT)) {
        return false;
    }
    if (isStatus(STATUS_BIG_JUMP) || isStatus(STATUS_4E) || isOnSinkSand() || mBossDemoLandTimer != 0) {
        return false;
    }
    return true;
}

bool daPlBase_c::fn_80052500(int p, float f, int i2) {
    mVec3_c tmp(
        mPos.x + f * sc_DirSpeed[p],
        mPos.y,
        mPos.z
    );
    float a = 4.0f;
    if (a < 1.2f * f) {
        a = 1.2f * f;
    }
    tmp.y = mPos.y + a;
    float y;
    if (mBc.checkGround(&tmp, &y, mLayer, mAmiLayer, -1) && std::fabs(y - mPos.y) < a) {
        if (i2 == 1) {
            tmp.y = mPos.y - 4.0f;
            float y2;
            if (dBc_c::checkWater(tmp.x, tmp.y, mLayer, &y2) &&
                mLastPosDelta.y >= 0.0f &&
                y <= y2 &&
                mPos.y <= y2 - 4.0f
            ) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool daPlBase_c::isHitWallKinopioWalk(int dir) {
    static const BgCross1_e scViewHitFlag[] = { BGC_SIDE_LIMIT_R, BGC_SIDE_LIMIT_L };
    if (checkBGCrossWall(dir) || isNowBgCross(scViewHitFlag[dir])) {
        return true;
    }
    return false;
}

bool daPlBase_c::checkKinopioWaitBG(int dir) {
    if (isHitWallKinopioWalk(dir)) {
        return true;
    }
    return !fn_80052500(dir, 10.0, 0);
}

void daPlBase_c::initializeState_DemoControl() {
    if (isStatus(STATUS_5F)) {
        offStatus(STATUS_5F);
    }
    mKey.onStatus(dAcPyKey_c::STATUS_DEMO);
    onStatus(STATUS_72);
    offStatus(STATUS_71);
    mIsDemoMode = false;
    initializeDemoControl();
    mDemoState = mDemoStateArg;
    switch (mDemoState) {
        case CONTROL_DEMO_4:
            mDemoSubstateTimer = 60;
            onStatus(STATUS_7A);
            break;
        case CONTROL_DEMO_KINOPIO_WALK:
            mControlDemoTargetPos.x = mPos.x;
            mControlDemoSpeedF = 0.9f;
            mItemKinopioDirection = mDirection;
            mItemKinopioTurnTimer = 150;
            if (!fn_80052500(mDirection, 8.0f, 1)) {
                mItemKinopioDirection ^= 1;
            }
            break;
        case CONTROL_DEMO_KINOPIO_SWIM:
            if (mSpeedF > 0.0f) {
                mItemKinopioDirection = 0;
            } else {
                mItemKinopioDirection = 1;
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::finalizeState_DemoControl() {
    mKey.offStatus(dAcPyKey_c::STATUS_DEMO);
    offStatus(STATUS_72);
    offStatus(STATUS_74);
    offStatus(STATUS_63);
    offStatus(STATUS_73);
    if (mDemoState == CONTROL_DEMO_4) {
        offStatus(STATUS_7A);
    }
}

void daPlBase_c::executeState_DemoControl() {
    offStatus(STATUS_74);
    if (isStatus(STATUS_73)) {
        if (isNowBgCross(BGC_FOOT)) {
            offStatus(STATUS_73);
        } else {
            mSpeedF *= 0.98f;
        }
    }

    switch ((ControlDemoSubstate_e) mDemoState) {
        case CONTROL_DEMO_WALK: {
            onStatus(STATUS_74);
            if (isNowBgCross(BGC_FOOT)) {
                if (!isState(StateID_Walk) && !isState(StateID_Turn)) {
                    changeState(StateID_Walk, BLEND_DEFAULT);
                }
            } else {
                if (!isState(StateID_Fall)) {
                    changeState(StateID_Fall, false);
                }
            }
            if (std::fabs(mPos.x - mControlDemoTargetPos.x) < mControlDemoSpeedF) {
                mDemoState = CONTROL_DEMO_WAIT;
                mSpeedF = 0.0f;
                mPos.x = mControlDemoTargetPos.x;
                break;
            }
            if (mPos.x < mControlDemoTargetPos.x) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                mSpeedF = mControlDemoSpeedF;
                break;
            }
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
            mSpeedF = -mControlDemoSpeedF;
                break;
        }
        case CONTROL_DEMO_4: {
            if (mDemoSubstateTimer == 0) {
                changeDemoState(StateID_DemoNone, false);
            }
            break;
        }
        case CONTROL_DEMO_KINOPIO_WALK: {
            if (
                !isNowBgCross(BGC_FOOT) ||
                !mStateMgr.getStateID()->isEqual(StateID_Walk) && !mStateMgr.getStateID()->isEqual(StateID_Turn)
            ) {
                mDemoState = CONTROL_DEMO_WAIT;
                break;
            }
            onStatus(STATUS_74);
            if (isOnSinkSand() || (checkKinopioWaitBG(0) && checkKinopioWaitBG(1))) {
                mDemoState = CONTROL_DEMO_KINOPIO_SINK_SAND;
                mControlDemoSpeedF = 0.0f;
                mSpeedF = 0.0f;
                break;
            }
            int prevDirection = (int) (short) mItemKinopioDirection;
            sLib::calcTimer(&mItemKinopioTurnTimer);
            if (isHitWallKinopioWalk(mItemKinopioDirection) || mItemKinopioTurnTimer == 0) {
                mItemKinopioDirection ^= 1;
                mControlDemoTargetPos.x = mPos.x + sc_DirSpeed[mItemKinopioDirection] * 24.0f;
            } else if (!fn_80052500(mItemKinopioDirection, 4.0f, 1) && !fn_80052500(mItemKinopioDirection, 8.0f, 1)) {
                mItemKinopioDirection ^= 1;
                mControlDemoTargetPos.x = mPos.x + sc_DirSpeed[mItemKinopioDirection] * 24.0f;
            }
            float tmp = mControlDemoTargetPos.x + sc_DirSpeed[mItemKinopioDirection] * 24.0f;
            if (mPos.x < tmp) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                mSpeedF = mControlDemoSpeedF;
                mItemKinopioDirection = 0;
            } else {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
                mItemKinopioDirection = 1;
                mSpeedF = -mControlDemoSpeedF;
            }
            if (prevDirection != mItemKinopioDirection) {
                mItemKinopioTurnTimer = 180;
            }
            break;
        }
        case CONTROL_DEMO_KINOPIO_SWIM: {
            if (!mStateMgr.getStateID()->isEqual(StateID_Swim)) {
                mDemoState = CONTROL_DEMO_WAIT;
                break;
            }
            onStatus(STATUS_74);
            if (isNowBgCross(BGC_FOOT) && isHitWallKinopioWalk(mItemKinopioDirection)) {
                mItemKinopioDirection ^= 1;
            }
            if (mItemKinopioDirection == 0) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                sLib::chase(&mSpeedF, 0.5625f, 0.1f);
            } else {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
                sLib::chase(&mSpeedF, -0.5625f, 0.1f);
            }
            break;
        }
        case CONTROL_DEMO_KINOPIO_SINK_SAND: {
            if (
                !isNowBgCross(BGC_FOOT) ||
                !mStateMgr.getStateID()->isEqual(StateID_Walk) && !mStateMgr.getStateID()->isEqual(StateID_Turn)
            ) {
                mDemoState = CONTROL_DEMO_WAIT;
                break;
            }
            onStatus(STATUS_74);
            onStatus(STATUS_63);
            if (!checkKinopioWaitBG(0) || !checkKinopioWaitBG(1)) {
                setControlDemoKinopioWalk();
            }
            break;
        }
        default:
            break;
    }
}

void daPlBase_c::setDemoOutNextGotoBlock(int nextGotoID, int delay, int fadeType) {
    if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoNextGotoBlock)) {
        return;
    }
    changeDemoState(StateID_DemoNextGotoBlock, nextGotoID | ((fadeType & 0xFF) << 8));
    mDemoSubstateTimer = delay;
    switch (fadeType) {
        case 3:
            startSound(SE_PLY_DOKAN_IN_OUT, false);
            break;
        case 1:
            mIsDemoMode = false;
            if (!isStatus(STATUS_PENGUIN_SLIDE)) {
                changeState(StateID_Fall, false);
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::initializeState_DemoNextGotoBlock() {
    mKey.onStatus(dAcPyKey_c::STATUS_DEMO);
    onStatus(STATUS_DEMO_NEXT_GOTO_BLOCK);
    onStatus(STATUS_7A);
    initializeDemoControl();
}

void daPlBase_c::finalizeState_DemoNextGotoBlock() {
    mKey.offStatus(dAcPyKey_c::STATUS_DEMO);
    offStatus(STATUS_DEMO_NEXT_GOTO_BLOCK);
    offStatus(STATUS_7A);
}

void daPlBase_c::executeState_DemoNextGotoBlock() {
    switch (mDemoSubstate) {
        case 0:
            if (mDemoSubstateTimer == 0) {
                dFader_c::fader_type_e f = dFader_c::FADER_DRIP_DOWN;
                int param = (int) mDemoStateArg;
                int nextGotoID = param & 0xff;
                if (((param >> 8) & 0xff) == 3) {
                    f = dFader_c::FADER_CIRCLE_TARGET;
                }
                dNext_c::m_instance->setChangeSceneNextDat(dScStage_c::m_instance->mCurrFile, nextGotoID, f);
                changeNextScene(0);
                mDemoSubstate = 1;
            }
            break;
        case 1:
            break;
    }
}

void daPlBase_c::updateEndingDance() {
    offStatus(STATUS_ENDING_DANCE_AUTO);
    if (!dScStage_c::m_isStaffCredit || isDemoType(DEMO_PLAYER)) {
        return;
    }
    int isActive = 0;
    if (dGameKey_c::m_instance->checkButtonsDown(mPlayerNo) || dGameKey_c::m_instance->checkShaking(mPlayerNo)) {
        isActive = 1;
    }
    if (!isDemoType(DEMO_ENDING_DANCE)) {
        if (isActive == 1) {
            mEndingDanceInactivityTimer = 0;
        } else {
            mEndingDanceInactivityTimer++;
            if (mEndingDanceInactivityTimer >= 180) {
                setControlDemoEndingDance();
                mEndingDanceInactivityTimer = 0;
            }
        }
    } else if (isActive == 1) {
        endControlDemo(0);
    } else if (isStatus(STATUS_45)) {
        for (int i = 0; i < ARRAY_SIZE(mEndingDanceKeyTimers); i++) {
            mEndingDanceKeyTimers[i] = 0;
        }
    } else {
        onStatus(STATUS_ENDING_DANCE_AUTO);
        if (dAudio::isBgmAccentSign(2)) {
            mDirection = 0;
        } else if (dAudio::isBgmAccentSign(4)) {
            mDirection = 1;
        } else if (dAudio::isBgmAccentSign(8)) {
            mKey.onDemoButton(dAcPyKey_c::BUTTON_DOWN);
            mEndingDanceKeyTimers[2] = 6;
        } else if (dAudio::isBgmAccentSign(16)) {
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_TWO);
            mKey.onDemoButton(dAcPyKey_c::BUTTON_TWO);
            mEndingDanceKeyTimers[3] = 5;
        } else if (dAudio::isBgmAccentSign(32)) {
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_TWO);
            mKey.onDemoButton(dAcPyKey_c::BUTTON_TWO);
            mEndingDanceKeyTimers[4] = 30;
        } else if (dAudio::isBgmAccentSign(64)) {
            mKey.onDemoShake();
        } else if (dAudio::isBgmAccentSign(128)) {
            if (dBgParameter_c::ms_Instance_p->m_48 > mPos.x) {
                mDirection = 0;
            } else {
                mDirection = 1;
            }
        }

        for (int i = 0; i < 5; i++) {
            if (mEndingDanceKeyTimers[i] != 0 && --mEndingDanceKeyTimers[i] == 0) {
                switch (i) {
                    case 2:
                        mKey.offDemoButton(dAcPyKey_c::BUTTON_DOWN);
                        break;
                    case 3:
                    case 4:
                        mKey.offDemoButton(dAcPyKey_c::BUTTON_TWO);
                        break;
                }
            }
        }
    }
}

bool daPlBase_c::setEnemyStageClearDemo() {
    if (isStatus(STATUS_ENEMY_STAGE_CLEAR)) {
        if (
            isNowBgCross(BGC_LIFT) &&
            mBc.mpCtrHead != nullptr &&
            mBc.mpCtrHead->mpActor != nullptr &&
            mBc.mpCtrHead->mpActor->mProfName == fProfile::EN_CHIKUWA_BLOCK
        ) {
            return false;
        }
        if (!isDemoType(DEMO_PLAYER)) {
            changeDemoState(StateID_DemoControl, CONTROL_DEMO_WAIT);
        }
        mSpeed.y = 0.0f;
        mSpeedF = 0.0f;
        return true;
    }
    return false;
}

int daPlBase_c::getCcLineKind() {
    if (isNowBgCross(BgCross2_e(BGC_VINE_TOUCH_L | BGC_VINE_TOUCH_R))) {
        return mAmiLayer;
    }
    return 3;
}

void daPlBase_c::initCollision(sCcDatNewF *dat1, sCcDatNewF *dat2) {
    mCc.set(this, dat1);
    mCc1.set(this, dat1);
    mAttCc1.set(this, dat2);
    mAttCc2.set(this, dat2);
    mAttCc3.set(this, dat2);
}

void daPlBase_c::releaseCcData() {
    u32 mVsKind =
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI) |
        BIT_FLAG(CC_KIND_ENEMY) |
        BIT_FLAG(CC_KIND_BALLOON) |
        BIT_FLAG(CC_KIND_ITEM) |
        BIT_FLAG(CC_KIND_TAMA) |
        BIT_FLAG(CC_KIND_KILLER) |
        BIT_FLAG(CC_KIND_GOAL_POLE);

    u32 mVsKindAttack =
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_YOSHI) |
        BIT_FLAG(CC_KIND_ENEMY) |
        BIT_FLAG(CC_KIND_BALLOON) |
        BIT_FLAG(CC_KIND_ITEM) |
        BIT_FLAG(CC_KIND_KILLER);

    u32 vsDamage =
        CC_FLAG_ATTACK_ALL &
        ~BIT_FLAG(CC_ATTACK_NONE) &
        ~BIT_FLAG(CC_ATTACK_YOSHI_MOUTH) &
        ~BIT_FLAG(CC_ATTACK_SAND_PILLAR);

    mCc.release();
    mCc.mCcData.mVsKind = mVsKind;
    mCc.mCcData.mAttack = 0;
    mCc.mCcData.mVsDamage = vsDamage;

    mCc1.release();
    mCc1.mCcData.mVsKind = mVsKind;
    mCc1.mCcData.mAttack = 0;
    mCc1.mCcData.mVsDamage = vsDamage;

    mAttCc1.release();
    mAttCc1.mCcData.mVsKind = mVsKindAttack;
    mAttCc1.mCcData.mAttack = 0;
    mAttCc1.mCcData.mVsDamage = 0;

    mAttCc2.release();
    mAttCc2.mCcData.mVsKind = mVsKindAttack;
    mAttCc2.mCcData.mAttack = 0;
    mAttCc2.mCcData.mVsDamage = 0;

    mAttCc3.release();
    mAttCc3.mCcData.mVsKind = mVsKindAttack;
    mAttCc3.mCcData.mAttack = 0;
    mAttCc3.mCcData.mVsDamage = 0;
}

void daPlBase_c::clearCcData() {
    mCc.clear();
    mCc1.clear();
    mAttCc1.clear();
    mAttCc2.clear();
    mAttCc3.clear();
}

void daPlBase_c::setCcAtBody(int attackCategory) {
    mAttCc2.mCcData.mBase.mOffset.set(mCc.mCcData.mBase.mOffset);
    mAttCc2.mCcData.mBase.mSize.set(mCc.mCcData.mBase.mSize);
    mAttCc2.mCcData.mAttack = attackCategory;
    mAttCc2.mCcData.mVsKind =
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI);

    mAttCc1.mCcData.mBase.mOffset.set(mCc.mCcData.mBase.mOffset);
    mAttCc1.mCcData.mBase.mSize.set(mCc.mCcData.mBase.mSize);
    mAttCc1.mCcData.mAttack = attackCategory;
    mAttCc1.mCcData.mVsKind = mCc.mCcData.mVsKind & ~(
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI)
    );
}

void daPlBase_c::setCcAtSlip() {
    setCcAtBody(CC_ATTACK_SLIP);
}

void daPlBase_c::setCcAtPenguinSlip() {
    if (isStatus(STATUS_PENGUIN_SLIDE)) {
        setCcAtBody(CC_ATTACK_PENGUIN_SLIDE);
        mAttCc2.mCcData.mVsDamage = BIT_FLAG(CC_ATTACK_PENGUIN_SLIDE);
        mAttCc1.mCcData.mVsDamage = BIT_FLAG(CC_ATTACK_PENGUIN_SLIDE);
    }
}

void daPlBase_c::setCcAtHipAttack() {
    setCcAtBody(CC_ATTACK_HIP_ATTACK);
    mAttCc2.mCcData.mVsDamage = 0;
    mAttCc1.mCcData.mVsDamage = 0;
}

void daPlBase_c::setCcAtStar() {
    mCc.mCcData.mAttack = CC_ATTACK_STAR;
    if ((mAttCc2.mCcData.mVsKind & (
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI)
    )) == 0) {
        mCc1.mCcData.mAttack = CC_ATTACK_STAR;
    }
    if (mAttCc1.mCcData.mAttack != CC_ATTACK_NONE &&
        mAttCc1.mCcData.mAttack != CC_ATTACK_YOSHI_EAT
    ) {
        mAttCc3.mCcData.mBase.mOffset.set(mAttCc1.mCcData.mBase.mOffset);
        mAttCc3.mCcData.mBase.mSize.set(mAttCc1.mCcData.mBase.mSize);
        mAttCc3.mCcData.mAttack = CC_ATTACK_STAR;
    }
    onStatus(STATUS_84);
}

void daPlBase_c::setCcAtCannon() {
    setCcAtBody(CC_ATTACK_CANNON);
}

void daPlBase_c::entryCollision() {
    if (!isStatus(STATUS_OUT_OF_PLAY) && !isStatus(STATUS_STUNNED)) {
        int lineKind = getCcLineKind();
        mCc.mAmiLine = lineKind;
        if (mAttCc1.mCcData.mAttack == CC_ATTACK_WIRE_NET) {
            lineKind ^= 3;
        }
        mAttCc1.mAmiLine = lineKind;
        mAttCc3.mAmiLine = lineKind;
        mCc.mLayer = mLayer;
        mAttCc1.mLayer = mLayer;
        mAttCc3.mLayer = mLayer;
        if (isStatus(STATUS_74)) {
            mCc.mCcData.mStatus |= CC_STATUS_NO_REVISION;
        } else {
            mCc.mCcData.mStatus &= ~CC_STATUS_NO_REVISION;
        }
        if (!isStatus(STATUS_7A)) {
            mCc.entry();
            if (isStatus(STATUS_78)) {
                mCc1.entry();
            }
        }
        if (mAttCc1.mCcData.mAttack != CC_ATTACK_NONE) {
            mAttCc1.entry();
        }
        if (mAttCc2.mCcData.mAttack != CC_ATTACK_NONE) {
            mAttCc2.entry();
        }
        if (mAttCc3.mCcData.mAttack != CC_ATTACK_NONE) {
            mAttCc3.entry();
        }
    }
}

bool daPlBase_c::isActionRevisionY() {
    if (isStatus(STATUS_POLE) || isStatus(STATUS_36)) {
        return true;
    }
    return false;
}

void daPlBase_c::setCcPlayerRev(dCc_c *cc1, dCc_c *cc2, float f, int idx) {
    daPlBase_c *other = (daPlBase_c *) cc2->mpOwner;
    float speedF = other->mSpeedF;
    float offsX = cc1->mCollOffsetX[idx];
    float offsY = cc1->mCollOffsetY[idx];
    if (isActionRevisionY()) {
        if (other->isActionRevisionY()) {
            if (m_1071) {
                m_1068 = offsY + m_1068;
            } else {
                m_1071 = true;
                m_1068 = offsY * f;
            }
        }
    } else if (!(std::fabs(offsY) < 1.0f || other->isActionRevisionY())) {
        if (m_1070) {
            float tmp1 = get_1064() * get_106c();
            float tmp2 = offsX * f;
            if (std::fabs(tmp1) < std::fabs(tmp2)) {
                m_1060 = speedF;
            }
            m_106c = 1.0f;
            m_1064 = tmp1 + tmp2;
        } else {
            m_106c = f;
            m_1060 = speedF;
            m_1064 = offsX;
            m_1070 = true;
        }
    }
}

void daPlBase_c::clearCcPlayerRev() {
    m_106c = 0.0f;
    m_1060 = 0.0f;
    m_1064 = 0.0f;
    m_1068 = 0.0f;
    m_1070 = false;
    m_1071 = false;
}

bool daPlBase_c::calcCcPlayerRev(float *f) {
    if (m_1070) {
        float tmp = m_106c;
        if (isStatus(STATUS_IS_SPIN_HOLD_REQ) || m_1072 != 0) {
            tmp = 0.0f;
        }
        if (isDemoType(DEMO_PLAYER) && isNowBgCross(BGC_FOOT)) {
            tmp = 0.0f;
        }
        float prev_1064 = m_1064;
        m_1064 = 0.0f;
        if (prev_1064) {
            float m = prev_1064 * tmp;
            float tmp2 = 3.0f;
            if (m > 3.0f) {
                m = 3.0f;
            } else if (m < -3.0f) {
                m = -3.0f;
            }
            if (mSpeedF * prev_1064 <= 0.0f) {
                *f = m;
                if (isDemoType(DEMO_PLAYER) || isStatus(STATUS_5F)) {
                    return false;
                }
                float f1 = m_1060;
                float f2 = mSpeedF;
                if (f2 * f1 <= 0.0f && std::fabs(mSpeedF) + std::fabs(m_1060) > 2.5f) {
                    mSpeedF = m_1060 * 0.4f;
                    return true;
                }
                if (std::fabs(mSpeedF) > std::fabs(m_1060)) {
                    if (std::fabs(mSpeedF) > 1.5f) {
                    mSpeedF = 0.0f;
                    return true;
                    }
                }
            }
        }
    }
    return false;
}

bool daPlBase_c::isEnableStampPlayerJump(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *other = (daPlBase_c *) cc2->mpOwner;
    if (isNowBgCross(BGC_FOOT)) {
        return false;
    }
    if (mSpeed.y >= 0.0f) {
        return false;
    }
    if (isStatus(STATUS_HIP_ATTACK_FALL) || isStatus(STATUS_SPIN_HIP_ATTACK_FALL)) {
        return false;
    }
    if (other->isLiftUp() || other->mSquishCooldownTimer != 0) {
        return false;
    }
    if (isDemoType(DEMO_PLAYER) || other->isDemoType(DEMO_PLAYER)) {
        return false;
    }
    float topPos = cc2->getTopPos() - 16.0f;
    if (topPos < cc2->getCenterPosY()) {
        topPos = cc2->getCenterPosY();
    }
    if (cc1->getUnderPos() > topPos) {
        return true;
    }
    return false;
}

void daPlBase_c::setStampReduction() {
    if (!isStatus(STATUS_SWIM)) {
        if (mSpeed.y > 0.0f) {
            mSpeed.y = 0.0f;
        }
        setReductionScale();
    } else {
        mSpeed.y = -1.0f;
    }
}

void daPlBase_c::setStampPlayerJump(bool b, float f) {
    if (!isStatus(STATUS_SWIM)) {
        float scale = daPlBase_c::sc_JumpSpeed;
        if (isMameAction()) {
            scale = daPlBase_c::sc_JumpSpeed - 0.35f;
        }
        if (b) {
            dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_7, 0, false);
            if (mKey.buttonJump()) {
                scale = daPlBase_c::sc_JumpSpeed + 0.5f;
            }
            setJump(scale, mSpeedF, true, 1, 0);
        } else {
            setJump(scale, mSpeedF, true, 0, 0);
        }
        mPos.y += f;
    } else {
        mSpeed.y = 1.0f;
    }
}

void daPlBase_c::setReductionScale() {
    setReductionBoyon();
    initStampReduction();
}

void daPlBase_c::initStampReduction() {
    if (!isStatus(STATUS_JUMP) || mSquishKeyframeIdx == 0) {
        mSquishNoMoveTimer = 4;
    }
    mSquishCooldownTimer = 10;
    startPlayerVoice(VOICE_STOMPED, 0);
    dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_7, 0, false);
}

void daPlBase_c::calcJumpDaiReductionScale(int i1, int i2) {
    if (i1 > i2) {
        i1 = i2;
    }
    mSquishScale = i1 * 0.6f / i2;
    mSquishState = SQUISH_INIT;
}

void daPlBase_c::setReductionBoyon() {
    mSquishState = SQUISH_ANIMATION;
    mSquishKeyframeIdx = 4;
}

void daPlBase_c::calcReductionScale() {
    switch (mSquishState) {
        case SQUISH_OFF:
            break;
        case SQUISH_INIT:
            mSquishState = SQUISH_SET_REDUCTION;
            break;
        case SQUISH_SET_REDUCTION:
            setReductionBoyon();
            break;
        case SQUISH_ANIMATION:
            if (mSquishKeyframeIdx) {
                int idx = mSquishKeyframeIdx - 1;
                static const float scStampScaleDt[] = { 0.0f, 0.2f, 0.0f, 0.4f };
                static const float scStampAcceleDt[] = { 0.02f, 0.025f, 0.04f, 0.06f };
                if (sLib::chase(&mSquishScale, scStampScaleDt[idx], scStampAcceleDt[idx])) {
                    mSquishKeyframeIdx--;
                }
            }
            if (mSquishKeyframeIdx == 0) {
                mSquishState = SQUISH_OFF;
            }
            break;
    }
}

mVec3_c daPlBase_c::getReductionModelScale() {
    return mVec3_c(
        1.0f + mSquishScale,
        1.0f - mSquishScale,
        1.0f + mSquishScale
    );
}

void daPlBase_c::setNoHitPlayer(const daPlBase_c *player, int duration) {
    mpNoHitPlayer = player;
    mNoHitTimer = duration;
}

void daPlBase_c::updateNoHitPlayer() {
    if (mNoHitTimer != 0) {
        mNoHitTimer--;
    }
    if (mNoHitTimer == 0) {
        mpNoHitPlayer = nullptr;
    }
}

daPlBase_c *daPlBase_c::getHipAttackDamagePlayer() {
    return (daPlBase_c *) fManager_c::searchBaseByID(mHipAttackPlayerID);
}

void daPlBase_c::setHipAttackDamagePlayer(daPlBase_c *player) {
    onStatus(STATUS_HIP_ATTACK_DAMAGE_PLAYER);
    mHipAttackPlayerID = player->mUniqueID;
}

void daPlBase_c::clearHipAttackDamagePlayer() {
    offStatus(STATUS_HIP_ATTACK_DAMAGE_PLAYER);
    mHipAttackPlayerID = (fBaseID_e) 0;
}

void daPlBase_c::setNoHitObjBg(dActor_c *obj, int duration) {
    mBc.mpNoHitActor = obj;
    mNoHitObjTimer = duration;
}

void daPlBase_c::calcNoHitObjBgTimer() {
    if (mNoHitObjTimer == 0) {
        return;
    }
    if (--mNoHitObjTimer == 0) {
        mBc.mpNoHitActor = nullptr;
    }
}

void daPlBase_c::setOldBGCross() {
    mOldBgCross1 = mNowBgCross1;
    mOldBgCross2 = mNowBgCross2;
    for (int i = 9; i > 0; i--) {
        mBgFootHistory[i] = mBgFootHistory[i - 1];
    }
    mBgFootHistory[0] = isNowBgCross(BGC_FOOT) ? 1 : 0;
}

void daPlBase_c::clearBgCheckInfo() {
    setOldBGCross();
    clearNowBgCross();
    mPrevStandOnUnitType = mStandOnUnitType;
    mStandOnUnitType = 0;
    mBgPushForce.set(0.0f, 0.0f, 0.0f);
    mExtraPushForceX = 0.0f;
    mGroundType = GROUND_TYPE_DEFAULT;
    mWaterDepth = 0;
    m_d80[0] = 0.0f;
    m_d80[1] = 0.0f;
}

void daPlBase_c::bgCheck(int i) {
    offStatus(STATUS_87);
    offStatus(STATUS_86);
    offStatus(STATUS_IS_SPIN_HOLD_REQ);
    offStatus(STATUS_5D);
    if (isNowBgCross(BGC_FOOT)) {
        m_d8c = mPos.y;
    }
    clearBgCheckInfo();
    sLib::calcTimer(&mAirWalkTimer);
    if (i == 1) {
        checkSideViewLemit();
    }
    if (!isStatus(STATUS_7E)) {
        checkBgCross();
        checkBgCrossSub();
    }
    checkWater();
    if (!isStatus(STATUS_7E)) {
        checkDamageBg();
    }
    postBgCross();
}

void daPlBase_c::checkBgCross() {
    if (mPowerup == POWERUP_MINI_MUSHROOM && (isOldBgCross(BGC_WALL_TOUCH_L_2) | isOldBgCross(BGC_WALL_TOUCH_R_2)) == 0) {
        float dir = 1.0f;
        if (mLastPos.x > mPos.x) {
            dir = -1.0f;
        }
        const sBcPointData *wallData = getWallBgPointData();
        int wallSize = wallData->mInfMargin + wallData->mSupMargin;
        float offset = ((float) wallSize / 4096.0f) / 2.0f;
        mVec3_c p1(
            mLastPos.x - dir * 3.0f,
            mLastPos.y + offset,
            mLastPos.z
        );
        mVec3_c p2(
            mPos.x + dir * 3.0f,
            mPos.y + offset,
            mPos.z
        );
        float outX;
        if (mBc.checkWallPlayer(&p1, &p2, &outX) && outX != p1.x) {
            mPos.x = outX - dir * 3.0f;
        }
    }

    onStatus(STATUS_80);
    u32 bgFlags = mBc.checkBgPlr(this);
    offStatus(STATUS_80);

    if (isStatus(STATUS_7F)) {
        return;
    }
    mPrevMoveSakaAngle = mMoveSakaAngle;
    mPrevStillSakaAngle = mStillSakaAngle;
    mStillSakaAngle = 0;
    mMoveSakaAngle = 0;
    mStandOnUnitType = mBc.mLastUnitType;

    if ((bgFlags & 0x3c000000) != 0) {
        onNowBgCross(BGC_HEAD);
        if (bgFlags & 0x4000000) {
            onNowBgCross(BGC_PRESS_HEAD_HIT);
        }
        if (bgFlags & 0x40000000) {
            onNowBgCross(BGC_63);
        }
        if (bgFlags & 0x80000000) {
            onNowBgCross(BGC_NON_BREAK_BLOCK_HIT);
        }
        if (mBc.getHeadAttr() == 6) {
            onNowBgCross(BGC_HANG_ROPE);
        }
        if (bgFlags & 0x20000000) {
            onNowBgCross(BGC_BLOCK_HIT);
            switch (mBc.m_c4) {
                case 1:
                    onNowBgCross(BGC_58);
                    break;
                case 2:
                    onNowBgCross(BGC_57);
                    break;
            }
            if (mBc.mpCtrFoot != nullptr &&
                mBc.mpCtrFoot->mpActor != nullptr &&
                !(
                    mBc.mpCtrFoot->mpActor->mProfName != fProfile::EN_HATENA_BLOCK_LINE &&
                    mBc.mpCtrFoot->mpActor->mProfName != fProfile::EN_RENGA_BLOCK_LINE
                )
            ) {
                onNowBgCross(BGC_LINE_BLOCK_HIT);
            }
        }
    }

    if (bgFlags & 0x1fe000) {
        mMoveSakaAngle = mBc.getSakaAngleBySpeed(mSpeedF);
        mStillSakaAngle = mBc.getSakaAngle(0);
        if (bgFlags & 0x4000) {
            onNowBgCross(BGC_SLOPE);
        }
        if (bgFlags & 0x400000) {
            onNowBgCross(BGC_62);
        }
        if (bgFlags & 0x800000) {
            onNowBgCross(BGC_54);
        }
        if (bgFlags & 0x1000000) {
            onNowBgCross(BGC_36);
        }
        if (mMoveSakaAngle > 0 && isNowBgCross(BGC_HEAD)) {
            onNowBgCross(BGC_SLOPE_AND_HEAD);
            mSpeedF = 0.0f;
        }
        u16 footAttr = mBc.getFootAttr();
        switch (footAttr) {
            case 3:
                onNowBgCross(BGC_ON_SINK_SAND);
                break;
            case 7:
                onNowBgCross(BGC_AUTOSLIP);
                break;
            case 4:
                onNowBgCross(BGC_ON_BELT_R);
                break;
            case 5:
                onNowBgCross(BGC_ON_BELT_L);
                break;
        }
        if (mSpeed.y <= 0.0f) {
            onNowBgCross(BGC_FOOT);
            if (bgFlags & 0x200000) {
                onNowBgCross(BGC_SEMISOLID);
            }
            if (bgFlags & 0x18000) {
                onNowBgCross(BGC_LIFT);
            }
            if (mPos.y > mLastPos.y && !isNowBgCross(BGC_SLOPE)) {
                onNowBgCross(BGC_GROUNDED_MOVE_UP);
            }
            switch (footAttr) {
                case 2:
                    mGroundType = GROUND_TYPE_SNOW;
                    onNowBgCross(BGC_ON_SNOW);
                    break;
                case 3:
                case 12:
                    onNowBgCross(BGC_ON_SAND);
                    if (isNowBgCross(BGC_LIFT)) {
                        mGroundType = GROUND_TYPE_FUNSUI;
                    } else {
                        mGroundType = GROUND_TYPE_SAND;
                    }
                    break;
                case 15:
                    onNowBgCross(BGC_ON_SAND);
                    mGroundType = GROUND_TYPE_BEACH;
                    break;
                case 13:
                    mGroundType = GROUND_TYPE_DIRT;
                    break;
                case 1:
                    mGroundType = GROUND_TYPE_ICE;
                    if (bgFlags & 0x2000000) {
                        onNowBgCross(BGC_ON_ICE_LOW_SLIP);
                    } else {
                        onNowBgCross(BGC_ON_ICE);
                    }
                    break;
                case 8:
                    if (isStatus(STATUS_PENGUIN_SLIDE)) {
                        mVec3_c tmp(
                            mPos.x,
                            mPos.y + 4.0f,
                            mPos.z
                        );
                        float groundHeight;
                        if (dBc_c::checkGround(&tmp, &groundHeight, mLayer, mAmiLayer, 8)) {
                            if (std::fabs(groundHeight - mPos.y) < 2.0f) {
                                onNowBgCross(BGC_CLIFF);
                            }
                        }
                    } else {
                        onNowBgCross(BGC_CLIFF);
                    }
                    break;
                case 14:
                    mGroundType = GROUND_TYPE_CLOUD;
                    break;
                case 11:
                    mGroundType = GROUND_TYPE_CARPET;
                    break;
            }
            if (isNowBgCross(BGC_LIFT)) {
                if (bgFlags & 0x40000) {
                    mGroundType = GROUND_TYPE_MANTA;
                }
                if (bgFlags & 0x80000) {
                    mGroundType = GROUND_TYPE_LEAF;
                }
                if (bgFlags & 0x100000) {
                    mGroundType = GROUND_TYPE_WOOD;
                }
            }
        }
    } else {
        if (isStatus(STATUS_RIDE_NUT_2)) {
            mPos.y = mRideNutHeight;
            mMoveSakaAngle = 0;
            onNowBgCross(BGC_FOOT);
        }
        float s = getModelHeight() + mPos.y;
        mVec3_c checkPos(mPos.x, s + 8.0f, mPos.z);
        float groundHeight;
        int groundType;
        if (dBc_c::checkGround(&checkPos, &groundHeight, &groundType, mLayer, mAmiLayer, 8)) {
            if (checkPos.y >= groundHeight && s <= groundHeight) {
                if (groundType == 2) {
                    onNowBgCross(BGC_CLIFF_ABOVE_2);
                } else {
                    onNowBgCross(BGC_CLIFF_ABOVE_1);
                }
                mKaniHeight = groundHeight;
            }
        }
    }

    if (mSpeed.y < 0.0f && (isStatus(STATUS_BIG_JUMP) || isStatus(STATUS_4E))) {
        onNowBgCross(BGC_LIFT);
        onNowBgCross(BGC_FOOT);
        onNowBgCross(BGC_37);
    }

    if (bgFlags & 0x1fe000) {
        onNowBgCross(BGC_37);
    } else {
        float groundY3;
        if (dBc_c::checkGroundHalf(&mPos, &groundY3, mLayer, mAmiLayer) && (mPos.y - 16.0f < groundY3)) {
            onNowBgCross(BGC_37);
        }
    }

    if (bgFlags & 0x100) {
        onNowBgCross(BGC_61);
    }
    if ((bgFlags & 8) || isStatus(STATUS_1A)) {
        onNowBgCross(BGC_WALL_TOUCH_L_2);
    }
    if ((bgFlags & 4) || isStatus(STATUS_1B)) {
        onNowBgCross(BGC_WALL_TOUCH_R_2);
    }

    if ((isNowBgCross(BGC_WALL_TOUCH_L_2) | isNowBgCross(BGC_WALL_TOUCH_R_2)) != 0) {
        float sx = mSpeed.x;
        mAdjacentSlopeAngle = mBc.mAdjacentSlopeAngle;
        if (sx < 0.0f) {
            if (isNowBgCross(BGC_WALL_TOUCH_L_2)) {
                onNowBgCross(BGC_WALL);
            }
        } else if (sx > 0.0f) {
            if (isNowBgCross(BGC_WALL_TOUCH_R_2)) {
                onNowBgCross(BGC_WALL);
            }
        } else {
            static const int flags[] = { 0x1, 0x2 };
            if (bgFlags & flags[mDirection]) {
                onNowBgCross(BGC_WALL);
            }
        }
    }

    if (bgFlags & 0xc0) {
        dBg_ctr_c *ctrWall = mBc.mpCtrWall;
        if (ctrWall) {
            if (ctrWall->m_d0 & 0x800000) {
                onNowBgCross(BGC_11);
            }
            if (ctrWall->m_c8 == 0 && *ctrWall->mRotation != 0) {
                onNowBgCross(BGC_11);
            }
        }
        if (bgFlags & 0x80) {
            onNowBgCross(BGC_OBJBG_TOUCH_L);
            if (isCarryObjBgCarried(1)) {
                onNowBgCross(BGC_OBJBG_TOUCH_CARRIED_L);
            }
        }
        if (bgFlags & 0x40) {
            onNowBgCross(BGC_OBJBG_TOUCH_R);
            if (isCarryObjBgCarried(0)) {
                onNowBgCross(BGC_OBJBG_TOUCH_CARRIED_R);
            }
        }
    }

    if (bgFlags & 0x2a) {
        onNowBgCross(BGC_WALL_TOUCH_L);
    }
    if (bgFlags & 0x15) {
        onNowBgCross(BGC_WALL_TOUCH_R);
    }
    if (bgFlags & 0x800) {
        onNowBgCross(BGC_12);
    }
    if (bgFlags & 0x1000) {
        onNowBgCross(BGC_13);
    }

    if (isNowBgCross(BGC_ON_SINK_SAND)) {
        checkSinkSand();
    }

    if (!isNowBgCross(BGC_FOOT)) {
        if (mTopHeight < mPos.y) {
            mTopHeight = mPos.y;
        }
        mAirTopHeight = mTopHeight;
    } else {
        mTopHeight = mPos.y;
    }

    if (isNowBgCross(BGC_HEAD) && isNowBgCross(BGC_PRESS_HEAD_HIT) && !isNowBgCross(BGC_63)) {
        fn_80056370(nullptr, BG_PRESS_HEAD);
    }
    if (isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_LIFT)) {
        fn_80056370(nullptr, BG_PRESS_FOOT);
    }
    if (isNowBgCross(BGC_WALL_TOUCH_L_2) && !isNowBgCross(BGC_OBJBG_TOUCH_L)) {
        fn_80056370(nullptr, BG_PRESS_L);
    }
    if (isNowBgCross(BGC_WALL_TOUCH_R_2) && !isNowBgCross(BGC_OBJBG_TOUCH_R)) {
        fn_80056370(nullptr, BG_PRESS_R);
    }
}

bool daPlBase_c::isCarryObjBgCarried(u8 i) {
    dBg_ctr_c *ctrWall = mBc.mpCtrWalls[i];
    if (ctrWall != nullptr) {
        mVec2_c diff = ctrWall->m_a0 - ctrWall->m_ac;
        set_m_d80(i, diff.x);
        if (ctrWall->m_d0 & 0x800) {
            dActor_c* carriedActor = ctrWall->mpActor;
            if (carriedActor != nullptr && carriedActor->checkCarried(nullptr)) {
                return true;
            }
        }
    }

    return false;
}

void daPlBase_c::postBgCross() {
    if (isNowBgCross(BGC_HEAD)) {
        if (mSpeed.y > 0.0f || isStatus(STATUS_4E)) {
            if (
                !isOldBgCross(BGC_HEAD) &&
                !isNowBgCross(BGC_57) &&
                !isNowBgCross(BGC_63) &&
                !isNowBgCross(BGC_HANG_ROPE)
            ) {
                bool m = false;
                if (mPowerup == POWERUP_MINI_MUSHROOM) {
                    m = true;
                }
                if (isNowBgCross(BGC_NON_BREAK_BLOCK_HIT)) {
                    startSound(SE_PLY_HIT_BLOCK_BOUND, m, false);
                } else if (isNowBgCross(BGC_BLOCK_HIT)) {
                    if (isNowBgCross(BGC_58)) {
                        startSound(SE_PLY_HIT_BLOCK, m, false);
                    } else {
                        startSound(SE_PLY_HIT_GENERAL_OBJ, m, false);
                    }
                } else {
                    startSound(SE_PLY_HIT_BLOCK, m, false);
                }
            }
        }
        if (mSpeed.y > 0.0f) {
            mSpeed.y = 0.0f;
            mNoGravityTimer = 0;
            onStatus(STATUS_BF);
        }
    }
    if (isNowBgCross(BGC_FOOT)) {
        if (isNowBgCross(BGC_ON_BELT_L) | isNowBgCross(BGC_ON_BELT_R)) {
            mBgPushForce = mBc.mPushForce;
        }
        if (isNowBgCross(BGC_WATER_SHALLOW) || isNowBgCross(BGC_HANG_ROPE)) {
            mSpeed.y = 0.0f;
        } else if (isNowBgCross(BGC_ON_SINK_SAND)) {
            mSpeed.y = 0.0f;
            mBgPushForce.set(0.0f, mBc.mPushForce.y * getSandSinkRate(), 0.0f);
        } else if (isSaka() || isStatus(STATUS_30)) {
            mSpeed.y = 0.0f;
        } else {
            mSpeed.y = -2.0f;
        }
        if (isStatus(STATUS_5F)) {
            mSpeedF = 0.0f;
        }
        if (isNowBgCross(BGC_WATER_TOUCH) && !isNowBgCross(BGC_WATER_SHALLOW)) {
            mGroundType = GROUND_TYPE_WATER;
        }
        if (!isNowBgCross(BGC_LIFT) && isNowBgCross(BGC_HEAD)) {
            if (mBc.getSakaType() != 0 && mBc.getSakaUpDown(mDirection) == 1) {
                if (mDirection == DIR_LR_R) {
                    onStatus(STATUS_1B);
                } else {
                    onStatus(STATUS_1A);
                }
                mSpeedF = 0.0f;
                mPos.x = mLastPos.x;
            }
        }
    }
    if (!isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_LIFT)) {
        if (isOldBgCross(BGC_FOOT) && isOldBgCross(BGC_LIFT)) {
            m_1134 = 0.0f;
            if (std::fabs(mSpeedF) >= 0.01f && m_1eb.x * mSpeedF > 0.0f) {
                m_1134 = m_1eb.x;
                float add = m_1eb.x;
                if (std::fabs(add) > 0.01f && std::fabs(add) < 1.0f) {
                    if (add > 1.0f) {
                        add = 1.0f;
                    } else {
                        add = -1.0f;
                    }
                }
                mPos.x += add;
            }
        }
    }
}

float daPlBase_c::getWaterCheckPosY() {
    static const float scWaterOffsetY[] = { 4.0f, 8.0f, 16.0f };
    return mPos.y + scWaterOffsetY[getTallType(-1)];
}

void daPlBase_c::checkWater() {
    mWaterType = dBc_c::WATER_CHECK_NONE;
    mPrevWaterHeight = mWaterHeight;
    mWaterHeight = dBg_c::m_bg_p->m_8fe00;
    u8 waterCheck = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &mWaterHeight);
    if (waterCheck != dBc_c::WATER_CHECK_NONE && mPos.y <= mWaterHeight) {
        onNowBgCross(BGC_WATER_TOUCH);
        if (waterCheck == dBc_c::WATER_CHECK_WATER_BUBBLE) {
            onNowBgCross(BGC_WATER_BUBBLE);
        }
    }
    if (waterCheck == dBc_c::WATER_CHECK_NONE || waterCheck == dBc_c::WATER_CHECK_WATER_BUBBLE) {
        waterCheck = dBc_c::checkWater(mPos.x, getWaterCheckPosY(), mLayer, &mWaterHeight);
        if (waterCheck != dBc_c::WATER_CHECK_WATER_BUBBLE) {
            return;
        }
        onNowBgCross(BGC_WATER_SHALLOW);
        onNowBgCross(BGC_WATER_SUBMERGED);
    }
    switch (waterCheck) {
        case dBc_c::WATER_CHECK_WATER_BUBBLE: {
            onNowBgCross(BGC_WATER_BUBBLE);
            mVec2_c pos;
            dBc_c::getAirWaterHitPos(&pos);
            mAirWaterHitPos.set(pos.x, pos.y, mPos.z);
            short s;
            dBc_c::getAirWaterHitAngle(&s);
            mAirWaterHitAngle = s;
            break;
        }
        case dBc_c::WATER_CHECK_WATER: {
            if (getWaterCheckPosY() <= mWaterHeight) {
                onNowBgCross(BGC_WATER_SHALLOW);
            }
            const sBcPointData *p = getHeadBgPointData();
            float f = p->mOffset;
            if (mPos.y + f / 4096.0f <= mWaterHeight) {
                onNowBgCross(BGC_WATER_SUBMERGED);
            }
            mWaterDepth = dBc_c::checkWaterDepth(mPos.x, mWaterHeight, mLayer, mAmiLayer, nullptr);
            break;
        }
        case dBc_c::WATER_CHECK_YOGAN:
        case dBc_c::WATER_CHECK_POISON:
            if (!isStatus(STATUS_7E)) {
                mWaterType = waterCheck;
            }
            break;
    }
}

void daPlBase_c::checkDamageBg() {
    if (isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_53) ||
        isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE)
    ) {
        return;
    }
    if (!isDemoAll() || isDemoType(DEMO_KINOPIO)) {
        mIsBgDamage = mBc.m_e0;
        mBgDamageType = mBc.m_e1;
        switch (mWaterType) {
            case dBc_c::WATER_CHECK_YOGAN:
                mIsBgDamage = 1;
                mBgDamageType = 7;
                break;
            case dBc_c::WATER_CHECK_POISON:
                mIsBgDamage = 1;
                mBgDamageType = 8;
                break;
        }
    }
}

bool daPlBase_c::setBgDamage() {
    if (mIsBgDamage) {
        mIsBgDamage = false;
        DamageType_e damageType = DAMAGE_1;
        switch (mBgDamageType) {
            case 7:
                damageType = DAMAGE_YOGAN;
                break;
            case 8:
                damageType = DAMAGE_POISON;
                break;
            case 9:
                damageType = DAMAGE_B;
                break;
        }
        if (damageType == DAMAGE_1 && isNoDamage()) {
            return false;
        }
        return setDamage2(nullptr, damageType);
    }
    return false;
}

bool daPlBase_c::checkSinkSand() {
    mVec3_c pos = mPos;
    pos.y += 128.0f;
    if (dBc_c::checkGround(&pos, &mSinkSandHeight, mLayer, mAmiLayer, 3)) {
        if (mSinkSandHeight > mPos.y) {
            onNowBgCross(BGC_ON_SINK_SAND);
        }
        if (mSinkSandHeight > getCenterPos().y) {
            onNowBgCross(BGC_IN_SINK_SAND);
        }
        if (mSinkSandHeight > mPos.y + getModelHeight()) {
            onNowBgCross(BGC_INSIDE_SINK_SAND);
        }
        return true;
    }
    return false;
}


bool daPlBase_c::checkBGCrossWall(u8 direction) {
    static const BgCross1_e flags[] = { BGC_WALL_TOUCH_R_2, BGC_WALL_TOUCH_L_2 };
    return isNowBgCross(flags[direction]);
}

void daPlBase_c::checkSideViewLemit() {
    if (!daPyDemoMng_c::mspInstance->m_94 && isDemoType(DEMO_PLAYER)) {
        return;
    }
    if (isStatus(STATUS_53) || isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE) || isStatus(STATUS_8D)) {
        return;
    }
    if (dScStage_c::m_loopType == 0) {
        float tmpL = dBg_c::m_bg_p->getLeftLimit() + mViewLimitPadding;
        if (mPos.x <= tmpL) {
            calcSideLimitMultL(tmpL);
        }
        float tmpR = dBg_c::m_bg_p->getRightLimit() - mViewLimitPadding;
        if (mPos.x >= tmpR) {
            calcSideLimitMultR(tmpR);
        }
    }
    checkDispSideLemit();
}

bool daPlBase_c::revSideLimitCommon(float f) {
    if (mPos.x != f) {
        u8 dir = 0;
        if (mPos.x <= f) {
            dir = 1;
        }
        if (!isStatus(STATUS_7E)) {
            u16 ang = mBc.getSakaMoveAngle(dir);
            if (ang != 0) {
                mPos.y += (f - mPos.x) * (mAng(ang).sin() / mAng(ang).cos());
            }
        }
        mPos.x = f;
        return true;
    }
    return false;
}

bool daPlBase_c::calcSideLimitMultL(float f) {
    onNowBgCross(BGC_SIDE_LIMIT_L);
    revSideLimitCommon(f);
    if (mSpeedF < 0.0f) {
        if (mDirection == 1) {
            mSpeedF = -0.01f;
        } else {
            mSpeedF = 0.0f;
        }
        return true;
    }
    return false;
}

bool daPlBase_c::calcSideLimitMultR(float f) {
    onNowBgCross(BGC_SIDE_LIMIT_R);
    revSideLimitCommon(f);
    if (mSpeedF > 0.0f) {
        if (mDirection == 0) {
            mSpeedF = 0.01f;
        } else {
            mSpeedF = 0.0f;
        }
        return true;
    }
    return false;
}

bool daPlBase_c::checkDispSideLemit() {
    if (isStatus(STATUS_B8) || isStatus(STATUS_81)) {
        return false;
    }
    if (dScStage_c::m_loopType == 1) {
        return false;
    }
    float l = dBgParameter_c::ms_Instance_p->fn_80082240(mPos.x);
    float s = l + mViewLimitPadding;
    float m = l + dBgParameter_c::ms_Instance_p->xSize() - mViewLimitPadding + 1.0f;
    switch (mDispLimitRelatedR) {
        case 0:
            if (mPos.x < s) {
                if (isOldBgCross(BGC_OBJBG_TOUCH_CARRIED_R) || !isOldBgCross(BGC_WALL_TOUCH_R_2)) {
                    calcSideLimitMultL(s);
                    return true;
                }
                mDispLimitRelatedR = 1;
                mDispLimitRelatedR2 = mPos.x - s;
            }
            break;
        case 1: {
            if (mPos.x > s) {
                mDispLimitRelatedR = 0;
            }
            float diff = mPos.x - s;
            if (mDispLimitRelatedR2 < diff || isOldBgCross(BGC_WALL_TOUCH_R_2)) {
                mDispLimitRelatedR2 = diff;
            }
            if (mPos.x < s + mDispLimitRelatedR2) {
                calcSideLimitMultL(s + mDispLimitRelatedR2);
                return true;
            }
            break;
        }
    }
    switch (mDispLimitRelatedL) {
        case 0:
            if (mPos.x > m) {
                if (isOldBgCross(BGC_OBJBG_TOUCH_CARRIED_L) || !isOldBgCross(BGC_WALL_TOUCH_L_2)) {
                    calcSideLimitMultR(m);
                    return true;
                }
                mDispLimitRelatedL = 1;
                mDispLimitRelatedL2 = mLastPos.x - m;
            }
            break;
        case 1: {
            if (mPos.x < m) {
                mDispLimitRelatedL = 0;
            }
            float diff = mPos.x - m;
            if (mDispLimitRelatedL2 > diff || isOldBgCross(BGC_WALL_TOUCH_L_2)) {
                mDispLimitRelatedL2 = diff;
            }
            if (mPos.x > m + mDispLimitRelatedL2) {
                calcSideLimitMultR(m + mDispLimitRelatedL2);
                return true;
            }
        }
    }
    return false;
}

void daPlBase_c::fn_80055d00() {
    if (dBg_c::m_bg_p->mAutoscrolls[0].mActive) {
        return;
    }
    float l = dBgParameter_c::ms_Instance_p->fn_80082240(mPos.x);
    float s = l + mViewLimitPadding;
    float m = l + dBgParameter_c::ms_Instance_p->xSize() - mViewLimitPadding + 1.0f;
    if (mPos.x < s) {
        mDispLimitRelatedR = 1;
        mDispLimitRelatedR2 = mPos.x - s;
    }
    if (mPos.x > m) {
        mDispLimitRelatedL = 1;
        mDispLimitRelatedL2 = mLastPos.x - m;
    }
}

void daPlBase_c::underOverCheck() {
    float tmp = dBgParameter_c::ms_Instance_p->yStart() - dBgParameter_c::ms_Instance_p->ySize();
    float bgTop = tmp - 24.0f;
    float selfTop = mPos.y + getVisOffsetY() + getVisSizeY();
    int cond = 0;
    if (isItemKinopio()) {
        cond = 1;
    }
    if (!dBg_c::m_bg_p->mAutoscrolls[0].mActive) {
        if (dBg_c::m_bg_p->m_90009 == 1 || dBg_c::m_bg_p->m_90009 == 3 || daPyMng_c::mNum > 1) {
            cond = true;
        }
    }
    if (cond == 1) {
        if (selfTop < bgTop && selfTop < dBg_c::m_bg_p->m_8fe00) {
            setFallDownDemo();
        }
    } else if (selfTop < bgTop) {
        setFallDownDemo();
    }
}

void daPlBase_c::checkDispOver() {
    offStatus(STATUS_DISPLAY_OUT_DEAD);
    offStatus(STATUS_DISPLAY_OUT_NO_DAMAGE);
    offStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT);
    if (isStatus(STATUS_OUT_OF_PLAY) || isStatus(STATUS_53)) {
        return;
    }
    if (!isStatus(STATUS_7E)) {
        float adj = dBgParameter_c::ms_Instance_p->yStart() + 96.0f;
        if (dScStage_c::m_instance->mCurrWorld == WORLD_1 &&
            dScStage_c::m_instance->mCurrCourse == STAGE_CASTLE &&
            dScStage_c::m_instance->mCurrFile == 0
        ) {
            adj = dBgParameter_c::ms_Instance_p->yStart() + 192.0f;
        }
        if (mPos.y > adj) {
            mPos.y = adj;
        }
        underOverCheck();
        checkPressBg();
        mBgPressActive = 0;
        setBgDamage();
    }
    checkDisplayOutDead();
}

void daPlBase_c::checkDisplayOutDead() {
    float offset = 20.0f;
    if (daPyMng_c::mNum > 1 || dBg_c::m_bg_p->m_90009 == 1 || dBg_c::m_bg_p->m_90009 == 3) {
        offset = 64.0f;
    }

    float bgBottom = dBgParameter_c::ms_Instance_p->yStart() - dBgParameter_c::ms_Instance_p->ySize();
    float bgSide1 = bgBottom - 16.0f;
    float bgSide2 = bgBottom - offset;
    float edgePos = mPos.y + getVisOffsetY() + getVisSizeY();
    if (edgePos < bgSide1) {
        onStatus(STATUS_DISPLAY_OUT_DEAD);
        onStatus(STATUS_DISPLAY_OUT_NO_DAMAGE);
        if (isItemKinopio()) {
            onStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT);
        }
    }
    if (edgePos < bgSide2) {
        setBalloonInDispOut(3);
    }

    float bgTop = dBgParameter_c::ms_Instance_p->yStart();
    bgSide1 = bgTop + 16.0f;
    edgePos = mPos.y + getVisOffsetY() - getVisSizeY();
    if (edgePos > bgSide1) {
        onStatus(STATUS_DISPLAY_OUT_DEAD);
    }

    if (isItemKinopio() && edgePos > dBgParameter_c::ms_Instance_p->yStart() + 128.0f) {
        onStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT);
    }
    if (isStatus(STATUS_81)) {
        return;
    }

    offset = 0.0f;
    if (!isStatus(STATUS_B8)) {
        if (dBg_c::m_bg_p->mAutoscrolls[0].mActive && dBg_c::m_bg_p->m_9008e != 4) {
            offset = -(mWallBcData.mOffset / 4096.0f - 1.0f + getVisSizeX());
        } else if (daPyMng_c::mNum > 1) {
            offset = 16.0f;
        }
    }

    float bgLeft = dBgParameter_c::ms_Instance_p->fn_80082240(mPos.x);
    bgSide1 = bgLeft - 16.0f;
    bgSide2 = bgLeft - offset;
    edgePos = mPos.x + getVisOffsetX() + getVisSizeX();
    if (edgePos < bgSide1) {
        onStatus(STATUS_DISPLAY_OUT_DEAD);
        onStatus(STATUS_DISPLAY_OUT_NO_DAMAGE);
        if (isItemKinopio()) {
            onStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT);
        }
    }
    if (edgePos < bgSide2) {
        setBalloonInDispOut(2);
    }

    bgSide1 = bgLeft + dBgParameter_c::ms_Instance_p->xSize() + 16.0f;
    bgSide2 = bgLeft + dBgParameter_c::ms_Instance_p->xSize() + offset;
    edgePos = mPos.x + getVisOffsetX() - getVisSizeX();
    if (edgePos > bgSide1) {
        onStatus(STATUS_DISPLAY_OUT_DEAD);
        onStatus(STATUS_DISPLAY_OUT_NO_DAMAGE);
        if (isItemKinopio()) {
            onStatus(STATUS_ITEM_KINOPIO_DISPLAY_OUT);
        }
    }
    if (edgePos > bgSide2) {
        setBalloonInDispOut(0);
    }
}

void daPlBase_c::fn_80056370(dActor_c *actor, BgPress_e i) {
    mBgPressActive |= (1 << i);
    if (actor != nullptr) {
        mBgPressIDs[i] = actor->mUniqueID;
    } else {
        mBgPressIDs[i] = BASE_ID_NULL;
    }
}

bool daPlBase_c::isBgPress(dActor_c *actor) {
    u32 param = mBgPressFlags;
    if (param == 0) {
        return false;
    }
    for (int i = 1; i < BG_PRESS_COUNT; i++) {
        if (param & (1 << i) && mBgPressIDs[i] == actor->getID()) {
            return true;
        }
    }
    return false;
}

bool daPlBase_c::setPressBgDamage(int i1, int i2) {
    if (i1 == DAMAGE_B) {
        if (setDamage2(nullptr, DAMAGE_B)) {
            mBc.clearBgcSaveAll();
            return true;
        }
    } else {
        if (setDamage2(nullptr, DAMAGE_NONE)) {
            mBc.clearBgcSaveAll();
            dQuake_c::m_instance->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
            return true;
        }
    }
    return false;
}

bool daPlBase_c::isEnablePressUD() {
    dBg_ctr_c *ctrHead = mBc.mpCtrHead;
    dBg_ctr_c *ctrFoot = mBc.mpCtrFoot;
    if (ctrHead != nullptr && ctrFoot != nullptr) {
        if (ctrHead == ctrFoot || ctrHead->m_e0 == ctrFoot->m_e0) {
            return false;
        }
    }
    return true;
}

bool daPlBase_c::isEnablePressLR() {
    dBg_ctr_c *ctrL = mBc.mpCtrWalls[1];
    dBg_ctr_c *ctrR = mBc.mpCtrWalls[0];
    if (ctrL != nullptr && ctrR != nullptr) {
        if (ctrL == ctrR || ctrL->m_e0 == ctrR->m_e0) {
            return false;
        }
    }
    return true;
}

bool daPlBase_c::checkPressBg() {
    if (
        (isNowBgCross(BGC_SIDE_LIMIT_L) && isNowBgCross(BGC_WALL_TOUCH_R_2)) ||
        (isNowBgCross(BGC_SIDE_LIMIT_R) && isNowBgCross(BGC_WALL_TOUCH_L_2))
    ) {
        return true;
    }
    if (dScStage_c::m_instance->mCurrWorld == WORLD_6 &&
        dScStage_c::m_instance->mCurrCourse == STAGE_CASTLE &&
        dScStage_c::m_instance->mCurrFile == 1 &&
        mPos.y >= -1420.0f
    ) {
        if (setPressBgDamage(DAMAGE_B, 1)) {
            mBgPressFlags |= 0x8;
            return true;
        }
    }
    if (mBgPressActive & 0x20a && mBgPressActive & 0x414 && isEnablePressUD()) {
        if (mBgPressActive & 0x18 && setPressBgDamage(DAMAGE_B, 1)) {
            if (mBgPressActive & 0x8) {
                mBgPressFlags |= 0x8;
            }
            if (mBgPressActive & 0x10) {
                mBgPressFlags |= 0x10;
            }
            return true;
        }
        if (mBgPressActive & 6 && setPressBgDamage(DAMAGE_1, 1)) {
            if (mBgPressActive & 2) {
                mBgPressFlags |= 0x2;
            }
            if (mBgPressActive & 4) {
                mBgPressFlags |= 0x4;
            }
            return true;
        }
    }
    if (isStatus(STATUS_PENGUIN_SLIDE)) {
        return false;
    }
    if (mBgPressActive & 0x1140 && mBgPressActive & 0x8a0 && isEnablePressLR()) {
        if (mBgPressActive & 0x60 && setPressBgDamage(DAMAGE_B, 0)) {
            if (mBgPressActive & 0x20) {
                mBgPressFlags |= 0x20;
            }
            if (mBgPressActive & 0x40) {
                mBgPressFlags |= 0x40;
            }
            return true;
        }
        if (mBgPressActive & 0x180 && setPressBgDamage(DAMAGE_1, 0)) {
            if (mBgPressActive & 0x80) {
                mBgPressFlags |= 0x80;
            }
            if (mBgPressActive & 0x100) {
                mBgPressFlags |= 0x100;
            }
            return true;
        }
    }
    return false;
}

void daPlBase_c::setStatus87() {
    onStatus(STATUS_87);
}

bool daPlBase_c::isRideCheckEnable() {
    if (mSpeed.y > 0.0f) {
        return false;
    }
    if (isStatus(STATUS_4E)) {
        dActor_c *rideActor = (dActor_c *) fManager_c::searchBaseByID(mRideActorID);
        if (rideActor != nullptr && rideActor->mSpeed.y > 0.0f) {
            return false;
        }
    }
    return true;
}

void daPlBase_c::setStatus5D(float f) {
    onStatus(STATUS_5D);
    mExtraPushForceX = f;
}

bool daPlBase_c::checkInsideCrossBg(float f) {
    const sBcPointData *wallBg = getWallBgPointData();
    if (wallBg == nullptr) {
        return false;
    }
    const sBcPointData *footBg = getFootBgPointData();
    if (footBg == nullptr) {
        return false;
    }
    float offs2[] = {
        footBg->mInfMargin / 4096.0f + f,
        footBg->mSupMargin / 4096.0f - f
    };
    float offs[] = {
        wallBg->mInfMargin / 4096.0f,
        wallBg->mSupMargin / 4096.0f
    };
    for (int i = 0; i < 2; i++) {
        mVec3_c modPos = mVec3_c(
            mPos.x,
            mPos.y + offs[i],
            mPos.z
        );
        mVec3_c copy;
        copy.set(
            modPos.x + offs2[0],
            modPos.y,
            modPos.z
        );
        if (mBc.checkWallPlayer(&modPos, &copy, nullptr)) {
            return true;
        }
        copy.set(
            modPos.x + offs2[1],
            modPos.y,
            modPos.z
        );
        if (mBc.checkWallPlayer(&modPos, &copy, nullptr)) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::setPowerup(PLAYER_POWERUP_e powerup, int) {
    mPowerup = powerup;
}

bool daPlBase_c::isMameAction() {
    if (mPowerup == POWERUP_MINI_MUSHROOM && !isLiftUpExceptMame()) {
        return true;
    }
    return false;
}

void daPlBase_c::setStatus(int id) {
    mStatusFlags[id / 32] = (1 << (id % 32));
}

void daPlBase_c::onStatus(int id) {
    mStatusFlags[id / 32] |= (1 << (id % 32));
}

void daPlBase_c::offStatus(int id) {
    mStatusFlags[id / 32] &= ~(1 << (id % 32));
}

bool daPlBase_c::isStatus(int id) {
    return mStatusFlags[id / 32] & (1 << (id % 32));
}

u8 daPlBase_c::getTallType(s8) {
    return 2;
}

void daPlBase_c::calcTimerProc() {
    if (sLib::calcTimer(&mDamageInvulnTimer) != 0) {
        if (mDamageInvulnTimer < 60) {
            if (mDamageInvulnTimer & 4) {
                onStatus(STATUS_INVULNERABLILITY_BLINK);
            }
        } else if (mDamageInvulnTimer & 8) {
            onStatus(STATUS_INVULNERABLILITY_BLINK);
        }
    }

    sLib::calcTimer(&mPowerupChangeInvulnTimer);
    sLib::calcTimer(&mTimer_ce8);
    sLib::calcTimer(&mNoGravityTimer);
    sLib::calcTimer(&mTimer_a8);
    sLib::calcTimer(&mSquishNoMoveTimer);
    sLib::calcTimer(&mSquishCooldownTimer);
    sLib::calcTimer(&mJumpDaiFallTimer);
    sLib::calcTimer(&m_1072);
    sLib::calcTimer(&mBossDemoLandTimer);
    sLib::calcTimer(&mTimer_f4);
    sLib::calcTimer(&mSlipEndTimer);

    updateNoHitPlayer();
    calcNoHitObjBgTimer();
}

dPyMdlBase_c * daPlBase_c::getModel() {
    return mpMdlMng->mpMdl;
}

void daPlBase_c::calcPlayerSpeedXY() {
    static const float ratios[] = { 0.6f, 0.55f, 0.5f, 0.45f, 0.4f };
    float extraXAccel = 0.0f;
    float t = 0.0f;
    bool x = calcCcPlayerRev(&t);

    float c = 1.0f;
    float b = mMaxSpeedF;

    if (isStatus(STATUS_FOLLOW_MAME_KURIBO)) {
        int v = getFollowMameKuribo() - 1;

        if (v < 0) {
            v = 0;
        }
        if (v > 4) {
            v = 4;
        }

        b = mMaxSpeedF * ratios[v];
    }

    if (x) {
        if (b < -0.5f) {
            b = -0.5f;
        }
        if (b > 0.5f) {
            b = 0.5f;
        }
        if (std::fabs(mSpeedF) > std::fabs(b)) {
            c = 1.0f;
        } else {
            c = 1.8f;
        }
    }

    sLib::chase(&mSpeedF, b, mAccelF * c);
    calcWindSpeed();

    float d = 5.0f;
    if (isStatus(STATUS_88)) {
        if (std::fabs(b) > 5.0f) {
            d = std::fabs(b);
        }
    }

    float f = mSpeedF + mWindSpeed;
    if (f < -d) {
        f = -d;
    } else if (f > d) {
        f = d;
    }

    mSpeed.x = f;

    if (f * t >= 0.0f) {
        mVec3_c wallvec1(mPos.x + f + t, mPos.y + getModelHeight() / 2.0f, mPos.z);
        mVec3_c wallvec2(wallvec1.x + f + t, wallvec1.y, wallvec1.z);

        float g;

        if (dBc_c::checkWall(&wallvec1, &wallvec2, &g, mLayer, mAmiLayer, nullptr)) {
            t = 0.0f;
        }
    }

    extraXAccel += t;

    if (mFinalAirPushForceX != 0.0f) {
        if (!isNowBgCross(BGC_FOOT)) {
            extraXAccel += mFinalAirPushForceX;
        } else {
            if (mFinalAirPushForceX * mSpeedF < 0.0f) {
                mSpeedF += mFinalAirPushForceX;
            }
            mFinalAirPushForceX = 0.0f;
        }
    }

    if (m_1138 != 0.0f) {
        if (!isNowBgCross(BGC_FOOT)) {
            extraXAccel += m_1138;
            sLib::chase(&m_1138, 0.0f, m_113c);
        } else {
            m_1138 = 0.0f;
        }
    }

    if (isNowBgCross(BGC_FOOT) && isStatus(STATUS_5F)) {
        mSpeedF = 0.0f;
    }
    mPrevSpeedF = mSpeedF;

    float k = mMaxFallSpeed;
    if (isNowBgCross(BGC_ON_SINK_SAND) | isNowBgCross(BGC_IN_SINK_SAND)) {
        k = 0.0f;
    }

    setSandEffect();

    mPos += mVec3_c(mBgPushForce.x, mBgPushForce.y, mBgPushForce.z);

    mPrevSpeedY = mSpeed.y;
    mSpeed.y += mAccelY;

    if (mSpeed.y < k) {
        mSpeed.y = k;
    }

    mVec3_c speed(
        mSpeed.x + extraXAccel,
        mSpeed.y,
        mSpeed.z
    );

    posMoveAnglePlayer(speed);
}

void daPlBase_c::posMoveAnglePenguin(mVec3_c a, unsigned short b) {
    mVec3_c _40(0.0f, a.y, 0.0f);

    if (isNowBgCross(BGC_HEAD) && (_40.y > 0.0f)) {
        mAng angle = mBc.getHeadSakaMoveAngle(mDirection);

        if (angle.mAngle > 0) {
            _40.x = sc_DirSpeed[mDirection] * std::fabs(a.y * angle.sin());
        }

        _40.y = a.y * std::fabs(angle.cos());
    }

    if (isNowBgCross(BGC_FOOT) && (a.y < 0.0f)) {
        mAng angle = mBc.getSakaMoveAngle(mDirection);

        if (angle.mAngle < 0) {
            _40.x = sc_DirSpeed[mDirection] * std::fabs(a.y * angle.sin());
        }

        _40.y = a.y * std::fabs(angle.cos());
    }

    float x_mag = std::fabs(a.x);

    mVec3_c delta(
        _40.x + x_mag * mAng(b).cos(),
        _40.y + x_mag * mAng(b).sin(),
        a.z
    );

    posMove(delta);
}

void daPlBase_c::posMoveAnglePlayer(mVec3_c a) {
    if (
        ((a.x > 0.0f && isNowBgCross(BGC_WALL_TOUCH_R_2)) || (a.x < 0.0f && isNowBgCross(BGC_WALL_TOUCH_L_2))) &&
        (std::fabs(a.x) > 2.5f)
    ) {
        if (a.x > 0.0f) {
            a.x = 2.5f;
        } else {
            a.x = -2.5f;
        }
    }

    u8 dir = 0;
    if (a.x < 0.0f) {
        dir = 1;
    }

    u16 x = mBc.getSakaMoveAngle(dir);
    if (isNowBgCross(BGC_HEAD)) {
        x = mBc.getHeadSakaMoveAngle(dir);
    }

    if (isStatus(STATUS_PENGUIN_SWIM)) {
        posMoveAnglePenguin(a, x);
        return;
    }

    u16 x2 = 0;
    if (isStatus(STATUS_WALL_SLIDE)) {
        x2 = mAdjacentSlopeAngle;
    }

    float x_mag = std::fabs(a.x);
    float y = a.y;

    mVec3_c delta(
        x_mag * mAng(x).cos() - y * mAng(x2).sin(),
        x_mag * mAng(x).sin() + y * std::fabs(mAng(x2).cos()),
        a.z
    );

    posMove(delta);
}

const float *daPlBase_c::getSpeedData() {
    if (isStar()) {
        return mSpeedDataStar;
    } else {
        return mSpeedDataNormal;
    }
}

void daPlBase_c::setZPosition() {
    if (isStatus(STATUS_99)) {
        return;
    }

    if ((mAmiLayer == 1) && (mLayer == 0)) {
        mPos.z = 3000.0f - (float) (mPlayerLayer * 32);
    } else {
        mPos.z = -1800.0f - (float) (mPlayerLayer * 32);
    }
}

void daPlBase_c::setZPosition(float a) {
    onStatus(STATUS_99);
    mPos.z = a - (float) (mPlayerLayer * 32);
}

void daPlBase_c::setZPositionDirect(float a) {
    onStatus(STATUS_99);
    mPos.z = a;
}

void daPlBase_c::offZPosSetNone() {
    offStatus(STATUS_99);
}

float daPlBase_c::setJumpAddSpeedF(float a) {
    if (isNowBgCross(BGC_FOOT)) {
        return a;
    }

    if (a >= 2.0f) {
        a = 2.0f;
    }

    if (a <= -2.0f) {
        a = -2.0f;
    }

    mFinalAirPushForceX = a;
    return a;
}

float daPlBase_c::setAddLiftSpeedF() {
    float t = mExtraPushForceX;

    if (isStatus(STATUS_5D)) {
        t = mExtraPushForceX * 0.4f;
    }

    return setJumpAddSpeedF(mBgPushForce.x + t);
}

bool daPlBase_c::setDelayHelpJump() {
    if (mKey.triggerJump() && std::fabs(mSpeedF) > 1.3f) {
        bool x = false;

        if (mBgFootHistory[0]) {
            x = true;
        } else if (mBgFootHistory[1]) {
            x = true;
        }

        if (x && startJump(BLEND_DEFAULT, 1)) {
            return true;
        }
    }
    return false;
}

bool daPlBase_c::startJump(AnmBlend_e blendMode, int jumpType) {
    if (isNowBgCross(BGC_WATER_SHALLOW) || isStatus(STATUS_87)) {
        return false;
    }

    if (isNowBgCross(BGC_HEAD) && !isNowBgCross(BGC_LINE_BLOCK_HIT)) {
        return false;
    }

    if ((mpMdlMng->mpMdl->mFlags & 1) && checkStandUpRoofOnLift()) {
        return setCrouchJump();
    }

    if (mKey.triggerJump()) {
        daPlBase_c::jmpInf_c info(0.0f, jumpType, blendMode);
        changeState(StateID_Jump, &info);
        return true;
    }

    return false;
}

bool daPlBase_c::checkJumpTrigger() {
    if (isNowBgCross(BGC_FOOT) && !isNowBgCross(BGC_CLIFF)) {
        if (startJump(BLEND_DEFAULT, 1)) {
            return true;
        }
    }
    return false;
}

bool daPlBase_c::isNoDamage() {
    return false;
}

bool daPlBase_c::setDamage(dActor_c *, daPlBase_c::DamageType_e) {
    return false;
}

bool daPlBase_c::setForcedDamage(dActor_c *, daPlBase_c::DamageType_e) {
    return false;
}

bool daPlBase_c::setDamage2(dActor_c *, daPlBase_c::DamageType_e) {
    return false;
}

bool daPlBase_c::setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    return false;
}

bool daPlBase_c::_setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    return false;
}

bool daPlBase_c::setWaitJump(float jumpSpeed) {
    if (isDemo() || isStatus(STATUS_OUT_OF_PLAY)) {
        return false;
    }

    mSpeedF = 0.0f;
    mSpeed.y = jumpSpeed;
    changeState(StateID_WaitJump);
    return true;
}

bool daPlBase_c::setSwimSpeed(float a, float b) {
    return false;
}

void daPlBase_c::onFollowMameKuribo() {
    onStatus(STATUS_FOLLOW_MAME_KURIBO);
    mNewFollowMameKuribo++;
}

void daPlBase_c::clearFollowMameKuribo() {
    offStatus(STATUS_FOLLOW_MAME_KURIBO);
    mFollowMameKuribo = mNewFollowMameKuribo;
    mNewFollowMameKuribo = 0;
}

u32 daPlBase_c::getFollowMameKuribo() {
    return mFollowMameKuribo;
}

bool daPlBase_c::isMaskDraw() {
    if (isStatus(STATUS_53) || isStatus(STATUS_64)) {
        return false;
    }
    return true;
}

void daPlBase_c::setRideNat(float nutHeight) {
    onStatus(STATUS_RIDE_NUT);
    mRideNutHeight = nutHeight;
}

void daPlBase_c::updateRideNat() {
    if (isStatus(STATUS_RIDE_NUT) && mSpeed.y <= 0) {
        if (!isStatus(STATUS_RIDE_NUT_2) && mPos.y <= mRideNutHeight) {
            onStatus(STATUS_RIDE_NUT_2);
        }
    } else {
        offStatus(STATUS_RIDE_NUT_2);
    }
}

bool daPlBase_c::suppressSound(int suppressionMode) {
    if (suppressionMode == 1 && isStatus(STATUS_ENDING_DANCE_AUTO)) {
        return true;
    }

    if (isStatus(STATUS_ABOUT_TO_BE_DELETED)) {
        return true;
    }

    return false;
}

void daPlBase_c::startSound(ulong soundID, bool b) {
    if (!suppressSound(b)) {
        mSndObj.startSound(soundID, 0);
    }
}

void daPlBase_c::startSound(ulong soundID, short s, bool b) {
    if (!suppressSound(b)) {
        mSndObj.startSound(soundID, s, 0);
    }
}

void daPlBase_c::holdSound(ulong soundID, bool b) {
    if (!suppressSound(b)) {
        mSndObj.holdSound(soundID, 0);
    }
}

void daPlBase_c::holdSound(ulong soundID, short s, bool b) {
    if (!suppressSound(b)) {
        mSndObj.holdSound(soundID, s, 0);
    }
}

void daPlBase_c::startPlayerVoice(int a, int b) {
    if (!suppressSound(b)) {
        mSndObj.fn_8019AAB0(a, 0);
    }
}

void daPlBase_c::holdPlayerVoice(int a, int b) {
    if (!suppressSound(b)) {
        mSndObj.fn_8019ABB0(a, 0);
    }
}

void daPlBase_c::startFootSoundPlayer(unsigned long a) {
  if (!suppressSound(1)) {
    mSndObj.startFootSound(a, std::fabs(mSpeedF), 0);
  }
}

void daPlBase_c::setItemCompleteVoice() {
    startPlayerVoice(VOICE_ITEM_COMPLETE, 0);
}

void daPlBase_c::setStar(daPlBase_c::StarSet_e, int) {}

void daPlBase_c::clearTreadCount() {
    mTreadCount = 0;
}

s8 daPlBase_c::calcTreadCount(int max) {
    if (mTreadCount < max) {
        mTreadCount++;
    }

    return mTreadCount;
}

void daPlBase_c::clearStarCount() {
    mStarCount = 0;
}

s8 daPlBase_c::calcStarCount(int max) {
    if (isStar() && mStarCount < max) {
        mStarCount++;
    }

    return mStarCount;
}

void daPlBase_c::clearComboCount() {
    mPlComboCount = 0;
}

s8 daPlBase_c::calcComboCount(int max) {
    if (mPlComboCount < max) {
        mPlComboCount++;
    }
    return mPlComboCount;
}

void daPlBase_c::startQuakeShock(dQuake_c::TYPE_SHOCK_e arg) {
    dQuake_c::m_instance->shockMotor(mPlayerNo, arg, 0, false);
}

void daPlBase_c::startPatternRumble(const char *pattern) {
    if (dRemoconMng_c::m_instance->mRemocons[mPlayerNo]->mIsRumbleEnabled) {
        mPad::g_core[dRemoconMng_c::m_instance->mRemocons[mPlayerNo]->mControllerID]->startPatternRumble(pattern, 0, false);
    }
}

mVec3_c daPlBase_c::getAnkleCenterPos() {
    mVec3_c joint7Pos;
    mVec3_c joint4Pos;

    mpMdlMng->mpMdl->getJointPos(&joint4Pos, 4);
    mpMdlMng->mpMdl->getJointPos(&joint7Pos, 7);

    return (joint4Pos + joint7Pos) / 2.0f;
}

void daPlBase_c::calcHeadAttentionAngle() {
    if (isStatus(STATUS_NO_ANIM)) {
        return;
    }

    mAng3_c angle(
        mAng(0),
        mpMdlMng->getAng().y,
        mpMdlMng->getAng().z
    );

    if (!isLiftUp() && !isStatus(STATUS_94) && !isStatus(STATUS_WAS_TWIRL)) {
        if ((mpMdlMng->mpMdl->mFlags & 0x80) || (mpMdlMng->mpMdl->mFlags & 0x100)) {
            mAng r31 = mAngle.y;
            mAng r30 = 0;
            bool cond = false;
            daPlBase_c *pdVar10 = nullptr;

            if (mpMdlMng->mpMdl->mFlags & 0x100) {
                if (isStatus(STATUS_95)) {
                    pdVar10 = (daPlBase_c *) dAttention_c::mspInstance->searchPlayer(this, mpMdlMng->getHatPos());
                }
            } else {
                pdVar10 = (daPlBase_c *) dAttention_c::mspInstance->search(mpMdlMng->getHatPos());
            }

            if (pdVar10 != nullptr) {
                mVec3_c hatPos = mpMdlMng->getHatPos();
                mVec3_c _60 = mVec3_c(
                    pdVar10->getLookatPos().x - hatPos.x,
                    pdVar10->getLookatPos().y - hatPos.y,
                    8.0f
                );

                int tmp = cM::atan2s(_60.y, _60.xzLen());
                short r27 = -tmp;
                short r4 = 0x2000;

                if (pdVar10->mAttentionFlags & 2) {
                    r4 = 0xC00;
                }

                if (r27 > r4) {
                    r27 = r4;
                }

                int neg = -r4;
                if (r27 < neg) {
                    r27 = neg;
                }

                int temp = cM::atan2s(_60.x, _60.z);
                short iVar2 = (temp - mAngle.y.mAngle);

                if (pdVar10->mAttentionMode == 3) {
                    mpMdlMng->mpMdl->m_204 = 2;

                    int r29 = mpMdlMng->mpMdl->mFlags & 0x100;
                    s16 r30_tmp = 0x5000;
                    if (r29) {
                        r30_tmp = 0x2000;
                    }

                    int r3 = abs(iVar2);

                    if (r3 > r30_tmp) {
                        if (iVar2 > 0) {
                            temp = (mAng(r30_tmp) + mAngle.y).mAngle;
                        } else {
                            temp = (mAngle.y - mAng(r30_tmp)).mAngle;
                        }
                    }

                    r31 = temp;

                    if (r29) {
                        r30 = r27;
                    } else {
                        float tmpf = std::fabs(mAng(r3 / 2.0f).cos());
                        r30 = r27 * tmpf;
                    }
                    cond = true;
                } else if (abs(iVar2) < 0x4000) {
                    mpMdlMng->mpMdl->m_204 = 1;
                    r30 = r27;
                    cond = true;
                }
            }

            sLib::addCalcAngle(&angle.y.mAngle, r31.mAngle, 8, 0x400, 0x40);
            if (pdVar10 != nullptr && (pdVar10->mAttentionFlags & 1)) {
                sLib::addCalcAngle(&angle.z.mAngle, r30.mAngle, 8, 0x180, 0x40);
            } else {
                sLib::addCalcAngle(&angle.z.mAngle, r30.mAngle, 8, 0x400, 0x40);
            }

            mpMdlMng->setAng(angle);

            if (!cond && angle.z == 0 && abs(angle.y.mAngle - mAngle.y.mAngle) < 0x100) {
                mpMdlMng->mpMdl->m_204 = 0;
            }

            return;
        }
    }

    angle.y = 0;
    angle.z = 0;
    mpMdlMng->setAng(angle);
    mpMdlMng->mpMdl->m_204 = 0;
}

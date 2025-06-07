#include "game/bases/d_game_com.hpp"
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_cd.hpp>
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
#include <game/mLib/m_fader.hpp>
#include <game/sLib/s_math.hpp>
#include <constants/sound_list.h>

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

    const float scDokanOutTurnSpeed[] = { 2048.0f };
}

daPlBase_c::daPlBase_c() :
    mpMdlMng(nullptr),
    mDemoStateMgr(*this, sStateID::null),
    mStateMgr(*this, sStateID::null)
{
    mAreaNo = dScStage_c::m_instance->mCurrAreaNo;
    mLayer = dScStage_c::m_instance->mCurrLayer;
    m_ca1 = true;
    mBc.mOwningPlrNo = mPlayerNo;
    changeDemoState(StateID_DemoNone, 0);
    changeState(StateID_None, 0);
    m_25b = 3;
    m_58 = 16.0f;
    mEatSpitType = 0;
}

daPlBase_c::~daPlBase_c() {
    releaseCcData();
    mpMdlMng = nullptr;
}

int daPlBase_c::create() {
    mKey.init();
    if (dScStage_c::m_gameMode == 2) {
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_RIGHT);
    }
    setZPosition();
    return SUCCEEDED;
}

int daPlBase_c::preExecute() {
    offStatus(STATUS_01);
    if (dActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    if (isStatus(STATUS_7D)) {
        return NOT_READY;
    }
    onStatus(STATUS_01);
    if (m_d44 & 2) {
        mLastPos = m_cb0;
    }
    m_20 = 0;
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
    if (isStatus(STATUS_6F)) {
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
        m_ca4 = mLastPos - mPos;
        m_cb0 = mPos;
        offStatus(STATUS_1A);
        offStatus(STATUS_1B);
        offStatus(STATUS_BE);
        offStatus(STATUS_59);
        offStatus(STATUS_62);
        offStatus(STATUS_5B);
        offStatus(STATUS_5C);
        offStatus(STATUS_89);
        offStatus(STATUS_8B);
        offStatus(STATUS_8C);
        offStatus(STATUS_41);
        offStatus(STATUS_42);
        offStatus(STATUS_82);
        if (isStatus(STATUS_8D)) {
            offStatus(STATUS_8D);
            if (isStatus(STATUS_B9)) {
                onStatus(STATUS_7D);
            }
        }
        offStatus(STATUS_2E);
        if (isStatus(STATUS_2D)) {
            offStatus(STATUS_2D);
            onStatus(STATUS_2E);
        }
        offStatus(STATUS_A4);
        clearFollowMameKuribo();
        clearCcPlayerRev();
        clearCcData();
    }
    dActor_c::postExecute(status);
}

int daPlBase_c::doDelete() {
    m_360 = 1;
    onStatus(STATUS_B5);
    mStateMgr.finalizeState();
    return SUCCEEDED;
}

int daPlBase_c::preDraw() {
    if (isStatus(STATUS_BB)) {
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

STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, None);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Walk);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Jump);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, SitJump);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Fall);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Land);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Crouch);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Slip);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Turn);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, HipAttack);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Swim);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, JumpDai);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, PlayerJumpDai);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Funsui);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Kani);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, Cloud);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, AnimePlay);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, WaitJump);
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

void daPlBase_c::changeState(const sStateIDIf_c &state, void *param) {
    if (isStatus(STATUS_03)) {
        return;
    }
    clearComboCount();
    mSubstate = 0;
    m_1114 = 0;
    m_1118 = 0;
    offStatus(STATUS_97);
    offStatus(STATUS_98);
    mStateChangeParam = param;
    mStateMgr.changeState(state);
}

void daPlBase_c::executeState() {
    if (m_1114 != 0) {
        m_1114--;
    }
    mStateMgr.executeState();
}

short daPlBase_c::getMukiAngle(u8 direction) {
    static const short scMukiAngle[] = { 0x3000, -0x3000 };
    return scMukiAngle[direction];
}

int daPlBase_c::turnAngle() {
    return sLib::chase(&mAngle.y, getMukiAngle(mDirection), 0x2000);
}

int daPlBase_c::addCalcAngleY(short p1, short p2) {
    return sLib::addCalcAngle(&mAngle.y, p1, p2, 0x2000, 0x400);
}

short daPlBase_c::getBesideMukiAngle(u8 direction) {
    static const short scMukiAngle[] = { 0x4000, -0x4000 };
    return scMukiAngle[direction];
}

void daPlBase_c::turnBesideAngle() {
    sLib::chase(&mAngle.y, getBesideMukiAngle(mDirection), 0x2000);
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

void daPlBase_c::vf378(int) {}
void daPlBase_c::vf37c(int) {}
void daPlBase_c::vf380(int) {}
void daPlBase_c::walkAction_Wait() {}
void daPlBase_c::vf384(int) {}
void daPlBase_c::walkAction_Move() {
    int arg = 1;
    if (!mSpeedF) {
        if ((m_d40 & 0x800) || m_f4 || m_112c) {
            if (mKey.buttonWalk(nullptr)) {
                arg = 0;
            } else {
                vf380(1);
                return;
            }
        } else {
            if (m_d40 & 4) {
                if (mPowerup == POWERUP_MINI_MUSHROOM && mBc.getSakaUpDown(mDirection) == 1) {
                    arg = 0;
                } else {
                    vf380(1);
                    return;
                }
            }
            if (m_d40 & 0x2000000 || !mKey.buttonWalk(nullptr)) {
                vf380(1);
                return;
            }
        }
    } else if (m_d44 & 2) {
        m_f4 = 3;
    }
    vf37c(arg);
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
    static const float csSpeedMult[] = { 0.5f, 0.8f };
    static const float csSpeedMultNoMove[] = { 0.3f, 1.0f };
    if (mPowerup != POWERUP_PENGUIN_SUIT && (m_d44 & 2) && m_d40 & 0x800000 && fabsf(mBc.mIceSpeed) > 1.0f) {
        u8 idx = 0;
        if (mBc.mIceSpeed < 0.0f) {
            idx = 1;
        }
        if (mKey.buttonWalk(nullptr)) {
            mSpeedF *= csSpeedMult[idx];
        } else {
            mSpeedF *= csSpeedMultNoMove[idx];
        }

        float newSpeedF = fabs(mSpeedF);
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
    if ((m_d44 & 2) && m_d40 & 0x800000 && !mKey.buttonWalk(nullptr) && fabsf(mBc.mIceSpeed) > 1.0f) {
        mAccelF = calcStarAccel(mAccelF);
    }
}

void daPlBase_c::initializeState_Jump() {
    onStatus(STATUS_A0);
    onStatus(STATUS_A5);
    offStatus(STATUS_88);
    if (m_358) {
        onStatus(STATUS_4D);
    }
}
void daPlBase_c::finalizeState_Jump() {
    mAngle.x = 0;
    if (isStatus(STATUS_A5)) {
        mSndObj.stopPlyJumpSound();
    }
    offStatus(STATUS_0A);
    offStatus(STATUS_88);
    offStatus(STATUS_A5);
    offStatus(STATUS_AB);
    offStatus(STATUS_A0);
    offStatus(STATUS_4D);
    m_358 = 0;
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
}
void daPlBase_c::executeState_Jump() {
    if (m_358 == 0) {
        offStatus(STATUS_4D);
    }
    if (isStatus(STATUS_A5)) {
        if (!mKey.buttonJump() || m_d40 & 2) {
            mSndObj.stopPlyJumpSound();
            offStatus(STATUS_A5);
        }
        if (mSpeed.y <= 1.0f) {
            offStatus(STATUS_A5);
        }
    }
    onStatus(STATUS_0A);
}

bool daPlBase_c::setCrouchJump() {
    if (mKey.triggerJump()) {
        changeState(StateID_SitJump, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::initializeState_SitJump() {}
void daPlBase_c::finalizeState_SitJump() {}
void daPlBase_c::executeState_SitJump() {}

void daPlBase_c::setFallAction() {
    if (m_d40 & 1) {
        return;
    }
    changeState(StateID_Fall, nullptr);
}

void daPlBase_c::initializeState_Fall() {
    onStatus(STATUS_A0);
    if (m_358) {
        onStatus(STATUS_4D);
    }
    if (!mStateChangeParam) {
        float f = ((float *) &dPyMdlMng_c::m_hio)[10];
        mpMdlMng->mpMdl->setAnm(6, f, 0.0f, 10.0f);
    }
    if (m_d4c & 2) {
        if (m_1134 * mSpeedF > 0.0f) {
            m_1138 = m_1134;
            m_1134 = 0.0f;
            m_113c = fabsf(m_1134 / 10.0f);
        }
    }
}
void daPlBase_c::finalizeState_Fall() {
    offStatus(STATUS_A0);
    offStatus(STATUS_88);
    offStatus(STATUS_4D);
    m_358 = 0;
}
void daPlBase_c::executeState_Fall() {
    if (!m_358) {
        offStatus(STATUS_4D);
    }
}

void daPlBase_c::initializeState_Land() {}
void daPlBase_c::finalizeState_Land() {}
void daPlBase_c::executeState_Land() {}

bool daPlBase_c::checkCrouch() {
    return false;
}

bool daPlBase_c::setCancelCrouch() {
    if (fn_80047ee0()) {
        return false;
    }
    if (mpMdlMng->mpMdl->m_154 != 21) {
        float f = ((float *) &dPyMdlMng_c::m_hio)[10];
        mpMdlMng->mpMdl->setAnm(6, f, 0.0f, 10.0f);
        mpMdlMng->mpMdl->setAnm(6, f, 0.0f, 10.0f);
    }
    if (m_d40 & 0x4000) {
        mpMdlMng->mpMdl->setFrame(4.0f);
    } else {
        mpMdlMng->mpMdl->setFrame(-dPyMdlBase_c::scWaterCrouchAnmSpeed);
    }
    onStatus(STATUS_52);
    changeState(StateID_Walk, (void *) 1);
    return true;
}

bool daPlBase_c::fn_80047ee0() {
    if ((m_d40 & 1) != 0 && (m_d44 & 2) == 0) {
        return false;
    }
    return fn_80047f10();
}

bool daPlBase_c::fn_80047f10() {
    void *headBgP = getHeadBgPointData();
    if (headBgP == nullptr) {
        return false;
    }
    float standHeadBgY = getStandHeadBgPointY();
    float tmpCalc = mPos.y + standHeadBgY - 1.0f;
    mVec3_c p = mVec3_c(
        mPos.x + *((int *)(headBgP) + 1) / 4096.0f,
        mPos.y + 4.0f,
        mPos.z
    );
    float res;
    if (dBc_c::checkTenjou(p, res, mLayer, m_ca1) && res < tmpCalc) {
        return true;
    }
    p.x = mPos.x + *((int *)(headBgP) + 2) / 4096.0f;
    if (dBc_c::checkTenjou(p, res, mLayer, m_ca1) && res < tmpCalc) {
        return true;
    }
    return false;
}

void daPlBase_c::initializeState_Crouch() {}
void daPlBase_c::finalizeState_Crouch() {}
void daPlBase_c::executeState_Crouch() {}

void daPlBase_c::initializeState_Slip() {
    mSubstate = SLIP_ACTION_NONE;
    float f1 = ((float *) &dPyMdlMng_c::m_hio)[10];
    float f2 = ((float *) &dPyMdlMng_c::m_hio)[11];
    mpMdlMng->mpMdl->setAnm(21, f1, f2, 0.0f);
    mMaxSpeedF = getSlipMaxSpeedF();
    m_1114 = 8;
    m_fc = 0;
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
    sLib::calcTimer(&m_fc);
    short ang = 0;
    if (m_d40 & 1) {
        ang = mBc.getSakaAngle(mDirection);
        ang = (short) (ang * 0.3f);
    }
    sLib::addCalcAngle(&mAngle.x, ang, 4, 0x2000, 0x100);
    int angle = turnAngle();
    if (m_d44 & 8) {
        m_fc = 9;
    } else if (checkJumpTrigger()) {
        return;
    } else if (angle && !m_fc && checkSlipEndKey()) {
        fn_800488f0(0);
        return;
    }
    switch ((SlipSubstate_e) mSubstate) {
        case SLIP_ACTION_NONE:
            if ((m_d40 & 1) == 0 && (mKey.triggerJump() || mKey.triggerCross())) {
                changeState(StateID_Fall, nullptr);
            } else {
                slipActionMove(angle);
            }
            break;
        case SLIP_ACTION_STOOP:
            mMaxSpeedF = 0.0f;
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                changeState(StateID_Crouch, (void *) 1);
            }
            break;
        case SLIP_ACTION_END:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                fn_800488f0(1);
            }
        default:
            break;
    }
}

void daPlBase_c::setSlipAction_ToStoop() {
    if (fn_80047ee0()) {
        return;
    }
    offStatus(STATUS_30);
    if (isStatus(STATUS_3C)) {
        offStatus(STATUS_3C);
    }
    mSubstate = SLIP_ACTION_STOOP;
    mpMdlMng->mpMdl->setAnm(24, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
}

void daPlBase_c::setSlipAction_ToEnd() {
    if (fn_80047ee0()) {
        return;
    }
    offStatus(STATUS_30);
    if (isStatus(STATUS_3C)) {
        offStatus(STATUS_3C);
    }
    mSubstate = SLIP_ACTION_END;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mpMdlMng->mpMdl->setAnm(23, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
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
    } else if (dScStage_c::m_instance->mCurrWorld == WORLD_1 && dScStage_c::m_instance->mCurrLevel == STAGE_6 && dScStage_c::m_instance->mCurrFile == 1) {
        if (m_d44 & 2) {
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
    if (m_d48 & 1 && m_d96 > 0 && m_d98 * m_d9a <= 0) {
        m_d40 &= ~1;
        float baseSpeed = mSpeedF;
        float cos = nw4r::math::CosS(m_d96);
        mMaxSpeedF = baseSpeed;
        mSpeedF = baseSpeed * cos;
        mSpeed.y = fabsf(baseSpeed) * nw4r::math::SinS(m_d96);
    }
    if (m_d40 & 1) {
        if (mSpeedF <= 0.0f && m_d40 & 0x80000) {
            setSlipActionViewLimitEnd();
        } else if (checkSakaReverse()) {
            setSlipActionEnd();
        } else if (m_d44 & 0x80) {
            m_f8 = 3;
            m_1114 = 8;
            mMaxSpeedF = getSlipMaxSpeedF();
            if (!mBc.getSakaUpDown(mDirection)) {
                if (param == 0) {
                    mAccelF = 0.5f * mAccelF;
                }
                if (fabsf(mSpeedF) < 1.0f &&
                    ((mCc.mCollOffsetX[0] < 0.0f && mSpeedF >= 0.0f) ||
                    (mCc.mCollOffsetX[0] > 0.0f && mSpeedF <= 0.0f)))
                {
                    setSlipActionEnd();
                }
                if (!mSpeedF && checkBGCrossWall(mDirection)) {
                    setSlipActionEnd();
                }
            }
        } else {
            m_1114 = 0;
            mMaxSpeedF = 0.0f;
            if (m_f8 == 0 && fabsf(mSpeedF) < 1.1f) {
                if (!mKey.buttonCrouch()) {
                    setSlipAction_ToEnd();
                } else {
                    setSlipAction_ToStoop();
                }
            }
        }
    } else {
        if (m_1114) {
            mAccelY = 0.0f;
            onStatus(STATUS_98);
        } else {
            offStatus(STATUS_98);
        }
    }
}

void daPlBase_c::fn_800488f0(int param) {
    if (
        (mKey.buttonLeft() && mDirection == 0 && mSpeedF > 0.0f) ||
        (mKey.buttonRight() &&mDirection == 1 && mSpeedF < 0.0f)
    ) {
        mAngle.y = getMukiAngle(mDirection);
        mDirection ^= 1;
        changeState(StateID_Turn, nullptr);
        return;
    }
    if (
        (mKey.buttonLeft() && mDirection == 0 && mSpeedF < 0.0f) ||
        (mKey.buttonRight() && mDirection == 1 && mSpeedF > 0.0f)
    ) {
        mAngle.y = getMukiAngle(mDirection);
        mDirection ^= 1;
    }
    changeState(StateID_Walk, (void *) param);
}

bool daPlBase_c::checkTurn() {
    if (!isCarry() && !isStatus(STATUS_74) && fabsf(mSpeedF) >= 2.5f) {
        if ((m_d40 & 0x800000) != 0) {
            if (
                (mSpeedF < 0.0f && mKey.buttonRight() && mDirection == 0) ||
                (mSpeedF > 0.0f && mKey.buttonLeft() && mDirection == 1)
            ) {
                changeState(StateID_Turn, nullptr);
                return true;
            }
        } else {
            changeState(StateID_Turn, nullptr);
            return true;
        }
    }
    return false;
}

void daPlBase_c::setTurnEnd() {
    mAngle.y = getMukiAngle(mDirection);
    changeState(StateID_Walk, nullptr);
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
    if ((m_d40 & 0x18000000) == 0) {
        setLandSmokeEffect(getTallType(-1));
    }
    if (m_d88 == T_5) {
        fn_80057e70(SE_PLY_HPDP_SPLASH, false);
    }
    if (mPowerup == POWERUP_MINI_MUSHROOM) {
        fn_80057e70(SE_PLY_HIP_ATTACK_M, false);
        return;
    }
    if ((m_d88 < T_0 || m_d88 > T_6) && (m_d88 < T_8 || m_d88 > T_A)) {
        switch (m_d88) {
            case T_7:
            case T_B:
                fn_80057e70(SE_PLY_HIP_ATTACK_SOFT, false);
                break;
            case T_C:
                fn_80057e70(SE_PLY_HIP_ATTACK, false);
                break;
            default:
                break;
        }
    }
}

void daPlBase_c::setHipAttackDropEffect() {
    static const float xOffsets[] = { 0.7f, 0.8f, 1.0f };
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    mVec3_c efPosOffset;
    efPosOffset.x = xOffsets[getTallType(-1)];
    efPosOffset.y = efPosOffset.x;
    efPosOffset.z = efPosOffset.x;
    dEf::createPlayerEffect(mPlayerNo, &mLevelEf1, "Wm_mr_drop", 0, &pos, nullptr, &efPosOffset);
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
    mpMdlMng->mpMdl->setAnm(16, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    mSpeed.y = 1.0f;
}

void daPlBase_c::setHipAttack_KinopioStart() {
    setHipAttack_AttackStart();
    onStatus(STATUS_7F);
    onStatus(STATUS_7A);
    if (isItemKinopio()) {
        fn_80057e70(SE_VOC_ITEM_KO_FOUND, false);
    }
}

void daPlBase_c::setHipAttack_AttackStart() {
    mSubstate = HIP_ACTION_ATTACK_START;
    mpMdlMng->mpMdl->setAnm(17, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    m_1114 = 5;
    mSpeed.y = 0.0f;
}

void daPlBase_c::setHipAttack_AttackFall() {
    mSubstate = HIP_ACTION_ATTACK_FALL;
    mAccelF = 0.1f;
    mMaxSpeedF = 0.0f;
    mAccelY = *getGravityData();
    mMaxFallSpeed = -6.0f;
    if ((int) mStateChangeParam == 1) {
        mSpeed.y = -2.0f;
    } else {
        mSpeed.y = -6.0f;
    }
    onStatus(STATUS_1C);
    setHipBlockBreak();
    m_1114 = 5;
}

void daPlBase_c::setHipAttack_StandNormal() {
    m_1114 = 20;
    mSubstate = HIP_ACTION_STAND_NORMAL;
    mpMdlMng->mpMdl->setAnm(18, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    onStatus(STATUS_1E);
    offStatus(STATUS_1C);
    onStatus(STATUS_9F);
    mAccelY = *getGravityData();
    mMaxFallSpeed = -6.0f;
    mSpeed.y = 0.0f;
}

void daPlBase_c::setHipAttack_StandNormalEnd() {
    mSubstate = HIP_ACTION_STAND_NORMAL_END;
    mpMdlMng->mpMdl->setAnm(19, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    offStatus(STATUS_1E);
}

void daPlBase_c::setHipAttack_ToStoop() {
    mSubstate = HIP_ACTION_TO_STOOP;
    mpMdlMng->mpMdl->setAnm(20, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    offStatus(STATUS_1E);
}

void daPlBase_c::HipAction_Ready() {
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        setHipAttack_AttackStart();
    }
}

void daPlBase_c::HipAction_AttackStart() {
    if (!m_1114) {
        setHipAttack_AttackFall();
    }
}

void daPlBase_c::HipAction_AttackFall() {
    setHipAttackDropEffect();
    if (m_1114 == 0 || m_d40 & 1) {
        offStatus(STATUS_7F);
        offStatus(STATUS_7A);
    }
    if ((m_d40 & 1) == 0) {
        if (m_d40 & 0x4000) {
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
        if (!isStatus(STATUS_22) && mKey.buttonUp()) {
            changeState(StateID_Fall, nullptr);
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
        onStatus(STATUS_1D);
        offStatus(STATUS_91);
        if (m_d44 & 0x100) {
            changeState(StateID_Kani, (void *) 2);
        } else if (isSlipSaka()) {
            if ((m_d40 & 0x8000) == 0) {
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
    if (!mKey.buttonDown() && (m_d40 & 1) == 0) {
        changeState(StateID_Fall, nullptr);
    } else {
        setHipAttack_StandNormal();
    }
}

void daPlBase_c::HipAction_StandNormal() {
    if (mpMdlMng->mpMdl->m_154 != 18) {
        mpMdlMng->mpMdl->setAnm(18, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    }
    if (m_d40 & 1) {
        if (mpMdlMng->mpMdl->mAnm.isStop()) {
            if (!mKey.buttonDown()) {
                offStatus(STATUS_AE);
                setHipAttack_StandNormalEnd();
            } else {
                setHipBlockBreak();
                if (m_344 == 0) {
                    if (m_d44 & 0x400000) {
                        m_1114 = 15;
                    }
                    if (dScStage_c::m_isStaffCredit && m_d40 & 1 && (m_d44 & 2) == 0) {
                        m_344 = 1;
                    }
                }
                if (m_1114 == 0) {
                    offStatus(STATUS_AE);
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
            HipAction_AttackFall();
        }
    }
}

void daPlBase_c::HipAction_StandNormalEnd() {
    if (!checkCrouch()) {
        if (mpMdlMng->mpMdl->mAnm.isStop()) {
            changeState(StateID_Walk, (void *) 1);
        } else {
            turnAngle();
        }
    }
}

void daPlBase_c::HipAction_ToStoop() {
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        changeState(StateID_Crouch, (void *) 1);
    } else {
        turnAngle();
    }
}

void daPlBase_c::initializeState_HipAttack() {
    mAngle.y = getMukiAngle(mDirection);
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mAccelY = 0.0f;
    switch ((int) mStateChangeParam) {
        case 0:
            setHipAttack_Ready();
            break;
        case 1:
            setHipAttack_KinopioStart();
            break;
    }
    onStatus(STATUS_A8);
    onStatus(STATUS_AA);
    onStatus(STATUS_AE);
}
void daPlBase_c::finalizeState_HipAttack() {
    mMaxFallSpeed = -4.0f;
    offStatus(STATUS_1C);
    offStatus(STATUS_A8);
    offStatus(STATUS_AA);
    offStatus(STATUS_C4);
    offStatus(STATUS_22);
    offStatus(STATUS_1D);
    offStatus(STATUS_1E);
    offStatus(STATUS_9F);
    offStatus(STATUS_AE);
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

    offStatus(STATUS_1D);
    if (isStatus(STATUS_1C)) {
        setCcAtHipAttack();
    }
    if (m_d40 & 0x10000) {
        offStatus(STATUS_AA);
    }
    if (mSubstate < HIP_ACTION_STAND_NORMAL || !checkJumpTrigger()) {
        (this->*l_HipActionProc[mSubstate])();
        if (isStatus(STATUS_22)) {
            offStatus(STATUS_22);
            mPos.x = m_111c.x;
            mPos.y = m_111c.y;
        }
    }
}

void daPlBase_c::initializeState_Swim() {}
void daPlBase_c::finalizeState_Swim() {}
void daPlBase_c::executeState_Swim() {}

bool daPlBase_c::setJumpDaiRide() {
    if (isStatus(STATUS_45) || m_d40 & 2) {
        return false;
    }
    changeState(StateID_JumpDai, nullptr);
    return true;
}

void daPlBase_c::initializeState_JumpDai() {
    onStatus(STATUS_15);
    onStatus(STATUS_86);
    onStatus(STATUS_14);
    mKey.onStatus(dAcPyKey_c::STATUS_NO_INPUT);
    mSpeed.y = 0.0f;
    mSpeedF = 0.0f;
    if (mpMdlMng->mpMdl->m_154 == 20) {
        mSubstate = JUMP_DAI_ACTION_1;
    } else {
        mpMdlMng->mpMdl->setAnm(7, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    }
}
void daPlBase_c::finalizeState_JumpDai() {
    offStatus(STATUS_14);
    offStatus(STATUS_15);
    mKey.offStatus(dAcPyKey_c::STATUS_NO_INPUT);
}
void daPlBase_c::executeState_JumpDai() {
    if (!isStatus(STATUS_86)) {
        changeState(StateID_Fall, nullptr);
    } else {
        turnAngle();
        if (mSubstate == JUMP_DAI_ACTION_0 && mpMdlMng->mpMdl->mAnm.isStop()) {
            vf378(1);
            mSubstate = JUMP_DAI_ACTION_1;
        }
    }
}

bool daPlBase_c::setPlayerJumpDai(daPlBase_c *other) {
    if (mRideActorID == 0) {
        float topPos = *(other->getHeadTopPosP() + 1) - 4.0f;
        mVec3_c pos = mVec3_c(
            mPos.x,
            topPos,
            mPos.z
        );
        float f;
        if (mBc.checkRoofPlayer(&pos, &f) && f < topPos + m_c9c) {
            return false;
        }
        mRideActorID = other->mUniqueID;
        changeState(StateID_PlayerJumpDai, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::fn_80049d70() {
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor == nullptr) {
        return;
    }
    mPos.set(
        m_348.x + rideActor->mPos.x,
        *(rideActor->getHeadTopPosP() + 1) - 4.0f,
        m_348.z + rideActor->mPos.z
    );
}

void daPlBase_c::initializeState_PlayerJumpDai() {
    onStatus(STATUS_16);
    onStatus(STATUS_14);
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor != nullptr) {
        rideActor->initStampReduction();
        setNoHitPlayer(rideActor, 5);
        m_348 = mPos - rideActor->mPos;
    }
    m_354 = mSpeedF;
    mSpeedF = 0.0f;
    mSpeed.y = 0.0f;
    if (mpMdlMng->mpMdl->m_154 != 20) {
        mpMdlMng->mpMdl->setAnm(7, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    }
    if (mKey.triggerJumpBuf(5)) {
        mSubstate = JUMP_DAI_ACTION_1;
    } else {
        mSubstate = JUMP_DAI_ACTION_0;
    }
    m_1118 = 0;
}
void daPlBase_c::finalizeState_PlayerJumpDai() {
    offStatus(STATUS_16);
    offStatus(STATUS_14);
    mRideActorID = (fBaseID_e) 0;
}
void daPlBase_c::executeState_PlayerJumpDai() {
    daPlBase_c *rideActor = (daPlBase_c *) fManager_c::searchBaseByID(mRideActorID);
    if (rideActor == nullptr) {
        changeState(StateID_Fall, nullptr);
    } else if (m_d40 & 2) {
        changeState(StateID_Fall, nullptr);
        m_358 = 30;
    } else {
        setNoHitPlayer(rideActor, 5);
        turnAngle();
        if (mpMdlMng->mpMdl->m_154 == 7 && mpMdlMng->mpMdl->mAnm.isStop()) {
            vf378(1);
        }
        switch ((JumpDaiSubstate_e) mSubstate) {
            case JUMP_DAI_ACTION_0:
                if (m_1118 < 5) {
                    if (mKey.triggerJump()) {
                        mSubstate = JUMP_DAI_ACTION_1;
                    }
                } else if (isMameAction()) {
                    vf3fc(3.278f, m_354, 1, 0, 0);
                    return;
                } else if (mKey.buttonJump()) {
                    vf3fc(3.828f, m_354, 1, 0, 2);
                    return;
                } else {
                    vf3fc(4.628f, m_354, 1, 2, 0);
                    return;
                }
                break;
            case JUMP_DAI_ACTION_1:
                if (m_1118 > 5) {
                    float f = 4.428f;
                    if (isMameAction()) {
                        f = 3.828f;
                    }
                    vf3fc(f, 0.0f, 1, 1, 2);
                    return;
                }
                break;
        }
        rideActor->calcJumpDaiReductionScale(m_1118, 5);
        m_1118++;
    }
}

bool daPlBase_c::setFunsui() {
    if (isDemo()) {
        return false;
    }
    if (!mStateMgr.getStateID()->isEqual(StateID_Funsui)) {
        changeState(StateID_Funsui, nullptr);
    }
    return true;
}

bool daPlBase_c::updateFunsuiPos(float x, float y) {
    if (mStateMgr.getStateID()->isEqual(StateID_Funsui)) {
        mPos.x = x;
        mPos.y = y;
        return true;
    }
    return false;
}

bool daPlBase_c::releaseFunsui(float f) {
    if (mStateMgr.getStateID()->isEqual(StateID_Funsui)) {
        mSpeed.y = f;
        mSubstate = FUNSUI_ACTION_START;
        mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
        offStatus(STATUS_7F);
    }
    return true;
}

void daPlBase_c::releaseFunsuiAction() {
    changeState(StateID_Fall, nullptr);
}

void daPlBase_c::initializeState_Funsui() {
    onStatus(STATUS_7F);
    mAccelY = 0.0f;
    mSpeedF *= 0.7f;
    mSpeed.y = 0.0f;
    mSubstate = FUNSUI_ACTION_NONE;
    mpMdlMng->mpMdl->setAnm(138, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    if (mPlayerNo >= 0) {
        vf434(50, 0);
        dQuake_c::m_instance->shockMotor(mPlayerNo, 7, 0, 0);
        m_1114 = 8;
    }
}
void daPlBase_c::finalizeState_Funsui() {
    offStatus(STATUS_7F);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}
void daPlBase_c::executeState_Funsui() {
    if (mPlayerNo >= 0) {
        dEf::createPlayerEffect(mPlayerNo, &mLevelEfs2, "Wm_mr_sprisesmoke", 0, &mPos, nullptr, nullptr);
        if (m_1114 == 0) {
            m_1114 = 8;
            dQuake_c::m_instance->shockMotor(mPlayerNo, 8, 0, 0);
        }
    }
    turnAngle();
    if (mSubstate == FUNSUI_ACTION_START) {
        gravitySet();
        maxFallSpeedSet();
        if (mSpeed.y <= 0.0f || m_d40 & 2) {
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
    if (fabsf(mSpeedF) > 0.35f) {
        mAccelF = 0.1f;
    } else {
        mAccelF = 0.04f;
    }
}

void daPlBase_c::initializeState_Kani() {}
void daPlBase_c::finalizeState_Kani() {}
void daPlBase_c::executeState_Kani() {}

bool daPlBase_c::setCloudOn(dActor_c *cloudActor) {
    if (m_d40 & 1 || isStatus(STATUS_7E) || isStatus(STATUS_33)) {
        return false;
    }
    if (cloudActor->mPos.y + getCloudOffsetY() >= mPos.y && mSpeed.y < 0.0f && mRideActorID == 0) {
        mRideActorID = cloudActor->mUniqueID;
        changeState(StateID_Cloud, nullptr);
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
    fn_80057e70(SE_PLY_RIDE_CLOUD, 0);
    onStatus(STATUS_4E);
    mAccelY = 0.0f;
    mSpeed.y = 0.1f;
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mpMdlMng->mpMdl->setAnm(7, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
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
        changeState(StateID_Fall, nullptr);
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
    if (!isStatus(STATUS_2B)) {
        mAngle = rideActor->mAngle;
        mDirection = rideActor->mDirection;
    }
    return false;
}

void daPlBase_c::executeState_Cloud() {}

void daPlBase_c::initializeState_AnimePlay() {
    m_35c = (int) mStateChangeParam;
    if (m_35c) {
        onStatus(STATUS_03);
    }
}

void daPlBase_c::finalizeState_AnimePlay() {
    offStatus(STATUS_03);
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
    if (isStatus(STATUS_03) || checkWalkNextAction() == 0) {
        (this->*scDemoAnmFunc[m_35c])();
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
        mpMdlMng->mpMdl->setAnm(147, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
        mSubstate++;
        offStatus(STATUS_03);
    }
}

void daPlBase_c::DemoAnmBossGlad() {
    addCalcAngleY(0, 10);
    switch ((AnimePlaySubstate_e) mSubstate) {
        case ANIME_PLAY_ACTION_0:
            m_1114 = 15;
            mSubstate++;
            // fallthrough
        case ANIME_PLAY_ACTION_1:
            if (m_1114 == 0) {
                initDemoKimePose();
                mSubstate++;
            }
            break;
        case ANIME_PLAY_ACTION_2: {
            int arg = 1;
            if (m_35c == 5) {
                arg = 2;
            }
            if (!vf284(arg)) {
                break;
            }
            mpMdlMng->mpMdl->setAnm(148, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
            mSubstate++;
            offStatus(STATUS_03);
            break;
        }
        default:
            break;
    }
}

void daPlBase_c::DemoAnmBossAttention() {
    daPlBase_c *boss = (daPlBase_c *) dAttention_c::mspInstance->search(mpMdlMng->mpMdl->m_cc);
    if (boss != nullptr) {
        if (boss->getLookatPos().x > mPos.x) {
            mDirection = 0;
        } else {
            mDirection = 1;
        }
    }
    switch (mSubstate) {
        default:
            break;
        case HIP_ACTION_READY:
            if (!mDirection) {
                mpMdlMng->mpMdl->setAnm(145, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
            } else {
                mpMdlMng->mpMdl->setAnm(144, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
            }
            mSubstate++;
            break;
        case HIP_ACTION_ATTACK_START:
            if (sLib::chase(&mAngle.y, getMukiAngle(mDirection), 0x800)) {
                mSubstate++;
                offStatus(STATUS_03);
            }
            break;
    }
}

void daPlBase_c::DemoAnmBossKeyGet() {
    addCalcAngleY(0, 10);
    switch ((AnimePlaySubstate_e) mSubstate) {
        case ANIME_PLAY_ACTION_0:
            m_1114 = 15;
            mSubstate++;
        case ANIME_PLAY_ACTION_1:
            if (m_1114 == 0) {
                mpMdlMng->mpMdl->setAnm(148, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                fn_80051d00(1);
                mSubstate++;
                offStatus(STATUS_03);
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::initializeState_WaitJump() {
    startQuakeShock(5);
    onStatus(STATUS_12);
    mKey.onStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
    mSpeedF = 0.0f;
    mpMdlMng->mpMdl->setAnm(0, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    mAngle.y = getMukiAngle(mDirection);
}

void daPlBase_c::finalizeState_WaitJump() {
    offStatus(STATUS_12);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

void daPlBase_c::executeState_WaitJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    if (mSpeed.y <= 0.0f) {
        changeState(StateID_Fall, nullptr);
    }
}

bool daPlBase_c::checkSakaReverse() {
    if (fabsf(mSpeedF) < 0.5f && mBc.mPlayerFlags & 4) {
        return true;
    }
    return false;
}

bool daPlBase_c::isSaka() {
    if (m_d44 & 0x80) {
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
    m_d94 = mBc.getSakaAngle(mSpeedF);
    changeState(StateID_Slip, nullptr);
}

bool daPlBase_c::checkSlip() {
    if ((mSpeedF > 0.0f || (m_d40 & 0x80000) == 0) && (m_d44 & 8)) {
        setSlipAction();
        return true;
    }
    return false;
}

bool daPlBase_c::checkCrouchSlip() {
    if (m_d40 & 0x4000) {
        return false;
    }
    if ((mSpeedF > 0.0f || (m_d40 & 0x80000) == 0) && ((m_d44 & 8) || isSlipSaka())) {
        setSlipAction();
        return true;
    }
    return false;
}

bool daPlBase_c::checkSlipEndKey() {
    if ((m_d40 & 1)) {
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
    return l_sakaSlipOffSpeed[mBc.getSakaDir()][mBc.getSakaType()];
}

bool daPlBase_c::setSandMoveSpeed() {
    if ((m_d40 & 0x18000000)) {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            if (isStatus(STATUS_2B)) {
                mMaxSpeedF = sc_DirSpeed[dir];
            } else {
                mMaxSpeedF = sc_DirSpeed[dir] * 0.5f;
            }
        } else {
            mMaxSpeedF = 0.0f;
        }
        if (mSpeedF * mMaxSpeedF >= 0.0f) {
            if (fabsf(mSpeedF) > fabsf(mMaxSpeedF)) {
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
            if ((m_d40 & 1) == 0) {
                float absSpeed = absSpeedF();
                float speed1 = sc_DirSpeed[dir] * *(getSpeedData() + 0);
                float speed2 = sc_DirSpeed[dir] * *(getSpeedData() + 2);
                float tmp = sc_DirSpeed[dir] * absSpeed;
                if (absSpeed >= fabsf(speed2) || mKey.buttonDush()) {
                    mMaxSpeedF = speed2;
                } else if (absSpeed > fabsf(speed1)) {
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
            } else if (getPowerChangeType(false) == POWER_CHANGE_1) {
                mMaxSpeedF = getIceSakaSlipOffSpeed();
            }
        }
        if (!(m_d40 & 1) && !isStatus(STATUS_88) && absSpeedF() > *(getSpeedData() + 2)) {
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
    if (m_d40 & 0x18000000) {
        mAccelF = *(getSpeedData() + 7);
    } else if (isStatus(STATUS_30)) {
        slipPowerSet(1);
    } else {
        slipPowerSet(0);
    }
}

void daPlBase_c::slipPowerSet(int mode) {
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
                    mAccelF = calcStarAccel(mAccelF);
                }
            } else if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
                mAccelF = *(getSpeedData() + 6);
                if (isStatus(STATUS_89)) {
                    mAccelF = calcStarAccel(mAccelF);
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
    if (absSpeedF() > *(getSpeedData() + 2)) {
        mAccelF = 0.75f;
        return;
    }
    PowerChangeType_e powerChangeType = getPowerChangeType(false);
    SpeedData_t data;
    fn_8004bf80(&data);
    if (!mKey.buttonWalk(nullptr)) {
        if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
            mAccelF = data.data[2];
        } else if (absSpeedF() < *getSpeedData()) {
            mAccelF = data.data[1];
        } else {
            mAccelF = data.data[0];
        }
        if (powerChangeType == POWER_CHANGE_1 && absSpeedF() < 0.5f) {
            mAccelF = 0.004f;
        }
        if (isStatus(STATUS_89)) {
            mAccelF = calcStarAccel(mAccelF);
        }
        return;
    }
    if (mSpeedF * sc_DirSpeed[mDirection] < 0.0f) {
        mAccelF = data.data[3];
        if (isStatus(STATUS_89)) {
            mAccelF = calcStarAccel(mAccelF);
        }
        return;
    }

    float absMaxSpeed = absMaxSpeedF();
    float absSpeed = absSpeedF();

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

daPlBase_c::PowerChangeType_e daPlBase_c::getPowerChangeType(bool mode) {
    if (!mode && isStatus(STATUS_AD)) {
        return POWER_CHANGE_0;
    }
    if (m_d40 & 0x800000) {
        return POWER_CHANGE_1;
    }
    if (m_d40 & 0x1000000) {
        return POWER_CHANGE_2;
    }
    if (m_d40 & 0x400000) {
        return POWER_CHANGE_2;
    }
    return POWER_CHANGE_0;
}

void daPlBase_c::fn_8004bf80(daPlBase_c::SpeedData_t *data) {
    switch (getPowerChangeType(false)) {
        case POWER_CHANGE_0:
            *data = *(SpeedData_t *) (getSpeedData() + 3);
            break;
        case POWER_CHANGE_1:
            *data = *(SpeedData_t *) (getSpeedData() + 12);
            break;
        case POWER_CHANGE_2:
            *data = *(SpeedData_t *) (getSpeedData() + 21);
            break;
    }
}

void daPlBase_c::icePowerChange(int mode) {
    PowerChangeType_e powerChangeType = getPowerChangeType(false);
    if (powerChangeType == POWER_CHANGE_1 || (powerChangeType == POWER_CHANGE_2 && mode == 1)) {
        if (mMaxSpeedF) {
            if (mSpeedF * mMaxSpeedF < 0.0f) {
                mAccelF = mAccelF * 0.375f;
            } else if (mode == 0) {
                if (isSaka()) {
                    mAccelF = mAccelF * 0.25f;
                } else if (absSpeedF() < 0.5f) {
                    mAccelF = mAccelF * 0.375f;
                }
            }
        } else if (!isSaka() && absSpeedF() < 0.5f) {
            mAccelF = 0.004;
        } else {
            mAccelF = mAccelF * 0.375f;
        }
    }
}

extern float lbl_802f5880[4][7];

void daPlBase_c::airPowerSet() {
    int dir;
    bool idx = isStar();
    float *data = lbl_802f5880[idx];
    if (mKey.buttonWalk(&dir)) {
        if (mSpeedF * sc_DirSpeed[dir] < 0.0f) {
            mAccelF = data[6];
            return;
        } else if (absSpeedF() < 0.5f) {
            mAccelF = data[1];
            return;
        }

        float absSpeed = absSpeedF();

        if (absSpeed < *(getSpeedData() + 0)) {
            if (mKey.buttonDush()) {
                mAccelF = data[3];
            } else {
                mAccelF = data[2];
            }
        } else {
            if (absSpeed < *(getSpeedData() + 1)) {
                mAccelF = data[4];
            } else {
                mAccelF = data[5];
            }
        }
    } else {
        mAccelF = data[0];
    }
}

void daPlBase_c::powerSet() {
    if (m_d40 & 1) {
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
    if (m_d40 & 1) {
        mAccelY = *getGravityData();
    } else {
        setJumpGravity();
    }
}

void daPlBase_c::setJumpGravity() {
    if (m_cd8) {
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
        m_112c = 0.0f;
        return;
    }
    if (isStatus(STATUS_A0) && (m_d40 & 0x18000000) == 0) {
        float tmp = fn_8004c700(windActor->m_00);
        float halfTmp = tmp * 0.5f;
        tmp = fabs(tmp);
        tmp = tmp * 3.0f;
        if (m_112c > 0.0f && m_d40 & 0x40 || m_112c < 0.0f && m_d40 & 0x20) {
            m_112c = 0.0f;
        }
        if (m_d40 & 1) {
            if (isStatus(STATUS_43)) {
                offStatus(STATUS_43);
                mSpeedF += m_112c;
                m_112c = 0.0f;
            }
            float scale;
            if (isStatus(STATUS_5B)) {
                tmp = 0.45f * tmp;
            } else {
                tmp = 0.55f * tmp;
            }
            m_1128 = 3;
            m_112c += halfTmp;
        } else {
            if (!isStatus(STATUS_43)) {
                onStatus(STATUS_43);
            }
            if (m_1128) {
                m_1128--;
            } else if (mSpeedF * m_112c < 0.0f) {
                mSpeedF += halfTmp;
            } else {
                m_112c += halfTmp;
            }
        }
        if (m_112c > tmp) {
            m_112c = tmp;
        } else if (m_112c < -tmp) {
            m_112c = -tmp;
        }
    } else {
        m_112c = 0.0f;
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
    startFootSoundPlayer(scLandSeID[m_d88]);
}

void daPlBase_c::setSlipSE() {
    if (m_d88 == POWERUP_PENGUIN_SUIT) {
        fn_80057fd0(SE_PLY_PNGN_SLIP_SEA, (short) fabsf(mSpeedF), false);
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
    fn_80057f60(scSlipSeID[m_d88], false);
}

void daPlBase_c::setLandSmokeEffect(int param1) {
    static const char *csLandLookup[][3] = {
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
    if (m_d88 == 7) {
        setSandFunsuiLandEffect();
    } else if (m_d88 == 5) {
        PLAYER_POWERUP_e powerup = mPowerup;
        float sz = 1.0f;
        if (powerup == POWERUP_MINI_MUSHROOM) {
            sz = 0.6f;
        } else if (powerup == POWERUP_NONE) {
            sz = 0.8f;
        }
        mVec3_c size(sz, sz, sz);
        dEf::createPlayerEffect(mPlayerNo, csLandLookup[m_d88][param1], 0, &mPos, nullptr, &size);
    } else {
        dEf::createPlayerEffect_change(mPlayerNo, csLandLookup[m_d88][param1], 0, &mPos, nullptr, nullptr);
    }
}

void daPlBase_c::setLandSmokeEffectLight() {
    mVec3_c pos = mPos;
    if (dMaskMng::isCaveMask() && mLayer == 0) {
        if (m_ca1 == 1) {
            pos.z = 3700.0f;
        }
    }
    float sz = ((float *) &dPyMdlMng_c::m_hio)[mpMdlMng->mpMdl->m_154];
    mVec3_c size(sz, sz, sz);
    if (m_d40 & 0x4000000) {
        if (m_d88 == 7) {
            setSandFunsuiLandEffect();
        } else if ((m_d40 & 0x18000000) == 0) {
            dEf::createPlayerEffect(mPlayerNo, "Wm_mr_cmnsndlandsmk", 0, &pos, nullptr, &size);
        }
    } else {
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_cmnlandsmoke", 0, &pos, nullptr, &size);
    }
}

bool daPlBase_c::setSandFunsuiLandEffect() {
    if (m_d88 == 7) {
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_spsmoke", 0, &mPos, nullptr, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::setStartJumpEffect(int param1) {
    if (m_d40 & 1) {
        if (m_d40 & 0x18000000) {
            setSandJumpEffect();
            onStatus(STATUS_0E);
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
    static const float scEffectScale[] = { 0.5f, 0.8f, 1.0f };
    float sz = scEffectScale[getTallType(-1)];
    mVec3_c size(sz, sz, sz);
    mVec3_c pos(
        mPos.x,
        mPos.y + sz * 10.0f,
        mPos.z
    );
    dEf::createPlayerEffect(mPlayerNo, effect, "Wm_mr_foot_water", 0, &pos, nullptr, &size);
}

void daPlBase_c::setSlipSmokeEffect() {
    static const char *csSlipSmokeEffectNames[][2] = {
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

    if (m_d88 == 5) {
        setSlipOnWaterEffect(&mLevelEfs3);
        return;
    }
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    int idx = 0;
    if (mPowerup != POWERUP_MINI_MUSHROOM) {
        idx = 1;
    }
    dEf::createPlayerEffect_change(mPlayerNo, &mLevelEfs3, csSlipSmokeEffectNames[m_d88][idx], 0, &pos, nullptr, nullptr);
}

void daPlBase_c::setBrakeSmokeEffect(mVec3_c &offset) {
    static const char *csBrakeSmokeEffectNames[][2] = {
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

    if (m_d88 == 5) {
        setSlipOnWaterEffect(&mLevelEfs4);
        return;
    }
    int idx = 0;
    if (mPowerup != POWERUP_MINI_MUSHROOM) {
        idx = 1;
    }
    dEf::createPlayerEffect_change(mPlayerNo, &mLevelEfs4, csBrakeSmokeEffectNames[m_d88][idx], 0, &offset, nullptr, nullptr);
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
        fn_80057f60(scTurnSeID[m_d88], 0);
    }
    static const char *scTurnSmokeNames[][2] = {
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
    static const float scEffectScale[] = { 0.5f, 0.8f, 1.0f };
    mVec3_c pos;
    mpMdlMng->mpMdl->getJointPos(&pos, 1);
    if (m_d88 == 5) {
        if (mPos.y < m_da4 - 4.0f) {
            fadeOutTurnEffect();
            return;
        }
        pos.y = m_da4;
    }
    float sz = scEffectScale[getTallType(-1)];
    mVec3_c size(sz, sz, sz);
    if (mFollowEf.m_118 == 1 && mFollowEf.m_114 == m_d88) {
        mFollowEf.follow(&pos, 0, 0);
    } else {
        dEf::createPlayerEffect(mPlayerNo, &mFollowEf, scTurnSmokeNames[m_d88][mDirection], 0, &pos, nullptr, &size);
        mFollowEf.m_114 = m_d88;
        mFollowEf.m_118 = 1;
    }
}

void daPlBase_c::fadeOutTurnEffect() {
    if (mFollowEf.m_118 != 1) {
        return;
    }
    mFollowEf.followFade();
    mFollowEf.m_118 = 0;
}

void daPlBase_c::setRunFootEffect() {
    static const char *scRunEffectNames[] = {
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
    if ((m_d40 & 0x18000000) == 0 && isStatus(STATUS_62)) {
        switch (m_d88) {
            default:
                break;
            case T_1:
            case T_2:
            case T_3:
            case T_5:
            case T_7:
            case T_9:
                mVec3_c pos;
                mpMdlMng->mpMdl->getJointPos(&pos, 1);
                static const float scEffectScale[] = { 0.5f, 0.8f, 1.0f };
                float sz = scEffectScale[getTallType(-1)];
                mVec3_c size(sz, sz, sz);
                dEf::createPlayerEffect(mPlayerNo, &mLevelEfs5, scRunEffectNames[m_d88], 0, &pos, nullptr, &size);
                break;
        }
    }
}

void daPlBase_c::setSandEffect() {
    if (isStatus(STATUS_4E) || isStatus(STATUS_4B)) {
        return;
    }
    if (m_d40 & 0x18000000 && (m_d40 & 0x20000000) == 0 || isStatus(STATUS_0E)) {
        mVec3_c pos;
        mpMdlMng->mpMdl->getJointPos(&pos, 8);
        dEf::createPlayerEffect(mPlayerNo, &mLevelEfs6, "Wm_mr_sandsplash", 0, &pos, nullptr, nullptr);
    }
    if (m_d40 & 0x10000000) {
        mVec3_c pos = getCenterPos();
        dEf::createPlayerEffect(mPlayerNo, &mLevelEfs7, "Wm_mr_quicksand", 0, &pos, nullptr, nullptr);
    }
    if (m_d40 & 0x18000000 && (m_d48 & 0x18000000) == 0) {
        if (mPos.y > m_db0 - 8.0f && m_cc0 < 0.0f) {
            int idx = 2;
            if ((m_cc0 < -4.0f || m_cc8 > m_db0 + 58.0f) && mPowerup != POWERUP_MINI_MUSHROOM) {
                mSpeedF = 0.0f;
                idx = (mPowerup == POWERUP_NONE);
            }
            mVec3_c pos(
                mPos.x,
                m_db0,
                mPos.z
            );
            static const char *scSandJumpEffectNames[] = {
                "Wm_mr_sanddive",
                "Wm_mr_sanddive_m",
                "Wm_mr_sanddive_s"
            };
            dEf::createPlayerEffect(mPlayerNo, scSandJumpEffectNames[idx], 0, &pos, nullptr, nullptr);
        }
    }
}

bool daPlBase_c::setSandJumpEffect() {
    if (m_d40 & 0x18000000 && m_c9c + mPos.y + 16.0f > m_db0) {
        mVec3_c pos(mPos.x, m_db0, mPos.z);
        dEf::createPlayerEffect(mPlayerNo, "Wm_mr_sanddive_s", 0, &pos, nullptr, nullptr);
        return true;
    }
    return false;
}

void daPlBase_c::setSoundPlyMode() {
    static const int csSoundPlyMode[] = {
        0, 1, 2, 3, 4, 5, 1
    };
    mSndObj.m_b0 = csSoundPlyMode[mPowerup];
}

void daPlBase_c::setFootSound() {
    if ((isDemo() || (m_d40 & 1)) && dScStage_c::m_gameMode != 2 && mpMdlMng->mpMdl->isFootStepTiming()) {
        if (mPowerup == POWERUP_PENGUIN_SUIT) {
            startFootSoundPlayer(SE_PLY_FOOTNOTE_PNGN);
            return;
        }
        static const dAudio::SoundEffectID_t scFootSoundID[] = {
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
        startFootSoundPlayer(scFootSoundID[m_d88]);
    }
}

STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoNone);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoStartWait);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoWait);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanU);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanD);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanR);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoInDokanL);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanU);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanD);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanR);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanL);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutDokanRoll);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoInWaterTank);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoOutWaterTank);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoRailDokan);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoDown);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoNextGotoBlock);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoGoal);
STATE_BASE_VIRTUAL_DEFINE(daPlBase_c, DemoControl);
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
    changeDemoState(StateID_DemoInDokanD, 0);
}
void daPlBase_c::initialDokanUper() {
    changeDemoState(StateID_DemoInDokanU, 0);
}
void daPlBase_c::initialDokanRight() {
    changeDemoState(StateID_DemoInDokanR, 0);
}
void daPlBase_c::initialDokanLeft() {
    changeDemoState(StateID_DemoInDokanL, 0);
}
void daPlBase_c::initialDokanUnderM() {
    changeDemoState(StateID_DemoOutDokanD, 2);
}
void daPlBase_c::initialDokanUperM() {
    changeDemoState(StateID_DemoOutDokanU, 2);
}
void daPlBase_c::initialDokanRightM() {
    changeDemoState(StateID_DemoOutDokanR, 2);
}
void daPlBase_c::initialDokanLeftM() {
    changeDemoState(StateID_DemoOutDokanL, 2);
}
void daPlBase_c::initialDokanDepth() {
    changeDemoState(StateID_DemoInWaterTank, 0);
}
void daPlBase_c::initialDoor() {}
void daPlBase_c::initialJumpRight() {}
void daPlBase_c::initialJumpLeft() {}
void daPlBase_c::initialVine() {}

void daPlBase_c::initialFall() {
    changeState(StateID_Fall, 0);
    changeDemoState(StateID_DemoNone, 0);
    mSpeed.y = -1.0f;
    bgCheck(0);
}

void daPlBase_c::initialHipAttack() {
    changeState(StateID_HipAttack, 0);
    changeDemoState(StateID_DemoWait, 0);
}

void daPlBase_c::initialSlip() {
    onStatus(STATUS_3E);
    setSlipAction();
    changeDemoState(StateID_DemoWait, 1);
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
    changeDemoState(StateID_DemoStartWait, 0);
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
    // bool res = false;
    switch (type) {
        case DEMO_1:
            if (isDemoMode() && isStatus(STATUS_04)) {
                return true;
            }
            break;
        case DEMO_2:
            if (isChange()) {
                return true;
            }
            break;
        case DEMO_3:
            if (isStatus(STATUS_76) || isDemoMode()) {
                return true;
            }
            break;
        case DEMO_4:
            if (!isItemKinopio() && (isStatus(STATUS_72) || isStatus(STATUS_71)) && m_60 <= 4) {
                return true;
            }
            break;
        case DEMO_5:
            if (isItemKinopio() && (isStatus(STATUS_72) || isStatus(STATUS_71)) && m_60 <= 7) {
                return true;
            }
            break;
        case DEMO_6:
            if ((isStatus(STATUS_72) || isStatus(STATUS_71)) && (s8) m_60 == 8) {
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
    if (isDemoType(DEMO_4) || isDemoType(DEMO_5) || isDemoType(DEMO_6)) {
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
    return mDemoMode;
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
    mDemoMode = true;
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
    mDemoMode = false;
}

void daPlBase_c::changeNextScene(int param1) {
    onStatus(STATUS_64);
    daPyMng_c::mPauseDisable = true;
    daPyDemoMng_c::mspInstance->setCourseOutList(mPlayerNo);
    if (mPlayerNo == daPyDemoMng_c::mspInstance->m_70) {
        dNext_c::m_instance->m_19 = true;
        daPyDemoMng_c::mspInstance->mPlNo = mPlayerNo;
    }
    if (param1 == 1) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            pl->onStatus(STATUS_7D);
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->onStatus(STATUS_7D);
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
            pl->mStopMask &= ~2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->mStopMask &= ~2;
        }
    } else if (mKind == 2) {
        mStopMask &= ~4;
    }
}

void daPlBase_c::playOther() {
    dActor_c::mExecStopReq &= ~0xf;
    if (mPlayerNo != -1) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            pl->mStopMask |= 2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(mPlayerNo);
        if (yoshi != nullptr) {
            yoshi->mStopMask |= 2;
        }
    } else if (mKind == 2) {
        mStopMask |= 4;
    }
}

void daPlBase_c::changeNormalAction() {
    bgCheck(0);
    offZPosSetNone();
    changeState(StateID_Walk, (void *) 1);
    changeDemoState(StateID_DemoNone, 0);
}

bool daPlBase_c::checkTimeOut() {
    if (dStageTimer_c::m_instance->convertToIGT() == 0) {
        if (setTimeOverDemo()) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::changeDemoState(const sStateIDIf_c &stateID, int param) {
    if (stateID == StateID_DemoNone && isItemKinopio()) {
        changeDemoState(StateID_DemoControl, 5);
    } else {
        onDemo();
        mDemoSubstate = 0;
        mDemoStateChangeParam = (void *) param;
        mDemoStateMgr.changeState(stateID);
    }
}

bool daPlBase_c::executeDemoState() {
    offStatus(STATUS_79);
    if (m_10c8 != 0) {
        m_10c8--;
    }
    mDemoStateMgr.executeState();
    if (isStatus(STATUS_71)) {
        if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoNone)) {
            changeDemoState(StateID_DemoControl, 0);
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
    m_10c8 = 15;
    daPyDemoMng_c::mspInstance->clearDemoNo(mPlayerNo);
    offDemo();
    if ((int) mDemoStateChangeParam != 1) {
        playOther();
    }
    offStatus(STATUS_BB);
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
    if (m_d40 & 1 || isStatus(STATUS_3A)) {
        if (mKey.buttonRight()) {
            m_8d++;
            if (m_8d > 10) {
                m_8d = 10;
            }
        } else {
            m_8d = 0;
        }
        if (mKey.buttonRight()) {
            m_8c++;
            if (m_8c > 10) {
                m_8c = 10;
            }
        } else {
            m_8c = 0;
        }
    } else {
        m_8d = 0;
        m_8c = 0;
    }
    if (m_10c8 == 0 && isEnableDokanInStatus()) {
        if (setDokanIn(1)) {
            return;
        }
        if (setDokanIn(0)) {
            return;
        }
        if (m_8d >= 10 && mDirection == 0) {
            if (setDokanIn(3)) {
                return;
            }
        }
        if (m_8c >= 10 && mDirection == 1) {
            if (setDokanIn(2)) {
                return;
            }
        }
        if (mKey.buttonDown()) {
            onStatus(STATUS_A4);
        }
    }
}

void daPlBase_c::initializeState_DemoStartWait() {}
void daPlBase_c::finalizeState_DemoStartWait() {}
void daPlBase_c::executeState_DemoStartWait() {
    if (dScStage_c::m_gameMode == 2){
        changeDemoState(StateID_DemoNone, 0);
    } else {
        if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
            bgCheck(0);
            changeDemoState(StateID_DemoNone, 0);
        }
    }
}

void daPlBase_c::initializeState_DemoWait() {
    if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
        mDemoSubstate = 1;
        m_10c8 = 0;
    } else {
        mDemoSubstate = 0;
    }
}
void daPlBase_c::finalizeState_DemoWait() {}
void daPlBase_c::executeState_DemoWait() {
    if (mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
        switch (mDemoSubstate) {
            case 0:
                if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                    mDemoSubstate = 1;
                    m_10c8 = 10;
                }
                break;
            case 1:
                if (m_10c8 == 0) {
                    daPyDemoMng_c::mspInstance->turnNextDemoNo();
                    switch ((int) mDemoStateChangeParam) {
                        case 0:
                            changeDemoState(StateID_DemoNone, 0);
                            onDemo();
                            bgCheck(0);
                            break;
                        case 1:
                            changeDemoState(StateID_DemoControl, 4);
                            break;
                    }
                }
                break;
        }
    }
}

float daPlBase_c::getWaterDokanCenterOffset(float param1) {
    static const float l_maxOffsets[] = { 12.0f, 15.0f, 13.0f, 13.0f };
    float max = 16.0f;
    if (mKind == 1) {
        max = l_maxOffsets[m_1140];
    }
    float pos = getYCenter() - mPos.y;
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
    if ((int) mDemoStateChangeParam == 1) {
        m_ce0 = 0;
    } else {
        m_ce0 = 35;
        if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
            stopOther();
        }
    }
    mDemoSubstate = DEMO_IN_DOKAN_ACTION_0;
}

void daPlBase_c::endDemoInDokan() {
    m_ce0 = 0;
    m_ce4 = 0;
    offStatus(STATUS_C1);
    offStatus(STATUS_2A);
    offStatus(STATUS_5E);
}

void daPlBase_c::executeDemoInDokan(u8 dir) {
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0:
            if (!mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                break;
            }
            if (!daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                break;
            }
            offStatus(STATUS_BB);
            mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
            m_10c8 = 40;
            if (mBc.checkWater(m_68.x, m_68.y, mLayer, nullptr)) {
                m_d40 |= 0x4000;
            }
            if (m_d40 & 0x4000) {
                if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoInDokanL) ||
                    mDemoStateMgr.getStateID()->isEqual(StateID_DemoInDokanR)
                ) {
                    mpMdlMng->mpMdl->setAnm(132, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                    if ((int) mDemoStateChangeParam != 1) {
                        mPos.y = getWaterDokanCenterOffset(mPos.y);
                        m_68.y = mPos.y;
                    }
                }
                fn_80057e70(SE_PLY_WATER_DOKAN_IN_OUT, false);
                break;
            }
            fn_80057e70(SE_PLY_DOKAN_IN_OUT, false);
            break;
        case DEMO_IN_DOKAN_ACTION_1:
            if (m_10c8 == 0) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_2;
            }
        case DEMO_IN_DOKAN_ACTION_2:
            onStatus(STATUS_79);
            if (isStatus(STATUS_2A) && (dir == 2 || dir == 3) && fabsf(mPos.x - m_68.x) <= 20.0f) {
                offStatus(STATUS_2A);
            }
            if (isStatus(STATUS_5E) && fabsf(mPos.x - m_68.x) <= 20.0f) {
                offStatus(STATUS_5E);
            }
            if (!demo_dokan_move_x(0.75f, 0.0f)) {
                break;
            }
            if (!demo_dokan_move_y(0.75f, 0.0f)) {
                break;
            }
            if (mDemoSubstate == DEMO_IN_DOKAN_ACTION_1) {
                daPyDemoMng_c::mspInstance->turnNextDemoNo();
            }
            mDemoSubstate = DEMO_IN_DOKAN_ACTION_3;
            break;
        case DEMO_IN_DOKAN_ACTION_3:
            if (!mKey.buttonWalk(nullptr)) {
                if (!sLib::chase(&mAngle.y, getMukiAngle(mDirection), scDokanOutTurnSpeed[0])) {
                    break;
                }
            }
            changeNormalAction();
            if (m_d40 & 0x4000) {
                m_d48 |= 0x4000;
            }
            break;
    }
}

void daPlBase_c::initDemoInDokanUD(u8 dir) {
    static const float tmps[] = { 34.0f, 36.0f, 38.0f, 38.0f };
    static const float tmps_big[] = { 40.0f, 42.0f, 44.0f, 44.0f };
    mpMdlMng->mpMdl->setAnm(0, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    m_68 = mPos;
    if (dir == 1) {
        if ((int) mDemoStateChangeParam == 2) {
            m_68.y = mPos.y + 16.0f;
        } else {
            m_68.y = mPos.y + 32.0f;
        }
        if (mKind == 2) {
            mPos.y -= 16.0f;
        } else {
            mPos.y -= 32.0f;
        }
    } else {
        onStatus(STATUS_2A);
        float tmp;
        if (mKind == 2) {
            tmp = 30.0f;
            daPlBase_c *pl = ((daYoshi_c *) this)->getPlayerRideOn();
            if (pl != nullptr) {
                int t = pl->m_1140;
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
                    tmp = m_c9c;
                    tmp = tmp + 8.0f;
                    break;
                default:
                    tmp = m_c9c;
                    break;
            }
        }
        m_68.y = -tmp + mPos.y;
        mPos.y += 2.0f;
    }
    if (mKind == 2) {
        mAngle.y = 0;
    }
    initDemoInDokan();
}

void daPlBase_c::initDemoInDokanLR(u8 dir) {
    static const float tmps[] = { 32.0f, 32.0f, 20.0f };
    mpMdlMng->mpMdl->setAnm(131, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    onStatus(STATUS_2A);
    if (dir == 3) {
        mPos.x += 8.0f;
    } else {
        mPos.x -= 8.0f;
    }
    float tmp;
    if (mKind == 2) {
        tmp = 32.0f;
    } else {
        tmp = tmps[(int) mDemoStateChangeParam];
    }
    if (dir == 3) {
        tmp = -tmp;
    }
    m_68.set(mPos.x + tmp, mPos.y, mPos.z);
    if (dir == 3) {
        mDirection = 1;
    } else {
        mDirection = 0;
    }
    mAngle.y = getMukiAngle(mDirection);
    initDemoInDokan();
}

void daPlBase_c::initializeState_DemoInDokanU() { initDemoInDokanUD(0); }
void daPlBase_c::finalizeState_DemoInDokanU() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanU() { executeDemoInDokan(0); }

void daPlBase_c::initializeState_DemoInDokanD() { initDemoInDokanUD(1); }
void daPlBase_c::finalizeState_DemoInDokanD() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanD() { executeDemoInDokan(1); }

void daPlBase_c::initializeState_DemoInDokanL() { initDemoInDokanLR(2); }
void daPlBase_c::finalizeState_DemoInDokanL() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanL() { executeDemoInDokan(2); }

void daPlBase_c::initializeState_DemoInDokanR() { initDemoInDokanLR(3); }
void daPlBase_c::finalizeState_DemoInDokanR() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoInDokanR() { executeDemoInDokan(3); }

bool daPlBase_c::demo_dokan_move_x(float p1, float p2) {
    sLib::chase(&mPos.x, m_68.x, p1);
    return fabsf(mPos.x - m_68.x) <= p2;
}

bool daPlBase_c::demo_dokan_move_y(float p1, float p2) {
    return sLib::chase(&mPos.y, m_68.y + p2, p1);
}

bool daPlBase_c::isEnableDokanInStatus() {
    if (isDemo()) {
        return false;
    }
    if (isStatus(STATUS_04) || isStatus(STATUS_06) || isStatus(STATUS_08) || isStatus(STATUS_53)) {
        return false;
    }
    return true;
}

bool daPlBase_c::setDokanIn(DokanDir_e dir) {
    if (isStatus(STATUS_7E)) {
        return false;
    }
    int res = false;
    int res2;
    switch (dir) {
        case DOKAN_D:
            if (mKey.buttonDown()) {
                res = mBc.checkDokanDown(&m_68, &res2);
            }
            break;
        case DOKAN_U:
            if (mKey.buttonUp()) {
                res = mBc.checkDokanUp(&m_68, &res2);
            }
            break;
        case DOKAN_L:
        case DOKAN_R:
            float x = 0.0f;
            float y = x;
            if (isStatus(STATUS_3A)) {
                x = 2.0f;
                y = -2.0f;
            } else {
                if (isStatus(STATUS_17)) {
                    x = 8.0f;
                    y = 8.0f;
                }
            }
            res = mBc.checkDokanLR(&m_68, mDirection, &res2, x, y);
            break;
    }
    if (res == 1 && setDemoOutDokanAction(res2, dir)) {
        return true;
    }
    return false;
}

bool daPlBase_c::setDemoOutDokanAction(int param1, DokanDir_e dir) {
    mDokanRelated = param1;
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    dNextGoto_c *nextGoto = cdFile->getNextGotoP(mDokanRelated);
    m_80 = 1;
    if (nextGoto->mFlags & 8) {
        m_80 = 2;
    } else if (nextGoto->mFlags & 4) {
        m_80 = 3;
    }
    static sFStateVirtualID_c<daPlBase_c> *stateIDs[] = {
        &StateID_DemoOutDokanU,
        &StateID_DemoOutDokanD,
        &StateID_DemoOutDokanL,
        &StateID_DemoOutDokanR
    };
    switch (m_80) {
        case 1:
            if (dNext_c::m_instance->fn_800cfed0(dScStage_c::m_instance->mCurrFile, mDokanRelated)) {
                return false;
            }
            if (daPyDemoMng_c::mspInstance->m_5c) {
                return false;
            }
            dNext_c::m_instance->setChangeSceneNextDat(dScStage_c::m_instance->mCurrFile, mDokanRelated, dFader_c::FADER_CIRCLE_TARGET);
            if (nextGoto->m_0b == 22) {
                changeDemoState(StateID_DemoOutWaterTank, 0);
            } else {
                changeDemoState(*stateIDs[dir], 0);
            }
            return true;
        case 2:
            dRail_c::getRailInfoP(nextGoto->m_0f);
            changeDemoState(*stateIDs[dir], 0);
            return true;
        case 3:
            if (nextGoto->m_0b == 22) {
                changeDemoState(StateID_DemoOutWaterTank, dir);
            } else {
                changeDemoState(*stateIDs[dir], 0);
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
    if (m_d40 & 0x4000) {
        fn_80057e70(SE_PLY_WATER_DOKAN_IN_OUT, false);
    } else {
        fn_80057e70(SE_PLY_DOKAN_IN_OUT, false);
    }
}

void daPlBase_c::endDemoOutDokan() {
    offStatus(STATUS_2A);
    offStatus(STATUS_5E);
}

void daPlBase_c::initDemoOutDokanUD(u8 dir) {
    m_84 = dir;
    changeState(StateID_Walk, nullptr);
    mpMdlMng->mpMdl->setAnm(0, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    if (m_80 == 2) {
        if (dir == 0) {
            m_90 = 0.0f;
        } else {
            m_90 = -34.0f;
        }
    } else if (dir == 0) {
        m_90 = 2.0f;
    } else if (mKind == 2) {
        m_90 = -16.0f;
    } else {
        m_90 = -10.0f;
    }
    m_94 = 0.0f;
    if (m_80 == 1 && daPyMng_c::mNum == 1) {
        stopOther();
    }
    initDemoOutDokan();
}

void daPlBase_c::executeDemoOutDokanUD() {
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0: {
            int cond = 0;
            if (mKind == 2) {
                if (sLib::chase(&mAngle.y, 0, scDokanOutTurnSpeed[0])) {
                    cond = 1;
                }
            } else {
                if (sLib::chase(&mAngle.y, getMukiAngle(mDirection), 0x2000)) {
                    cond = 1;
                }
            }
            if (demo_dokan_move_x(1.0f, m_94) && cond == 1) {
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
                m_10c8 = 10;
            }
            break;
        }
        case DEMO_IN_DOKAN_ACTION_1:
            if (demo_dokan_move_y(0.75f, m_90)) {
                onStatus(STATUS_5E);
                switch (m_80) {
                    case 2:
                        changeDemoState(StateID_DemoRailDokan, 0);
                        break;
                    case 3:
                        onStatus(STATUS_BB);
                        mLayer = 0;
                        if (m_84 == 0) {
                            m_68.y = mPos.y + 80.0f;
                        } else {
                            m_68.y = mPos.y - 80.0f;
                        }
                        mDemoSubstate = DEMO_IN_DOKAN_ACTION_2;
                        break;
                    default:
                        changeNextScene(1);
                        mDemoSubstate = DEMO_IN_DOKAN_ACTION_3;
                        break;
                }
            }
            break;
        case DEMO_IN_DOKAN_ACTION_2:
            if (sLib::chase(&mPos.y, m_68.y, 1.0f)) {
                changeState(StateID_Walk, nullptr);
                changeDemoState(StateID_DemoInWaterTank, 1);
            }
            break;
        case DEMO_IN_DOKAN_ACTION_3:
            break;
    }
}

void daPlBase_c::initDemoOutDokanLR(u8 dir) {
    m_84 = dir;
    if (isStatus(STATUS_3A)) {
        mpMdlMng->mpMdl->setAnm(132, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    } else {
        mpMdlMng->mpMdl->setAnm(130, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    }
    onStatus(STATUS_2A);
    if (m_80 == 1 && daPyMng_c::mNum == 1) {
        stopOther();
    }
    if (isStatus(STATUS_3A) || mKind == 2) {
        if (dir == 3) {
            m_68.x += 8.0f;
        } else {
            m_68.x -= 8.0f;
        }
    }
    initDemoOutDokan();
}

void daPlBase_c::executeDemoOutDokanLR() {
    if (mpMdlMng->mpMdl->mAnm.isStop()) {
        mpMdlMng->mpMdl->setAnm(131, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    }
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0: {
            int cond = 0;
            if (!addCalcAngleY(getMukiAngle(mDirection), 10)) {
                float tmp = 0.0f;
                if (isStatus(STATUS_3A)) {
                    tmp = getWaterDokanCenterOffset(m_68.y) - m_68.y;
                }
                if (demo_dokan_move_y(0.75f, tmp)) {
                    cond = 1;
                }
            }
            if (cond && demo_dokan_move_x(0.75f, 0.0f)) {
                onStatus(STATUS_5E);
                switch (m_80) {
                    case 2:
                        changeDemoState(StateID_DemoRailDokan, 0);
                        break;
                    case 3:
                        onStatus(STATUS_BB);
                        mLayer = 0;
                        m_68.x = mPos.x + sc_DirSpeed[mDirection] * 48.0f;
                        mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
                        break;
                    default:
                        changeNextScene(1);
                        break;
                }
            }
            break;
        }
        case DEMO_IN_DOKAN_ACTION_1:
            if (sLib::chase(&mPos.x, m_68.x, 1.0f)) {
                m_68.y -= 16.0f;
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_2;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_2:
            if (sLib::chase(&mPos.y, m_68.y, 1.0f)) {
                m_68.x = mPos.x + sc_DirSpeed[mDirection] * 32.0f;
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_3;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_3:
            if (sLib::chase(&mPos.x, m_68.x, 1.0f)) {
                changeState(StateID_Walk, nullptr);
                changeDemoState(StateID_DemoInWaterTank, 1);
            }
            break;
    }
}

void daPlBase_c::initializeState_DemoOutDokanU() { initDemoInDokanUD(0); }
void daPlBase_c::finalizeState_DemoOutDokanU() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoOutDokanU() { executeDemoOutDokan(); }

void daPlBase_c::initializeState_DemoOutDokanD() { initDemoInDokanUD(1); }
void daPlBase_c::finalizeState_DemoOutDokanD() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoOutDokanD() { executeDemoOutDokan(); }

void daPlBase_c::initializeState_DemoOutDokanL() { initDemoInDokanLR(2); }
void daPlBase_c::finalizeState_DemoOutDokanL() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoOutDokanL() { executeDemoOutDokan(); }

void daPlBase_c::initializeState_DemoOutDokanR() { initDemoInDokanLR(3); }
void daPlBase_c::finalizeState_DemoOutDokanR() { endDemoInDokan(); }
void daPlBase_c::executeState_DemoOutDokanR() { executeDemoOutDokan(); }

void daPlBase_c::initializeState_DemoOutDokanRoll() {
    initDemoOutDokanUD(4);
    m_80 = 1;
    m_74.x = 0.0f;
    m_74.y = 0.0f;
    mBc.setRideOnObjBg(mpBgCtr, mPos);
    mpBgCtr->addDokanMoveDiff(&m_68);
}

void daPlBase_c::finalizeState_DemoOutDokanRoll() {}

void daPlBase_c::executeState_DemoOutDokanRoll() {
    mBc.setRideOnObjBg(mpBgCtr, mPos);
    mpBgCtr->addDokanMoveDiff(&m_68);
    mVec3_c delta(m_68.x, m_68.y, mPos.z);
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0: {
            int cond = 0;
            if (mKind == 2) {
                if (sLib::chase(&mAngle.y, 0, scDokanOutTurnSpeed[0])) {
                    cond = 1;
                }
            } else {
                if (sLib::chase(&mAngle.y, getMukiAngle(mDirection), 0x2000)) {
                    cond = 1;
                }
            }
            mVec3_c tmp = delta - mPos;
            tmp.normalize();
            mVec3_c tmp2 = tmp * 1.0f;
            mPos += tmp2;
            if (!(mPos - delta).lt1() || cond != 1) {
                break;
            }
            mPos = delta;
            mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
            break;
        }
        case DEMO_IN_DOKAN_ACTION_1: {
            mAng v = mFollowEf.mAng;
            if (mDirection == 1) {
                v = -v;
            }
            sLib::addCalcAngle(&mAngle.x, v.mAngle, 4, 0x1000, 0x100);
            float tmp = m_90 - 32.0f;
            sLib::chase(&m_74.y, tmp, 0.75f);
            mMtx_c m1, m2;
            m1.trans(delta);
            m1.ZrotM(mFollowEf.mAng);
            m2.trans(0.0f, m_74.y, 0.0f);
            m1.concat(m2);
            m2.multVecZero(mPos);
            if (m_74.y <= tmp) {
                changeNextScene(1);
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_3;
                onStatus(STATUS_BB);
            }
            break;
        }
        default:
            break;
    }
}

void daPlBase_c::initializeState_DemoInWaterTank() {
    onStatus(STATUS_BB);
    mDirection = (u8) (int) mDemoStateChangeParam;
    mpMdlMng->mpMdl->setAnm(131, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    mAngle.y = 0;
    if ((int) mDemoStateChangeParam == 1) {
        m_ce0 = 0;
        mLayer = 0;
        setZPosition(3000.0f);
    } else {
        m_ce0 = 35;
        if (daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
            stopOther();
        }
    }
    mDemoSubstate = DEMO_IN_DOKAN_ACTION_0;
}

void daPlBase_c::finalizeState_DemoInWaterTank() {}

void daPlBase_c::executeState_DemoInWaterTank() {
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0:
            if (!mFader_c::mFader->isStatus(mFaderBase_c::HIDDEN)) {
                break;
            }
            if (!daPyDemoMng_c::mspInstance->checkDemoNo(mPlayerNo)) {
                break;
            }
            offStatus(STATUS_BB);
            mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
            m_10c8 = 60;
            m_d44 = 0;
            m_d40 = 0;
            checkWater();
            if (m_d40 & 0x4000) {
                mpMdlMng->mpMdl->setAnm(132, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                mPos.y = getWaterDokanCenterOffset(mPos.y);
                fn_80057e70(SE_PLY_WATER_DOKAN_IN_OUT, false);
            } else {
                fn_80057e70(SE_PLY_DOKAN_IN_OUT, false);
            }
            break;
        case DEMO_IN_DOKAN_ACTION_1:
            if (m_10c8 == 0) {
                changeNormalAction();
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::initializeState_DemoOutWaterTank() {
    mpMdlMng->mpMdl->setAnm(0, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    mSpeedF = 0.0f;
    mMaxSpeedF = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0.0f;
    mAngle.y = -0x8000;
}

void daPlBase_c::finalizeState_DemoOutWaterTank() {}

void daPlBase_c::executeState_DemoOutWaterTank() {
    switch ((DemoInDokanSubstate_e) mDemoSubstate) {
        case DEMO_IN_DOKAN_ACTION_0:
            if (sLib::chase(&mPos.x, m_68.x, 1.0f)) {
                mpMdlMng->mpMdl->setAnm(132, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                fn_80057e70(SE_PLY_DOKAN_IN_OUT, false);
                m_10c8 = 60;
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_1:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                mpMdlMng->mpMdl->setAnm(131, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
            }
            if (m_10c8 == 0) {
                onStatus(STATUS_BB);
                if (m_80 == 3) {
                    setZPosition(-1800.0f);
                    if ((int) mDemoStateChangeParam <= 1) {
                        m_68.x = mPos.x + sc_DirSpeed[(int) mDemoStateChangeParam] * 32.0f;
                        mDemoSubstate = DEMO_IN_DOKAN_ACTION_2;
                    } else {
                        m_68.y = mPos.y - sc_DirSpeed[(int) mDemoStateChangeParam & 1] * 80.0f;
                        mDemoSubstate = DEMO_IN_DOKAN_ACTION_4;
                    }
                } else {
                    changeNextScene(1);
                }
                break;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_2:
            if (sLib::chase(&mPos.x, m_68.x, 1.0f)) {
                m_68.y = getWaterDokanCenterOffset(m_68.y + 16.0f);
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_3;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_3:
            if (sLib::chase(&mPos.y, m_68.y, 1.0f)) {
                m_68.x = mPos.x + sc_DirSpeed[(int) mDemoStateChangeParam] * 48.0f;
                mDemoSubstate = DEMO_IN_DOKAN_ACTION_4;
            }
            break;
        case DEMO_IN_DOKAN_ACTION_4:
            if (sLib::chase(&mPos.x, m_68.x, 1.0f) && sLib::chase(&mPos.y, m_68.y, 1.0f)) {
                mLayer = 1;
                mAngle.y = 0;
                switch ((int) mDemoStateChangeParam) {
                    case 0:
                        changeDemoState(StateID_DemoInDokanL, 1);
                        break;
                    case 1:
                        changeDemoState(StateID_DemoInDokanR, 1);
                        break;
                    case 2:
                        changeDemoState(StateID_DemoInDokanU, 1);
                        break;
                    case 3:
                        changeDemoState(StateID_DemoInDokanD, 1);
                        break;
                }
                setZPosition(-1800.0f);
            }
            break;
    }
}

void daPlBase_c::initializeState_DemoRailDokan() {
    onStatus(STATUS_BB);
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    dNextGoto_c *nextGoto = cdFile->getNextGotoP(mDokanRelated);
    void *rail = dRail_c::getRailInfoP(nextGoto->m_0f);
    dCdFile_c *cdFile2 = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
}

void daPlBase_c::finalizeState_DemoRailDokan() {
    offStatus(STATUS_BB);
}

void daPlBase_c::setExitRailDokan() {
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    dNextGoto_c *nextGoto = cdFile->getNextGotoP(mDokanRelated);
    mLayer = nextGoto->mLayer;
    switch (nextGoto->m_0b) {
        case 3:
            changeDemoState(StateID_DemoInDokanU, 1);
            break;
        case 4:
            changeDemoState(StateID_DemoInDokanU, 1);
            break;
        case 5:
            changeDemoState(StateID_DemoInDokanR, 1);
            break;
        case 6:
            changeDemoState(StateID_DemoInDokanL, 1);
            break;
    }
}

void daPlBase_c::executeState_DemoRailDokan() {}

void daPlBase_c::setObjDokanIn(dBg_ctr_c *bgCtr, mVec3_c &pos, int param3) {
    mpBgCtr = bgCtr;
    m_68.set(pos.x, pos.y, mPos.z);
    m_7c = *bgCtr->m_bc;
    setDemoOutDokanAction(param3, DOKAN_ROLL);
}

bool daPlBase_c::isDispOutCheckOn() {
    if (isStatus(STATUS_04) ||
        isStatus(STATUS_06) ||
        isStatus(STATUS_53) ||
        isStatus(STATUS_6F) ||
        isStatus(STATUS_76) ||
        isStatus(STATUS_8D)
    ) {
        return false;
    }
    return true;
}

void daPlBase_c::initializeState_DemoDown() {}
void daPlBase_c::finalizeState_DemoDown() {}
void daPlBase_c::executeState_DemoDown() {}

int daPlBase_c::vf130(float f, mVec2_c *v, int param3) {
    if (daPyDemoMng_c::mspInstance->mFlags & 4) {
        return -1;
    }
    mPos.x = v->x;
    changeState(StateID_None, nullptr);
    changeDemoState(StateID_DemoGoal, 0);
    m_68.x = f;
    m_68.y = v->y;
    m_68.z = v->x + 80.0f;
    m_9c = daPyDemoMng_c::mspInstance->setGoalDemoList(mPlayerNo);
    if (m_9c == 0) {
        daPyDemoMng_c::mspInstance->setDemoMode(daPyDemoMng_c::MODE_1, 0);
        daPyDemoMng_c::mspInstance->m_14 = param3;
        daPyDemoMng_c::mspInstance->mPlNo = mPlayerNo;
        mVec3_c pos(f - 112.0f, v->y, 5500.0f);
        float tmp;
        if (dBc_c::checkGround(&pos, &tmp, mLayer, 1, -1)) {
            pos.y = tmp + 112.0f;
        }
        daPyDemoMng_c::mspInstance->mFireworkPos = pos;
    }
    bool cond = false;
    daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
    if (pl != nullptr && pl->isItemKinopio()) {
        cond = true;
    }
    if (!cond) {
        if (m_9c != 0 && m_9c + 1 == daPyMng_c::getNumInGame()) {
            dMultiMng_c::mspInstance->setClapSE();
        }
    } else {
        daPyDemoMng_c::mspInstance->m_42 = 1;
        dGameCom::FUN_800b3780(mPlayerNo, 8);
    }
    int sum = daPyMng_c::getItemKinopioNum() + daPyMng_c::getNumInGame();
    if (sum == m_9c + 1) {
        daPyDemoMng_c::mspInstance->stopBgmGoalDemo();
    }
    return -1;
}

bool daPlBase_c::setHideNotGoalPlayer() {
    if (!isStatus(STATUS_65)) {
        mSpeedF = 0.0f;
        mSpeed.y = 0.0f;
        onStatus(STATUS_6F);
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
        if (ctrlPl == nullptr || !ctrlPl->isStatus(STATUS_65)) {
            continue;
        }

        daPlBase_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr) {
            pl->mStopMask &= ~2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(i);
        if (yoshi != nullptr) {
            yoshi->mStopMask &= ~2;
        }
    }
}

void daPlBase_c::playGoalOther() {
    dActor_c::mExecStopReq &= ~0xf;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        daPlBase_c *ctrlPl = daPyMng_c::getCtrlPlayer(i);
        if (ctrlPl == nullptr || ctrlPl->isStatus(STATUS_65)) {
            continue;
        }

        daPlBase_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr) {
            pl->mStopMask |= 2;
        }
        daPlBase_c *yoshi = daPyMng_c::getYoshi(i);
        if (yoshi != nullptr) {
            yoshi->mStopMask |= 2;
        }
    }
}

void daPlBase_c::initDemoGoalBase() {
    onStatus(STATUS_65);
    if ((int) mDemoStateChangeParam == 0) {
        vf434(31, 0);
        onStatus(STATUS_7E);
    }
    clearJumpActionInfo(0);
    endStar();
    setDemoGoalMode(0, 0);
    mpMdlMng->mpMdl->setAnm(85, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    mAngle.x = 0;
    setZPositionDirect(3000.0f);
    mSpeed.x = 0.0f;
    mSpeedF = 0.0f;
    mSpeed.y = 0.0f;
    mAccelY = 0.0f;
    mMaxFallSpeed = -4.0f;
}

void daPlBase_c::finalizeDemoGoalBase() {
    offStatus(STATUS_65);
    offStatus(STATUS_66);
    offStatus(STATUS_68);
    offStatus(STATUS_69);
    offStatus(STATUS_6A);
    offStatus(STATUS_6C);
    offStatus(STATUS_6D);
    offStatus(STATUS_6E);
    offStatus(STATUS_7E);
}

void daPlBase_c::initializeState_DemoGoal() { initDemoGoalBase(); }
void daPlBase_c::finalizeState_DemoGoal() { finalizeDemoGoalBase(); }

float daPlBase_c::getDemoGoalLandPos() {
    float pos = m_68.z + daPyDemoMng_c::mspInstance->m_1c * 16.0f;
    if (daPyDemoMng_c::mspInstance->m_1c > 1) {
        pos -= m_a0 * 32.0f;
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
    m_60 = 4;
    mpMdlMng->mpMdl->setAnm(88, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
    if (daPyDemoMng_c::mspInstance->m_1c > 1) {
        initGoalJump(pos, 5.128f);
    } else {
        initGoalJump(pos, 4.928f);
    }
}

void daPlBase_c::executeDemoGoal_Pole() {
    switch (m_60) {
        case 0:
            if (!addCalcAngleY(-0x4000, 10) && mpMdlMng->mpMdl->mAnm.isStop()) {
                m_60 = 1;
                mpMdlMng->mpMdl->setAnm(86, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                onStatus(STATUS_66);
            }
            break;
        case 1:
            if (isStatus(STATUS_67)) {
                int polePlayer = daPyDemoMng_c::mspInstance->getPoleBelowPlayer(mPlayerNo);
                if (polePlayer != -1) {
                    daPlBase_c *ctrlPl = daPyMng_c::getCtrlPlayer(polePlayer);
                    if (ctrlPl != nullptr) {
                        if (0.7f * ctrlPl->m_c9c + ctrlPl->mPos.y < mPos.y) {
                            if (!ctrlPl->isStatus(STATUS_68)) {
                                break;
                            }
                        }
                    }
                }
                m_60 = 2;
                offStatus(STATUS_66);
                daPyDemoMng_c::mspInstance->mFlags |= 1;
                mSpeed.y = -1.9f;
            }
            break;
        case 2: {
            float tmp = m_a4;
            mPos.y += mSpeed.y;
            if (mPos.y < m_a4) {
                mPos.y = m_a4;
                onStatus(STATUS_68);
                mSpeed.y = 0.0f;
                m_60 = 3;
                mpMdlMng->mpMdl->setAnm(87, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
            }
            break;
        }
        case 3:
            if (isStatus(STATUS_69)) {
                setDemoGoal_MultiJump();
            }
            break;
        case 4:
            if (calcGoalJump()) {
                m_60 = 5;
                mpMdlMng->mpMdl->setAnm(89, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                setLandSE();
            }
            break;
        case 5:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                mpMdlMng->mpMdl->setAnm(143, ((float *) &dPyMdlMng_c::m_hio)[10], ((float *) &dPyMdlMng_c::m_hio)[11], 0.0f);
                m_60 = 6;
                m_10c8 = 5;
            }
            break;
        case 6:
            if (m_10c8 == 0) {
                onStatus(STATUS_6A);
                m_60 = 7;
            }
            break;
        case 7:
            if (daPyDemoMng_c::mspInstance->mFlags & 8 && !addCalcAngleY(0, 10)) {
                m_10c8 = 7;
                m_60 = 8;
            }
            break;
        case 8:
            if (m_10c8 == 0) {
                setDemoGoalMode(1, 0);
            }
            break;
    }
}

void daPlBase_c::executeDemoGoal_Wait() {
    if (isStatus(STATUS_6B)) {
        offStatus(STATUS_6B);
        onStatus(STATUS_6C);
        setDemoGoalMode(2, 0);
    }
    if (isStatus(STATUS_6D)) {
        setDemoGoalMode(3, 0);
    }
}

void daPlBase_c::executeDemoGoal_KimePose() {
    if (m_5c && mpMdlMng->mpMdl->mAnm.checkFrame(107.0f)) {
        daPlBase_c *pl = daPyMng_c::getPlayer(mPlayerNo);
        if (pl != nullptr) {
            if (pl->isItemKinopio()) {
                if (m_5c == 2 || m_5c == 3) {
                    if (pl->mPowerup == POWERUP_NONE) {
                        dScoreMng_c::m_instance->fn_800e25a0(8, mPlayerNo, 1);
                    } else {
                        dScoreMng_c::m_instance->fn_800e25a0(9, mPlayerNo, 1);
                    }
                }
            } else {
                if (dInfo_c::mGameFlag & 0x10 && dInfo_c::mGameFlag & 0x40) {
                    if (m_9c == 0) {
                        SndAudioMgr::sInstance->startSystemSe(SE_OBJ_GOAL_GET_COIN_BONUS, 1);
                    }
                    static const int coinBonus[] = {20, 15, 10, 5 };
                    dMultiMng_c::mspInstance->setBattleCoin(mPlayerNo, coinBonus[m_a0]);
                }
            }
        }
    }
    if (vf284(0)) {
        setDemoGoalMode(1, 0);
    }
}

void daPlBase_c::executeDemoGoal_Run() {}

void daPlBase_c::setDemoGoalMode(int mode, int param) {
    mDemoSubstate = mode;
    m_60 = param;
}

void daPlBase_c::executeState_DemoGoal() {
    switch (mDemoSubstate) {
        case 0:
            executeDemoGoal_Pole();
            break;
        case 1:
            executeDemoGoal_Wait();
            break;
        case 2:
            executeDemoGoal_KimePose();
            break;
        case 3:
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
    m_ac = pos;
    m_b8 = count;
    mSpeed.y = ySpeed;
}

bool daPlBase_c::calcGoalJump() {
    if (m_b8 != 0) {
        sLib::chase(&mPos.x, m_ac.x, (m_ac.x - mPos.x) / m_b8);
        m_b8--;
    }
    mAccelY = -0.28f;
    mSpeed.y += mAccelY;
    if (mSpeed.y < -4.0f) {
        mSpeed.y = -4.0f;
    }
    mPos.y += mSpeed.y;
    if (mSpeed.y < 0.0f && m_d40 & 1) {
        mPos = m_ac;
        return true;
    }
    return false;
}

void daPlBase_c::initDemoKimePose() {
    onStatus(STATUS_6C);
    m_5c = 0;
}

bool daPlBase_c::vf284(int) {
    return false;
}

void daPlBase_c::fn_80051cf0(int p) {
    int iVar1;
    if (p == 0) {
        iVar1 = daPyDemoMng_c::mspInstance->m_1c;
    } else {
        iVar1 = daPyDemoMng_c::mspInstance->getControlDemoPlayerNum();
    }
    if (dInfo_c::m_startGameInfo.mScreenType == 1) {
        if (iVar1 >= 2) {
            vf434(59, 0);
        } else {
            vf434(60, 0);
        }
    } else if (p == 2) {
        vf434(62, 0);
    } else if (iVar1 >= 2) {
        vf434(59, 0);
    } else if (p == 0) {
        if (daPyDemoMng_c::mspInstance->m_14 == 0) {
            vf434(57, 0);
        } else {
            vf434(58, 0);
        }
    } else {
        vf434(61, 0);
    }
}

bool daPlBase_c::startControlDemo() {
    if (isDemoType(DEMO_4)) {
        return true;
    } else if (isDemoType(DEMO_6)) {
        changeDemoState(StateID_DemoControl, 0);
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
        if (!mStateMgr.getStateID()->isEqual(StateID_Walk)) {
            changeState(StateID_Walk, (void *) 1);
        }
    }
}

void daPlBase_c::setControlDemoWait() {
    if (isStatus(STATUS_72)) {
        m_60 = 0;
        changeState(StateID_Walk, (void *) 1);
    }
}

bool daPlBase_c::isControlDemoWait() {
    if (isStatus(STATUS_72)) {
        if (m_60 == 0) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::setControlDemoWalk(const float &f1, const float &f2) {
    if (isStatus(STATUS_72)) {
        m_bc = f1;
        m_60 = 1;
        m_c8 = fabs(f2);
        if (m_c8 > *(getSpeedData() + 2)) {
            m_c8 = *(getSpeedData() + 2);
        }
    }
}

bool daPlBase_c::isControlDemoWalk() {
    if (isStatus(STATUS_72)) {
        if (m_60 == 1) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::setControlDemoAnm(int anmNo) {
    if (isStatus(STATUS_72)) {
        m_60 = 2;
        changeState(StateID_AnimePlay, nullptr);
        int idx = anmNo * 3;
        float f1 = ((float *) &dPyMdlMng_c::m_hio)[idx];
        float f2 = ((float *) &dPyMdlMng_c::m_hio)[1 + idx];
        mpMdlMng->mpMdl->setAnm(anmNo, f1, f2, 0.0f);
    }
}
bool daPlBase_c::isControlDemoAnm(int anmNo) {
    if (isStatus(STATUS_72)) {
        if (m_60 == 2 && anmNo == *(int*) &mpMdlMng->mpMdl->mAnm.mFrameStart) {
            return true;
        }
    }
    return false;
}

void daPlBase_c::fn_80052290(int param) {
    if (isStatus(STATUS_72)) {
        m_60 = 3;
        changeState(StateID_AnimePlay, (void *) param);
    }
}

void daPlBase_c::setControlDemoKinopioWalk() {
    if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoNone) || (isDemoType(DEMO_5) && m_60 != 5)) {
        changeDemoState(StateID_DemoControl, DEMO_5);
    }
}

void daPlBase_c::setControlDemoKinopioSwim() {
    if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoNone) || (isDemoType(DEMO_5) && m_60 != 6)) {
        changeDemoState(StateID_DemoControl, DEMO_6);
    }
}

void daPlBase_c::setControlDemoEndingDance() {
    if (!isControlDemoAll()) {
        changeDemoState(StateID_DemoControl, DEMO_8);
    }
}

bool daPlBase_c::isBossDemoLand() {
    if ((m_d40 & 1) == 0) {
        return false;
    }
    if (isStatus(STATUS_14) || isStatus(STATUS_4E) || (m_d40 & 0x18000000) || m_d4) {
        return false;
    }
    return true;
}

bool daPlBase_c::fn_80052500(int p, float f, int) {
    mVec3_c tmp = mPos;
    float tmpf = f * 1.2f;
    tmp.x += f + sc_DirSpeed[p];
    float a = 4.0f;
    if (tmpf < 4.0f) {
        a = tmpf;
    }
    tmp.y = mPos.y + a;
    float y;
    if (mBc.checkGround(&tmp, &y, mLayer, m_ca1, -1)) {
        float mpy = mPos.y;
        if (fabsf(y - mpy) < a) {
            if (p == 1) {
                tmp.y = mpy - 4.0f;
                float y2;
                if (dBc_c::checkWater(tmp.x, tmp.y, mLayer, &y2) &&
                    m_ca4.y >= 0.0f &&
                    y <= y2 &&
                    mPos.y <= y2 - 4.0f
                ) {
                    return false;
                }
            }
        }
    } else {
        return false;
    }
    return true;
}

bool daPlBase_c::isHitWallKinopioWalk(int dir) {
    static const int sc_DirFlag[] = { 0x100000, 0x80000 };
    if (checkBGCrossWall(dir) || sc_DirFlag[dir] & m_d40) {
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
    onStatus(STATUS_71);
    mDemoMode = 0;
    initializeDemoControl();
    m_60 = (u8) mDemoStateChangeParam;
    switch (m_60) {
        case DEMO_4:
            m_10c8 = 60;
            onStatus(STATUS_7A);
            break;
        case DEMO_5:
            m_bc = mPos.x;
            m_c8 = 0.9f;
            m_cc = mDirection;
            m_d0 = 150;
            if (!fn_80052500(mDirection, 8.0f, 1)) {
                m_cc ^= 1;
            }
            break;
        case DEMO_6:
            if (mSpeedF > 0.0f) {
                m_cc = 0;
            } else {
                m_cc = 1;
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
    if (m_60 == DEMO_4) {
        offStatus(STATUS_7A);
    }
}

void daPlBase_c::executeState_DemoControl() {
    offStatus(STATUS_74);
    if (isStatus(STATUS_73)) {
        if (m_d40 & 1) {
            offStatus(STATUS_73);
        } else {
            mSpeedF *= 0.98f;
        }
    }

    switch (m_60) {
        case 1: {
            onStatus(STATUS_74);
            if (m_d40 & 1) {
                if (!mStateMgr.getStateID()->isEqual(StateID_Walk) && !mStateMgr.getStateID()->isEqual(StateID_Turn)) {
                    changeState(StateID_Walk, (void*)1);
                }
            } else {
                if (!mStateMgr.getStateID()->isEqual(StateID_Fall)) {
                    changeState(StateID_Fall, 0);
                }
            }
            if (fabsf(mPos.x - m_bc) < m_c8) {
                m_60 = 0;
                mSpeedF = 0.0f;
                mPos.x = m_bc;
                break;
            }
            if (mPos.x < m_bc) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                mSpeedF = m_c8;
                break;
            }
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
            mSpeedF = -m_c8;
                break;
        }
        case 4: {
            if (m_10c8 == 0) {
                changeDemoState(StateID_DemoNone, 0);
            }
            break;
        }
        case 5: {
            if ((m_d40 & 1) == 0 ||
                !mStateMgr.getStateID()->isEqual(StateID_Walk) && !mStateMgr.getStateID()->isEqual(StateID_Turn)
            ) {
                m_60 = 0;
                break;
            }
            onStatus(STATUS_74);
            if ((m_d40 & 0x18000000) || (checkKinopioWaitBG(0) && checkKinopioWaitBG(1))) {
                m_60 = 7;
                m_c8 = 0.0f;
                mSpeedF = 0.0f;
                break;
            }
            int prevM_cc = (int) (short) m_cc;
            sLib::calcTimer(&m_d0);
            if (isHitWallKinopioWalk(m_cc) || m_d0 == 0) {
                m_cc ^= 1;
                m_bc = mPos.x + sc_DirSpeed[m_cc] * 24.0f;
            } else if (!fn_80052500(m_cc, 4.0f, 1) && !fn_80052500(m_cc, 8.0f, 1)) {
                m_cc ^= 1;
                m_bc = mPos.x + sc_DirSpeed[m_cc] * 24.0f;
            }
            float tmp = m_bc + sc_DirSpeed[m_cc] * 24.0f;
            if (mPos.x < tmp) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                mSpeedF = m_c8;
                m_cc = 0;
            } else {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
                m_cc = 1;
                mSpeedF = -m_c8;
            }
            if (prevM_cc != m_cc) {
                m_d0 = 180;
            }
            break;
        }
        case 6: {
            if (!mStateMgr.getStateID()->isEqual(StateID_Swim)) {
                m_60 = 0;
                break;
            }
            onStatus(STATUS_74);
            if ((m_d40 & 1) && isHitWallKinopioWalk(m_cc)) {
                m_cc ^= 1;
            }
            if (m_cc == 0) {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_RIGHT);
                sLib::chase(&mSpeedF, 0.5625f, 0.1f);
            } else {
                mKey.onDemoTrigger(dAcPyKey_c::BUTTON_LEFT);
                sLib::chase(&mSpeedF, -0.5625f, 0.1f);
            }
            break;
        }
        case 7: {
            if ((m_d40 & 1) == 0 ||
                !mStateMgr.getStateID()->isEqual(StateID_Walk) && !mStateMgr.getStateID()->isEqual(StateID_Turn)
            ) {
                m_60 = 0;
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

void daPlBase_c::fn_80052ef0(int p2, int p3, int p4) {
    if (mDemoStateMgr.getStateID()->isEqual(StateID_DemoNextGotoBlock)) {
        return;
    }
    changeDemoState(StateID_DemoNextGotoBlock, p2 | ((p4 & 0xFF) << 8));
    m_10c8 = p3;
    switch (p4) {
        case 3:
            fn_80057e70(SE_PLY_DOKAN_IN_OUT, false);
            break;
        case 1:
            mDemoMode = 0;
            if (!isStatus(STATUS_3C)) {
                changeState(StateID_Fall, nullptr);
            }
            break;
        default:
            break;
    }
}

void daPlBase_c::initializeState_DemoNextGotoBlock() {
    mKey.onStatus(dAcPyKey_c::STATUS_DEMO);
    onStatus(STATUS_76);
    onStatus(STATUS_7A);
    initializeDemoControl();
}

void daPlBase_c::finalizeState_DemoNextGotoBlock() {
    mKey.offStatus(dAcPyKey_c::STATUS_DEMO);
    offStatus(STATUS_76);
    offStatus(STATUS_7A);
}

void daPlBase_c::executeState_DemoNextGotoBlock() {
    if (mDemoSubstate == DEMO_IN_DOKAN_ACTION_0 && m_10c8 == 0) {
        dFader_c::fader_type_e f = dFader_c::FADER_DRIP_DOWN;
        int param = (int) mDemoStateChangeParam;
        int lower = (param & 0xff);
        int upper = (param >> 8) & 0xff;
        if (upper == 3) {
            f = dFader_c::FADER_CIRCLE_TARGET;
        }
        dNext_c::m_instance->setChangeSceneNextDat(dScStage_c::m_instance->mCurrFile, lower, f);
        changeNextScene(0);
        mDemoSubstate = DEMO_IN_DOKAN_ACTION_1;
    }
}

void daPlBase_c::updateEndingDance() {
    offStatus(STATUS_75);
    if (!dScStage_c::m_isStaffCredit || isDemoType(DEMO_4)) {
        return;
    }
    int cond = 0;
    dGameKeyCore_c **keys = dGameKey_c::m_instance->keys;
    dGameKeyCore_c *key = keys[mPlayerNo];
    if (key->mDownButtons != 0 || key->mIsShaking != 0) {
        cond = 1;
    }
    if (!isDemoType(DEMO_6)) {
        if (cond == 1) {
            m_f4 = 0;
        } else {
            m_f4++;
            if (m_f4 >= 180) {
                setControlDemoEndingDance();
                m_f4 = 0;
            }
        }
    } else if (cond == 1) {
        endControlDemo(0);
    } else if (isStatus(STATUS_45)) {
        for (int i = 0; i < ARRAY_SIZE(m_d8); i++) {
            m_d8[i] = 0;
        }
    } else {
        onStatus(STATUS_75);
        if (dAudio::isBgmAccentSign(2)) {
            mDirection = 0;
        } else if (dAudio::isBgmAccentSign(4)) {
            mDirection = 1;
        } else if (dAudio::isBgmAccentSign(8)) {
            mKey.onDemoButton(dAcPyKey_c::BUTTON_DOWN);
            m_d8[2] = 6;
        } else if (dAudio::isBgmAccentSign(16)) {
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_TWO);
            mKey.onDemoButton(dAcPyKey_c::BUTTON_TWO);
            m_d8[3] = 5;
        } else if (dAudio::isBgmAccentSign(32)) {
            mKey.onDemoTrigger(dAcPyKey_c::BUTTON_TWO);
            mKey.onDemoButton(dAcPyKey_c::BUTTON_TWO);
            m_d8[4] = 30;
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
            if (m_d8[i] != 0 && --m_d8[i] == 0) {
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
    if (isStatus(STATUS_60)) {
        if (m_d44 & 2 &&
            mBc.mpCtrHead != nullptr &&
            mBc.mpCtrHead->mpActor != nullptr &&
            mBc.mpCtrHead->mpActor->mProfName == fProfile::EN_CHIKUWA_BLOCK
        ) {
            return false;
        }
        if (!isDemoType(DEMO_4)) {
            changeDemoState(StateID_DemoControl, 0);
        }
        mSpeed.y = 0.0f;
        mSpeedF = 0.0f;
        return true;
    }
    return false;
}

int daPlBase_c::getCcLineKind() {
    if (m_d44 & 0x180000) {
        return m_ca1;
    }
    return 3;
}

void daPlBase_c::initCollision(sCcDatNewF *dat1, sCcDatNewF *dat2) {
    mCc.registerCc(this, dat1);
    mCc1.registerCc(this, dat1);
    mAttCc1.registerCc(this, dat2);
    mAttCc2.registerCc(this, dat2);
    mAttCc3.registerCc(this, dat2);
}

void daPlBase_c::releaseCcData() {
    u32 categoryInteract =
        BIT_FLAG(dCc_c::CAT_PLAYER_GENERIC) |
        BIT_FLAG(dCc_c::CAT_PLAYER_ATTACK) |
        BIT_FLAG(dCc_c::CAT_YOSHI) |
        BIT_FLAG(dCc_c::CAT_ENTITY) |
        BIT_FLAG(dCc_c::CAT_BALLOON) |
        BIT_FLAG(dCc_c::CAT_ITEM) |
        BIT_FLAG(dCc_c::CAT_PROJECTILE) |
        BIT_FLAG(dCc_c::CAT_CANNON) |
        BIT_FLAG(dCc_c::CAT_GOAL_POLE);

    u32 categoryForAtt =
        BIT_FLAG(dCc_c::CAT_PLAYER_GENERIC) |
        BIT_FLAG(dCc_c::CAT_YOSHI) |
        BIT_FLAG(dCc_c::CAT_ENTITY) |
        BIT_FLAG(dCc_c::CAT_BALLOON) |
        BIT_FLAG(dCc_c::CAT_ITEM) |
        BIT_FLAG(dCc_c::CAT_CANNON);

    u32 attackCategoryInteract =
        dCc_c::ATTACK_ALL &
        ~BIT_FLAG(dCc_c::ATTACK_UNK0) &
        ~BIT_FLAG(dCc_c::ATTACK_YOSHI_MOUTH) &
        ~BIT_FLAG(dCc_c::ATTACK_SAND_PILLAR);

    mCc.release();
    mCc.mCcData.mCategoryInteract = categoryInteract;
    mCc.mCcData.mAttackCategory = 0;
    mCc.mCcData.mAttackCategoryInteract = attackCategoryInteract;

    mCc1.release();
    mCc1.mCcData.mCategoryInteract = categoryInteract;
    mCc1.mCcData.mAttackCategory = 0;
    mCc1.mCcData.mAttackCategoryInteract = attackCategoryInteract;

    mAttCc1.release();
    mAttCc1.mCcData.mCategoryInteract = categoryForAtt;
    mAttCc1.mCcData.mAttackCategory = 0;
    mAttCc1.mCcData.mAttackCategoryInteract = 0;

    mAttCc2.release();
    mAttCc2.mCcData.mCategoryInteract = categoryForAtt;
    mAttCc2.mCcData.mAttackCategory = 0;
    mAttCc2.mCcData.mAttackCategoryInteract = 0;

    mAttCc3.release();
    mAttCc3.mCcData.mCategoryInteract = categoryForAtt;
    mAttCc3.mCcData.mAttackCategory = 0;
    mAttCc3.mCcData.mAttackCategoryInteract = 0;
}

void daPlBase_c::clearCcData() {
    mCc.clear();
    mCc1.clear();
    mAttCc1.clear();
    mAttCc2.clear();
    mAttCc3.clear();
}

void daPlBase_c::setCcAtBody(int attackCategory) {
    mAttCc2.mCcData.mOffsetX = mCc.mCcData.mOffsetX;
    mAttCc2.mCcData.mOffsetY = mCc.mCcData.mOffsetY;
    mAttCc2.mCcData.mWidth = mCc.mCcData.mWidth;
    mAttCc2.mCcData.mHeight = mCc.mCcData.mHeight;
    mAttCc2.mCcData.mAttackCategory = attackCategory;
    mAttCc2.mCcData.mCategoryInteract =
        BIT_FLAG(dCc_c::ATTACK_UNK0) |
        BIT_FLAG(dCc_c::ATTACK_FIRE) |
        BIT_FLAG(dCc_c::ATTACK_ICE);

    mAttCc1.mCcData.mOffsetX = mCc.mCcData.mOffsetX;
    mAttCc1.mCcData.mOffsetY = mCc.mCcData.mOffsetY;
    mAttCc1.mCcData.mWidth = mCc.mCcData.mWidth;
    mAttCc1.mCcData.mHeight = mCc.mCcData.mHeight;
    mAttCc1.mCcData.mAttackCategory = attackCategory;
    mAttCc1.mCcData.mCategoryInteract = mCc.mCcData.mCategoryInteract & ~7;
}

void daPlBase_c::setCcAtSlip() {
    setCcAtBody(dCc_c::ATTACK_SLIP);
}

void daPlBase_c::setCcAtPenguinSlip() {
    if (isStatus(STATUS_3C)) {
        setCcAtBody(dCc_c::ATTACK_PENGUIN_SLIDE);
        mAttCc2.mCcData.mAttackCategoryInteract = BIT_FLAG(dCc_c::ATTACK_PENGUIN_SLIDE);
        mAttCc1.mCcData.mAttackCategoryInteract = BIT_FLAG(dCc_c::ATTACK_PENGUIN_SLIDE);
    }
}

void daPlBase_c::setCcAtHipAttack() {
    setCcAtBody(dCc_c::ATTACK_HIP_ATTK);
    mAttCc2.mCcData.mAttackCategoryInteract = 0;
    mAttCc1.mCcData.mAttackCategoryInteract = 0;
}

void daPlBase_c::setCcAtStar() {
    mCc.mCcData.mAttackCategory = dCc_c::ATTACK_STAR;
    if ((mAttCc2.mCcData.mCategoryInteract & 7) == 0) {
        mCc1.mCcData.mAttackCategory = dCc_c::ATTACK_STAR;
    }
    if (mAttCc1.mCcData.mAttackCategory != dCc_c::ATTACK_UNK0 &&
        mAttCc1.mCcData.mAttackCategory != dCc_c::ATTACK_YOSHI_EAT
    ) {
        mAttCc3.mCcData.mOffsetX = mAttCc1.mCcData.mOffsetX;
        mAttCc3.mCcData.mOffsetY = mAttCc1.mCcData.mOffsetY;
        mAttCc3.mCcData.mWidth = mAttCc1.mCcData.mWidth;
        mAttCc3.mCcData.mHeight = mAttCc1.mCcData.mHeight;
        mAttCc3.mCcData.mAttackCategory = dCc_c::ATTACK_STAR;
    }
    onStatus(STATUS_84);
}

void daPlBase_c::setCcAtCannon() {
    setCcAtBody(dCc_c::ATTACK_CANNON);
}

void daPlBase_c::entryCollision() {
    if (!isStatus(STATUS_04) && !isStatus(STATUS_06)) {
        int lineKind = getCcLineKind();
        mCc.mNonCollideMask = lineKind;
        if (mAttCc1.mCcData.mAttackCategory == dCc_c::ATTACK_WIRE_NET) {
            lineKind ^= 3;
        }
        mAttCc1.mNonCollideMask = lineKind;
        mAttCc3.mNonCollideMask = lineKind;
        mCc.mLayer = mLayer;
        mAttCc1.mLayer = mLayer;
        mAttCc3.mLayer = mLayer;
        if (isStatus(STATUS_74)) {
            mCc.mCcData.mFlag |= 1;
        } else {
            mCc.mCcData.mFlag &= ~1;
        }
        if (!isStatus(STATUS_7A)) {
            mCc.entry();
            if (isStatus(STATUS_78)) {
                mCc1.entry();
            }
        }
        if (mAttCc1.mCcData.mAttackCategory != 0) {
            mAttCc1.entry();
        }
        if (mAttCc2.mCcData.mAttackCategory != 0) {
            mAttCc2.entry();
        }
        if (mAttCc3.mCcData.mAttackCategory != 0) {
            mAttCc3.entry();
        }
    }
}

bool daPlBase_c::isActionRevisionY() {
    if (isStatus(STATUS_35) || isStatus(STATUS_36)) {
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
    } else if (!(fabsf(offsY) < 1.0f || other->isActionRevisionY())) {
        if (m_1070) {
            float tmp1 = offsX * f;
            float tmp2 = m_106c * m_1064;
            if (fabsf(tmp1) < fabsf(tmp2)) {
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
        if (isStatus(STATUS_2C) || m_1074) {
            tmp = 0.0f;
        }
        if (isDemoType(DEMO_4) && m_d40 & 1) {
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
                if (isDemoType(DEMO_4) || isStatus(STATUS_5F)) {
                    return false;
                }
                float f1 = m_1060;
                float f2 = mSpeedF;
                if (f2 * f1 <= 0.0f && fabsf(f2) + fabsf(f1) > 2.5f) {
                    mSpeedF = f1 * 0.4f;
                    return true;
                }
                if (fabsf(mSpeedF) > fabsf(m_1060) && mSpeedF > 1.5f) {
                    mSpeedF = 0.0f;
                    return true;
                }
            }
        }
    }
    return false;
}

bool daPlBase_c::isEnableStampPlayerJump(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *other = (daPlBase_c *) cc2->mpOwner;
    if (m_d40 & 1) {
        return false;
    }
    if (mSpeed.y >= 0.0f) {
        return false;
    }
    if (isStatus(STATUS_1C) || isStatus(STATUS_1F)) {
        return false;
    }
    if (other->isLiftUp() || other->m_10) {
        return false;
    }
    if (isDemoType(DEMO_4) || other->isDemoType(DEMO_4)) {
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
    if (!isStatus(STATUS_3A)) {
        if (mSpeed.y > 0.0f) {
            mSpeed.y = 0.0f;
        }
        setReductionScale();
    } else {
        mSpeed.y = -1.0f;
    }
}

void daPlBase_c::setStampPlayerJump(bool b, float f) {
    if (!isStatus(STATUS_3A)) {
        float scale = 3.628f;
        if (isMameAction()) {
            scale = 3.278f;
        }
        if (b) {
            dQuake_c::m_instance->shockMotor(mPlayerNo, 7, 0, 0);
            if (mKey.buttonJump()) {
                scale = 4.128f;
            }
            vf3fc(scale, mSpeedF, 1, 1, 0);
        } else {
            vf3fc(scale, mSpeedF, 1, 0, 0);
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
    if (!isStatus(STATUS_0A) || m_04 == 0) {
        m_0c = 4;
    }
    m_10 = 10;
    vf434(52, 0);
    dQuake_c::m_instance->shockMotor(mPlayerNo, 7, 0, 0);
}

void daPlBase_c::calcJumpDaiReductionScale(int i1, int i2) {
    if (i1 > i2) {
        i1 = i2;
    }
    m_00 = 1;
    m_08 = (0.6f * i1) / i2;
}

void daPlBase_c::setReductionBoyon() {
    m_00 = 3;
    m_04 = 4;
}

void daPlBase_c::calcReductionScale() {
    switch (m_00) {
        case 1:
            m_00 = 2;
            break;
        case 2:
            setReductionBoyon();
            break;
        case 3:
            if (m_04) {
                int idx = m_04 - 1;
                static const float floats[] = { 0.0f, 0.2f, 0.0f, 0.4f };
                static const float floats2[] = { 0.02f, 0.025f, 0.04f, 0.06f };
                if (sLib::chase(&m_08, floats[idx], floats2[idx])) {
                    m_04--;
                }
            }
            if (m_04 == 0) {
                m_00 = 0;
            }
            break;
    }
}

mVec3_c daPlBase_c::getReductionModelScale() {
    return mVec3_c(
        1.0f + m_08,
        1.0f - m_08,
        1.0f + m_08
    );
}

void daPlBase_c::setNoHitPlayer(const daPlBase_c *player, int i) {
    mpNoHitPlayer = player;
    m_18 = i;
}

void daPlBase_c::updateNoHitPlayer() {
    if (m_18 != 0) {
        m_18--;
    }
    if (m_18 == 0) {
        mpNoHitPlayer = nullptr;
    }
}

daPlBase_c *daPlBase_c::getHipAttackDamagePlayer() {
    return (daPlBase_c *) fManager_c::searchBaseByID(mHipAttackPlayerID);
}

void daPlBase_c::setHipAttackDamagePlayer(daPlBase_c *player) {
    onStatus(STATUS_23);
    mHipAttackPlayerID = player->mUniqueID;
}

void daPlBase_c::clearHipAttackDamagePlayer() {
    offStatus(STATUS_23);
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
    m_d48 = m_d40;
    m_d4c = m_d44;
    for (int i = 9; i > 0; i--) {
        m_d50[i] = m_d50[i - 1];
    }
    m_d50[0] = m_d40 & 1;
}

void daPlBase_c::clearBgCheckInfo() {
    setOldBGCross();
    m_d44 = 0;
    m_d40 = 0;
    m_d7c = m_d78;
    m_d78 = 0;
    m_d30 = 0.0f;
    m_d34 = 0.0f;
    m_d38 = 0.0f;
    m_d3c = 0.0f;
    m_d88 = T_0;
    m_dac = 0;
    m_d80.x = 0.0f;
    m_d80.y = 0.0f;
}

void daPlBase_c::bgCheck(int i) {
    offStatus(STATUS_87);
    offStatus(STATUS_86);
    offStatus(STATUS_2C);
    offStatus(STATUS_5D);
    if (m_d40 & 1) {
        m_d8c = mPos.y;
    }
    clearBgCheckInfo();
    sLib::calcTimer(&m_da0);
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
    if (mPowerup == POWERUP_MINI_MUSHROOM && (m_d48 & 0x60) == 0) {
        float dir = 1.0f;
        if (mLastPos.x > mPos.x) {
            dir = -1.0f;
        }
        void *wallData = getWallBgPointData();
        int wallSize = *((int *)((char *)wallData + 4)) + *((int *)((char *)wallData + 8));
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
    m_d96 = m_d94;
    m_d9a = m_d98;
    m_d98 = 0;
    m_d94 = 0;
    m_d78 = mBc.mLastUnitType;

    if ((bgFlags & 0x3c000000) != 0) {
        m_d40 |= 2;
        if (bgFlags & 0x4000000) {
            m_d44 |= 0x800000;
        }
        if (bgFlags & 0x40000000) {
            m_d44 |= 0x80000000;
        }
        if (bgFlags & 0x80000000) {
            m_d44 |= 0x200000;
        }
        u16 headAttr = mBc.getHeadAttr();
        if (headAttr == 6) {
            m_d44 |= 4;
        }
        if (bgFlags & 0x20000000) {
            m_d44 |= 0x1000000;
            switch (mBc.m_c4) {
                case 1:
                    m_d44 |= 0x4000000;
                    break;
                case 2:
                    m_d44 |= 0x2000000;
                    break;
            }
            if (mBc.mpCtrFoot != nullptr &&
                mBc.mpCtrFoot->mpActor != nullptr &&
                !(mBc.mpCtrFoot->mpActor->mProfName != fProfile::EN_HATENA_BLOCK_LINE &&
                mBc.mpCtrFoot->mpActor->mProfName != fProfile::EN_RENGA_BLOCK_LINE)
            ) {
                m_d44 |= 0x8000000;
            }
        }
    }

    if ((bgFlags & 0x1fe000)) {
        m_d94 = mBc.getSakaAngleBySpeed(mSpeedF);
        m_d98 = mBc.getSakaAngle(0);
        if (bgFlags & 0x4000) {
            m_d44 |= 0x80;
        }
        if (bgFlags & 0x400000) {
            m_d44 |= 0x40000000;
        }
        if (bgFlags & 0x800000) {
            m_d44 |= 0x400000;
        }
        if (bgFlags & 0x1000000) {
            m_d44 |= 0x10;
        }
        if (m_d94 > 0 && (m_d40 & 2)) {
            m_d40 |= 0x2000000;
            mSpeedF = 0.0f;
        }
        u16 footAttr = mBc.getFootAttr();
        switch (footAttr) {
            case 3:
                m_d40 |= 0x8000000;
                break;
            case 7:
                m_d44 |= 8;
                break;
            case 4:
                m_d40 |= 0x80000000;
                break;
            case 5:
                m_d40 |= 0x40000000;
                break;
        }
        if (mSpeed.y <= 0.0f) {
            m_d40 |= 1;
            if (bgFlags & 0x200000) {
                m_d44 |= 1;
            }
            if (bgFlags & 0x18000) {
                m_d44 |= 2;
            }
            if (mPos.y > mLastPos.y && !(m_d44 & 0x80)) {
                m_d44 |= 0x20;
            }
            switch (footAttr) {
                case 2:
                    m_d40 |= 0x400000;
                    m_d88 = T_1;
                    break;
                case 3:
                case 12:
                    m_d40 |= 0x4000000;
                    if (m_d44 & 2) {
                        m_d88 = T_7;
                    } else {
                        m_d88 = T_2;
                    }
                    break;
                case 15:
                    m_d40 |= 0x4000000;
                    m_d88 = T_9;
                    break;
                case 13:
                    m_d88 = T_4;
                    break;
                case 1:
                    m_d88 = T_3;
                    if (bgFlags & 0x2000000) {
                        m_d40 |= 0x1000000;
                    } else {
                        m_d40 |= 0x800000;
                    }
                    break;
                case 8:
                    if (isStatus(STATUS_3C)) {
                        mVec3_c tmp(
                            mPos.x,
                            mPos.y + 4.0f,
                            mPos.z
                        );
                        float outCheckGround;
                        if (dBc_c::checkGround(&tmp, &outCheckGround, mLayer, m_ca1, 8) && fabsf(outCheckGround - mPos.y) < 2.0f) {
                            m_d44 |= 0x100;
                        }
                    } else {
                        m_d44 |= 0x100;
                    }
                    break;
                case 14:
                    m_d88 = T_6;
                    break;
                case 11:
                    m_d88 = T_A;
                    break;
            }
            if (m_d44 & 2) {
                if (bgFlags & 0x40000) {
                    m_d88 = T_8;
                }
                if (bgFlags & 0x80000) {
                    m_d88 = T_B;
                }
                if (bgFlags & 0x100000) {
                    m_d88 = T_C;
                }
            }
        }
    } else {
        if (isStatus(0x58)) {
            mPos.y = m_dcc;
            m_d94 = 0;
            m_d40 |= 1;
        }
        float s = m_c9c + mPos.y;
        mVec3_c checkPos(mPos.x, s + 8.0f, mPos.z);
        float groundY;
        int groundType;
        if (dBc_c::checkGround(&checkPos, &groundY, &groundType, mLayer, m_ca1, 8)) {
            if (checkPos.y >= groundY && s <= groundY) {
                if (groundType == 2) {
                    m_d44 |= 0x400;
                } else {
                    m_d44 |= 0x200;
                }
                m_dc8 = groundY;
            }
        }
    }

    if (mSpeed.y < 0.0f && (isStatus(0x14) || isStatus(0x4e))) {
        m_d44 |= 0x42;
        m_d40 |= 1;
    }

    if (bgFlags & 0x1fe000) {
        m_d44 |= 0x40;
    } else {
        float groundY3;
        if (dBc_c::checkGroundHalf(&mPos, &groundY3, mLayer, m_ca1) && (mPos.y - 16.0f < groundY3)) {
            m_d44 |= 0x40;
        }
    }

    if (bgFlags & 0x100) {
        m_d44 |= 0x20000000;
    }
    if ((bgFlags & 8) || isStatus(0x1a)) {
        m_d40 |= 0x20;
    }
    if ((bgFlags & 4) || isStatus(0x1b)) {
        m_d40 |= 0x40;
    }

    if (m_d40 & 0x60) {
        float sx = mSpeed.x;
        m_d9c = mBc.mAdjacentSlopeAngle;
        if (sx < 0.0f) {
            if (m_d40 & 0x20) {
                m_d40 |= 4;
            }
        } else if (sx > 0.0f) {
            if (m_d40 & 0x40) {
                m_d40 |= 4;
            }
        } else {
            static int flags[] = { 0x1, 0x2 };
            if (bgFlags & flags[mDirection]) {
                m_d40 |= 4;
            }
        }
    }

    if (bgFlags & 0xc0) {
        dBg_ctr_c *ctrWall = mBc.mpCtrWall;
        if (ctrWall) {
            if (ctrWall->m_d0 & 0x800000) {
                m_d40 |= 0x800;
            }
            if (ctrWall->m_c8 == 0 && *ctrWall->m_bc != 0) {
                m_d40 |= 0x800;
            }
        }
        if (bgFlags & 0x80) {
            m_d40 |= 0x80;
            if (isCarryObjBgCarried(1)) m_d40 |= 0x200;
        }
        if (bgFlags & 0x40) {
            m_d40 |= 0x100;
            if (isCarryObjBgCarried(0)) m_d40 |= 0x400;
        }
    }

    if (bgFlags & 0x2a) {
        m_d40 |= 8;
    }
    if (bgFlags & 0x15) {
        m_d40 |= 0x10;
    }
    if (bgFlags & 0x800) {
        m_d40 |= 0x1000;
    }
    if (bgFlags & 0x1000) {
        m_d40 |= 0x2000;
    }

    if (m_d40 & 0x8000000) {
        checkSinkSand();
    }

    if ((m_d40 & 1) == 0) {
        if (m_cc4 < mPos.y) {
            m_cc4 = mPos.y;
        }
        m_cc8 = m_cc4;
    } else {
        m_cc4 = mPos.y;
    }

    if ((m_d40 & 2) && (m_d44 & 0x800000) && !(m_d44 & 0x80000000)) {
        fn_80056370(nullptr, 10);
    }
    if ((m_d40 & 1) && !(m_d44 & 2)) {
        fn_80056370(nullptr, 9);
    }
    if ((m_d40 & 0x20) && !(m_d40 & 0x80)) {
        fn_80056370(nullptr, 12);
    }
    if ((m_d40 & 0x40) && !(m_d40 & 0x100)) {
        fn_80056370(nullptr, 11);
    }
}

bool daPlBase_c::isCarryObjBgCarried(u8 i) {
    if (mBc.mpCtrWalls[i] != nullptr) {
        ((float *) &m_d30)[i] = mBc.mpCtrWalls[i]->m_a0.x - mBc.mpCtrWalls[i]->m_ac.x;
        if (mBc.mpCtrWalls[i]->mFlags & 0x800) {
            dActor_c* carriedActor = mBc.mpCtrWalls[i]->mpActor;
            if (carriedActor != nullptr && carriedActor->checkCarried(0)) {
                return true;
            }
        }
    }

    return false;
}

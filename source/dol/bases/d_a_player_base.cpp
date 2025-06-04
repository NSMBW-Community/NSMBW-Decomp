#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_attention.hpp>
#include <game/bases/d_quake.hpp>
#include <game/sLib/s_math.hpp>
#include <constants/sound_list.h>

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
    mAttentionMode = 3;
    m_58 = 16.0f;
    mEatBehaviour = 0;
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
    return sLib::chase(&mAngle.y.mAngle, getMukiAngle(mDirection), 0x2000);
}

int daPlBase_c::addCalcAngleY(short p1, short p2) {
    return sLib::addCalcAngle(&mAngle.y.mAngle, p1, p2, 0x2000, 0x400);
}

short daPlBase_c::getBesideMukiAngle(u8 direction) {
    static const short scMukiAngle[] = { 0x4000, -0x4000 };
    return scMukiAngle[direction];
}

void daPlBase_c::turnBesideAngle() {
    sLib::chase(&mAngle.y.mAngle, getBesideMukiAngle(mDirection), 0x2000);
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
    sLib::addCalcAngle(&mAngle.x.mAngle, ang, 4, 0x2000, 0x100);
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
    } else if (dScStage_c::m_instance->mCurrWorld == WORLD_1 && dScStage_c::m_instance->mCurrCourse == STAGE_6 && dScStage_c::m_instance->mCurrAreaNo == 1) {
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
    static void (daPlBase_c::*l_HipActionProc[])() = {
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
        fn_800541e0(rideActor, 5);
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
        fn_800541e0(rideActor, 5);
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
    static void (daPlBase_c::*scDemoAnmFunc[])() = {
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
            if (sLib::chase(&mAngle.y.mAngle, getMukiAngle(mDirection), 0x800)) {
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
    static const float sc_data[][3] = {
        { 1.0f, 1.0f, 1.0f },
        { 1.25f, 0.6875f, 1.0f },
        { 1.25f, 0.6875f, 1.0f },
        { 1.44f, 0.5f, 1.0f },
        { 1.62f, 0.3f, 1.0f },
    };
    return sc_data[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::getSakaStopAccele(u8 direction) {
    static const float sc_data[][3] = {
        {  0.035f, 0.035f, 0.035f },
        {  0.0315f, 0.046f, 0.035f },
        {  0.028f, 0.06f, 0.035f },
        {  0.021f, 0.123f, 0.035f },
        {  0.014f, 0.175f, 0.035f },
    };
    return sc_data[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::getSakaMoveAccele(u8 direction) {
    static const float sc_data[][3] = {
        { 0.1f, 0.03f, 0.03f },
        { 0.04f, 0.03f, 0.03f },
        { 0.05f, 0.045f, 0.03f },
        { 0.07f, 0.12f, 0.03f },
        { 0.17f, 0.17, 0.03f },
    };
    return sc_data[mBc.getSakaType()][mBc.getSakaUpDown(direction)];
}

float daPlBase_c::fn_8004b5e0() {
    static const float sc_data[][3] = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.2f, -0.2f, 0.0f },
        { 0.4f, -0.4f, 0.0f },
        { 0.4f, -0.4f, 0.0f },
    };
    return sc_data[mBc.getSakaDir()][mBc.getSakaType()];
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
                mMaxSpeedF = fn_8004b5e0();
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
    for (int i = 0; i < 2; i++) {
        if (v < 0.1f) {
            v = 0.0f;
        } else {
            v = (v - 0.1f) / 0.9f;
        }
    }
    // if (v < 0.1f) {
    //     v = 0.0f;
    // } else {
    //     v -= 0.1f;
    //     v /= 0.9f;
    // }
    if (isNeg == true) {
        v = -v;
    }
    v *= 1.11111f;
    return v;
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

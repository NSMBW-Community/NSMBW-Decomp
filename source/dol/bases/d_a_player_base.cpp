#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/sLib/s_math.hpp>

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
    m_1110 = 0;
    m_1114 = 0;
    m_1118 = 0;
    offStatus(STATUS_97);
    offStatus(STATUS_98);
    m_110c = param;
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
        mAccelF = getAccelF() * 3.0f;
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
    if (!m_110c) {
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
    return true;
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
    m_1110 = 0;
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
    switch (m_1110) {
        case 0:
            if ((m_d40 & 1) == 0 && (mKey.triggerJump() || mKey.triggerCross())) {
                changeState(StateID_Fall, nullptr);
            } else {
                slipActionMove(angle);
            }
            break;
        case 1:
            mMaxSpeedF = 0.0f;
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                changeState(StateID_Crouch, (void *) 1);
            }
            break;
        case 2:
            if (mpMdlMng->mpMdl->mAnm.isStop()) {
                fn_800488f0(1);
            }
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
    m_1110 = 1;
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
    m_1110 = 2;
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

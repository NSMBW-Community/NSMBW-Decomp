#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_mario_mdl.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(YOSHI, daYoshi_c, 2);

namespace {
    const sCcDatNewF scYoshiCcData = {
        { 0.0f, 16.0f, 8.0f, 16.0f },
        CC_KIND_YOSHI,
        CC_ATTACK_NONE,
        0,
        0,
        CC_STATUS_NONE,
        &daYoshi_c::ccCallBack
    };

    const sCcDatNewF scYoshiAtCcData = {
        { 0.0f, 16.0f, 8.0f, 16.0f },
        CC_KIND_PLAYER_ATTACK,
        CC_ATTACK_NONE,
        0,
        0,
        CC_STATUS_NONE,
        &daYoshi_c::atCcCallBack
    };

    const sBcSensorLine scBcFoot = {
        SENSOR_IS_LINE,
        -0x5000,
        0x4000,
        0
    };

    const sBcSensorLine scBcHead = {
        SENSOR_IS_LINE,
        -0x2000,
        0x1000,
        0x10000
    };

    const sBcSensorLine scBcWall = {
        SENSOR_IS_LINE,
        0x4000,
        0xB000,
        0x8000
    };
}

static const sBcYoshiPointData scBgPointData_Normal = {
    { 0x2c8001, -0x5000, 0x4000, 0 },
    { 0x9c0001, -0x2000, 0x1000, 0x1b000 },
    { 0xc0001, 0x4000, 0x13000, 0x8000 }
};

static const sBcYoshiPointData scBgPointData_Squat = {
    { 0x2c8001, -0x5000, 0x4000, 0 },
    { 0x9c0001, -0x2000, 0x1000, 0x10000 },
    { 0xc0001, 0x4000, 0xb000, 0x8000 }
};

daYoshi_c::daYoshi_c() : mModelMng(dPyMdlMng_c::MODEL_TYPE_YOSHI), mEatStateMgr(*this, sStateID::null) {
    setKind(STAGE_ACTOR_YOSHI);
    mExecStopMask = BIT_FLAG(STAGE_ACTOR_YOSHI);
    mEatBehavior = EAT_TYPE_NONE;
    mpMdlMng = &mModelMng;
    mModelMng.mpMdl->mpOwner = this;
    daPyMng_c::initYoshiPriority(this);
}

daYoshi_c::~daYoshi_c() {}

int daYoshi_c::create() {
    if (!daPyMng_c::setYoshi(this)) {
        return FAILED;
    }

    mVisibleAreaOffset.set(0.0f, 20.0f);
    mVisibleAreaSize.set(32.0f, 32.0f);
    mModelMng.create(ACTOR_PARAM(PlayerNo), 0, dPyMdlMng_c::SCENE_TYPE_0);
    mModelHeight = 31.0f;
    mCenterOffs = mVec3_c(0.0f, mModelHeight / 2.0f, 0.0f);
    mBc.set(this, scBcFoot, scBcHead, scBcWall);
    initCollision((sCcDatNewF *) &scYoshiCcData, (sCcDatNewF *) &scYoshiAtCcData);
    changePlrNo(-1);
    mNum = -1;
    setPowerup(POWERUP_MUSHROOM, 0);

    mSpeedDataNormal = &dAcPy_HIO_Speed_c::sc_player_normal;
    mSpeedDataStar = &dAcPy_HIO_Speed_c::sc_player_normal_star;
    mGravityData = dAcPy_HIO_Speed_c::sc_gravity_data;
    mMaxFallSpeed = -4.0f;
    mAccelY = -0.15f;
    mPlayerRideOn = BASE_ID_NULL;
    int beginType = ACTOR_PARAM(BeginType);
    if (beginType == 1) {
        changeState(StateID_BlockOut);
    } else {
        changeState(StateID_AloneWait);
    }
    changeEatState(StateID_EatNone);
    changeDemoState(StateID_DemoNone);
    daPlBase_c::create();
    onStatus(STATUS_B8);
    mSndObj.m_00 = 4;
    setSoundPlyMode();

    return SUCCEEDED;
}

int daYoshi_c::doDelete() {
    if (mPlayerRideOn != BASE_ID_NULL) {
        return NOT_READY;
    }
    dActor_c *pl = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (pl != nullptr && pl->mKind != STAGE_ACTOR_PLAYER) {
        pl->deleteRequest();
    }
    daPlBase_c::doDelete();
    daPyMng_c::releaseYoshi(this);
    return SUCCEEDED;
}

int daYoshi_c::preExecute() {
    if (daPlBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    if (mSomeTimer != 0) {
        mSomeTimer--;
        return NOT_READY;
    }
    return SUCCEEDED;
}

STATE_DEFINE(daYoshi_c, AloneWait);
STATE_DEFINE(daYoshi_c, BlockOut);
STATE_DEFINE(daYoshi_c, DamageRun);
STATE_DEFINE(daYoshi_c, StartRideOn);
STATE_VIRTUAL_DEFINE(daYoshi_c, Walk);
STATE_VIRTUAL_DEFINE(daYoshi_c, Jump);
STATE_VIRTUAL_DEFINE(daYoshi_c, Fall);
STATE_VIRTUAL_DEFINE(daYoshi_c, Land);
STATE_VIRTUAL_DEFINE(daYoshi_c, Crouch);
STATE_VIRTUAL_DEFINE(daYoshi_c, SitJump);
STATE_VIRTUAL_DEFINE(daYoshi_c, Slip);
STATE_VIRTUAL_DEFINE(daYoshi_c, Turn);
STATE_VIRTUAL_DEFINE(daYoshi_c, HipAttack);
STATE_VIRTUAL_DEFINE(daYoshi_c, Funsui);
STATE_VIRTUAL_DEFINE(daYoshi_c, Cloud);
STATE_DEFINE(daYoshi_c, EatNone);
STATE_DEFINE(daYoshi_c, Eat);
STATE_DEFINE(daYoshi_c, EatOut);
STATE_DEFINE(daYoshi_c, EatMouth);

void daYoshi_c::changeState(const sStateIDIf_c &stateID, void *arg) {
    if (
        mPlayerNo >= 0 ||
        stateID == StateID_AloneWait ||
        stateID == StateID_BlockOut ||
        stateID == StateID_DamageRun ||
        stateID == StateID_Funsui
    ) {
        daPlBase_c::changeState(stateID, arg);
    }
}

short daYoshi_c::getMukiAngle(u8 direction) {
    static const short scMukiAngle[2] = { 0x3800, -0x3800 };
    return scMukiAngle[direction];
}

int daYoshi_c::turnAngle() {
    static const float scChaseStep[] = { 0xc00, 0 };

    if (isStatus(STATUS_B0)) {
        short target = getMukiAngle(mYoshiDirection);
        return mAngle.y.chase(target, scChaseStep[0]);
    } else {
        short target = getMukiAngle(mDirection);
        return mAngle.y.chase(target, scChaseStep[0]);
    }
}

void daYoshi_c::initializeState_AloneWait() {
    onStatus(STATUS_CB);
    if (isNowBgCross(BGC_FOOT)) {
        mModelMng.setAnm(PLAYER_ANIM_WAIT, 0.0f);
        mSubstate = ALONE_WAIT_ACTION_0;
    } else {
        mModelMng.setAnm(PLAYER_ANIM_JUMP2, 0.0f, 0.0f);
        mSubstate = ALONE_WAIT_ACTION_1;
    }
    daPlBase_c *pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl == nullptr) {
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
    }
    mAccelY = getGravityData()[0];
    mMaxFallSpeed = -4.0f;
    mAccelF = 0.0f;
    m_54 = mSpeed.y;
}

void daYoshi_c::finalizeState_AloneWait() {
    offStatus(STATUS_CB);
}

void daYoshi_c::executeState_AloneWait() {
    mAccelY = getGravityData()[0];
    mMaxFallSpeed = -4.0f;
    mAccelF = 0.0f;
    m_54 = mSpeed.y;
    turnAngle();
    switch ((AloneWaitSubstate_e) mSubstate) {
        case ALONE_WAIT_ACTION_2:
            if (mModelMng.isAnmStop()) {
                mSubstate = ALONE_WAIT_ACTION_0;
                mModelMng.setAnm(PLAYER_ANIM_WAIT);
            }
            // fallthrough
        case ALONE_WAIT_ACTION_0:
            if (m_54 < -3.0f) {
                mSpeed.y = 2.0f;
            }
            sLib::chase(&mSpeedF, 0.0f, 0.3f);
            if (dAudio::getBgmAccentSign() >= 1) {
                mSubstate = ALONE_WAIT_ACTION_2;
                static const int scAnm[] = { PLAYER_ANIM_POSE_R, PLAYER_ANIM_POSE_L };
                mModelMng.setAnm(scAnm[mDirection]);
                mModelMng.mpMdl->setFrame(0.0f);
                return;
            }
            if (!isNowBgCross(BGC_FOOT)) {
                mSubstate = ALONE_WAIT_ACTION_1;
                mModelMng.setAnm(PLAYER_ANIM_JUMP2, 10.0f, 0.0f);
            }
            break;
        case ALONE_WAIT_ACTION_1:
            m_54 = mSpeed.y;
            sLib::chase(&mSpeedF, 0.0f, 0.01f);
            if (isNowBgCross(BGC_FOOT)) {
                mSubstate = ALONE_WAIT_ACTION_2;
                mModelMng.setAnm(PLAYER_ANIM_JUMPED);
                setLandSE();
            }
            break;
    }
    if (mSpeedF && checkBGCrossWall(mDirection)) {
        mDirection ^= 1;
    }
}

void daYoshi_c::initializeState_BlockOut() {
    mDirection = DIR_LR_R;
    onStatus(STATUS_7E);
    onStatus(STATUS_7A);
    onStatus(STATUS_AA);
    onStatus(STATUS_97);
    onStatus(STATUS_A8);
    mAccelF = 0.0f;
    mSpeed.y = 0.0f;
    mSpeedF = 0.0f;
    mSubstateTimer = 5;
    mLastPos = mPos;
    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    mdl->m_294 = 0;
    if (isNowBgCross(BGC_WATER_SHALLOW)) {
        mSpeed.y = 4.5f;
        mSubstate = BLOCK_OUT_ACTION_0;
        mAccelY = -0.5f;
        mMaxFallSpeed = sc_MaxFallSpeed;
        mModelMng.setAnm(PLAYER_ANIM_SWIM_WAIT, 10.0f, 0.0f);
    } else {
        mSpeed.y = 0.0f;
        mSubstate = BLOCK_OUT_ACTION_1;
        mAccelY = 0.0f;
        mMaxFallSpeed = sc_MaxFallSpeed;
        mModelMng.setAnm(PLAYER_ANIM_SET, 0.0f, 0.0f);
    }
    startSound(SE_VOC_YS_YOSHI, false);
    mAngle.y = 0;
}

void daYoshi_c::finalizeState_BlockOut() {
    offStatus(STATUS_A8);
    offStatus(STATUS_97);
    offStatus(STATUS_7E);
    offStatus(STATUS_7A);
    offStatus(STATUS_AA);
    offStatus(STATUS_INVISIBLE);
}

void daYoshi_c::executeState_BlockOut() {
    switch ((BlockOutSubstate_e) mSubstate) {
        case BLOCK_OUT_ACTION_0:
            if (mSubstateTimer == 0) {
                mAngle.y = getMukiAngle(mDirection);
                changeState(StateID_AloneWait);
            }
            break;
        case BLOCK_OUT_ACTION_1:
            if (mModelMng.mpMdl->mAnm.checkFrame(20.0f)) {
                offStatus(STATUS_7A);
            }
            if (mModelMng.isAnmStop()) {
                mSubstate = BLOCK_OUT_ACTION_2;
                mModelMng.setAnm(PLAYER_ANIM_SET_TO_WAIT, 0.0f, 0.0f);
            }
            break;
        case BLOCK_OUT_ACTION_2:
            if (mModelMng.isAnmStop()) {
                mAngle.y = getMukiAngle(mDirection);
                changeState(StateID_AloneWait);
            }
            break;
    }
}

void daYoshi_c::initializeState_DamageRun() {
    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    daPlBase_c *pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl == nullptr) {
        mdl->m_294 = 0;
    }
    mModelMng.setAnm(PLAYER_ANIM_RF_JUMP);
    startSound(SE_VCO_YS_DAMAGE, false);
}

void daYoshi_c::finalizeState_DamageRun() {
    offStatus(STATUS_84);
    mSpeedF = 0.0f;
}

void daYoshi_c::executeState_DamageRun() {
    mAccelY = getGravityData()[0];
    mMaxFallSpeed = sc_MaxFallSpeed;
    turnAngle();
    setDamageRunEffect();
    onStatus(STATUS_84);
    mSpeedF = sc_DirSpeed[mDirection] * 2.0f;
    if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND))) {
        mSpeedF *= 0.5f;
    }
    if (
        mDirection == DIR_LR_L && ((mNowBgCross1 >> 5 & 1) | (mNowBgCross1 >> 19 & 1)) ||
        mDirection == DIR_LR_R && ((mNowBgCross1 >> 6 & 1) | (mNowBgCross1 >> 20 & 1))
    ) {
        mDirection ^= 1;
    }
}

void daYoshi_c::setDamageRunEffect() {
    mVec3_c pos;
    mModelMng.mpMdl->getJointPos(&pos, 15);
    mLevelEffect1.createEffect("Wm_mr_yssweatrun", 0, &pos, nullptr, nullptr);
}

void daYoshi_c::initializeState_StartRideOn() {
    onStatus(STATUS_7A);
    mModelMng.setAnm(PLAYER_ANIM_RIDE_ON, 0.0f, 0.0f);
}

void daYoshi_c::finalizeState_StartRideOn() {
    offStatus(STATUS_7A);
}

void daYoshi_c::executeState_StartRideOn() {
    gravitySet();
    mMaxFallSpeed = sc_MaxFallSpeed;
    mMaxSpeedF = 0.0f;
    grandPowerSet();
    turnAngle();

    if (checkCrouch()) {
        return;
    }

    if (mKey.triggerJump()) {
        setJump(sc_JumpSpeed, mSpeedF, true, 0, 1);
    } else if (mModelMng.isAnmStop()) {
        changeState(StateID_Walk, BLEND_DEFAULT);
    }
}

void daYoshi_c::changeWalkAction() {
    if (mSpeedF * mMaxSpeedF < 0.0f && checkTurn()) {
        return;
    }

    changeState(StateID_Walk, BLEND_NONE);
}

bool daYoshi_c::checkWalkNextAction() {
    if (mPlayerRideOn == BASE_ID_NULL) {
        changeState(StateID_AloneWait);
        return true;
    }

    if (isNowBgCross(BGC_CLIFF)) {
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
            changeState(StateID_Fall);
            return true;
        }
    }

    if (checkCrouch()) {
        return true;
    } else {
        return false;
    }
}

void daYoshi_c::initializeState_Walk() {
    AnmBlend_e blend = stateArg<AnmBlend_e>();
    calcSpeedOnIceLift();
    if (mSpeedF) {
        walkActionInit_Move(blend);
    } else {
        walkActionInit_Wait(blend);
    }
    onStatus(STATUS_A0);
    onStatus(STATUS_B2);
}

void daYoshi_c::finalizeState_Walk() {
    daPlBase_c::finalizeState_Walk();
    offStatus(STATUS_A0);
    offStatus(STATUS_B2);
    mWalkAnmState = 0;
}

void daYoshi_c::executeState_Walk() {
    daPlBase_c::executeState_Walk();
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    turnAngle();

    if (checkWalkNextAction()) {
        return;
    }

    if (mKey.buttonWalk(nullptr) && mDirection != mPrevDirection) {
        dAcPy_c *pl = getPlayerRideOn();
        if (pl != nullptr) {
            pl->mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
        }
        if (checkTurn()) {
            return;
        }
    }

    switch ((WalkSubstate_e) mSubstate) {
        case WALK_ANM_STATE_WAIT:
            walkAction_Wait();
            break;
        case WALK_ANM_STATE_MOVE:
            walkAction_Move();
            break;
    }
}

void daYoshi_c::walkActionInit_Wait(AnmBlend_e blend) {
    mSubstate = WALK_ANM_STATE_WAIT;
    setWaitActionAnm(blend);
}

void daYoshi_c::walkAction_Wait() {
    if (mSpeedF) {
        walkActionInit_Move(BLEND_DEFAULT);
    } else {
        setWaitActionAnm(BLEND_DEFAULT);
    }
}

void daYoshi_c::walkActionInit_Move(AnmBlend_e blend) {
    mSubstate = WALK_ANM_STATE_MOVE;
    setWalkActionAnm(blend);
}

void daYoshi_c::walkAction_Move() {
    daPlBase_c::walkAction_Move();
}

void daYoshi_c::setWalkActionAnm(AnmBlend_e blend) {
    float b = 0.0f;
    float absSpeed = std::fabs(mSpeedF);
    if (
        isNowBgCross(BGC_ON_BELT_L) && mSpeedF > 0.0f ||
        isNowBgCross(BGC_ON_BELT_R) && mSpeedF < 0.0f
    ) {
        if (blend == BLEND_DEFAULT) {
            b = 5.0f;
        }
        float rate = 2.0f;
        rate = (absSpeed * 2.5f < 2.0f) ? rate : 5.0f;
        rate = (absSpeed > 5.0f) ? 5.0f : rate;
        mModelMng.setAnm(PLAYER_ANIM_WALK, b, rate, 0.0f);
        return;
    }

    int anmID;
    float speed;
    if (absSpeed <= getSpeedData()->mLowSpeed) {
        anmID = PLAYER_ANIM_RUN;
        speed = (absSpeed * 2.5f < 2.0f) ? 2.0f : absSpeed * 2.5f;
    } else if (absSpeed < getSpeedData()->mHighSpeed) {
        anmID = PLAYER_ANIM_B_DASH;
        speed = (absSpeed * 1.5f < 0.5f) ? absSpeed * 1.5f : 0.5f;
    } else {
        anmID = PLAYER_ANIM_B_DASH2;
        speed = (absSpeed * 1.5f < 0.5f) ? absSpeed * 1.5f : 0.5f;
    }
    if (isSaka() && mBc.getSakaUpDown(mDirection) == 1 && mSpeedF * sc_DirSpeed[mDirection] >= 0.0f) {
        speed = (speed * 2.0f > 4.0f) ? 4.0f : speed * 2.0f;
    }
    if (isNowBgCross(BGC_ON_ICE)) {
        if (absSpeed < getSpeedData()->mMediumSpeed) {
            speed = 4.0f;
            speed = (speed * 8.0f > 4.0f) ? 4.0f : speed * 8.0f;
        }
        if (!mKey.buttonWalk(nullptr)) {
            calcAccOnIceLift();
            setWaitActionAnm(blend);
            return;
        }
    }
    mWalkAnmState = 0;
    onStatus(STATUS_62);
    if (isNowBgCross((BgCross1_e) (BGC_ON_SINK_SAND | BGC_IN_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            speed *= 0.7f;
        } else {
            speed *= 0.5f;
        }
    }
    if (blend == BLEND_DEFAULT) {
        b = mModelMng.getHIO(anmID).mBlendDuration;
    }
    mModelMng.setAnm(anmID, speed, b, 0.0f);
}

void daYoshi_c::setWaitActionAnm(AnmBlend_e blend) {
    if (mModelMng.getFlags() & 0x20) {
        int prevAnm = mModelMng.getPrevAnm();
        float blendDuration = 0.0f;
        if (prevAnm != -1) {
            if (blend == BLEND_DEFAULT) {
                blendDuration = mModelMng.getHIO(prevAnm).mBlendDuration;
                if (blendDuration < 0.0f) {
                    blendDuration = 0.0f;
                }
            }
            mModelMng.setAnm(prevAnm, mModelMng.getAnm2().getRate(), blendDuration, mModelMng.getAnm2().getFrame());
            return;
        }
    }
    if (blend == BLEND_DEFAULT) {
        mModelMng.setAnm(PLAYER_ANIM_WAIT, 10.0f, 0.0f);
    } else {
        mModelMng.setAnm(PLAYER_ANIM_WAIT, 10.0f);
    }
}

void daYoshi_c::setJumpCommonBase() {
    offNowBgCross(BGC_FOOT);
    setAddLiftSpeedF();
    gravitySet();
    maxFallSpeedSet();
}

void daYoshi_c::setJumpSpeed() {
    float baseSpeed = 0.0f;
    float absSpeedF = std::fabs(mSpeedF);
    if (absSpeedF >= getSpeedData()->mHighSpeed || absSpeedF >= 2.8f) {
        baseSpeed = 0.3f;
    } else if (absSpeedF > 1.5f) {
        baseSpeed = 0.24f;
    } else if (absSpeedF > 0.7f) {
        baseSpeed = 0.18f;
    }

    float jumpSpeed;
    if (isNowBgCross((BgCross1_e) (BGC_ON_SINK_SAND | BGC_IN_SINK_SAND))) {
        if (isNowBgCross(BGC_IN_SINK_SAND)) {
            jumpSpeed = sc_JumpSpeedNuma1;
        } else {
            jumpSpeed = sc_JumpSpeedNuma2;
        }
    } else {
        jumpSpeed = sc_JumpSpeed + baseSpeed;
    }

    mSpeed.y = jumpSpeed;
}

void daYoshi_c::setJumpAnm() {
    if (isStatus(STATUS_STAR_JUMP)) {
        mModelMng.setAnm(PLAYER_ANIM_STAR_ROLL_DUPLICATE);
    } else if (isStatus(STATUS_KANI_JUMP)) {
        mModelMng.setAnm(PLAYER_ANIM_W_JUMP1, 0.0f, 0.0f);
    } else if (isStatus(STATUS_SIT_JUMP)) {
        mModelMng.setAnm(PLAYER_ANIM_S_JUMP);
    } else {
        mModelMng.setAnm(PLAYER_ANIM_JUMP, 0.0f, 0.0f);
    }
}

void daYoshi_c::setFunbariJumpEffect() {
    startSound(SE_PLY_YOSHI_FJUMP, false);
    mVec3_c pos;
    mModelMng.mpMdl->getJointPos(&pos, 15);
    mAng3_c ang(0, 0, 0);
    if (mDirection == DIR_LR_L) {
        ang.y = DEG_TO_ANGLE(-180);
    }
    mLevelEffect2.createEffect("Wm_mr_yssweat", 0, &pos, &ang, nullptr);
}

void daYoshi_c::calcYoshiJump() {
    gravitySet();
    switch (m_58) {
        case 0:
            if (!m_59) {
                m_58 = 4;
            } else if (mSpeed.y < 0.0f) {
                if (isStatus(STATUS_SIT_JUMP)) {
                    mModelMng.setAnm(PLAYER_ANIM_S_JUMP2);
                } else {
                    mModelMng.setAnm(STATUS_STUNNED, 10.0f, 0.0f);
                }
                m_58 = 1;
            }
            break;
        case 1:
            if (mSpeed.y < -2.0f && mKey.buttonYoshiJump()) {
                m_59 = false;
                m_58 = 2;
                mModelMng.setAnm(PLAYER_ANIM_F_JUMP);
            }
            break;
        case 2:
            setFunbariJumpEffect();
            float rate = mSpeed.y;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            mModelMng.mpMdl->setFunbariRate(rate / 2.0f);
            float limit = 1.5f;
            if (std::fabs(mMaxSpeedF) > limit) {
                if (mMaxSpeedF > 0.0f) {
                    mMaxSpeedF = limit;
                } else {
                    mMaxSpeedF = -limit;
                }
                mAccelF = 0.05f;
            }
            if (mSpeed.y > -1.0f) {
                mAccelY = 0.07f;
            } else {
                mAccelY = 0.2f;
            }
            if (isNowBgCross(BGC_HEAD)) {
                m_58 = 4;
            } else if (!mKey.buttonYoshiJump() || mSpeed.y > 2.4f) {
                mModelMng.setAnm(PLAYER_ANIM_JUMP2, 10.0f, 0.0f);
                m_58 = 4;
            }
            break;
    }
}

void daYoshi_c::updateJumpAction() {
    turnAngle();

    if (setHipAttackAction()) {
        return;
    }

    if (isNowBgCross((BgCross1_e) (BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
        mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    }
    calcYoshiJump();

    if (!isNowBgCross(BGC_FOOT)) {
        return;
    }

    setLandJumpEffect(1);
    if (isStatus(STATUS_SIT_JUMP)) {
        changeState(StateID_Crouch, CROUCH_ARG_FROM_SIT_JUMP);
    } else if (!checkWalkNextAction()) {
        mAngle.x = 0;
        if (isStatus(STATUS_JUMP) && isNowBgCross(BGC_GROUNDED_MOVE_UP)) {
            changeState(StateID_Land, true);
        } else if (mSpeedF) {
            changeWalkAction();
        } else {
            if (isStatus(STATUS_KANI_JUMP)) {
                mModelMng.setAnm(PLAYER_ANIM_2JUMPED_DUPLICATE, 0.0f, 0.0f);
            } else {
                mModelMng.setAnm(PLAYER_ANIM_JUMPED);
            }
            changeState(StateID_Land, false);
        }
    }
}

void daYoshi_c::initializeState_Jump() {
    daPlBase_c::initializeState_Jump();
    m_58 = 0;
    onStatus(STATUS_B2);
    mSpeedMax.x = 0.0f;
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    setStartJumpEffect(1);
    jmpInf_c *jmpInf = stateArg<jmpInf_c *>();
    int jumpMode = 1;
    if (jmpInf != nullptr) {
        jumpMode = jmpInf->mJumpMode;
        if (jumpMode == 2 && !mKey.buttonJump()) {
            jumpMode = 0;
        }
    }
    switch (jumpMode) {
        case 2:
            startSound(SE_PLY_JUMP_S_HIGH, false);
            break;
        case 1:
            startSound(SE_PLY_JUMP, false);
            break;
    }
    if (jmpInf != nullptr && jmpInf->mSpeed) {
        mSpeed.y = jmpInf->mSpeed;
    } else {
        setJumpSpeed();
    }
    setJumpAnm();
    setJumpCommonBase();
}

void daYoshi_c::finalizeState_Jump() {
    daPlBase_c::finalizeState_Jump();
    offStatus(STATUS_B2);
}

void daYoshi_c::executeState_Jump() {
    daPlBase_c::executeState_Jump();
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    updateJumpAction();
}

void daYoshi_c::setFallAction() {
    if (isNowBgCross(BGC_FOOT)) {
        return;
    }

    if (mPlayerRideOn == BASE_ID_NULL) {
        changeState(StateID_AloneWait);
    } else {
        changeState(StateID_Fall);
    }
}

void daYoshi_c::initializeState_Fall() {
    daPlBase_c::initializeState_Fall();
    onStatus(STATUS_B2);
    m_58 = 0;
    setJumpCommonBase();
    moveSpeedSet();
}

void daYoshi_c::finalizeState_Fall() {
    daPlBase_c::finalizeState_Fall();
    offStatus(STATUS_B2);
}

void daYoshi_c::executeState_Fall() {
    daPlBase_c::executeState_Fall();
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    powerSet();
    updateJumpAction();
    setDelayHelpJump();
}

void daYoshi_c::initializeState_Land() {
    if (mSubstate != LAND_ACTION_1) {
        return;
    }

    mSpeedF = 0.0f;
    mModelMng.setAnm(PLAYER_ANIM_2JUMPED, 2.0f, 0.0f, 0.0f);
}

void daYoshi_c::finalizeState_Land() {}

void daYoshi_c::executeState_Land() {
    gravitySet();
    maxFallSpeedSet();
    simpleMoveSpeedSet();
    powerSet();
    if (checkWalkNextAction()) {
        return;
    }

    if (isNowBgCross((BgCross1_e) (BGC_WALL_TOUCH_L_2 | BGC_WALL_TOUCH_R_2))) {
        mKey.offStatus(dAcPyKey_c::STATUS_DISABLE_LR);
    }

    if (mSubstate == 0) {
        if (mKey.buttonWalk(nullptr) || mModelMng.isAnmStop()) {
            changeWalkAction();
        }
    } else {
        mSpeedF = 0.0f;
        if (mKey.buttonWalk(nullptr) && mModelMng.mpMdl->mAnm.getFrame() >= 10.0f || mModelMng.isAnmStop()) {
            changeWalkAction();
        }
    }
}

void daYoshi_c::initializeState_SitJump() {
    m_58 = 0;
    onStatus(STATUS_CAN_SPIN);
    onStatus(STATUS_9E);
    onStatus(STATUS_SIT_JUMP);
    onStatus(STATUS_JUMP);
    onStatus(STATUS_B2);
    mSpeedMax.x = 0.0f;
    int arg = stateArg<int>();
    if (arg == 0) {
        startSound(SE_PLY_JUMP, false);
        setJumpSpeed();
        setJumpAnm();
    } else {
        mModelMng.setAnm(PLAYER_ANIM_S_JUMP2);
    }
    setStartJumpEffect(1);
    setJumpCommonBase();
}

void daYoshi_c::finalizeState_SitJump() {
    mAngle.x = 0;
    mAngle.y = getMukiAngle(mDirection);
    offStatus(STATUS_JUMP);
    offStatus(STATUS_9E);
    offStatus(STATUS_CAN_SPIN);
    offStatus(STATUS_88);
    offStatus(STATUS_SIT_JUMP);
    offStatus(STATUS_B2);
}

void daYoshi_c::executeState_SitJump() {
    gravitySet();
    maxFallSpeedSet();
    moveSpeedSet();
    airPowerSet();
    updateJumpAction();
    if (!(mModelMng.getFlags() & 1)) {
        offStatus(STATUS_SIT_JUMP);
    }
    if (mSpeed.y < 0.0f) {
        offStatus(STATUS_AA);
        if (!mKey.buttonCrouch() && !checkStandUpRoof()) {
            jmpInf_c jmpInf(mSpeed.y, 0, BLEND_DEFAULT);
            changeState(StateID_Jump, &jmpInf);
        }
    }
}

bool daYoshi_c::checkCrouch() {
    if (
        !isStatus(STATUS_51) &&
        !isStatus(STATUS_SIT_JUMP) &&
        mKey.buttonCrouch() &&
        !isNowBgCross((BgCross1_e) (BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) &&
        !isStatus(STATUS_BE)
    ) {
        changeState(StateID_Crouch, CROUCH_ARG_FROM_WALK);
        return true;
    }
    return false;
}

void daYoshi_c::initializeState_Crouch() {
    mSubstate = 0;
    switch (stateArg<CrouchArg_e>()) {
        case CROUCH_ARG_FROM_WALK:
            if (!isNowBgCross(BGC_WATER_SHALLOW)) {
                mModelMng.setAnm(PLAYER_ANIM_STOOP_START);
            } else {
                mModelMng.setAnm(PLAYER_ANIM_STOOP_START, 3.0f, 0.0f, dPyMdlBase_c::scWaterCrouchAnmSpeed);
            }
            break;
        case CROUCH_ARG_FROM_OTHER:
            mModelMng.setAnm(PLAYER_ANIM_STOOP, 3.0f, 0.0f);
            break;
        case CROUCH_ARG_FROM_SIT_JUMP:
            mModelMng.setAnm(PLAYER_ANIM_S_JUMPED, 0.0f);
            break;
    }
    startSound(SE_PLY_QUAT, true);
    onStatus(STATUS_51);
    onStatus(STATUS_B2);
}

void daYoshi_c::finalizeState_Crouch() {
    offStatus(STATUS_AA);
    offStatus(STATUS_51);
    offStatus(STATUS_A0);
    offStatus(STATUS_B2);
}

void daYoshi_c::executeState_Crouch() {
    gravitySet();
    maxFallSpeedSet();
    if (isNowBgCross(BGC_FOOT)) {
        mMaxSpeedF = 0.0f;
        grandPowerSet();
    } else {
        simpleMoveSpeedSet();
        airPowerSet();
    }
    turnAngle();
    CrouchAction_Ground();
    int anmID = mModelMng.getAnm();
    if ((anmID == PLAYER_ANIM_STOOP_START || anmID == PLAYER_ANIM_S_JUMPED) && mModelMng.isAnmStop()) {
        mModelMng.setAnm(PLAYER_ANIM_STOOP, 10.0f, 0.0f);
    }
}

bool daYoshi_c::setCancelCrouch() {
    daPlBase_c::setCancelCrouch();
}

void daYoshi_c::setCrouchSmokeEffect() {
    if (mSpeedF && !isNowBgCross(BGC_WATER_SHALLOW)) {
        mVec3_c pos;
        mModelMng.mpMdl->getJointPos(&pos, 1);
        setBrakeSmokeEffect(pos);
        setSlipSE();
    }
}

void daYoshi_c::CrouchAction_Ground() {
    offStatus(STATUS_A0);

    if (isNowBgCross((BgCross1_e) (BGC_IN_SINK_SAND | BGC_ON_SINK_SAND)) && setCancelCrouch()) {
        return;
    }

    if (setCrouchJump()) {
        return;
    }

    if (checkCrouchSlip()) {
        return;
    }

    if (!isNowBgCross(BGC_FOOT)) {
        changeState(StateID_SitJump, true);
    } else {
        setCrouchSmokeEffect();
        if (!mKey.buttonCrouch() && !setCancelCrouch()) {
            return;
        }
    }
}

void daYoshi_c::initializeState_Slip() {
    daPlBase_c::initializeState_Slip();
}

void daYoshi_c::finalizeState_Slip() {
    daPlBase_c::finalizeState_Slip();
}

void daYoshi_c::slipActionMove(int a) {
    if (isNowBgCross(BGC_FOOT) && std::fabs(mSpeedF) > 0.1f) {
        setSlipSmokeEffect();
        setSlipSE();
    }
    daPlBase_c::slipActionMove(a);
}

void daYoshi_c::executeState_Slip() {
    daPlBase_c::executeState_Slip();
}

void daYoshi_c::setTurnSmokeEffect() {
    mVec3_c pos;
    mModelMng.mpMdl->getJointPos(&pos, 7);
    setBrakeSmokeEffect(pos);
    setTurnSmokeEffect();
}

void daYoshi_c::setTurnMoveSpeed() {
    sTurnPowerData data;
    getTurnPower(data);
    if (mSubstate != 2) {
        mMaxSpeedF = 0.0f;
        if (isSaka()) {
            u8 dir = mDirection;
            if (mSpeedF < 0.0f) {
                dir = 1;
            } else if (mSpeedF > 0.0f) {
                dir = 0;
            }
            if (mBc.getSakaUpDown(dir) == 1) {
                mAccelF = data.mSakaUp;
            } else {
                mAccelF = data.mSakaDown;
            }
        } else {
            mAccelF = data.mNormal;
        }
    } else {
        int dir;
        if (mKey.buttonWalk(&dir)) {
            if (mKey.buttonDush()) {
                mMaxSpeedF = sc_DirSpeed[dir] * getSpeedData()->mHighSpeed;
            } else {
                mMaxSpeedF = sc_DirSpeed[dir] * getSpeedData()->mLowSpeed;
            }
        } else {
            mMaxSpeedF = 0.0f;
        }
        mAccelF = data.mAir;
    }
}

void daYoshi_c::initializeState_Turn() {
    u8 dir = 0;
    mSubstate = 0;
    mModelMng.setAnm(PLAYER_ANIM_TURN, 0.0f, 0.0f);
    if (mSpeedF < 0.0f) {
        dir = 1;
    }
    mAngle.y = getMukiAngle(dir);
    mDirection ^= 1;
    mAccelY = getGravityData()[0];
    mMaxFallSpeed = sc_MaxFallSpeed;
    setTurnMoveSpeed();
    dAcPy_c *pl = getPlayerRideOn();
    if (pl != nullptr) {
        pl->mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
    }
}

void daYoshi_c::finalizeState_Turn() {
    fadeOutTurnEffect();
}

void daYoshi_c::executeState_Turn() {
    if (checkWalkNextAction()) {
        return;
    }

    setTurnMoveSpeed();
    switch ((TurnSubstate_e) mSubstate) {
        case TURN_ACTION_0:
            setTurnSmokeEffect();
            if (mSpeedF == 0.0f) {
                mSubstateTimer = 8;
                mSubstate = TURN_ACTION_1;
            }
            if (!mKey.buttonWalk(nullptr)) {
                if (mSpeedF) {
                    mDirection = (mSpeedF < 0.0f) ? DIR_LR_L : DIR_LR_R;
                }
                setTurnEnd();
            }
            break;
        case TURN_ACTION_1:
            setTurnSmokeEffect();
            if (mDirection != mPrevDirection) {
                setTurnEnd();
            } else if (mSubstateTimer == 0) {
                fadeOutTurnEffect();
                mSubstate = TURN_ACTION_2;
                mModelMng.setAnm(PLAYER_ANIM_TURNED, 1.0f, 0.0f, 0.0f);
            }
            break;
        case TURN_ACTION_2:
            if (mDirection != mPrevDirection || mModelMng.isAnmStop()) {
                setTurnEnd();
            }
            break;
    }
}

bool daYoshi_c::setHipAttackAction() {
    if (
        !isStatus(STATUS_A7) &&
        !isNowBgCross(BGC_FOOT) &&
        !isNowBgCross(BGC_37) &&
        mKey.checkHipAttack()
    ) {
        changeState(StateID_HipAttack, HIP_ATTACK_ARG_PLAYER);
        return true;
    }
    return false;
}

bool daYoshi_c::setHipAttackOnEnemy(mVec3_c *attachPos) {
    if (isState(StateID_HipAttack) && isStatus(STATUS_HIP_ATTACK_FALL)) {
        onStatus(STATUS_PRESS_ATTACH);
        mPressAttachPos = *attachPos;
        return true;
    }

    return false;
}

void daYoshi_c::initializeState_HipAttack() {
    daPlBase_c::initializeState_HipAttack();
    onStatus(STATUS_91);
}

void daYoshi_c::finalizeState_HipAttack() {
    daPlBase_c::finalizeState_HipAttack();
    offStatus(STATUS_91);
}

void daYoshi_c::executeState_HipAttack() {
    daPlBase_c::executeState_HipAttack();
}

void daYoshi_c::initializeState_Cloud() {
    daPlBase_c::initializeState_Cloud();
    onStatus(STATUS_B2);
    dAcPy_c *pl = getPlayerRideOn();
    if (pl != nullptr) {
        pl->setScrollMode(6);
    }
}

void daYoshi_c::finalizeState_Cloud() {
    daPlBase_c::finalizeState_Cloud();
    offStatus(STATUS_B2);
    dAcPy_c *pl = getPlayerRideOn();
    if (pl != nullptr) {
        pl->setScrollMode(0);
    }
}

float daYoshi_c::getCloudOffsetY() {
    return dPyMdlMng_c::m_hio.mYoshiModel->mCloudOffset;
}

void daYoshi_c::setCloudStateMove() {
    setWaitActionAnm(BLEND_NONE);
    mSubstate = CLOUD_ANM_STATE_MOVE;
}

void daYoshi_c::setCloudStateCrouch() {
    onStatus(STATUS_51);
    mModelMng.setAnm(PLAYER_ANIM_STOOP_START);
    mSubstate = CLOUD_ANM_STATE_CROUCH;
}

void daYoshi_c::executeState_Cloud() {
    if (updateCloudMove()) {
        return;
    }

    switch (mSubstate) {
        case CLOUD_ANM_STATE_INITIAL:
            if (mModelMng.isAnmStop()) {
                setCloudStateMove();
            }
            // fallthrough
        case CLOUD_ANM_STATE_MOVE:
            if (mKey.buttonCrouch()) {
                setCloudStateCrouch();
            }
            break;
        case CLOUD_ANM_STATE_CROUCH:
            if (!mKey.buttonCrouch()) {
                offStatus(STATUS_51);
                mModelMng.mpMdl->setRate(1.0f);
                mSubstate = CLOUD_ANM_STATE_END_CROUCH;
            }
            break;
        case CLOUD_ANM_STATE_END_CROUCH:
            if (mModelMng.isAnmStop()) {
                setCloudStateMove();
            }
            break;
    }
}

void daYoshi_c::initializeState_Funsui() {
    daPlBase_c::initializeState_Funsui();
    onStatus(STATUS_91);
}

void daYoshi_c::finalizeState_Funsui() {
    daPlBase_c::finalizeState_Funsui();
    offStatus(STATUS_91);
}

void daYoshi_c::executeState_Funsui() {
    daPlBase_c::executeState_Funsui();
}

void daYoshi_c::releaseFunsuiAction() {
    setFallAction();
}

dAcPy_c *daYoshi_c::getPlayerRideOn() const {
    if (mPlayerRideOn != BASE_ID_NULL) {
        return (dAcPy_c *) fManager_c::searchBaseByID(mPlayerRideOn);
    }
    return nullptr;
}

bool daYoshi_c::checkRideOffAble() {
    if (
        isStatus(STATUS_91) ||
        isNowBgCross(BGC_IN_SINK_SAND) ||
        isEnableCreateEgg()
    ) {
        return false;
    }
    return true;
}

bool daYoshi_c::fn_8014eb70(dAcPy_c *player, int i) {
    if (player->mRideActorID != BASE_ID_NULL) {
        return false;
    }

    player->releaseCarryActor();
    changePlrNo(player->getPlrNo());
    mModelMng.mpMdl->mpSpinLiftParentMdl = player->getModel();
    mPlayerRideOn = player->getID();
    onStatus(STATUS_4C);
    changeState(StateID_StartRideOn);
    mDirection = player->mDirection;

    if (i == 0) {
        startSound(SE_VOC_YS_YOSHI, false);
        dQuake_c::getInstance()->shockMotor(player->getPlrNo(), dQuake_c::TYPE_7, 0, false);
    } else {
        mAngle.y = getMukiAngle(mDirection);
    }
    player->setRideOnYoshi(this);

    return true;
}

void daYoshi_c::setRideOffPlayer() {
    mModelMng.mpMdl->mpSpinLiftParentMdl = nullptr;
    if (!isDemo()) {
        if (fManager_c::searchBaseByID(m_4c) != nullptr && setDamageSpitOut(false)) {
            changeEatState(StateID_EatOut);
        }
        changeState(StateID_AloneWait);
    }

    changePlrNo(-1);
    daPyMng_c::setYoshiPriority(this);
    offStatus(STATUS_4C);
    mPlayerRideOn = BASE_ID_NULL;
}

void daYoshi_c::changePlrNo(u8 plrNo) {
    mPlayerNo = plrNo;
    mModelMng.mpMdl->mPlayerNo = plrNo;
    if (mPlayerNo != -1) {
        mKey.mRemoconID = mPlayerNo;
        mBc.mOwningPlrNo = mPlayerNo;
        mSndObj.m_58 = dAudio::getRemotePlayer(mPlayerNo);
        mExecStopMask = BIT_FLAG(STAGE_ACTOR_PLAYER);
    } else {
        mKey.mRemoconID = -1;
        mBc.mOwningPlrNo = 0;
        mSndObj.m_58 = dAudio::getRemotePlayer(0);
        if (!isStatus(STATUS_GOAL_POLE_TOUCHED)) {
            mExecStopMask = BIT_FLAG(STAGE_ACTOR_YOSHI);
        }
    }
}

void daYoshi_c::executeEatState() {
    mEatStateMgr.executeState();
}

void daYoshi_c::changeEatState(const sStateIDIf_c &state) {
    mEatAction = EAT_ACTION_START_TURN_WAIT;
    mEatStateMgr.changeState(state);
}

void daYoshi_c::initializeState_EatNone() {
    onStatus(STATUS_B1);
}

void daYoshi_c::finalizeState_EatNone() {
    offStatus(STATUS_B1);
}

void daYoshi_c::executeState_EatNone() {
    if (isStatus(STATUS_B2)) {
        setCcAtYoshiMouthReq();
    }
}

void daYoshi_c::setHitTongueActor(dActor_c *actor) {
    dActor_c *hitTongueActor =  (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (hitTongueActor != nullptr) {
        hitTongueActor->mEatState = EAT_STATE_NONE;
        hitTongueActor->mEatenByID = BASE_ID_NULL;
    }
    m_4c = actor->getID();
    setEatAction_Success(0);
    actor->mEatState = EAT_STATE_EATING;
    actor->mEatenByID = getID();
    setEatTongueCall(actor);
}

bool daYoshi_c::setEatAction() {
    if (mKey.triggerEat()) {
        if (!isStatus(STATUS_B1) || isStatus(STATUS_HIP_ATTACK)) {
            return false;
        }

        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
        }

        dActor_c *hitTongueActor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
        if (hitTongueActor == nullptr) {
            if (m_64 == nullptr) {
                changeEatState(StateID_Eat);
                return true;
            }
        } else {
            changeEatState(StateID_EatOut);
            return true;
        }
    }

    return false;
}

bool daYoshi_c::fn_8014f030(dAcPy_c *player) {
    if (!isEatState(StateID_EatNone)) {
        return false;
    }

    if (fManager_c::searchBaseByID(m_4c) != nullptr && fManager_c::searchBaseByID(m_4c) == player) {
        changeEatState(StateID_EatOut);
        return true;
    } else {
        return false;
    }
}

bool daYoshi_c::setDamageSpitOut(bool b) {
    if (isEatState(StateID_EatOut)) {
        b = true;
    }

    daPlBase_c *pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl != nullptr) {
        if (!b) {
            if (pl->mKind != STAGE_ACTOR_PLAYER) {
                return false;
            }
            if (pl->isItemKinopio()) {
                return false;
            }
        } else if (pl->mKind == STAGE_ACTOR_PLAYER) {
            pl->mTimer_a8 = 10;
        }

        pl->mEatState = EAT_STATE_SPIT;
        setEatSpitOutCall(pl);
        setEatOutSE();
        releaseEatActor();
        return true;
    }

    return false;
}

void daYoshi_c::initializeState_Eat() {
    mNum = mPlayerNo;
    onStatus(STATUS_B0);
    mYoshiDirection = mDirection;
    int absAngle1 = mAngle.y.abs();
    int absAngle2 = abs(getMukiAngle(mYoshiDirection));
    if (absAngle2 <= absAngle1) {
        setEatAction_StartTurnWait();
    } else {
        setEatAction_Start();
    }
    m_5c = 10;
    m_2e8 = 0;
}

void daYoshi_c::finalizeEatCommon() {
    mNum = -1;

    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;

    daPlBase_c *pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl != nullptr && pl->mEatState == EAT_STATE_EATING) {
        setEatTongueOffCall(pl);
        releaseEatActor();
    }

    pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl == nullptr) {
        mdl->m_294 = 0;
    }
    if (mFruitCount >= 5) {
        mFruitCount = 4;
    }
}

void daYoshi_c::finalizeState_Eat() {
    offStatus(STATUS_AF);
    offStatus(STATUS_B0);
    offStatus(STATUS_B3);
    offStatus(STATUS_DISABLE_STATE_CHANGE);

    mModelMng.mpMdl->m_17c &= ~BIT_FLAG(3);
    mModelMng.mpMdl->releaseBodyAnm(0.0f);
    finalizeEatCommon();
}

bool daYoshi_c::calcOpenMouth() {
    switch (m_7c) {
        case 1:
            if (mModelMng.getAnm2().checkFrame(9.0f)) {
                m_7c = 2;
                m_80 = mModelMng.getAnm2().getRate();
                if (mModelMng.mpMdl->isBodyAnmOn()) {
                    mModelMng.mpMdl->setBodyRate(0.0f);
                } else {
                    mModelMng.mpMdl->setRate(0.0f);
                }
            }
            break;
        case 2:
            if (m_78 != 0) {
                m_78--;
            }
            if (m_78 == 0) {
                m_7c = 0;
                if (mModelMng.mpMdl->isBodyAnmOn()) {
                    mModelMng.mpMdl->setBodyRate(m_80);
                } else {
                    mModelMng.mpMdl->setRate(m_80);
                }
            }
            break;
        default:
            break;
    }
    return m_7c != 0;
}

void daYoshi_c::setEatAction_StartTurnWait() {
    mEatAction = EAT_ACTION_START_TURN_WAIT;
}

void daYoshi_c::EatAction_StartTurnWait() {
    if (mPlayerRideOn == BASE_ID_NULL) {
        changeEatState(StateID_EatNone);
        return;
    }

    int absAngle1 = mAngle.y.abs();
    int absAngle2 = abs(getMukiAngle(mYoshiDirection));
    if (absAngle2 <= absAngle1) {
        setEatAction_Start();
    }
}

void daYoshi_c::setEatAction_Start() {
    onStatus(STATUS_AF);
    mModelMng.mpMdl->m_17c |= BIT_FLAG(3);
    if (isNowBgCross(BGC_FOOT)) {
        mModelMng.mpMdl->setBodyAnm(PLAYER_ANIM_R_EAT, dPyMdlMng_c::getHIO(PLAYER_ANIM_R_EAT).mRate, 0.0f, 0.0f);
    } else {
        float c = dPyMdlMng_c::getHIO()->mYoshiModel->mData[2];
        float b = dPyMdlMng_c::getHIO()->mYoshiModel->mData[1];
        float a = dPyMdlMng_c::getHIO()->mYoshiModel->mData[0];
        mModelMng.mpMdl->setBodyAnm(PLAYER_ANIM_R_EAT, a, b, c);
    }

    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    mdl->m_294 = 1;
    mdl->m_298 = dYoshiMdl_c::scTongueLengthMax;
    mEatAction = EAT_ACTION_START;
    startSound(SE_PLY_YOSHI_TONGUE, false);
}

void daYoshi_c::EatAction_Start() {
    setCcAtYoshiEatReq();
    if (mModelMng.getAnm2().isStop()) {
        setEatAction_Fail();
    }
}

void daYoshi_c::setEatActorMouthIn() {
    offStatus(STATUS_B0);
    daPlBase_c *pl = (daPlBase_c *) fManager_c::searchBaseByID(m_4c);
    if (pl != nullptr) {
        pl->mEatState = EAT_STATE_EATEN;
        setEatMouthCall(pl);
    }

    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    mdl->m_294 = 2;
    offStatus(STATUS_AF);
    if (isEnableCreateEgg()) {
        onStatus(STATUS_B3);
        onStatus(STATUS_DISABLE_STATE_CHANGE);
        onStatus(STATUS_C6);
    }
}

void daYoshi_c::setEatAction_Success(int a) {
    float frame = 0.0f;
    m_78 = 0;
    m_7c = 0;
    if (a == 0) {
        if (mModelMng.getAnm() == PLAYER_ANIM_R_EAT_FAIL) {
            frame = mModelMng.getAnm2().getFrame();
        }
    } else {
        frame = 15.0f;
    }
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        u8 behavior = actor->mEatBehavior;
        if (behavior == EAT_TYPE_DRINK_BIG || a == 1) {
            if (isEnableCreateEgg()) {
                mModelMng.mpMdl->m_17c &= ~BIT_FLAG(3);
                mModelMng.mpMdl->releaseBodyAnm(0.0f);
                mModelMng.setAnm(PLAYER_ANIM_R_EAT_SUCCESSB_DUPLICATE);
            } else {
                mModelMng.setBodyAnm(PLAYER_ANIM_R_EAT_SUCCESSB);
            }
            mModelMng.mpMdl->setFrame(frame);
            mEatAction = EAT_ACTION_SUCCESS_DRINK_BIG;
        } else if (behavior == EAT_TYPE_DRINK) {
            mModelMng.setBodyAnm(PLAYER_ANIM_R_EAT_SUCCESSB);
            mModelMng.mpMdl->setFrame(frame);
            mEatAction = EAT_ACTION_SUCCESS_DRINK;
        } else {
            mModelMng.setBodyAnm(PLAYER_ANIM_R_EAT_SUCCESS);
            mModelMng.mpMdl->setFrame(frame);
            mEatAction = EAT_ACTION_SUCCESS_EAT;
        }
    }
    startSound(SE_VOC_YS_EAT, false);
}

void daYoshi_c::EatAction_SuccessEat() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor == nullptr) {
        m_4c = BASE_ID_NULL;
        changeEatState(StateID_EatNone);
        return;
    }

    int frame = 9.0f - mModelMng.getAnm2().getFrame();
    if (frame < 10) {
        m_5c = frame;
    }
    if (mModelMng.getAnm2().checkFrame(9.0f)) {
        m_5c = 0;
        setEatActorMouthIn();
    }

    if (mModelMng.getAnm2().isStop()) {
        if (actor->mEatBehavior == EAT_TYPE_FIREBALL) {
            actor->deleteRequest();
            m_4c = BASE_ID_NULL;
            dActor_c *fire = dActor_c::construct(fProf::YOSHI_FIRE, mPlayerNo << 4, nullptr, nullptr, nullptr);
            m_4c = fire->getID();
        } else if (actor->mEatBehavior == EAT_TYPE_ICEBALL) {
            actor->deleteRequest();
            m_4c = BASE_ID_NULL;
            dActor_c *fire = dActor_c::construct(fProf::YOSHI_FIRE, mPlayerNo << 4 | 1, nullptr, nullptr, nullptr);
            m_4c = fire->getID();
        }
        changeEatState(StateID_EatNone);
    }
}

void daYoshi_c::EatAction_SuccessDrink() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    int frame = 9.0f - mModelMng.getAnm2().getFrame();
    if (frame >= 0 && frame < 10) {
        m_5c = frame;
    }

    if (calcOpenMouth()) {
        return;
    }

    if (mModelMng.getAnm2().checkFrame(10.0f)) {
        setEatActorMouthIn();
    } else if (mModelMng.getAnm2().checkFrame(27.0f)) {
        setEatGlupDownCall(actor);
        releaseEatActor();
        startSound(SE_VOC_YS_GOKUN, false);
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
    } else if (mModelMng.getAnm2().getFrame() >= 28.0f) {
        changeEatState(StateID_EatNone);
    }
}

void daYoshi_c::eatDrinkBigCommonAction() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (mModelMng.getAnm2().checkFrame(44.0f)) {
        setEatGlupDownCall(actor);
        releaseEatActor();
        startSound(SE_VOC_YS_GOKUN, false);
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
    } else if (isEnableCreateEgg()) {
        if (mModelMng.getAnm2().isStop()) {
            checkYoshiEggCommon();
        }
    } else if (mModelMng.getAnm2().getFrame() >= 45.0f) {
        changeEatState(StateID_EatNone);
    }
}

void daYoshi_c::EatAction_SuccessDrinkBig() {
    int frame = 9.0f - mModelMng.getAnm2().getFrame();
    if (frame >= 0 && frame < 10) {
        m_5c = frame;
    }

    if (!calcOpenMouth()) {
        if (mModelMng.getAnm2().checkFrame(10.0f)) {
            setEatActorMouthIn();
        } else {
            eatDrinkBigCommonAction();
        }
    }
}

void daYoshi_c::setEatAction_Fail() {
    if (mModelMng.getPrevAnm() != PLAYER_ANIM_R_EAT_FAIL) {
        mModelMng.setBodyAnm(PLAYER_ANIM_R_EAT_FAIL);
    }
    mEatAction = EAT_ACTION_FAIL;
}

void daYoshi_c::EatAction_Fail() {
    if (mModelMng.getAnm2().getFrame() < 8.0f) {
        setCcAtYoshiEatReq();
    }

    if (mModelMng.getAnm2().checkFrame(9.0f)) {
        offStatus(STATUS_B0);
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
    }

    if (mModelMng.getAnm2().isStop()) {
        changeEatState(StateID_EatNone);
    }
}

void daYoshi_c::executeState_Eat() {
    if (mPlayerRideOn == BASE_ID_NULL) {
        mYoshiDirection = mDirection;
    }

    static void (daYoshi_c::*l_EatActionProc[])() = {
        &daYoshi_c::EatAction_StartTurnWait,
        &daYoshi_c::EatAction_Start,
        &daYoshi_c::EatAction_SuccessEat,
        &daYoshi_c::EatAction_SuccessDrink,
        &daYoshi_c::EatAction_SuccessDrinkBig,
        &daYoshi_c::EatAction_Fail
    };

    (this->*l_EatActionProc[mEatAction])();
}

void daYoshi_c::initializeState_EatMouth() {
    onStatus(STATUS_B4);
    mNum = mPlayerNo;
    mYoshiDirection = mDirection;
    m_5c = 0;
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        actor->mEatenByID = getID();
        setEatTongueCall(actor);
    }
    setEatActorMouthIn();
    mModelMng.mpMdl->m_17c |= BIT_FLAG(3);
    setEatAction_Success(1);
    if (!isEnableCreateEgg()) {
        mSomeTimer = 5;
    }
}

void daYoshi_c::finalizeState_EatMouth() {
    offStatus(STATUS_B4);
    mModelMng.mpMdl->m_17c &= ~BIT_FLAG(3);
    mModelMng.mpMdl->releaseBodyAnm(0.0f);
    finalizeEatCommon();
    m_68 = 30;
    offStatus(STATUS_B3);
}

void daYoshi_c::executeState_EatMouth() {
    if (mPlayerRideOn == BASE_ID_NULL) {
        mYoshiDirection = mDirection;
    }
    eatDrinkBigCommonAction();
}

void daYoshi_c::initializeState_EatOut() {
    mModelMng.mpMdl->m_17c |= BIT_FLAG(4);
    mModelMng.setBodyAnm(PLAYER_ANIM_R_EAT_OUT);
    m_60 = 0;
    m_64 = 30;
}

void daYoshi_c::finalizeState_EatOut() {
    mModelMng.mpMdl->m_17c &= ~BIT_FLAG(4);
    mModelMng.mpMdl->releaseBodyAnm(0.0f);
    if (m_60 == 0 && fManager_c::searchBaseByID(m_4c) != nullptr) {
        setDamageSpitOut(false);
    }
    if (fManager_c::searchBaseByID(m_4c) == nullptr) {
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
    }
}

void daYoshi_c::setEatOutSE() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        if (actor->mProfName == fProf::YOSHI_FIRE) {
            startSound(SE_PLY_YOSHI_FIRE, false);
        } else {
            startSound(SE_VOC_YS_ATTACK, false);
        }
    }
}

void daYoshi_c::executeState_EatOut() {
    turnAngle();
    if (mModelMng.getAnm2().checkFrame(8.0f)) {
        m_60 = 1;
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
        if (actor != nullptr) {
            actor->mEatState = EAT_STATE_SPAT;
            if (setEatSpitOutCall(actor)) {
                setEatOutSE();
                releaseEatActor();
            }
        }
    }

    if (mModelMng.getAnm2().isStop()) {
        changeEatState(StateID_EatNone);
    }
}

bool daYoshi_c::releaseEatActor() {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        actor->mEatState = EAT_STATE_NONE;
        actor->mEatenByID = BASE_ID_NULL;
        m_4c = BASE_ID_NULL;
        dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
        mdl->m_294 = 0;
        return true;
    }
    return false;
}

void daYoshi_c::clearHitTongueReserve() {
    m_84 = nullptr;
    m_88 = 256.0f;
    m_8c = 1;
}

bool daYoshi_c::checkHitTongueReserve(dCc_c *cc) {
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        return false;
    }

    if (isEatState(StateID_EatNone)) {
        return false;
    }

    dActor_c *owner = cc->getOwner();
    if (owner == nullptr) {
        return false;
    }

    if (owner->mEatState != EAT_STATE_NONE) {
        return false;
    }

    float diff = std::fabs(mPos.x - cc->getCenterPosX());
    if (m_88 > diff) {
        m_84 = cc;
        m_88 = diff;
        if (owner->mEatBehavior != EAT_TYPE_NONE) {
            m_8c = 1;
        } else {
            m_8c = 0;
        }
        return true;
    }

    return false;
}

void daYoshi_c::setHitTongueReserve() {
    if (m_84 == nullptr) {
        return;
    }

    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    float x = m_84->getCenterPosX();
    if (m_8c == 1) {
        setHitTongueActor(m_84->getOwner());
        mdl->setTongueCcCheck(x);
    } else {
        setEatAction_Fail();
        if (x < mPos.x) {
            x = m_84->getLeftPos();
        } else {
            x = m_84->getRightPos();
        }
        mVec3_c efPos(x, mCc.getCenterPosY(), mPos.z);
        setTongueHitEffect(efPos);
        mdl->setTongueCcCheck(x);
    }
    clearHitTongueReserve();
}

bool daYoshi_c::checkHitMouth(dActor_c *a) {
    if (m_68 != 0) {
        return false;
    }

    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        return false;
    }

    if (!isEatState(StateID_EatNone)) {
        return false;
    }

    if (a == nullptr) {
        return false;
    }

    if (a->mEatState != EAT_STATE_NONE) {
        return false;
    }

    if (a->mEatBehavior != EAT_TYPE_NONE) {
        m_4c = a->getID();
        changeEatState(StateID_EatMouth);
        return true;
    }

    return false;
}

void daYoshi_c::createYoshiEggCommon() {
    if (createYoshiEgg()) {
        changeEatState(StateID_EatNone);
        offStatus(STATUS_B3);
        offStatus(STATUS_DISABLE_STATE_CHANGE);
        onStatus(STATUS_C7);
    }
}

bool daYoshi_c::createYoshiEgg() {
    if (isEnableCreateEgg()) {
        mMtx_c mtx;
        mVec3_c pos;
        mModelMng.mpMdl->getJointMtx(&mtx, 8);
        mtx.concat(mMtx_c::createTrans(-10.0f, 0.0f, 0.0f));
        mtx.multVecZero(pos);
        if (mFruitCount >= 5) {
            dEnemyMng_c::m_instance->createYoshiEgg(pos, ACTOR_PARAM(Color), mDirection ^ 1, mPlayerNo);
            mFruitCount = 0;
            return true;
        }
        if (m_94 > 0) {
            dEnemyMng_c::m_instance->SanboEatCreateYoshiEgg(pos, ACTOR_PARAM(Color), mDirection ^ 1, mPlayerNo, m_94);
            m_94 = 0;
            return true;
        }
    }

    return false;
}

bool daYoshi_c::isEnableCreateEgg() {
    if (mFruitCount >= 5) {
        return true;
    }
    return m_94 > 0;
}

void daYoshi_c::addFruitCount() {
    mFruitCount++;
    if (mFruitCount > 5) {
        mFruitCount = 5;
    }
}

void daYoshi_c::setEatTongueCall(dActor_c *actor) {
    if (actor != nullptr) {
        m_70 = 1;
        actor->setEatTongue(this);
        m_70 = 0;
    }
}

void daYoshi_c::setEatTongueOffCall(dActor_c *actor) {
    if (actor != nullptr) {
        m_70 = 1;
        actor->setEatTongueOff(this);
        m_70 = 0;
    }
}

void daYoshi_c::setEatMouthCall(dActor_c *actor) {
    if (actor != nullptr) {
        m_70 = 1;
        actor->setEatMouth(this);
        m_70 = 0;
    }
}

bool daYoshi_c::setEatSpitOutCall(dActor_c *actor) {
    int spitOut = false;
    if (actor != nullptr) {
        m_70 = 1;
        spitOut = actor->setEatSpitOut(this);
        m_70 = 0;
    }
    return spitOut;
}

bool daYoshi_c::setEatGlupDownCall(dActor_c *actor) {
    int gulpDown = false;
    if (actor != nullptr) {
        m_70 = 1;
        gulpDown = actor->setEatGlupDown(this);
        m_70 = 0;
    }
    return gulpDown;
}

void daYoshi_c::executeMain() {
    if (isChange()) {
        return;
    }

    if (!isStatus(STATUS_NO_ANIM)) {
        mModelMng.play();
    }

    setFootSound();
    mPrevDirection = mDirection;
    int dir;
    if (!isStatus(STATUS_96) && !isStatus(STATUS_97) && mKey.buttonWalk(&dir)) {
        mDirection = dir;
    }

    if (!executeDemoState() && !(mExecStopReq & mExecStopMask)) {
        updateRideNat();
        calcTimerProc();
        if (m_64 != 0) {
            m_64--;
        }
        if (m_68 != 0) {
            m_68--;
        }
        selectAction();
        executeEatState();
        if (!isStatus(STATUS_B3)) {
            executeState();
            calcPlayerSpeedXY();
        }
        setCcData();
        if (isStar()) {
            setCcAtStar();
        }
        setBcData();
        bgCheck(1);
        checkDispOver();
        if (mPlayerRideOn == BASE_ID_NULL && getVisTop() < dBg_c::m_bg_p->get_8fe00()) {
            deleteRequest();
            return;
        }

        onStatus(STATUS_77);
        if (mPlayerRideOn != BASE_ID_NULL && mKey.mActionTriggered) {
            dAcPy_c *pl = getPlayerRideOn();
            if (pl != nullptr) {
                pl->mKey.onStatus(dAcPyKey_c::STATUS_SHAKE_COOLDOWN);
            }
        }
    }
}

void daYoshi_c::selectAction() {
    if (mPlayerRideOn == BASE_ID_NULL) {
        return;
    }

    if (isStatus(STATUS_QUAKE_BIG)) {
        offStatus(STATUS_QUAKE_BIG);
        setDamage(nullptr, DAMAGE_4);
    } else if (isStatus(STATUS_QUAKE_SMALL)) {
        offStatus(STATUS_QUAKE_SMALL);
        if (isNowBgCross(BGC_FOOT)) {
            setWaitJump(4.0f);
        }
    } else {
        if (setEatAction()) {
            return;
        }
    }
}

void daYoshi_c::executeLastPlayer() {
    if (!isStatus(STATUS_CAN_EXECUTE)) {
        return;
    }

    clearHipAttackDamagePlayer();
    mModelMng.calc(mPos, mAngle, mScale);
    if (isStatus(STATUS_7B)) {
        offStatus(STATUS_7B);
        setCcAtYoshiEat();
        clearHitTongueReserve();
    }

    if (isStatus(STATUS_7C)) {
        offStatus(STATUS_7C);
        setCcAtYoshiMouth();
    }

    if (isStatus(STATUS_77)) {
        entryCollision();
        offStatus(STATUS_77);
        offStatus(STATUS_78);
    }

    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr) {
        actor->eatMove(this);
    }
}

void daYoshi_c::executeLastAll() {
    if (!isStatus(STATUS_CAN_EXECUTE)) {
        return;
    }

    setHitTongueReserve();
    calcHeadAttentionAngle();
    mModelMng.calc2();
    if (isStatus(STATUS_C6)) {
        offStatus(STATUS_C6);
        stopOther();
        daPyMng_c::mStopTimerInfo |= BIT_FLAG(mPlayerNo);
    }
    if (isStatus(STATUS_C7)) {
        offStatus(STATUS_C7);
        playOther();
        daPyMng_c::mStopTimerInfo &= ~BIT_FLAG(mPlayerNo);
    }
}

int daYoshi_c::draw() {
    mModelMng.draw();
    return SUCCEEDED;
}

s8 &daYoshi_c::getPlrNo() {
    if (m_70 == 1 && mNum != -1) {
        return mNum;
    }
    return mPlayerNo;
}

void daYoshi_c::setZPosition() {
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            mPos.z = player->mPos.z;
        }
    } else if (mAmiLayer == 1 && mLayer == 0) {
        mPos.z = 700.0f - mPlayerLayer * 32;
    } else {
        mPos.z = -2900.0f - mPlayerLayer * 32;
    }
}

void daYoshi_c::setZPosition(float z) {
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->setZPosition(z);
            setZPosition();
        }
    }
}

void daYoshi_c::setZPositionDirect(float z) {
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->setZPositionDirect(z);
            setZPosition();
        }
    }
}

void daYoshi_c::offZPosSetNone() {
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->offZPosSetNone();
        }
    }
}

bool daYoshi_c::setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    return _setJump(jumpSpeed, speedF, allowSteer, keyMode, jumpMode);
}


bool daYoshi_c::_setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) {
    if (isDemo() || isStatus(STATUS_OUT_OF_PLAY)) {
        return false;
    }

    clearJumpActionInfo(0);
    bool check = false;
    mSpeedF = speedF;
    if (jumpSpeed <= 0.0f) {
        if (isStatus(STATUS_80)) {
            if (isOldBgCross(BGC_FOOT)) {
                check = true;
            }
        } else if (isNowBgCross(BGC_FOOT)) {
            check = false;
        }
    }
    if (!check) {
        bool b = false;
        if (isStatus(STATUS_SIT_JUMP)) {
            b = true;
        }
        if (jumpSpeed == 0.0f) {
            changeState(StateID_Fall, false);
        } else {
            jmpInf_c jmpInf(jumpSpeed, jumpMode, BLEND_DEFAULT);
            changeState(StateID_Jump, &jmpInf);
        }
        onStatus(STATUS_AB);
        onStatus(STATUS_88);
        if (b) {
            changeState(StateID_SitJump, true);
        }
    }
    offStatus(STATUS_A7);
    offNowBgCross(BGC_FOOT);
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

bool daYoshi_c::isNoDamage() {
    if (
        mPlayerRideOn == BASE_ID_NULL ||
        isDemo() ||
        isDemoType(DEMO_PLAYER) ||
        isStatus(STATUS_DISPLAY_OUT_DEAD) ||
        isStatus(STATUS_84)
    ) {
        return true;
    }

    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr && (player->mDamageInvulnTimer | player->mPowerupChangeInvulnTimer) != 0) {
        return true;
    }

    return false;
}

bool daYoshi_c::setDamage(dActor_c *hitActor, DamageType_e type) {
    if (isNoDamage()) {
        return false;
    }

    int plrNo = mPlayerNo;
    if (setDamage2(hitActor, type)) {
        dQuake_c::getInstance()->shockMotor(plrNo, dQuake_c::TYPE_4, 0, false);
    }

    return false;
}

bool daYoshi_c::setForcedDamage(dActor_c *hitActor, DamageType_e type) {
    if (isDemo()) {
        return false;
    }

    if (setDamage2(hitActor, type)) {
        dQuake_c::getInstance()->shockMotor(mPlayerNo, dQuake_c::TYPE_4, 0, false);
    }

    return false;
}

bool daYoshi_c::setDamage2(dActor_c *hitActor, DamageType_e type) {
    if (isStatus(STATUS_OUT_OF_PLAY) || isStatus(STATUS_STUNNED)) {
        return false;
    }

    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            setDamageSpitOut(false);
            switch (type) {
                case DAMAGE_HIP_ATTACK:
                case DAMAGE_4:
                case DAMAGE_CLIMB:
                case DAMAGE_6:
                    player->setDamage(hitActor, type);
                    break;
                case DAMAGE_11:
                    setReductionScale();
                    break;
                default:
                    player->mDamageInvulnTimer = 127;
                    player->setJump(sc_JumpSpeed + 1.0f, 0.0f, true, 2, 0);
                    changeState(StateID_DamageRun);
                    break;
            }
        }
    }
    return true;
}

const sBcYoshiPointData *daYoshi_c::getBgPointData() {
    if (mModelMng.getFlags() & 1) {
        return &scBgPointData_Normal;
    } else {
        return &scBgPointData_Squat;
    }
}

float daYoshi_c::getStandHeadBgPointY() {
    return scBgPointData_Normal.mHead.mOffset / 4096.0f;
}

void daYoshi_c::setBcData() {
    offStatus(STATUS_B8);
    const sBcYoshiPointData *data = getBgPointData();
    mFootBcData = data->mFoot;
    mHeadBcData = data->mHead;
    mWallBcData = data->mWall;
    dAcPy_c *player = getPlayerRideOn();
    if (player == nullptr) {
        mFootBcData.mFlags = 0x801;
        mHeadBcData.mFlags = 0x801;
        mWallBcData.mFlags = 0x801;
        onStatus(STATUS_B8);
    } else {
        if (isState(StateID_HipAttack)) {
            switch (mModelMng.getAnm()) {
                case PLAYER_ANIM_HIPAT:
                    if (mSpeed.y < 0.0f) {
                        mFootBcData.mFlags |= BIT_FLAG(20) | BIT_FLAG(22);
                        if (isStatus(STATUS_C4)) {
                            mFootBcData.mFlags |= BIT_FLAG(23);
                        }
                    }
                    break;
                case PLAYER_ANIM_HIPED:
                    if (isStatus(STATUS_C4)) {
                        mFootBcData.mFlags |= BIT_FLAG(20) | BIT_FLAG(23);
                    }
                    break;
                default:
                    break;
            }
        }
        if (isStatus(STATUS_SLIP_ACTIVE)) {
            mFootBcData.mFlags |= BIT_FLAG(16);
        }
        if (isStatus(STATUS_A4)) {
            mFootBcData.mFlags |= BIT_FLAG(3);
        }
        if (isStatus(STATUS_EXTRA_PUSH_FORCE)) {
            mFootBcData.mFlags |= BIT_FLAG(5);
        }
        if (isStar()) {
            mWallBcData.mFlags |= BIT_FLAG(25);
            mHeadBcData.mFlags |= BIT_FLAG(25);
        }
        if (isStatus(STATUS_82)) {
            mHeadBcData.mFlags |= BIT_FLAG(26);
        }
        if (player->isItemKinopio()) {
            onStatus(STATUS_B8);
        }
    }
    mBc.set(this, mFootBcData, mHeadBcData, mWallBcData);
    mBc.mAmiLine = getCcLineKind();
    mRc.mLineKind = getCcLineKind();
    mBc.mLayer = mLayer;
    mRc.mLayer = mLayer;
}

void daYoshi_c::postBgCross() {
    daPlBase_c::postBgCross();
    if (!isNowBgCross(BGC_61)) {
        if (!isStatus(STATUS_SLOPE_HEAD_PUSH_L) && !isStatus(STATUS_SLOPE_HEAD_PUSH_R)) {
            if (
                isNowBgCross(BGC_WALL_TOUCH_L_2) && mSpeedF < 0.0f ||
                isNowBgCross(BGC_WALL_TOUCH_R_2) && mSpeedF > 0.0f
            ) {
                mSpeedF = 0.0f;
                mMaxSpeedF = 0.0f;
                mFinalAirPushForceX = 0.0f;
                m_1138 = 0.0f;
            }
        }
    }

    if (
        isNowBgCross(BGC_FOOT) ||
        isStatus(STATUS_53) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_OUT_OF_PLAY)
    ) {
        if (!isNowBgCross(BGC_62)) {
            clearTreadCount();
        }
    }
    if (!isNowBgCross(BGC_FOOT)) {
        return;
    }
    clearJumpActionInfo(1);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_JUMP);
    mKey.offStatus(dAcPyKey_c::STATUS_FORCE_NO_JUMP);
}

float daYoshi_c::getSandSinkRate() {
    float rate = 1.0f;
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            rate = player->getSandSinkRate();
        }
    }
    return rate;
}

void daYoshi_c::clearJumpActionInfo(int) {
    offStatus(STATUS_SINK_SAND_JUMP);
    offStatus(STATUS_A7);
    m_59 = 1;
}

void daYoshi_c::setCcAtYoshiEatReq() {
    onStatus(STATUS_7B);
}

void daYoshi_c::setCcAtYoshiEat() {
    mVec3_c pos1;
    mVec3_c pos2;
    mModelMng.mpMdl->getJointPos(&pos1, 23);
    mModelMng.mpMdl->getJointPos(&pos2, 17);
    // [TODO]
}

void daYoshi_c::setCcAtYoshiMouthReq() {
    onStatus(STATUS_7C);
}

void daYoshi_c::setCcAtYoshiMouth() {
    mMtx_c mtx;
    mVec3_c pos;
    mModelMng.mpMdl->getJointMtx(&mtx, 17);
    mtx.concat(mMtx_c::createTrans(5.0f, 3.0f, 0.0f));
    mtx.multVecZero(pos);
    pos -= mPos;
    mAttCc1.mCcData.mBase.mOffset.set(pos.x, pos.y);
    mAttCc1.mCcData.mBase.mSize.set(4.0f, 4.0f);
    mAttCc1.mCcData.mVsKind |= 0x42;
    mAttCc1.mCcData.mAttack = CC_ATTACK_YOSHI_MOUTH;
}

void daYoshi_c::setCcData() {
    static const sCcDatNew scCcData = {
        { 0.0f, 16.0f },
        { 8.0f, 16.0f }
    };

    static const sCcDatNew scSitCcData = {
        { 0.0f, 8.0f },
        { 8.0f, 8.0f }
    };

    static const sCcDatNew scSwimCcData = {
        { 0.0f, 8.0f },
        { 6.0f, 8.0f }
    };

    const sCcDatNew *data = &scCcData;
    if (mModelMng.getFlags() & 1) {
        data = &scSitCcData;
    } else if (mModelMng.getFlags() & 0x10) {
        data = &scSwimCcData;
    }

    mCc.mCcData.mBase.mOffset.x = data->mOffset.x;
    mCc.mCcData.mBase.mOffset.y = data->mOffset.y;
    mCc.mCcData.mBase.mSize.set(data->mSize.x, data->mSize.y);
    mCenterOffs.set(0.0f, getModelHeight() / 2.0f, 0.0f);
    if (mPlayerRideOn == BASE_ID_NULL) {
        if (isState(StateID_AloneWait)) {
            mCc.mCcData.mVsKind = 0x10d;
        } else {
            mCc.mCcData.mVsKind = 0x109;
        }
        mCc.mCcData.mAttack = CC_ATTACK_NONE;
        mCc.mCcData.mVsDamage = 0x400000;
        mCc.mCcData.mBase.mOffset.set(0.0f, 16.0f);
        if (isNowBgCross(BgCross1_e(BGC_IN_SINK_SAND | BGC_ON_SINK_SAND))) {
            mCc.mCcData.mBase.mSize.set(4.0f, 4.0f);
        } else {
            mCc.mCcData.mBase.mSize.set(8.0f, 4.0f);
        }
        mCenterOffs.set(0.0f, 8.0f, 0.0f);
    }
}

bool daYoshi_c::ccCheckAttack(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) self->getOwner();
    dAcPy_c *player = (dAcPy_c *) other->getOwner();

    if (yoshi->mPlayerRideOn == player->getID()) {
        return false;
    }

    dAcPy_c *owningPlayer = daPyMng_c::getPlayer(yoshi->getPlrNo());
    if (owningPlayer != nullptr) {
        switch (other->mCcData.mAttack) {
            case CC_ATTACK_HIP_ATTACK:
            case CC_ATTACK_SPIN_FALL:
                if (owningPlayer->setDamage(player, DAMAGE_4)) {
                    if (other->mCcData.mKind == CC_KIND_PLAYER) {
                        dQuake_c::getInstance()->shockMotor(player->getPlrNo(), dQuake_c::TYPE_4, 0, false);
                        if (yoshi->fn_8014eb70(player, 0)) {
                            player->mDirection = yoshi->mDirection;
                            yoshi->setJump(sc_JumpSpeed, yoshi->mSpeedF, true, 2, 0);
                        }
                    } else if (other->mCcData.mAttack == CC_ATTACK_HIP_ATTACK) {
                        player->setVsPlHipAttackEffect();
                    }
                }
                return true;
            case CC_ATTACK_YOSHI_BULLET:
            case CC_ATTACK_YOSHI_FIRE:
            case CC_ATTACK_ICE_2:
                if (owningPlayer->getPlrNo() != player->getPlrNo()) {
                    owningPlayer->setFireBallDamage(other->mCcData.mAttack);
                }
                return true;
            default:
                break;
        }
    }
    return false;

}

bool daYoshi_c::ccCheckStamp(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) self->getOwner();
    dAcPy_c *player = (dAcPy_c *) other->getOwner();

    if (
        yoshi->mSpeed.y <= 0.0f &&
        !yoshi->isStatus(STATUS_JUMP_DAI_COOLDOWN) &&
        !yoshi->isDemoType(DEMO_PLAYER) &&
        yoshi->isEnableStampPlayerJump(self, other)
    ) {
        if (player->isStatus(STATUS_JUMP) && player->mSpeed.y > 0.0f) {
            yoshi->setStampPlayerJump(true, self->mCollOffsetY[CC_KIND_PLAYER]);
            player->setStampReduction();
            return true;
        }

        if (yoshi->setPlayerJumpDai(player)) {
            return true;
        }
    }

    return false;
}

void daYoshi_c::ccCallBack(dCc_c *self, dCc_c *other) {
    dAcPy_c *player = (dAcPy_c *) other->getOwner();
    daYoshi_c *yoshi = (daYoshi_c *) self->getOwner();

    if (
        yoshi->mPlayerRideOn == BASE_ID_NULL ||
        other->mCcData.mKind == CC_KIND_YOSHI && player->mPc.m_28 == 0 ||
        (
            (other->mCcData.mKind == CC_KIND_PLAYER || other->mCcData.mKind == CC_KIND_YOSHI) &&
            (yoshi == player->mpNoHitPlayer || player == yoshi->mpNoHitPlayer)
        )
    ) {
        return;
    }

    if (ccCheckAttack(self, other)) {
        return;
    }

    if (other->mCcData.mKind == CC_KIND_PLAYER && player->mKind == STAGE_ACTOR_PLAYER) {
        if (yoshi->checkRideActor(player)) {
            return;
        }
        if (ccCheckStamp(self, other)) {
            return;
        }
        static const float sCcRevRate[] = { 0.0f, 0.2f, 0.4f };
        yoshi->setCcPlayerRev(self, other, sCcRevRate[player->getTallType(-1)], 0);
    }

    if (
        other->mCcData.mKind == CC_KIND_YOSHI &&
        !yoshi->checkRideActor(player) &&
        !ccCheckStamp(self, other)
    ) {
        yoshi->setCcPlayerRev(self, other, 0.5f, 2);
    }
}

void daYoshi_c::atCcCallBack(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) self->getOwner();

    if (yoshi->isDemo()) {
        return;
    }

    if (self->mCcData.mAttack == CC_ATTACK_YOSHI_EAT && yoshi->checkHitTongueReserve(other)) {
        other->mInfo |= CC_NO_HIT;
    } else if (self->mCcData.mAttack == CC_ATTACK_YOSHI_MOUTH && yoshi->checkHitMouth(other->getOwner())) {
        other->mInfo |= CC_NO_HIT;
    }
}

void daYoshi_c::setReductionScale() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->setReductionScale();
    }
}

void daYoshi_c::initStampReduction() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->initStampReduction();
    }
}

void daYoshi_c::calcJumpDaiReductionScale(int i1, int i2) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->calcJumpDaiReductionScale(i1, i2);
    }
}

void daYoshi_c::setReductionBoyon() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->setReductionBoyon();
    }
}

bool daYoshi_c::getTongueTipMtx(mMtx_c *mtx) {
    dYoshiMdl_c *mdl = (dYoshiMdl_c *) mModelMng.mpMdl;
    mMtx_c tongueTipMtx = mdl->getTongueTipMtx();
    *mtx = tongueTipMtx;
    return true;
}

bool daYoshi_c::getMouthMtx(mMtx_c *mtx) {
    mModelMng.mpMdl->getJointMtx(mtx, 16);
    return true;
}

int daYoshi_c::isStar() const {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        return player->isStar();
    }
    return 0;
}

void daYoshi_c::setStar(StarSet_e starSet, int timer) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->setStarBase(starSet, timer);
    }
    if (starSet == STAR_SET_0) {
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
        if (actor != nullptr && actor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *player = (dAcPy_c *) actor;
            setVirusStar(player);
        }
    }
}

void daYoshi_c::endStar() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->endStar();
    }
}

void daYoshi_c::setVirusStar(daPlBase_c *fromPlayer) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->setVirusStar(fromPlayer);
    }
}

void daYoshi_c::clearStarCount() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->clearStarCount();
    }
}

s8 daYoshi_c::getStarCount() const {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        return player->getStarCount();
    }
    return 0;
}

s8 daYoshi_c::calcStarCount(int count) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        return player->calcStarCount(count);
    }
    return 0;
}

bool daYoshi_c::isChange() {
    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            return player->isChange();
        }
    }
    return false;
}

void daYoshi_c::setLandSE() {
    startFootSoundPlayer(SE_PLY_LAND_YOSHI);
}

void daYoshi_c::set1UpKinokoEffect() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->set1UpKinokoEffect();
    }
}

void daYoshi_c::setFlagGetEffect() {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->setFlagGetEffect();
    }
}

void daYoshi_c::setTongueHitEffect(mVec3_c &pos) {
    if (m_2e8 == 0) {
        m_2e8 = 1;
        mAng3_c ang(0, 0, 0);
        if (mDirection == DIR_LR_L) {
            ang.y = DEG_TO_ANGLE(-180.0f);
        }
        mEf::createEffect("Wm_mr_ystonguehit", 0, &pos, &ang, nullptr);
        mSndObj.startSound(SE_PLY_YOSHI_TONGUE_HIT, 0);
    }
}

void daYoshi_c::startPlayerVoice(int a, int b) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        player->startPlayerVoice(a, b);
    }
}


void daYoshi_c::holdPlayerVoice(int a, int b) {
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        // [Bug: This should be player->holdPlayerVoice, this way it recursively calls itself until the game crashes.]
        holdPlayerVoice(a, b);
    }
}

void daYoshi_c::setCreateAction(int action) {
    daPlBase_c::setCreateAction(action);
}

bool daYoshi_c::setTimeOverDemo() {
    if (
        isStatus(STATUS_53) ||
        isStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE) ||
        isStatus(STATUS_OUT_OF_PLAY) ||
        isStatus(STATUS_STUNNED) ||
        isStatus(STATUS_B3)
    ) {
        return false;
    }

    if (mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            return player->setTimeOverDemo();
        }
    }

    return false;
}

void daYoshi_c::setFallDownDemo() {
    if (isDispOutCheckOn() && mPlayerRideOn != BASE_ID_NULL) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->setFallDownDemo();
        }
    }
}

bool daYoshi_c::setBalloonInDispOut(int a) {
    if (!isDispOutCheckOn()) {
        return false;
    }

    if (isStatus(STATUS_8D)) {
        return false;
    }

    if (mPlayerRideOn != BASE_ID_NULL) {
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
        if (actor != nullptr && actor->mKind == STAGE_ACTOR_PLAYER) {
            dAcPy_c *player = (dAcPy_c *) actor;
            player->setBalloonInDispOutByYoshi(a);
        }

        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr && player->mKind == STAGE_ACTOR_PLAYER) {
            if (player->setBalloonInDispOutByYoshi(a)) {
                changeState(StateID_DamageRun);
                return true;
            }
        }
    }
    return false;
}

void daYoshi_c::changeNextScene(int scene) {
    daPlBase_c::changeNextScene(scene);
    dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (actor != nullptr && actor->mKind == STAGE_ACTOR_PLAYER) {
        dAcPy_c *player = (dAcPy_c *) actor;
        player->changeNextScene(1);
    }
}

bool daYoshi_c::isEnableDokanInStatus() {
    if (!daPlBase_c::isEnableDokanInStatus()) {
        return false;
    }

    if (isStatus(STATUS_AF) || isStatus(STATUS_B3)) {
        return false;
    }

    return true;
}

int daYoshi_c::setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType) {
    dActor_c *actor;
    int plrNo;

    daPlBase_c::setDemoGoal(landPos, goalCastleX, goalType);
    plrNo = -1;
    actor = (dActor_c *) fManager_c::searchBaseByID(m_4c);
    if (setDamageSpitOut(true) && actor->mKind == STAGE_ACTOR_PLAYER) {
        dAcPy_c *player = (dAcPy_c *) actor;
        player->setDemoGoal(landPos, goalCastleX, goalType);
        plrNo = player->getPlrNo();
    }
    changeEatState(StateID_EatNone);
    changeState(StateID_None);
    return plrNo;
}

bool daYoshi_c::setHideNotGoalPlayer() {
    if (daPlBase_c::setHideNotGoalPlayer()) {
        dAcPy_c *player = getPlayerRideOn();
        if (player != nullptr) {
            player->onStatus(STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE);
        }
        return true;
    }
    return false;
}

void daYoshi_c::initDemoGoalBase() {
    daPlBase_c::initDemoGoalBase();
}


void daYoshi_c::executeDemoGoal_Run() {
    switch (mDemoState) {
        case 0:
            if (mPlayerRideOn != BASE_ID_NULL) {
                dAcPy_c *player = getPlayerRideOn();
                if (player != nullptr) {
                    player->setOffYoshiInGoal(this);
                    mAngle.y = 0;
                    mModelMng.setAnm(PLAYER_ANIM_WAIT, 10.0f, 0.0f);
                    mDemoState = 1;
                    mDirection = DIR_LR_R;
                    mDemoSubstateTimer = 70;
                }
            }
            break;
        case 1:
            if (mDemoSubstateTimer < 20) {
                mAngle.y.chase(getMukiAngle(mDirection), 0x400);
            }
            if (mDemoSubstateTimer == 0) {
                mModelMng.setAnm(PLAYER_ANIM_GORL_WAIT);
                mDemoState = 2;
            }
            break;
        case 2:
            mAngle.y.chase(getMukiAngle(mDirection), 0x400);
            break;
    }
}

bool daYoshi_c::updateDemoKimePose(ClearType_e clearType) {
    dPyMdlMng_c *playerMdlMng = nullptr;
    dAcPy_c *player = getPlayerRideOn();
    if (player != nullptr) {
        playerMdlMng = player->mpMdlMng;
    }
    switch (mKimePoseMode) {
        case KIME_POSE_NONE: {
            // mKimePoseMode = KIME_POSE_WITH_HAT;
            mModelMng.setAnm(PLAYER_ANIM_WAIT, 0.0f, 0.0f);
            int animID = PLAYER_ANIM_GOAL_PUTON_CAP;
            if (playerMdlMng != nullptr) {
                if (playerMdlMng->mpMdl->m_151 == 0 || playerMdlMng->mpMdl->m_151 == 1) {
                    if (player->mPowerup == POWERUP_PROPELLER_SHROOM) {
                        mKimePoseMode = KIME_POSE_PENGUIN;
                        animID = PLAYER_ANIM_PL_RGOAL_PUTON_CAP;
                    } else if (player->mPowerup == POWERUP_PENGUIN_SUIT) {
                        mKimePoseMode = KIME_POSE_PENGUIN;
                        animID = PLAYER_ANIM_P_RGOAL_PUTON_CAP;
                    } else {
                        mKimePoseMode = KIME_POSE_WITH_HAT;
                        animID = PLAYER_ANIM_GOAL_PUTON_CAP;
                    }
                } else {
                    mKimePoseMode = KIME_POSE_NO_HAT;
                    animID = PLAYER_ANIM_GOAL_PUTON_CAPF;
                }
            }
            mModelMng.setAnm(animID, 0.0f, 0.0f);
            daPlBase_c::startKimePoseVoice(clearType);
            // fallthrough
        }
        case KIME_POSE_WITH_HAT:
            if (playerMdlMng != nullptr) {
                dMarioMdl_c *marioMdl = (dMarioMdl_c *) playerMdlMng->mpMdl;
                if (marioMdl->mAnm.checkFrame(50.0f)) {
                    marioMdl->setHeadID(dMarioMdl_c::TYPE_1);
                }
                dMarioMdl_c *marioMdl2 = (dMarioMdl_c *) playerMdlMng->mpMdl;
                if (marioMdl2->mAnm.checkFrame(130.0f)) {
                    marioMdl2->setHeadID(dMarioMdl_c::TYPE_0);
                }
            }
            // fallthrough
        case KIME_POSE_PENGUIN:
        case KIME_POSE_NO_HAT:
            if (mModelMng.isAnmStop()) {
                offStatus(STATUS_6C);
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool daYoshi_c::isSpinLiftUpEnable() {
    return false;
}

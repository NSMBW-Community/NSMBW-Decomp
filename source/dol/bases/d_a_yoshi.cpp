#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_audio.hpp>
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
        &daYoshi_c::ccCallback
    };

    const sCcDatNewF scYoshiAtCcData = {
        { 0.0f, 16.0f, 8.0f, 16.0f },
        CC_KIND_PLAYER_ATTACK,
        CC_ATTACK_NONE,
        0,
        0,
        CC_STATUS_NONE,
        &daYoshi_c::atCcCallback
    };

    const sBcPointData scBcFoot = {
        SENSOR_IS_POINT,
        -0x4000,
        0x4000,
        0
    };

    const sBcPointData scBcHead = {
        SENSOR_IS_POINT,
        -0x1000,
        0x1000,
        0x10000
    };

    const sBcPointData scBcWall = {
        SENSOR_IS_POINT,
        -0x4000,
        0xB000,
        0x8000
    };
}

daYoshi_c::daYoshi_c() : mModelMng(dPyMdlMng_c::MODEL_TYPE_YOSHI), mStateMgr(*this, sStateID::null) {
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
    m_50 = 0;
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
    if (m_50 != 0) {
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
    if (m_50 == 0) {
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

    if (m_50 == 0) {
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

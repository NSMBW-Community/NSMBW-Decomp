#include <game/bases/d_a_obj_center2.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_center_save_mng.hpp>
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_quake.hpp>
#include <game/bases/d_switch_flag_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(OBJ_CENTER2, daObjCenter2_c, 0);

STATE_DEFINE(daObjCenter2_c, ReturnHome);
STATE_DEFINE(daObjCenter2_c, GoAim);
STATE_DEFINE(daObjCenter2_c, Move);

void daObjCenter2_c::coinSE() {
    if (ACTOR_PARAM(RotateCoinSE)) {
        dAudio::g_pSndObjMap->startSound(SE_OBJ_COIN_POP_UP, mPos, 0);
    }
}

int daObjCenter2_c::create() {
    mExecStopMask = 0;
    mRotDir = ACTOR_PARAM(IsCCW);
    mRotationID = ACTOR_PARAM(RotID);

    if (!dCenterSaveMng_c::m_instance->center_check(&mPos, ACTOR_PARAM(RotID), &mHomeRotation)) {
        mHomeRotation = l_EnMuki[mRotDir] * (ACTOR_PARAM(InitialRotation) * 0x1000) + 0xc000;
    }

    u32 rotSpeedMode = ACTOR_PARAM(RotationSpeed);
    if (rotSpeedMode >= 7) {
        rotSpeedMode = 0;
    }

    static const s16 l_RotSpeed[] = {
        0x40, 0x80, 0xc0, 0x100, 0x20, 0x200, 0x400
    };
    mRotationSpeed = l_RotSpeed[rotSpeedMode];
    mRotationMode = ACTOR_PARAM(RotationMode);
    mAngle.z = mHomeRotation;
    mZoomControl = ACTOR_PARAM(ZoomControl);

    dBg_c::m_bg_p->m_900a5 = mZoomControl;
    if (m_flag_keep[1] != 0 && dSwitchFlagMng_c::m_instance->mFlag & m_flagbit_keep) {
        if (mRotationMode == MODE_BACK_AND_FORTH) {
            mAngle.z = mHomeRotation + l_EnMuki[mRotDir] * (ACTOR_PARAM(TargetRotationOffset) * 0x1000);
            mStateMgr.changeState(StateID_GoAim);
            return SUCCEEDED;
        } else if (mRotationMode == MODE_PERMANENT) {
            mRotDir ^= 1;
        }
    }
    if (mRotationMode == MODE_BACK_AND_FORTH) {
        mStateMgr.changeState(StateID_ReturnHome);
    } else if (mRotationMode == MODE_PERMANENT) {
        mStateMgr.changeState(StateID_Move);
    } else {
        mStateMgr.changeState(StateID_GoAim);
    }
    return SUCCEEDED;
}

int daObjCenter2_c::execute() {
    if (!(mExecStop & BIT_FLAG(STAGE_ACTOR_ENEMY))) {
        mCoinSoundReady = false;
        mStateMgr.executeState();
    }

    if (mZoomControl != 0) {
        float ang = std::fabs((float) mAngle.z / 0x4000);
        if (ang >= 1.0f) {
            ang = ang - 1.0f;
        } else {
            ang = 1.0f - ang;
        }
        ang = 1.0f - ang;
        dBg_c::m_bg_p->mZoomRelated = 3;
        dBg_c::m_bg_p->mZoomWeight = 1.0f - ang * ang * ang;
        dBg_c::m_bg_p->setZoomPos(mPos);
    }

    return SUCCEEDED;
}

short daObjCenter2_c::getAngle() {
    return l_EnMuki[mRotDir] * (ACTOR_PARAM(InitialRotation) * 0x1000) + 0xc000;
}

void daObjCenter2_c::tryPlayCoinSE() {
    if (mCoinSoundReady && !mCoinSoundPlayed) {
        coinSE();
        mCoinSoundPlayed = true;
    }
}

void daObjCenter2_c::initializeState_ReturnHome() {
    mCoinSoundReady = true;
    mCoinSoundPlayed = false;
    mRotDir = ACTOR_PARAM(IsCCW) ^ 1;
    mAngDelta = mRotationSpeed * l_EnMuki[mRotDir];
    mTargetRotation = mHomeRotation;
}

void daObjCenter2_c::finalizeState_ReturnHome() {}

void daObjCenter2_c::executeState_ReturnHome() {
    if (dSwitchFlagMng_c::m_instance->mFlag & mEventMask) {
        mStateMgr.changeState(StateID_GoAim);
        return;
    }

    if (mAngle.z != mTargetRotation) {
        mAngle.z += mAngDelta;
        int deltaAbs = labs(mAngDelta);
        short target = mTargetRotation;
        if (abs(mAngle.z.mAngle - target) < deltaAbs) {
            mAngle.z = target;
            if (ACTOR_PARAM(RotateShock)) {
                dQuake_c::m_instance->startShockAll(dQuake_c::TYPE_0, 3, 0, false);
            }
        }
    }
}

void daObjCenter2_c::initializeState_GoAim() {
    mCoinSoundReady = true;
    mCoinSoundPlayed = false;
    mRotDir = ACTOR_PARAM(IsCCW);
    mAngDelta = mRotationSpeed * l_EnMuki[mRotDir];
    if (mRotationMode == MODE_BACK_AND_FORTH) {
        mTargetRotation = mHomeRotation + l_EnMuki[mRotDir] * (ACTOR_PARAM(TargetRotationOffset) * 0x1000);
    } else {
        mTargetRotation = mHomeRotation;
    }
}

void daObjCenter2_c::finalizeState_GoAim() {}

void daObjCenter2_c::executeState_GoAim() {
    if (mRotationMode == MODE_BACK_AND_FORTH) {
        if ((dSwitchFlagMng_c::m_instance->mFlag & mEventMask) == 0) {
            mStateMgr.changeState(StateID_ReturnHome);
            return;
        }
    } else if (mAngle.z == mTargetRotation && dSwitchFlagMng_c::m_instance->mFlag & mEventMask) {
        mHomeRotation += l_EnMuki[mRotDir] * (ACTOR_PARAM(TargetRotationOffset) * 0x1000);
        // Restart the state to start another rotation.
        mStateMgr.changeState(StateID_GoAim);
        return;
    }

    if (mAngle.z != mTargetRotation) {
        mAngle.z += mAngDelta;
        int deltaAbs = labs(mAngDelta);
        short target = mTargetRotation;
        if (abs(mAngle.z.mAngle - target) < deltaAbs) {
            mAngle.z = target;
            dCenterSaveMng_c::m_instance->center_entry(&mPos, mRotationID, mTargetRotation);
            if (ACTOR_PARAM(RotateShock)) {
                dQuake_c::m_instance->startShockAll(dQuake_c::TYPE_0, 3, 0, false);
            }
        }
    }
}

void daObjCenter2_c::initializeState_Move() {
    if (mRotDir != DIR_LR_R) {
        mAngDelta = -mRotationSpeed * 0x1000;
    } else {
        mAngDelta = mRotationSpeed * 0x1000;
    }
}

void daObjCenter2_c::finalizeState_Move() {}

void daObjCenter2_c::executeState_Move() {
    int delta;
    if (dSwitchFlagMng_c::m_instance->mFlag & mEventMask) {
        delta = -mRotationSpeed * 0x1000;
    } else {
        delta = mRotationSpeed * 0x1000;
    }
    if (mRotDir != DIR_LR_R) {
        delta = -delta;
    }
    int deltaAbs = labs(delta >> 7);
    if (mAngDelta != delta) {
        if (delta < 0) {
            mAngDelta -= deltaAbs;
            if (mAngDelta < delta) {
                mAngDelta = delta;
            }
        } else {
            mAngDelta += deltaAbs;
            if (mAngDelta > delta) {
                mAngDelta = delta;
            }
        }
    }
    mAngle.z += mAngDelta >> 12;
}

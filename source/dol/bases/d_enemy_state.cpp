#include <game/bases/d_enemy.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/mLib/m_effect.hpp>
#include <constants/sound_list.h>

STATE_BASE_VIRTUAL_DEFINE(dEn_c, Ice);
STATE_BASE_VIRTUAL_DEFINE(dEn_c, HitSpin);
STATE_BASE_VIRTUAL_DEFINE(dEn_c, EatIn);
STATE_BASE_VIRTUAL_DEFINE(dEn_c, EatNow);
STATE_BASE_VIRTUAL_DEFINE(dEn_c, EatOut);
STATE_VIRTUAL_DEFINE(dEn_c, Ice);
STATE_VIRTUAL_DEFINE(dEn_c, HitSpin);
STATE_VIRTUAL_DEFINE(dEn_c, EatIn);
STATE_VIRTUAL_DEFINE(dEn_c, EatNow);
STATE_VIRTUAL_DEFINE(dEn_c, EatOut);

void dEn_c::changeState(const sStateIDIf_c &id) {
    if (isState(StateID_Ice)) {
        mStateMgr.returnState();
    }
    mStateMgr.changeState(id);
}

void dEn_c::initializeState_Ice() {
    mIceMng.initialize();
    createIceActor();

    mBc.mFlags = 0;
    setIceAnm();
    removeCc();

    if (mIceMng.checkInstantBreak(mFlags & 0x10000)) {
        mIceMng.mDestroyMode = 1;
    }
    mIceMng.mActive = 1;
}

void dEn_c::finalizeState_Ice() {
    mIceMng.mActive = 0;
}

void dEn_c::executeState_Ice() {
    switch (mIceMng.manageProc()) {
        default:
            break;
        case dIceMng_c::PROC_DEFAULT:
            returnAnm_Ice();
            returnState_Ice();
            break;
        case dIceMng_c::PROC_MELT:
            reviveCc();
            break;
        case dIceMng_c::PROC_FROZEN:
            int destroyMode = mIceMng.mDestroyMode;
            if (destroyMode != 0) {
                mIceMng.breakEffect();
                dAudio::g_pSndObjMap->startSound(SE_OBJ_PNGN_ICE_BREAK, mPos, 0);
                switch (destroyMode) {
                    case 1:
                        setDeathInfo_IceBreak();
                        break;
                    case 3:
                        setDeathInfo_IceVanish();
                        break;
                    default:
                        break;
                }
            }
            break;
    }
}

bool dEn_c::createIceActor() {
    dIceInfo iceInfo[] = {
        {
            getIceMode(),
            mPos,
            mVec3_c(1.0f, 1.0f, 1.0f)
        }
    };
    return mIceMng.createIce(iceInfo, ARRAY_SIZE(iceInfo));
}

void dEn_c::setIceAnm() {}

void dEn_c::returnAnm_Ice() {}

void dEn_c::killIce() {
    mIceMng.removeIce();
}

void dEn_c::returnState_Ice() {
    mStateMgr.returnState();
}

void dEn_c::initializeState_HitSpin() {}

void dEn_c::finalizeState_HitSpin() {}

void dEn_c::executeState_HitSpin() {}

void dEn_c::initializeState_EatIn() {}

void dEn_c::finalizeState_EatIn() {}

void dEn_c::executeState_EatIn() {
    if (mEatState == EAT_STATE_EATEN) {
        changeState(StateID_EatNow);
    } else {
        dActor_c *eatActor = (dActor_c *) fManager_c::searchBaseByID(mEatenByID);
        if (eatActor != nullptr) {
            calcEatInScale(eatActor);
        }
    }
}

void dEn_c::initializeState_EatNow() {}

void dEn_c::finalizeState_EatNow() {}

void dEn_c::executeState_EatNow() {}

void dEn_c::initializeState_EatOut() {}

void dEn_c::finalizeState_EatOut() {}

void dEn_c::executeState_EatOut() {}

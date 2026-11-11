#include <game/bases/d_en_switch_door.hpp>
#include <game/bases/d_switch_flag_mng.hpp>

ACTOR_PROFILE(EN_SWITCHDOOR, dEnSwitchDoor_c, 2);

void dEnSwitchDoor_c::initialize() {
    if (dActor_c::m_flag_keep[1] != 0 && !(dSwitchFlagMng_c::m_instance->mFlag & *(u16 *) dActor_c::m_flag_keep)) {
        mIsEventActivated = FALSE;
    } else {
        mIsEventActivated = TRUE;
        mCc.entry();
    }

    mVisibleAreaOffset.set(0.0f, 24.0f);
}

void dEnSwitchDoor_c::waitProc() {
    if (mEventMask & dSwitchFlagMng_c::m_instance->mFlag) {
        if (mIsEventActivated) {
            return;
        }
        
        mIsEventActivated = TRUE;
        mCc.entry();
    } else if (mIsEventActivated) {
        mIsEventActivated = FALSE;
        mCc.release();
    }
}

int dEnSwitchDoor_c::preDraw() {
    if (!dEn_c::preDraw()) {
        return NOT_READY;
    };

    return mIsEventActivated ? SUCCEEDED : NOT_READY;
}
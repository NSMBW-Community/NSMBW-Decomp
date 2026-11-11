#include <game/bases/d_en_switch_door.hpp>
#include <game/bases/d_switch_flag_mng.hpp>

ACTOR_PROFILE(EN_SWITCHDOOR, dEnSwitchDoor_c, 2);

void dEnSwitchDoor_c::initialize() {
    if (dActor_c::m_flag_keep[1] != 0 && !(dSwitchFlagMng_c::m_instance->mFlag & *(u16 *) dActor_c::m_flag_keep)) {
        m_5dc = 0;
    } else {
        m_5dc = 1;
        mCc.entry();
    }

    mVisibleAreaOffset.set(0.0f, 24.0f);
}

void dEnSwitchDoor_c::waitProc() {
    if (mEventMask & dSwitchFlagMng_c::m_instance->mFlag) {
        if (m_5dc != 0)
            return;
        
        m_5dc = 1;
        mCc.entry();
    } else if (m_5dc != 0) {
        m_5dc = 0;
        mCc.release();
    }
}

int dEnSwitchDoor_c::preDraw() {
    if (!dEn_c::preDraw()) {
        return NOT_READY;
    };

    return m_5dc ? SUCCEEDED : NOT_READY;
}
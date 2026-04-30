#include <game/bases/d_a_wm_map.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_dokan.hpp>
#include <game/bases/d_wm_lib.hpp>

ACTOR_PROFILE(WM_DOKAN, daWmDokan_c, 0);

daWmDokan_c::daWmDokan_c() {}
daWmDokan_c::~daWmDokan_c() {}

int daWmDokan_c::create() {
    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 80.0f;
    return SUCCEEDED;
}

int daWmDokan_c::execute() {
    processCutsceneCommand(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);
    daWmMap_c::m_instance->GetNodePos(mResNodeIdx, mPos);
    return SUCCEEDED;
}

int daWmDokan_c::draw() {
    return SUCCEEDED;
}

int daWmDokan_c::doDelete() {
    return SUCCEEDED;
}

void daWmDokan_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId != dCsSeqMng_c::CUTSCENE_CMD_NONE) {
        mIsCutEnd = true;
    }
}

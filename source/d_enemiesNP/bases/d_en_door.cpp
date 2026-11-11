#include <game/bases/d_en_door.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(EN_DOOR, dEnDoor_c, 2);

const sCcDatNewF l_door_cc_data = {
    0.0f, 20.0f,
    12.0f, 20.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER),
    0,
    CC_STATUS_NONE,
    daEnDoor_c::ccCallback
};

void dEnDoor_c::createMdl() {
    mResFile = dResMng_c::m_instance->getRes("obj_door", "g3d/obj_door.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("obj_doorA");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1, nullptr);

    dActor_c::setSoftLight_MapObj(mModel);

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_doorA");
    mAnim.create(resMdl, resAnmChr, &mAllocator, nullptr);
}

void dEnDoor_c::initAnm() {
    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_doorA");
    mAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnim, 0.0f);
    mAnim.setRate(0.0f);
}

void dEnDoor_c::initCcData() {
    mCc.set(this, (sCcDatNewF *) &l_door_cc_data);
}

void dEnDoor_c::setOpenAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::FORWARD_ONCE;
}

void dEnDoor_c::setCloseAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::REVERSE_ONCE;
}

void dEnDoor_c::setWaitAnm() {
    mAnim.setRate(0.0f);
    mAnim.setFrame(0.0f);
}

// To get automatically copied virtual functions in the right order. @unused
void UNUSED_dEnDoor_c() {
    dEnDoor_c *door;
    door->setCloseMoveSE();
    door->initialize();
}

void dEnDoor_c::setCloseSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_DOOR_CLOSE, mPos, 0);
}

void dEnDoor_c::setOpenSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_DOOR_OPEN, mPos, 0);
}

bool dEnDoor_c::isClosed() {
    return *mStateMgr.getStateID() == StateID_Wait;
}
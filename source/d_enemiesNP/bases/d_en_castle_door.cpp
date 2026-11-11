#include <game/bases/d_en_castle_door.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(EN_CASTLEDOOR, dEnCastleDoor_c, 2);

const sCcDatNewF l_cdoor_cc_data = {
    0.0f, 20.0f,
    24.0f, 20.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER),
    0,
    CC_STATUS_NONE,
    daEnDoor_c::ccCallback
};

void dEnCastleDoor_c::createMdl() {
    mResFile = dResMng_c::m_instance->getRes("obj_door_shiroboss", "g3d/obj_door_shiroboss.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("obj_door_shiroboss");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1, nullptr);

    dActor_c::setSoftLight_MapObj(mModel);

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_door_shiroboss");
    mAnim.create(resMdl, resAnmChr, &mAllocator, nullptr);
}

void dEnCastleDoor_c::initAnm() {
    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_door_shiroboss");
    mAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnim, 0.0f);
    mAnim.setRate(0.0f);
}

void dEnCastleDoor_c::initCcData() {
    mCc.set(this, (sCcDatNewF *) &l_cdoor_cc_data);
    mCc.entry();
}

void dEnCastleDoor_c::initialize() {
    mDestroyBound = sRangeDataF(smc_CULL_YLIMIT, smc_CULL_YLIMIT, smc_CULL_XLIMIT + 128.0f, smc_CULL_XLIMIT + 128.0f);
}

void dEnCastleDoor_c::setOpenAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::FORWARD_ONCE;
}

void dEnCastleDoor_c::setCloseAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::REVERSE_ONCE;
}

void dEnCastleDoor_c::setWaitAnm() {
    mAnim.setRate(0.0f);
    mAnim.setFrame(0.0f);
}

// To get automatically copied virtual functions in the right order. @unused
void UNUSED_dEnCastleDoor_c() {
    dEnCastleDoor_c *door;
    door->waitProc();
    door->ActorDrawCullCheck();
    door->isDummyOpen();
}

void dEnCastleDoor_c::setCloseMoveSE() {
    dAudio::SndObjctCmnMap_c* sndObjMap = dAudio::g_pSndObjMap;
    sndObjMap->holdSound(SE_OBJ_BDOOR_MOVE, mUniqueID, mPos, 0);
}

void dEnCastleDoor_c::setCloseSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_BDOOR_CLOSE, mPos, 0);
}

void dEnCastleDoor_c::setOpenSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_BDOOR_OPEN, mPos, 0);
}

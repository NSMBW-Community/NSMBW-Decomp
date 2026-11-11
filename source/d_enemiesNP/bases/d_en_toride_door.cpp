#pragma inline_max_size(256)

#include <game/bases/d_en_toride_door.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(EN_TORIDEDOOR, dEnTorideDoor_c, 2);

const sCcDatNewF l_tdoor_cc_data = {
    0.0f, 20.0f,
    24.0f, 20.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER),
    0,
    CC_STATUS_NONE,
    daEnDoor_c::ccCallback
};

void dEnTorideDoor_c::createMdl() {
    mResFile = dResMng_c::m_instance->getRes("obj_door_torideboss", "g3d/obj_door_torideboss.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("obj_door_torideboss");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1, nullptr);

    dActor_c::setSoftLight_MapObj(mModel);

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_door_torideboss");
    mAnim.create(resMdl, resAnmChr, &mAllocator, nullptr);
}

void dEnTorideDoor_c::initAnm() {
    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("obj_door_torideboss");
    mAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnim, 0.0f);
    mAnim.setRate(0.0f);
}

void dEnTorideDoor_c::initCcData() {
    mCc.set(this, (sCcDatNewF *) &l_tdoor_cc_data);
    mCc.entry();
}

void dEnTorideDoor_c::initialize() {
    mDestroyBound = sRangeDataF(smc_CULL_YLIMIT, smc_CULL_YLIMIT, smc_CULL_XLIMIT + 128.0f, smc_CULL_XLIMIT + 128.0f);
}

void dEnTorideDoor_c::setOpenAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::FORWARD_ONCE;
}

void dEnTorideDoor_c::setCloseAnm() {
    mAnim.setRate(1.0f);
    mAnim.mPlayMode = m3d::REVERSE_ONCE;
}

void dEnTorideDoor_c::setWaitAnm() {
    mAnim.setRate(0.0f);
    mAnim.setFrame(0.0f);
}

void dEnTorideDoor_c::setCloseMoveSE() {
    dAudio::SndObjctCmnMap_c* sndObjMap = dAudio::g_pSndObjMap;
    sndObjMap->holdSound(SE_OBJ_BDOOR_MOVE, mUniqueID, mPos, 0);
}

void dEnTorideDoor_c::setCloseSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_BDOOR_CLOSE, mPos, 0);
}

void dEnTorideDoor_c::setOpenSE() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_BDOOR_OPEN, mPos, 0);
}

bool dEnTorideDoor_c::isClosed() {
    return *mStateMgr.getStateID() == StateID_Wait;
}
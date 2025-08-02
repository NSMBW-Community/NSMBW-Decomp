#include <game/bases/d_a_remo_door.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(REMO_DOOR, daRemoDoor_c, 2);

STATE_DEFINE(daRemoDoor_c, OpenReady);
STATE_DEFINE(daRemoDoor_c, Open);
STATE_DEFINE(daRemoDoor_c, Close);

void daRemoDoor_c::init() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);
    const char *fileName = "g3d/obj_door.brres";
    mRes = dResMng_c::m_instance->mRes.getRes("obj_door", fileName);
    nw4r::g3d::ResMdl mdl = mRes.GetResMdl("obj_doorA");
    mDoorModel.create(mdl, &mAllocator, 0x20, 1, nullptr);
    dActor_c::setSoftLight_MapObj(mDoorModel);
    mAllocator.adjustFrmHeap();
}

int daRemoDoor_c::create() {
    init();
    mScale.z = 1.0f;
    mScale.x = 1.5f;
    mScale.y = 5.0f / 3.0f;
    mPos.z = 5500.0f;
    mVisibleAreaOffset.x = 0.0f;
    mVisibleAreaOffset.y = 40.0f;
    mVisibleAreaSize.x = 96.0f;
    mVisibleAreaSize.y = 80.0f;
    mStateMgr.changeState(StateID_OpenReady);
    return SUCCEEDED;
}

int daRemoDoor_c::execute() {
    if (ActorScrOutCheck(SKIP_NONE)) {
        return SUCCEEDED;
    }
    mStateMgr.executeState();
    return SUCCEEDED;
}

int daRemoDoor_c::draw() {
    mMatrix.trans(mPos.x, mPos.y, mPos.z);
    mMatrix.YrotM(mAngle.y);
    mMtx_c transposeMtx;
    transposeMtx.trans(-24.0f, 0.0f, 0.0f);
    PSMTXConcat(mMatrix, transposeMtx, mMatrix);
    mDoorModel.setLocalMtx(&mMatrix);
    mDoorModel.setScale(mScale);
    mDoorModel.entry();
    return SUCCEEDED;
}

void daRemoDoor_c::initializeState_OpenReady() {}
void daRemoDoor_c::finalizeState_OpenReady() {}
void daRemoDoor_c::executeState_OpenReady() {
    // [Oversight: the player number check never fails]
    if (daPyMng_c::mCtrlPlrNo < 4 && dGameKey_c::m_instance->getAccVerticalAngleX(daPyMng_c::mCtrlPlrNo) >= 0x2000) {
        mStateMgr.changeState(StateID_Open);
    }
}

void daRemoDoor_c::initializeState_Open() {
    dAudio::SoundEffectID_t(SOUND_OPEN).playMapSound(mPos, 0);
}

void daRemoDoor_c::finalizeState_Open() {}

void daRemoDoor_c::executeState_Open() {
    short newAngle = mAngle.y;
    newAngle += 0x400;
    if (newAngle < 0) {
        newAngle = -0x8000;
    }
    mAngle.y = newAngle;

    // [Oversight: the player number check never fails]
    if (mAngle.y == mAng(-0x8000) && daPyMng_c::mCtrlPlrNo < 4 &&
        dGameKey_c::m_instance->getAccVerticalAngleX(daPyMng_c::mCtrlPlrNo) <= 0x1000) {
            mStateMgr.changeState(StateID_Close);
    }
}

void daRemoDoor_c::initializeState_Close() {}

void daRemoDoor_c::finalizeState_Close() {
    dAudio::SoundEffectID_t(SOUND_CLOSE).playMapSound(mPos, 0);
}

void daRemoDoor_c::executeState_Close() {
    short newAngle = mAngle.y;
    newAngle -= 0x400;
    if (newAngle < 0) {
        newAngle = 0;
    }
    mAngle.y = newAngle;
    if (mAngle.y == mAng(0)) {
        mStateMgr.changeState(StateID_OpenReady);
    }
}

const int daRemoDoor_c::SOUND_CLOSE = SE_OBJ_DOOR_CLOSE;
const int daRemoDoor_c::SOUND_OPEN = SE_OBJ_DOOR_OPEN;

#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_a_wm_map.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_cannon.hpp>
#include <game/bases/d_wm_lib.hpp>

ACTOR_PROFILE(WM_CANNON, daWmCannon_c, 0);

daWmCannon_c::daWmCannon_c() {}
daWmCannon_c::~daWmCannon_c() {}

int daWmCannon_c::create() {
    setClipSphere();
    loadModel();
    calcModel();

    return SUCCEEDED;
}

int daWmCannon_c::execute() {
    setCutEndSpecific(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);

    mModel.play();
    daWmMap_c::m_instance->GetNodePos(mResNodeIdx, mPos);
    calcModel();

    return SUCCEEDED;
}

int daWmCannon_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daWmCannon_c::doDelete() {
    return SUCCEEDED;
}

void daWmCannon_c::setClipSphere() {
    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 120.0f;
}

void daWmCannon_c::loadModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    nw4r::g3d::ResFile resFile = dResMng_c::m_instance->getRes("cobCannon", "g3d/model.brres");
    nw4r::g3d::ResMdl resMdl = resFile.GetResMdl("cobCannon");

    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1, nullptr);
    dWmActor_c::setSoftLight_MapObj(mModel);

    mAllocator.adjustFrmHeap();
}

void daWmCannon_c::calcModel() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daWmCannon_c::setCutEndSpecific(int cutsceneId, bool param2) {
    if (cutsceneId == dCsSeqMng_c::CUTSCENE_NONE) {
        return;
    }

    if (cutsceneId == 27) {
        mIsCutEnd = true;
    } else if (cutsceneId == 28) {
        mIsCutEnd = true;
    } else if (cutsceneId == 29) {
        mIsCutEnd = true;
    } else {
        mIsCutEnd = true;
    }
}
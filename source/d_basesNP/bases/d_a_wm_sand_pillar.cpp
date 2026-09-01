#include <game/sLib/s_GlobalData.hpp>
#include <game/bases/d_a_wm_sand_pillar.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_a_wm_map.hpp>

STATE_DEFINE(daWmSandPillar_c, Ready);
STATE_DEFINE(daWmSandPillar_c, BottomWait);
STATE_DEFINE(daWmSandPillar_c, MoveReady);
STATE_DEFINE(daWmSandPillar_c, MoveUp);
STATE_DEFINE(daWmSandPillar_c, TopWait);
STATE_DEFINE(daWmSandPillar_c, MoveDown);
STATE_DEFINE(daWmSandPillar_c, BottomWaitForever);
STATE_DEFINE(daWmSandPillar_c, TopWaitForever);
STATE_DEFINE(daWmSandPillar_c, TopWaitFromTheStart);

ACTOR_PROFILE(WM_SANDPILLAR, daWmSandPillar_c, 0);

template <>
const daWmSandPillar_c::GlobalData_t sGlobalData_c<daWmSandPillar_c>::mData = {
    { 0.0099999998f, 0.0099999998f, 0.0099999998f },
    { 0.001f, 0.003f, 0.003f },
    { 0.029999999f, 0.05f, 0.05f },
    { -320.0f, -200.0f, -200.0f },
    { 0.5f, 0.2f, 0.2f },
    { 2.0f, 1.4f, 1.6f },
    { 0.4f, 0.1f, 0.1f },
    { 1.1f, 1.8f, 1.8f },
    { 3, 1, 1 },
    { 100, 10, 40 },
    { 100, 100, 100 }
};

daWmSandPillar_c::daWmSandPillar_c() : mStateMgr(*this, StateID_Ready) {}
daWmSandPillar_c::~daWmSandPillar_c() {}

int daWmSandPillar_c::create() {
    createModel();

    mClipSphere.set(mPos, 350.0f);
    calcModel();

    float yPos = GLOBAL_DATA.mPosY[ACTOR_PARAM(Node)];
    mPos.y = yPos;
    mUnk254 = mPos;

    return SUCCEEDED;
}

int daWmSandPillar_c::execute() {
    dCsSeqMng_c *csSeqMng = dCsSeqMng_c::ms_instance;
    mPos.y = GLOBAL_DATA.mPosY[ACTOR_PARAM(Node)];

    mStateMgr.executeState();

    if (csSeqMng->FUN_80915600()) {
        processCutsceneCommand(csSeqMng->GetCutName(), csSeqMng->m_164);
    }

    mSrtAnim.play();
    mModel.play();
    calcModel();
    createEffect();

    return SUCCEEDED;
}

void daWmSandPillar_c::calcModel() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daWmSandPillar_c::finalUpdate() {}

int daWmSandPillar_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

void daWmSandPillar_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("cobSandpillar", "g3d/model.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("cobSandpillar");
    mModel.create(resMdl, &mAllocator,
        nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::BUFFER_RESTEXSRT | nw4r::g3d::ScnMdl::ANM_TEXPAT);

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.create(resMdl, resAnmChr, &mAllocator);

    mResAnmTexSrt = mResFile.GetResAnmTexSrt("cobSandpillar");
    mSrtAnim.create(resMdl, mResAnmTexSrt, &mAllocator);
    mSrtAnim.setAnm(mModel, mResAnmTexSrt, 0, m3d::FORWARD_LOOP);
    mModel.setAnm(mSrtAnim, 1.0f);

    mAllocator.adjustFrmHeap();
}

void daWmSandPillar_c::FUN_808e55e0() {
    if (mUnk4F4 < mUnk4F8) {
        mUnk4F4 += mUnk500;
        if (mUnk4F4 > mUnk4F8) {
            mUnk4F4 = mUnk4F8;
        }
    } else if (mUnk4F4 > mUnk4F8) {
        mUnk4F4 -= mUnk500;
        if (mUnk4F4 < mUnk4F8) {
            mUnk4F4 = mUnk4F8;
        }
    }
}

// NOT MATCHING
void daWmSandPillar_c::initializeState_Ready() {
    int nodeIdx = ACTOR_PARAM(Node);
    switch (nodeIdx) {
        case 1:
        case 2:
            mUnk504 = 1;
            break;
        default:
            mUnk504 = 0;
            break;
    }
}

void daWmSandPillar_c::executeState_Ready() {
    if (mUnk504 != 0) {
        int world = daWmMap_c::GetCurrentWorld();
        int course = dWmLib::GetCourseNoFromPointName("W205");
        if (dWmLib::IsCourseClear(world, course)) {
            if (dWmLib::IsCourseFirstClear(world, course)) {
                mStateMgr.changeState(StateID_BottomWait);
            } else {
                mStateMgr.changeState(StateID_TopWaitFromTheStart);
            }
        } else {
            mStateMgr.changeState(StateID_BottomWaitForever);
        }
    } else {
        mStateMgr.changeState(StateID_BottomWait);
    }
}

void daWmSandPillar_c::finalizeState_Ready() {}

void daWmSandPillar_c::initializeState_BottomWait() {
    int nodeIdx = ACTOR_PARAM(Node);
    mTimer1 = GLOBAL_DATA.mTimer1[nodeIdx];
    mScale.y = GLOBAL_DATA.mScaleY[nodeIdx];

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_LOOP);
    mChrAnim.setRate(1.0f);
    mModel.setAnm(mChrAnim, 1.0f);

    mUnk4EC = 0;
}

void daWmSandPillar_c::executeState_BottomWait() {
    if (mUnk4EC != 0) {
        return;
    }

    mTimer1--;
    if (mTimer1 <= 0) {
        mStateMgr.changeState(StateID_MoveReady);
    }
}

void daWmSandPillar_c::finalizeState_BottomWait() {}

void daWmSandPillar_c::initializeState_BottomWaitForever() {
    int nodeIdx = ACTOR_PARAM(Node);
    mScale.y = GLOBAL_DATA.mScaleY[nodeIdx];

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_LOOP);
    mChrAnim.setRate(1.0f);
    mModel.setAnm(mChrAnim, 1.0f);

    mUnk4EC = 0;
}

void daWmSandPillar_c::executeState_BottomWaitForever() {}

void daWmSandPillar_c::finalizeState_BottomWaitForever() {}

// NOT MATCHING
void daWmSandPillar_c::initializeState_MoveReady() {
    int nodeIdx = ACTOR_PARAM(Node);
    mTimer2 = GLOBAL_DATA.mTimer2[nodeIdx];
    mUnk4F4 = -GLOBAL_DATA.mUnk4F4[nodeIdx];
    mScale.y = GLOBAL_DATA.mScaleY[nodeIdx];
    mUnk500 = GLOBAL_DATA.mUnk500[nodeIdx];
    mUnk4F8 = -GLOBAL_DATA.mUnk4F8[nodeIdx];
    mUnk4EC = 1;
    mChrAnim.setRate(0.0f);
}

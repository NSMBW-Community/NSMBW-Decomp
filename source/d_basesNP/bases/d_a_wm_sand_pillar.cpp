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
    .mStepInitialScale = { 0.01f, 0.01f, 0.01f },
    .mStepAccel = { 0.001f, 0.003f, 0.003f },
    .mStepMaxScale = { 0.03f, 0.05f, 0.05f },
    .mPosY = { -320.0f, -200.0f, -200.0f },
    .mBottomHeight = { 0.5f, 0.2f, 0.2f },
    .mTopHeight = { 2.0f, 1.4f, 1.6f },
    .mPulseBottomHeight = { 0.4f, 0.1f, 0.1f },
    .mPulseTopHeight = { 1.1f, 1.8f, 1.8f },
    .mPulseCount = { 3, 1, 1 },
    .mBottomWaitTime = { 100, 10, 40 },
    .mTopWaitTime = { 100, 100, 100 }
};

daWmSandPillar_c::daWmSandPillar_c() : mStateMgr(*this, StateID_Ready) {}
daWmSandPillar_c::~daWmSandPillar_c() {}

int daWmSandPillar_c::create() {
    createModel();

    mClipSphere.set(mPos, 350.0f);
    calcModel();

    mPos.y = GLOBAL_DATA.mPosY[getNodeIdx()];
    mInitialPos = mPos;

    return SUCCEEDED;
}

int daWmSandPillar_c::execute() {
    dCsSeqMng_c *csSeqMng = dCsSeqMng_c::ms_instance;
    mPos.y = GLOBAL_DATA.mPosY[getNodeIdx()];

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

void daWmSandPillar_c::calcScaleStep() {
    if (mScaleStep < mTargetScaleStep) {
        mScaleStep += mStepAccel;
        if (mScaleStep > mTargetScaleStep) {
            mScaleStep = mTargetScaleStep;
        }
    } else if (mScaleStep > mTargetScaleStep) {
        mScaleStep -= mStepAccel;
        if (mScaleStep < mTargetScaleStep) {
            mScaleStep = mTargetScaleStep;
        }
    }
}

// -------------
// StateID_Ready
// -------------

void daWmSandPillar_c::initializeState_Ready() {
    if (getNodeIdx() == NODE_1 || getNodeIdx() == NODE_2) {
        mHasCourseCond = TRUE;
    } else {
        mHasCourseCond = FALSE;
    }
}

void daWmSandPillar_c::executeState_Ready() {
    if (mHasCourseCond) {
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

// ------------------
// StateID_BottomWait
// ------------------

void daWmSandPillar_c::initializeState_BottomWait() {
    mWaitTimer = GLOBAL_DATA.mBottomWaitTime[getNodeIdx()];
    mScale.y = GLOBAL_DATA.mBottomHeight[getNodeIdx()];

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_LOOP);
    mChrAnim.setRate(1.0f);
    mModel.setAnm(mChrAnim, 1.0f);

    mIsPulsing = false;
}

void daWmSandPillar_c::executeState_BottomWait() {
    if (mIsPulsing) {
        return;
    }

    mWaitTimer--;
    if (mWaitTimer <= 0) {
        mStateMgr.changeState(StateID_MoveReady);
    }
}

void daWmSandPillar_c::finalizeState_BottomWait() {}

// -------------------------
// StateID_BottomWaitForever
// -------------------------

void daWmSandPillar_c::initializeState_BottomWaitForever() {
    mScale.y = GLOBAL_DATA.mBottomHeight[getNodeIdx()];

    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_LOOP);
    mChrAnim.setRate(1.0f);
    mModel.setAnm(mChrAnim, 1.0f);

    mIsPulsing = false;
}

void daWmSandPillar_c::executeState_BottomWaitForever() {}

void daWmSandPillar_c::finalizeState_BottomWaitForever() {}

// -----------------
// StateID_MoveReady
// -----------------

void daWmSandPillar_c::initializeState_MoveReady() {
    mRemainingPulses = GLOBAL_DATA.mPulseCount[getNodeIdx()];
    mScaleStep = -GLOBAL_DATA.mStepInitialScale[getNodeIdx()];
    mScale.y = GLOBAL_DATA.mBottomHeight[getNodeIdx()];
    mStepAccel = GLOBAL_DATA.mStepAccel[getNodeIdx()];
    mTargetScaleStep = -GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
    mIsPulsing = true;
    mChrAnim.setRate(0.0f);
}

void daWmSandPillar_c::executeState_MoveReady() {
    float pulseBottomHeight = GLOBAL_DATA.mPulseBottomHeight[getNodeIdx()];
    float pulseTopHeight = GLOBAL_DATA.mPulseTopHeight[getNodeIdx()];

    if (mIsPulsing == true) {
        float prevScaleY = mScale.y;
        float prevStep = mScaleStep;
        calcScaleStep();
        mScale.y += mScaleStep;

        // Once the scale step is zero, reverse direction for the next pulse
        if (mScaleStep == 0.0f) {
            if (prevStep > 0.0f) {
                mTargetScaleStep = -GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
            } else if (prevStep < 0.0f) {
                mTargetScaleStep = GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
            }
        }

        if (prevScaleY < pulseTopHeight && mScale.y >= pulseTopHeight) {
            // Reached top pulse height, brake and count one completed pulse
            mTargetScaleStep = 0.0f;
            mRemainingPulses--;
        } else if (prevScaleY > pulseBottomHeight && mScale.y <= pulseBottomHeight) {
            // Reached bottom pulse height: brake before rising again
            mTargetScaleStep = 0.0f;
        }

        if (mRemainingPulses <= 0) {
            mStateMgr.changeState(StateID_MoveUp);
        }
    }
}

void daWmSandPillar_c::finalizeState_MoveReady() {}

// --------------
// StateID_MoveUp
// --------------

void daWmSandPillar_c::initializeState_MoveUp() {
    mTargetScaleStep = GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
    mStepAccel = GLOBAL_DATA.mStepAccel[getNodeIdx()];
    if (mHasCourseCond) {
        mStepAccel = GLOBAL_DATA.mStepAccel[getNodeIdx()];
        mTargetScaleStep = -GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
    }

    mChrAnim.setRate(0.0f);
}

void daWmSandPillar_c::executeState_MoveUp() {
    float topHeight = GLOBAL_DATA.mTopHeight[getNodeIdx()];
    float pulseTopHeight = GLOBAL_DATA.mPulseTopHeight[getNodeIdx()];

    calcScaleStep();
    mScale.y += mScaleStep;

    if (mHasCourseCond) {
        if (mScaleStep <= 0.0f && mScale.y <= topHeight) {
            mScale.y = topHeight;
            mStateMgr.changeState(StateID_TopWaitForever);
        }
    } else {
        if (topHeight >= pulseTopHeight) {
            if (mScale.y >= topHeight) {
                mScale.y = topHeight;
                mStateMgr.changeState(StateID_TopWait);
            }
        } else {
            if (mScale.y <= topHeight) {
                mScale.y = topHeight;
                mStateMgr.changeState(StateID_TopWait);
            }
        }
    }
}

void daWmSandPillar_c::finalizeState_MoveUp() {}

// ---------------
// StateID_TopWait
// ---------------

void daWmSandPillar_c::initializeState_TopWait() {
    mChrAnim.setRate(1.0f);
    mWaitTimer = GLOBAL_DATA.mTopWaitTime[getNodeIdx()];
}

void daWmSandPillar_c::executeState_TopWait() {
    mWaitTimer--;
    if (mWaitTimer <= 0) {
        mStateMgr.changeState(StateID_MoveDown);
    }
}

void daWmSandPillar_c::finalizeState_TopWait() {}

// ----------------------
// StateID_TopWaitForever
// ----------------------

void daWmSandPillar_c::initializeState_TopWaitForever() {
    mChrAnim.setRate(1.0f);
    mScaleStep = 0.0f;
    mWaitTimer = GLOBAL_DATA.mTopWaitTime[getNodeIdx()];
}

void daWmSandPillar_c::executeState_TopWaitForever() {}

void daWmSandPillar_c::finalizeState_TopWaitForever() {}

// ----------------
// StateID_MoveDown
// ----------------

void daWmSandPillar_c::initializeState_MoveDown() {
    mChrAnim.setRate(0.0f);
    mScaleStep = 0.0f;
    mTargetScaleStep = -GLOBAL_DATA.mStepMaxScale[getNodeIdx()];
}

void daWmSandPillar_c::executeState_MoveDown() {
    float bottomHeight = GLOBAL_DATA.mBottomHeight[getNodeIdx()];
    calcScaleStep();
    mScale.y += mScaleStep;
    if (mScale.y <= bottomHeight) {
        mScale.y = bottomHeight;
        mStateMgr.changeState(StateID_BottomWait);
    }
}

void daWmSandPillar_c::finalizeState_MoveDown() {}

// ---------------------------
// StateID_TopWaitFromTheStart
// ---------------------------

void daWmSandPillar_c::initializeState_TopWaitFromTheStart() {
    nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr("cobSandpillar");
    mChrAnim.setAnm(mModel, resAnmChr, m3d::FORWARD_LOOP);
    mChrAnim.setRate(1.0f);
    mModel.setAnm(mChrAnim, 1.0f);
    mScale.y = GLOBAL_DATA.mTopHeight[getNodeIdx()];
}

void daWmSandPillar_c::executeState_TopWaitFromTheStart() {
    mStateMgr.changeState(StateID_TopWaitForever);
}

void daWmSandPillar_c::finalizeState_TopWaitFromTheStart() {}

void daWmSandPillar_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId != dCsSeqMng_c::CUTSCENE_CMD_NONE && !isStaff()) {
        mIsCutEnd = true;
    }
}

void daWmSandPillar_c::createEffect() {
    mVec3_c efPos;
    ulong nodeId = m3d::getNodeID(mModel.getResMdl(), "ef_cobSandpillar");
    mModel.getNodeWorldMtxMultVecZero(nodeId, efPos);

    mEffect1.createEffect("Wm_cs_sandpillar01", 0, &efPos, nullptr, nullptr);
    mEffect2.createEffect("Wm_cs_sandpillar02", 0, &efPos, nullptr, nullptr);
}

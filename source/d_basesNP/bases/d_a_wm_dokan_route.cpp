#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_dokan_route.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_wm_se_manager.hpp>
#include <game/sLib/s_GlobalData.hpp>

const char *daWmDokanRoute_c::sNodeNames[6] = {
    "W601",
    "W603",
    "W603",
    "W605",
    "W605",
    "W605"
};

ACTOR_PROFILE(WM_DOKANROUTE, daWmDokanRoute_c, 0);

template <>
const daWmDokanRoute_c::GlobalData_t sGlobalData_c<daWmDokanRoute_c>::mData = {
    1.0f,
    {
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 60, 98, 30, 0},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 30, 75, 15, 3},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 90, 160, 15, 1},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 120, 155, 15, 3},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 110, 150, 15, 3},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 155, 175, 15, 3}
    }
};

// I thought it was a bool, apparently not
const u8 daWmDokanRoute_c::sIsNormalExit[6] = {
    true,
    true,
    true,
    true,
    false,
    true,
};

daWmDokanRoute_c::daWmDokanRoute_c() {}
daWmDokanRoute_c::~daWmDokanRoute_c() {}

int daWmDokanRoute_c::create() {
    mPosCopy = mPos;
    createModel();

    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 250.0f;

    FUN_808d0270();
    calcModel(mModel);
    return SUCCEEDED;
}

int daWmDokanRoute_c::execute() {
    setCutEndSpecific(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);

    // Could be a stubbed debug function
    static const ProcFunc Proc_tbl[PROC_STATE_COUNT] = {
        &daWmDokanRoute_c::FUN_808d02d0
    };

    (this->*Proc_tbl[mProcState])();
    FUN_808d0360();
    FUN_808d01a0();
    calcModel(mModel);
    return SUCCEEDED;
}

int daWmDokanRoute_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daWmDokanRoute_c::doDelete() {
    return SUCCEEDED;
}

void daWmDokanRoute_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("cobDokanRoute", "g3d/model.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("cobDokanRoute");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::ANM_TEXSRT | nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1);

    static const m3d::playMode_e playModes[1] = {
        m3d::FORWARD_ONCE
    };

    static const char *resAnmNames[1] = {
        "cobDokanRoute"
    };

    for (int i = 0; i < ARRAY_SIZE(resAnmNames); i++) {
        nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr(resAnmNames[i]);
        mChrAnim[i].create(resMdl, resAnmChr, &mAllocator, nullptr);
        mChrAnim[i].mPlayMode = playModes[i];
        mChrAnim[i].setRate(0.0f);
        mChrAnim[i].setFrame(0.0f);

        nw4r::g3d::ResAnmTexSrt resAnmTexSrt = mResFile.GetResAnmTexSrt(resAnmNames[i]);
        mSrtAnim[i].create(resMdl, resAnmTexSrt, &mAllocator, nullptr, 1);
        mSrtAnim[i].setPlayMode(playModes[i], 0);
        mSrtAnim[i].setRate(0.0f, 0);
        mSrtAnim[i].setFrame(0.0f, 0);

        mModel.setAnm(mChrAnim[i]);
        mModel.setAnm(mSrtAnim[i]);
    }

    dWmActor_c::setSoftLight_Map(mModel);
    mAllocator.adjustFrmHeap();
}

void daWmDokanRoute_c::FUN_808d01a0() {
    mModel.play();
}

void daWmDokanRoute_c::calcModel(m3d::mdl_c &model) {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    model.setLocalMtx(&mMatrix);
    model.setScale(mScale);
    model.calc(false);
}

void daWmDokanRoute_c::FUN_808d0270() {
    FUN_808d0360();
    FUN_808d0520();
    float s = sGlobalData_c<daWmDokanRoute_c>::mData.mInitialScale;
    mScale.set(s, s, s);
    FUN_808d02c0();
}

void daWmDokanRoute_c::FUN_808d02c0() {
    mProcState = PROC_STATE_DEFAULT;
}

void daWmDokanRoute_c::FUN_808d02d0() {}

void daWmDokanRoute_c::setCutEndSpecific(int cutsceneCommandId, bool param2) {
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_NONE) {
        return;
    }

    if (param2) {
        if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_2) {
            FUN_808d0660();
        }

    }
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_2) {
        FUN_808d0740();
    } else {
        setCutEnd();
    }
}

void daWmDokanRoute_c::FUN_808d0360() {
    u8 nodeType = ACTOR_PARAM(Node);
    mPos = mPosCopy + sGlobalData_c<daWmDokanRoute_c>::mData.mAnims[nodeType].mPosDelta;
    s16 pipeDir = sGlobalData_c<daWmDokanRoute_c>::mData.mAnims[nodeType].mDirection;

    float frame = 0.0f;
    mVec3_c rot = mVec3_c::Ez;

    switch (pipeDir) {
        case 0:
            frame = 2.0f;
            rot = mVec3_c::Ez;
            break;

        case 1:
            rot = -mVec3_c::Ez;
            frame = 1.0f;
            break;

        case 2:
            rot = mVec3_c::Ex;
            frame = 0.0f;
            break;

        case 3:
            rot = -mVec3_c::Ex;
            frame = 0.0f;
            break;

        default:
            break;
    }

    s16 angle = rot.xzAng();
    mAngle.y = angle;
    mAngle3D.y = angle;

    mSrtAnim[0].setFrame(frame, 0);
}

void daWmDokanRoute_c::FUN_808d0520() {
    u8 nodeType = ACTOR_PARAM(Node);
    int courseNo = dWmLib::GetCourseNoFromPointName(sNodeNames[nodeType]);
    dInfo_c *info = dInfo_c::m_instance;

    float frame = 0.0f;
    switch (sIsNormalExit[nodeType]) {
        case 1:
            if (dWmLib::IsCourseOmoteClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = GetFrame();
            }
        break;

        case 0:
            if (dWmLib::IsCourseUraClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = GetFrame();
            }
            break;

        default:
            break;
    }

    mChrAnim[0].setFrame(frame);
    mChrAnim[0].setRate(0.0f);
}

void daWmDokanRoute_c::FUN_808d0660() {
    u8 nodeType = ACTOR_PARAM(Node);
    int courseNo = dWmLib::GetCourseNoFromPointName(sNodeNames[nodeType]);
    dInfo_c *info = dInfo_c::m_instance;
    int exitType = sIsNormalExit[nodeType];

    mState = STATE_0;
    if (exitType != 0 && dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode) ||
        exitType == 0 && dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)) {
        mTimer = GLOBAL_DATA.mAnims[nodeType].mTimer;
        mState = STATE_1;
    }
}

void daWmDokanRoute_c::FUN_808d0740() {
    u8 nodeType = ACTOR_PARAM(Node);

    switch (mState) {
        case STATE_0:
            setCutEnd();
            break;

        case STATE_1:
            if (mTimer > 0) {
                mTimer--;
            } else {
                mState = STATE_2;
            }
            break;

        case STATE_2:
            dWmSeManager_c::m_pInstance->playSound(0x2D, mPos, 1);
            mChrAnim[0].setRate(GLOBAL_DATA.mAnims[nodeType].mAnmRate);
            mTimer = GLOBAL_DATA.mAnims[nodeType].mTimer2 - GLOBAL_DATA.mAnims[nodeType].mTimer;
            mState = STATE_3;
            break;

        case STATE_3:
            if (mTimer > 0) {
                mTimer--;
                if (mTimer <= GLOBAL_DATA.mAnims[nodeType].mTimerThreshold) {
                    float diff = (float) GLOBAL_DATA.mAnims[nodeType].mTimerThreshold - mTimer;
                    float v = diff / GLOBAL_DATA.mAnims[nodeType].mTimerThreshold;
                    float rate = (1.0f - v) * GLOBAL_DATA.mAnims[nodeType].mAnmRate;
                    mChrAnim[0].setRate(rate);
                }
            } else {
                mState = STATE_4;
            }
            break;

        case STATE_4:
            mChrAnim[0].setRate(0.0f);
            setCutEnd();
            break;

        default:
            break;
    }
}

float daWmDokanRoute_c::GetFrame() {
    u8 nodeType = ACTOR_PARAM(Node);
    float timerDiff = GLOBAL_DATA.mAnims[nodeType].mTimer2 - GLOBAL_DATA.mAnims[nodeType].mTimer;
    float frame = (timerDiff - GLOBAL_DATA.mAnims[nodeType].mTimerThreshold) * GLOBAL_DATA.mAnims[nodeType].mAnmRate;

    for (int i = 0; i < GLOBAL_DATA.mAnims[nodeType].mTimerThreshold; i++) {
        float v = i / (float) GLOBAL_DATA.mAnims[nodeType].mTimerThreshold;
        frame += (1.0f - v) * GLOBAL_DATA.mAnims[nodeType].mAnmRate;
    }

    return frame;
}

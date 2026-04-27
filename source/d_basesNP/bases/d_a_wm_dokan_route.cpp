#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_dokan_route.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_wm_se_manager.hpp>
#include <game/sLib/s_GlobalData.hpp>

const char *daWmDokanRoute_c::sPointNames[6] = {
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
    processCutsceneCommand(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);

    static const ProcFunc Proc_tbl[PROC_COUNT] = {
        &daWmDokanRoute_c::mode_exec
    };

    (this->*Proc_tbl[mCurrProc])();
    FUN_808d0360();
    calcAnim();
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

    static const char *resAnmNames[ANIM_COUNT] = {
        "cobDokanRoute"
    };

    static const m3d::playMode_e playModes[ANIM_COUNT] = {
        m3d::FORWARD_ONCE
    };

    for (int i = 0; i < ANIM_COUNT; i++) {
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

void daWmDokanRoute_c::calcAnim() {
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
    float scale = GLOBAL_DATA.mInitialScale;
    mScale.set(scale, scale, scale);
    init_exec();
}

void daWmDokanRoute_c::init_exec() {
    mCurrProc = PROC_TYPE_EXEC;
}

void daWmDokanRoute_c::mode_exec() {}

void daWmDokanRoute_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_NONE) {
        return;
    }

    if (isFirstFrame) {
        if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_COURSE_UNLOCK) {
            onCourseUnlockInit();
        }

    }
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_COURSE_UNLOCK) {
        onCourseUnlock();
    } else {
        setCutEnd();
    }
}

void daWmDokanRoute_c::FUN_808d0360() {
    u8 nodeNum = GetNodeNum();
    mPos = mPosCopy + GLOBAL_DATA.mAnims[nodeNum].mPosDelta;
    s16 pipeDir = GLOBAL_DATA.mAnims[nodeNum].mDirection;

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

    mSrtAnim[cobDokanRoute].setFrame(frame, 0);
}

void daWmDokanRoute_c::FUN_808d0520() {
    u8 nodeNum = GetNodeNum();
    int courseNo = dWmLib::GetCourseNoFromPointName(sPointNames[nodeNum]);
    dInfo_c *info = dInfo_c::getInstance();

    float frame = 0.0f;
    switch (sIsNormalExit[nodeNum]) {
        case 1:
            if (dWmLib::IsCourseOmoteClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = getFrame();
            }
        break;

        case 0:
            if (dWmLib::IsCourseUraClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = getFrame();
            }
            break;

        default:
            break;
    }

    mChrAnim[cobDokanRoute].setFrame(frame);
    mChrAnim[cobDokanRoute].setRate(0.0f);
}

void daWmDokanRoute_c::onCourseUnlockInit() {
    u8 nodeNum = GetNodeNum();
    int courseNo = dWmLib::GetCourseNoFromPointName(sPointNames[nodeNum]);
    dInfo_c *info = dInfo_c::getInstance();

    mState = STATE_IDLE;
    if (sIsNormalExit[nodeNum] != 0 && dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode) ||
        sIsNormalExit[nodeNum] == 0 && dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)) {
        mStateTimer = GLOBAL_DATA.mAnims[nodeNum].mTimer;
        mState = STATE_WAIT_DELAY;
    }
}

void daWmDokanRoute_c::onCourseUnlock() {
    u8 nodeNum = GetNodeNum();

    switch (mState) {
        case STATE_IDLE:
            setCutEnd();
            break;

        case STATE_WAIT_DELAY:
            if (mStateTimer > 0) {
                mStateTimer--;
            } else {
                mState = STATE_ANIM_START;
            }
            break;

        case STATE_ANIM_START:
            dWmSeManager_c::m_pInstance->playSound(0x2D, mPos, 1);
            mChrAnim[cobDokanRoute].setRate(GLOBAL_DATA.mAnims[nodeNum].mAnmRate);
            mStateTimer = GLOBAL_DATA.mAnims[nodeNum].mTimer2 - GLOBAL_DATA.mAnims[nodeNum].mTimer;
            mState = STATE_ANIM_PLAY;
            break;

        case STATE_ANIM_PLAY:
            if (mStateTimer > 0) {
                mStateTimer--;
                if (mStateTimer <= GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold) {
                    float diff = (float) GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold - mStateTimer;
                    float v = diff / GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold;
                    float rate = (1.0f - v) * GLOBAL_DATA.mAnims[nodeNum].mAnmRate;
                    mChrAnim[cobDokanRoute].setRate(rate);
                }
            } else {
                mState = STATE_ANIM_END;
            }
            break;

        case STATE_ANIM_END:
            mChrAnim[cobDokanRoute].setRate(0.0f);
            setCutEnd();
            break;

        default:
            break;
    }
}

float daWmDokanRoute_c::getFrame() {
    u8 nodeNum = GetNodeNum();
    float timerDiff = GLOBAL_DATA.mAnims[nodeNum].mTimer2 - GLOBAL_DATA.mAnims[nodeNum].mTimer;
    float frame = (timerDiff - GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold) * GLOBAL_DATA.mAnims[nodeNum].mAnmRate;

    for (int i = 0; i < GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold; i++) {
        float v = i / (float) GLOBAL_DATA.mAnims[nodeNum].mTimerThreshold;
        frame += (1.0f - v) * GLOBAL_DATA.mAnims[nodeNum].mAnmRate;
    }

    return frame;
}

#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_dokan_route.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_wm_se_manager.hpp>
#include <game/sLib/s_GlobalData.hpp>

const char *daWmDokanRoute_c::sPointNames[] = {
    "W601",
    "W603",
    "W603",
    "W605",
    "W605", // [unused]
    "W605"
};

ACTOR_PROFILE(WM_DOKANROUTE, daWmDokanRoute_c, 0);

template <>
const daWmDokanRoute_c::GlobalData_t sGlobalData_c<daWmDokanRoute_c>::mData = {
    1.0f,
    {
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 60, 98, 30, daWmDokanRoute_c::EAST},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 30, 75, 15, daWmDokanRoute_c::SOUTH},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 90, 160, 15, daWmDokanRoute_c::WEST},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 120, 155, 15, daWmDokanRoute_c::SOUTH},
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 110, 150, 15, daWmDokanRoute_c::SOUTH}, // [unused]
        {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 155, 175, 15, daWmDokanRoute_c::SOUTH}
    }
};

const u8 daWmDokanRoute_c::sExitTypes[] = {
    EXIT_TYPE_NORMAL,
    EXIT_TYPE_NORMAL,
    EXIT_TYPE_NORMAL,
    EXIT_TYPE_NORMAL,
    EXIT_TYPE_SECRET, // [unused]
    EXIT_TYPE_NORMAL,
};

daWmDokanRoute_c::daWmDokanRoute_c() {}
daWmDokanRoute_c::~daWmDokanRoute_c() {}

int daWmDokanRoute_c::create() {
    mInitialPos = mPos;
    createModel();

    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 250.0f;

    initState();
    calcModel(mModel);
    return SUCCEEDED;
}

int daWmDokanRoute_c::execute() {
    processCutsceneCommand(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);

    static const ProcFunc Proc_tbl[PROC_COUNT] = {
        &daWmDokanRoute_c::mode_exec
    };

    (this->*Proc_tbl[mCurrProc])();
    initPosRot(); // [unnecessary call]
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

void daWmDokanRoute_c::initState() {
    initPosRot();
    initAnim();
    float scale = GLOBAL_DATA.mScaleMultiplier;
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

void daWmDokanRoute_c::initPosRot() {
    u8 nodeNum = GetNodeNum();
    mPos = mInitialPos + GLOBAL_DATA.mNodeConfigs[nodeNum].mPosOffset;
    s16 pipeDir = GLOBAL_DATA.mNodeConfigs[nodeNum].mDirection;

    float frame = 0.0f;
    mVec3_c rot = mVec3_c::Ez;

    switch (pipeDir) {
        case EAST:
            frame = 2.0f;
            rot = mVec3_c::Ez;
            break;

        case WEST:
            rot = -mVec3_c::Ez;
            frame = 1.0f;
            break;

        case NORTH:
            rot = mVec3_c::Ex;
            frame = 0.0f;
            break;

        case SOUTH:
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

void daWmDokanRoute_c::initAnim() {
    u8 nodeNum = GetNodeNum();
    int courseNo = dWmLib::GetCourseNoFromPointName(sPointNames[nodeNum]);
    dInfo_c *info = dInfo_c::getInstance();

    float frame = 0.0f;
    switch (sExitTypes[nodeNum]) {
        case EXIT_TYPE_NORMAL:
            if (dWmLib::IsCourseOmoteClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = getEndFrame();
            }
        break;

        case EXIT_TYPE_SECRET:
            if (dWmLib::IsCourseUraClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = getEndFrame();
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
    if (sExitTypes[nodeNum] != EXIT_TYPE_SECRET && dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode) ||
        sExitTypes[nodeNum] == EXIT_TYPE_SECRET && dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)) {
        mStateTimer = GLOBAL_DATA.mNodeConfigs[nodeNum].mInitialDelay;
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
            dWmSeManager_c::m_pInstance->playSound(dWmSeManager_c::WM_SE_OBJ_CS_DOKAN, mPos, 1);
            mChrAnim[cobDokanRoute].setRate(GLOBAL_DATA.mNodeConfigs[nodeNum].mAnmRate);
            mStateTimer = GLOBAL_DATA.mNodeConfigs[nodeNum].mAnimDuration - GLOBAL_DATA.mNodeConfigs[nodeNum].mInitialDelay;
            mState = STATE_ANIM_PLAY;
            break;

        case STATE_ANIM_PLAY:
            if (mStateTimer > 0) {
                mStateTimer--;

                // Begin linearly decelerating if less than mDecelDuration frames are left
                if (mStateTimer <= GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration) {
                    float decelFrames = (float) GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration - mStateTimer;
                    float decelPercent = decelFrames / GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration;
                    float rate = (1.0f - decelPercent) * GLOBAL_DATA.mNodeConfigs[nodeNum].mAnmRate;
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

float daWmDokanRoute_c::getEndFrame() {
    u8 nodeNum = GetNodeNum();
    float movementDuration = GLOBAL_DATA.mNodeConfigs[nodeNum].mAnimDuration - GLOBAL_DATA.mNodeConfigs[nodeNum].mInitialDelay;
    float frame = (movementDuration - GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration) * GLOBAL_DATA.mNodeConfigs[nodeNum].mAnmRate;

    // Compute linear deceleration to get final frame
    for (int i = 0; i < GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration; i++) {
        float decelPercent = i / (float) GLOBAL_DATA.mNodeConfigs[nodeNum].mDecelDuration;
        frame += (1.0f - decelPercent) * GLOBAL_DATA.mNodeConfigs[nodeNum].mAnmRate;
    }

    return frame;
}

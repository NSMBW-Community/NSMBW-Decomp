#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_w_camera.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_a_wm_cloud.hpp>
#include <game/mLib/m_angle.hpp>
#include <game/sLib/s_GlobalData.hpp>

const char *daWmCloud_c::sGroupNodeNames[] = {
    "group01",
    "group02",
    "group03",
    "group04",
    "group05",
    "group06",
    "group07",
    "group08",
    // [group09 is missing]
    "group10",
    "group11",
    "group12",
    "group13",
    "group14",
    "group15",
    "group16",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

void DUMMY_UNUSED() {
    static const float idk[] = { 200.0f, 0.0f };
}

template <>
const daWmCloud_c::GlobalData_t sGlobalData_c<daWmCloud_c>::mData = {
    0.0f,
    {
        300.0f,
        300.0f,
        500.0f,
        300.0f,
        350.0f,
        420.0f,
        500.0f,
        800.0f,
        600.0f,
        300.0f,
        350.0f,
        550.0f,
        600.0f,
        600.0f,
        200.0f,
        100.0f,
        100.0f,
        100.0f,
        100.0f,
        100.0f
    },
    { 8, 0 }
};

ACTOR_PROFILE(WM_CLOUD, daWmCloud_c, 0);

daWmCloud_c::daWmCloud_c() {}

daWmCloud_c::~daWmCloud_c() {
    if (mpBgmSync != nullptr) {
        delete mpBgmSync;
    }
}

// not matching
int daWmCloud_c::create() {
    dWmBgmSync_c *bgmSync = new dWmBgmSync_c();
    mpBgmSync = bgmSync;
    bgmSync->m_18 = GLOBAL_DATA.mBgmValue;
    bgmSync->m_04 = 7;
    bgmSync->m_08 = 0;

    createModel();
    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 2500.0f;

    calcModel();
    initNodeGroupIds();
    initState();

    return SUCCEEDED;
}

int daWmCloud_c::execute() {
    mpBgmSync->execute();
    if (mpBgmSync->m_0c) {
        float rate = mpBgmSync->getAnmRate(mChrAnim->mFrameMax);
        mChrAnim->setRate(rate);
    }

    static const ProcFunc Proc_tbl[PROC_COUNT] = {
        &daWmCloud_c::mode_exec
    };

    dCsSeqMng_c *csSeqMng = dCsSeqMng_c::ms_instance;
    if (csSeqMng->FUN_80915600()) {
        processCutsceneCommand(csSeqMng->GetCutName(), csSeqMng->m_164);
    } else {
        (this->*Proc_tbl[mCurrProc])();
    }

    calcCulling();
    mModel.play();
    calcModel();

    return SUCCEEDED;
}

int daWmCloud_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daWmCloud_c::doDelete() {
    return SUCCEEDED;
}

// not matching, stack issues
void daWmCloud_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("CS_W7", "g3d/model.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("CS_W7_Cloud");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1);

    static const char *resAnmNames[ANIM_COUNT] = {
        "CS_W7_Cloud"
    };

    static const m3d::playMode_e playModes[ANIM_COUNT] = {
        m3d::FORWARD_LOOP
    };

    for (int i = 0; i < ANIM_COUNT; i++) {
        nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr(resAnmNames[i]);
        mChrAnim[i].create(resMdl, resAnmChr, &mAllocator, nullptr);
        mChrAnim[i].mPlayMode = playModes[i];
        mChrAnim[i].setRate(0.0f);
        mChrAnim[i].setFrame(0.0f);
        mModel.setAnm(mChrAnim[i]);
    }

    dWmActor_c::setSoftLight_Map(mModel);
    mAllocator.adjustFrmHeap();
}

void daWmCloud_c::calcModel() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daWmCloud_c::initState() {
    mPos = mVec3_c::Zero;

    mChrAnim[CS_W7_Cloud].setRate(1.0f);
    mChrAnim[CS_W7_Cloud].setFrame(0.0f);
    mModel.setPriorityDraw(0, 0);

    init_exec();
}

void daWmCloud_c::init_exec() {
    mCurrProc = PROC_TYPE_EXEC;
}

void daWmCloud_c::mode_exec() {}

void daWmCloud_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId != dCsSeqMng_c::CUTSCENE_CMD_NONE && !isStaff()) {
        mIsCutEnd = true;
    }
}

void daWmCloud_c::initNodeGroupIds() {
    for (int i = 0; i < NODE_COUNT; i++) {
        if (sGroupNodeNames[i] == nullptr) {
            mGroupNodeIds[i] = -1;
        } else {
            mGroupNodeIds[i] = m3d::getNodeID(mModel.getResMdl(), sGroupNodeNames[i]);
        }
    }
}

// not matching, stack issues
void daWmCloud_c::calcCulling() {
    dWCamera_c *worldCamera = dWCamera_c::m_instance;

    for (int i = 0; i < NODE_COUNT; i++) {
        if (mGroupNodeIds[i] < 0) {
            return;
        }

        mVec3_c nodePos;
        nodePos = dWmLib::GetModelNodePos(&mModel, mGroupNodeIds[i]);
        mCurrNodeClipSphere.set(nodePos, GLOBAL_DATA.mGroupNodeRadii[i]);

        nw4r::g3d::ResMdl resMdl = mModel.getResMdl();
        nw4r::g3d::ResNode resNode = resMdl.GetResNode(mGroupNodeIds[i]);
        bool onScreen = worldCamera->mViewClip.CheckClipSphere(&mCurrNodeClipSphere);
        if (!onScreen) {
            hideNode(resNode);
        } else {
            showNode(resNode);
        }
    }
}

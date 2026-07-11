#include <game/bases/d_a_wm_peach.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_wm_lib.hpp>

const char *daWmPeach_c::smc_resAnmNames[ANIM_COUNT] = {
    "fly_wmap_B1",
    "fly_wmap_B2"
};

ACTOR_PROFILE(WM_PEACH, daWmPeach_c, 0);

daWmPeach_c::daWmPeach_c() {}
daWmPeach_c::~daWmPeach_c() {}

int daWmPeach_c::create() {
    createModel();
    mClipSphere.set(mPos, 10000.0f);
    calcModel();

    initState();
    mScale.set(1.89f, 1.89f, 1.89f);

    return SUCCEEDED;
}

int daWmPeach_c::execute() {
    static const ProcFunc Proc_tbl[PROC_COUNT] = {
        &daWmPeach_c::mode_exec
    };

    dCsSeqMng_c *csSeqMng = dCsSeqMng_c::ms_instance;
    if (csSeqMng->FUN_80915600()) {
        processCutsceneCommand(csSeqMng->GetCutName(), csSeqMng->m_164);
    } else {
        (this->*Proc_tbl[mCurrProc])();
    }

    mModel.play();
    calcModel();
    return SUCCEEDED;
}

int daWmPeach_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daWmPeach_c::doDelete() {
    return SUCCEEDED;
}

void daWmPeach_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("peach", "g3d/peach.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("peach");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1);

    static const m3d::playMode_e playModes[ANIM_COUNT] = {
        m3d::FORWARD_ONCE,
        m3d::FORWARD_ONCE
    };

    for (int i = 0; i < ANIM_COUNT; i++) {
        nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr(smc_resAnmNames[i]);
        mChrAnim[i].create(resMdl, resAnmChr, &mAllocator, nullptr);
        mChrAnim[i].mPlayMode = playModes[i];
        mChrAnim[i].setRate(0.0f);
        mChrAnim[i].setFrame(0.0f);
    }

    mModel.setAnm(mChrAnim[fly_wmap_B1]);
    calcModel();
    dWmActor_c::setSoftLight_Enemy(mModel);

    mAllocator.adjustFrmHeap();
}

void daWmPeach_c::calcModel() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daWmPeach_c::initState() {
    init_exec();
}

void daWmPeach_c::init_exec() {
    mCurrProc = PROC_TYPE_EXEC;
}

void daWmPeach_c::mode_exec() {}

void daWmPeach_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId != dCsSeqMng_c::CUTSCENE_CMD_NONE && !isStaff()) {
        mIsCutEnd = true;
    }
}

void daWmPeach_c::setAnim(int animIdx, float blendFrame, float rate, float frame) {
    m3d::anmChr_c &anim = mChrAnim[animIdx];
    anim.setRate(rate);
    anim.setFrame(frame);
    mModel.removeAnm(nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_CHR);
    mModel.setAnm(anim, blendFrame);
}

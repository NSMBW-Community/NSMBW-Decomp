#include <game/bases/d_a_wm_peach_castle.hpp>
#include <game/bases/d_a_wm_player.hpp>
#include <game/bases/d_a_wm_map.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_s_chr_lib.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_wm_se_manager.hpp>

ACTOR_PROFILE(WM_PEACH_CASTLE, daWmPeachCastle_c, 0);

daWmPeachCastle_c::daWmPeachCastle_c() {}
daWmPeachCastle_c::~daWmPeachCastle_c() {}

int daWmPeachCastle_c::create() {
    createModel();
    calcModel();

    initState();
    mClipSphere.set(mPos, 200.0f);

    return SUCCEEDED;
}

int daWmPeachCastle_c::execute() {
    dCsSeqMng_c *csSeqMng = dCsSeqMng_c::ms_instance;
    processCutsceneCommand(csSeqMng->GetCutName(), csSeqMng->m_164);

    daWmMap_c::m_instance->GetNodePos(mResNodeIdx, mPos);
    calcModel();

    for (int i = 0; i < ANIM_COUNT; i++) {
        mChrAnim[i].play();
    }

    return SUCCEEDED;
}

int daWmPeachCastle_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

void daWmPeachCastle_c::initState() {
    initFlagAnim();
    mModel.setAnm(mChrBlendAnim);
    initUnk();
    if (dWmObjActor_c::GetCurrentPlayResultStatus() != 8) {
        dCsSeqMng_c::ms_instance->FUN_801017c0(dCsSeqMng_c::SMC_DEMO_CASTLE_CLR, this, daWmPlayer_c::ms_instance, 200);
    }
}

void daWmPeachCastle_c::initUnk() {
    mUnk2B8 = 0;
}

int daWmPeachCastle_c::doDelete() {
    return SUCCEEDED;
}

void daWmPeachCastle_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("cobPeachCastle", "g3d/model.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("cobPeachCastle");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC);

    static const char *resAnmNames[ANIM_COUNT] = {
        "cobPeachCastleFlag",
        "cobPeachCastleOpen",
        "cobPeachCastleClose"
    };

    static const m3d::playMode_e playModes[ANIM_COUNT] = {
        m3d::FORWARD_LOOP,
        m3d::FORWARD_ONCE,
        m3d::FORWARD_ONCE
    };

    for (int i = 0; i < ANIM_COUNT; i++) {
        nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr(resAnmNames[i]);
        mChrAnim[i].create(resMdl, resAnmChr, &mAllocator, nullptr);
        mChrAnim[i].mPlayMode = playModes[i];
        mChrAnim[i].setRate(0.0f);
        mChrAnim[i].setFrame(0.0f);
    }

    mChrBlendAnim.create(resMdl, 8, &mAllocator, nullptr);
    dWmActor_c::setSoftLight_MapObj(mModel);
    mAllocator.adjustFrmHeap();
}

void daWmPeachCastle_c::calcModel() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daWmPeachCastle_c::processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame) {
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_NONE) {
        return;
    }

    if (!isStaff()) {
        mIsCutEnd = true;
        return;
    }

    if (isFirstFrame) {
        switch (cutsceneCommandId) {
            case dCsSeqMng_c::CUTSCENE_CMD_17:
            case dCsSeqMng_c::CUTSCENE_CMD_19:
                initDoorOpenAnim();
                mChrAnim[cobPeachCastleOpen].setFrame(0.0f);
                mChrAnim[cobPeachCastleOpen].setRate(2.4f);
                dWmSeManager_c::m_pInstance->playSound(dWmSeManager_c::WM_SE_OBJ_CS_DOOR_OPEN_PEACH, mPos, 1);
                break;

            case dCsSeqMng_c::CUTSCENE_CMD_78:
                ensureCourseClear();

            case dCsSeqMng_c::CUTSCENE_CMD_20:
                initDoorCloseAnim();
                mChrAnim[cobPeachCastleClose].setFrame(0.0f);
                mChrAnim[cobPeachCastleClose].setRate(1.0f);
                dWmSeManager_c::m_pInstance->playSound(dWmSeManager_c::WM_SE_OBJ_CS_DOOR_CLOSE_PEACH, mPos, 1);
                break;

            default:
                break;
        }
    }

    switch (cutsceneCommandId) {
        case dCsSeqMng_c::CUTSCENE_CMD_17:
        case dCsSeqMng_c::CUTSCENE_CMD_19:
            if (mChrAnim[cobPeachCastleOpen].isStop()) {
                mIsCutEnd = true;
            }
            break;

        case dCsSeqMng_c::CUTSCENE_CMD_78:
        case dCsSeqMng_c::CUTSCENE_CMD_20:
            if (mChrAnim[cobPeachCastleClose].isStop()) {
                mIsCutEnd = true;
            }
            break;

        default:
            mIsCutEnd = true;
            break;
    }
}

void daWmPeachCastle_c::initFlagAnim() {
    static const char *nodeNames[] = {
        "FlagA0", "FlagA1", "FlagA2",
        "FlagB0", "FlagB1", "FlagB2",
        "FlagC0", "FlagC1", "FlagC2",
        "FlagD0", "FlagD1", "FlagD2",
        "FlagE0", "FlagE1", "FlagE2",
    };

    dsChrLib::releaseAnim(&mChrAnim[cobPeachCastleFlag]);
    dsChrLib::bindAnimToNodeList(&mModel, &mChrAnim[cobPeachCastleFlag], ARRAY_SIZE(nodeNames), nodeNames, nw4r::g3d::AnmObjChr::BIND_ONE);
    mChrAnim[cobPeachCastleFlag].setRate(1.0f);
    mChrBlendAnim.attach(0, &mChrAnim[cobPeachCastleFlag], 1.0f);
}

const char *daWmPeachCastle_c::smc_doorNodeNames[] = {
    "LeftDoor",
    "RightDoor"
};

void daWmPeachCastle_c::initDoorOpenAnim() {
    dsChrLib::releaseAnim(&mChrAnim[cobPeachCastleOpen]);
    dsChrLib::bindAnimToNodeList(&mModel, &mChrAnim[cobPeachCastleOpen], ARRAY_SIZE(smc_doorNodeNames), smc_doorNodeNames, nw4r::g3d::AnmObjChr::BIND_ONE);
    mChrAnim[cobPeachCastleOpen].setRate(0.0f);
    mChrAnim[cobPeachCastleOpen].setFrame(0.0f);
    mChrBlendAnim.attach(1, &mChrAnim[cobPeachCastleOpen], 1.0f);
}

void daWmPeachCastle_c::initDoorCloseAnim() {
    dsChrLib::releaseAnim(&mChrAnim[cobPeachCastleClose]);
    dsChrLib::bindAnimToNodeList(&mModel, &mChrAnim[cobPeachCastleClose], ARRAY_SIZE(smc_doorNodeNames), smc_doorNodeNames, nw4r::g3d::AnmObjChr::BIND_ONE);
    mChrAnim[cobPeachCastleClose].setRate(0.0f);
    mChrAnim[cobPeachCastleClose].setFrame(0.0f);
    mChrBlendAnim.attach(1, &mChrAnim[cobPeachCastleClose], 1.0f);
}

void daWmPeachCastle_c::ensureCourseClear() {
    if (!IsCourseClear()) {
        dMj2dGame_c *save = dSaveMng_c::m_instance->getSaveGame(-1);
        save->onCourseDataFlag(WORLD_1, STAGE_PEACH_CASTLE, dMj2dGame_c::GOAL_NORMAL);
        dInfo_c::m_instance->m_60 = 2;
    }
}

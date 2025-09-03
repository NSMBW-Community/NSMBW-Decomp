#include <game/bases/d_s_boot.hpp>
#include <game/bases/d_system.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_graph.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_reset.hpp>
#include <game/bases/d_remocon_manager.hpp>
#include <game/bases/d_dylink.hpp>
#include <game/bases/d_hbm.hpp>
#include <game/bases/d_fader.hpp>
#include <game/bases/d_warning_manager.hpp>
#include <game/bases/d_cursor_select.hpp>
#include <revolution/VI.h>

ACTOR_PROFILE(BOOT, dScBoot_c, 0);

dScBoot_c *dScBoot_c::m_instance;

namespace {

dDynamicModuleControl s_DBasesDMC("d_bases", nullptr);
dDynamicModuleControl s_EnemiesDMC("d_enemies", nullptr);
dDynamicModuleControl s_En_BossDMC("d_en_boss", nullptr);

} // anonymous namespace

namespace {

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0a(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0b(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0c(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2a(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2b(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2c(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2d(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2e(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_3a(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myDylinkInitPhase_3b(void *) { return sPhase_c::OK; }

} // anonymous namespace

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_WiiStrap(void *) { return sPhase_c::OK; }

sPhase_c::METHOD_RESULT_e myCreate_LogoScreen(void *) { return sPhase_c::OK; }

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_Painter(void *) { return sPhase_c::OK; }

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_LinkWindows(void *) { return sPhase_c::OK; }

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_CreateYesNoWindow(void *) { return sPhase_c::OK; }

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_8015c240(void *) { return sPhase_c::OK; }

sPhase_c::METHOD_RESULT_e myCreate_ExtensionMng(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_ExtensionMng(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_MakeControllerInformation(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_BootSound(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_Sound(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_resSndWait(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_SaveBanner(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_resWait(void *) { return sPhase_c::OK; }

sPhase_c::METHOD_RESULT_e myReadArc_RegRes(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_WipeKuppa(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_WipeDokan(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_WipeMario(void *) { return sPhase_c::OK; }
sPhase_c::METHOD_RESULT_e myReadArc_WipeCircle(void *) { return sPhase_c::OK; }

sPhase_c::phaseMethod myCreate_PhaseMethod[] = {
    myCreate_WiiStrap,
    myCreate_LogoScreen,
    myCreate_Painter
};

sPhase_c myCreate_InitPhase(myCreate_PhaseMethod, ARRAY_SIZE(myCreate_PhaseMethod));

sPhase_c::phaseMethod myBackGround_PhaseMethod[] = {
    myCreate_LinkWindows,
    myCreate_CreateYesNoWindow,
    myCreate_8015c240,
    myCreate_ExtensionMng,
    myReadArc_ExtensionMng,
    myReadArc_MakeControllerInformation,
    myReadArc_BootSound,
    myReadArc_Sound,
    myReadArc_resSndWait,
    myReadArc_SaveBanner,
    myReadArc_resWait,
    myDylinkInitPhase_0a,
    myDylinkInitPhase_0b,
    myDylinkInitPhase_0c,
    myDylinkInitPhase_2a,
    myDylinkInitPhase_2b,
    myDylinkInitPhase_2c,
    myDylinkInitPhase_2d,
    myDylinkInitPhase_2e,
    myDylinkInitPhase_3a,
    myDylinkInitPhase_3b,
    myReadArc_RegRes,
    myReadArc_WipeKuppa,
    myReadArc_WipeDokan,
    myReadArc_WipeMario,
    myReadArc_WipeCircle
};

sPhase_c myBackGround_Phase(myBackGround_PhaseMethod, ARRAY_SIZE(myBackGround_PhaseMethod));

STATE_DEFINE(dScBoot_c, ResetWait);
STATE_DEFINE(dScBoot_c, ResetFadeOut);
STATE_DEFINE(dScBoot_c, ResetFadeIn);
STATE_DEFINE(dScBoot_c, FadeOutWait);
STATE_DEFINE(dScBoot_c, WiiStrapKeyWait);
STATE_DEFINE(dScBoot_c, WiiStrapDispEndWait);
STATE_DEFINE(dScBoot_c, WiiStrapFadeOut);
STATE_DEFINE(dScBoot_c, ControllerInformationFadeIn);
STATE_DEFINE(dScBoot_c, ControllerInformationSoundWait);
STATE_DEFINE(dScBoot_c, ControllerInformationKeyWait);
STATE_DEFINE(dScBoot_c, ControllerInformationDispEndWait);
STATE_DEFINE(dScBoot_c, NandCommandEndWait);
STATE_DEFINE(dScBoot_c, ExistFileCheck);
STATE_DEFINE(dScBoot_c, NandSpaceCheck);
STATE_DEFINE(dScBoot_c, CreateFile);
STATE_DEFINE(dScBoot_c, Load);
STATE_DEFINE(dScBoot_c, GoToErrorFadeOut);
STATE_DEFINE(dScBoot_c, GoToErrorFadeIn);
STATE_DEFINE(dScBoot_c, NewSaveFileDisp);
STATE_DEFINE(dScBoot_c, ButtonInputWait);
STATE_DEFINE(dScBoot_c, WindowExitWait);
STATE_DEFINE(dScBoot_c, ProcEnd);
STATE_DEFINE(dScBoot_c, ErrorWindowOnStageWait);
STATE_DEFINE(dScBoot_c, ErrorInfinityDisp);

dScBoot_c::dScBoot_c() :
    mStateMgr1(*this, StateID_FadeOutWait),
    mStateMgr2(*this, StateID_ResetWait)
{
    mpPhase = &myCreate_InitPhase;
    m_instance = this;
}

dScBoot_c::~dScBoot_c() {
    m_instance = nullptr;
}

int dScBoot_c::create() {
    dSys_c::setClearColor(nw4r::ut::Color::BLACK);
    dSys_c::setFrameRate(1);
    setFadeInFrame(30);
    setFadeOutFrame(30);

    dGameCom::initRandomSeed();

    new dResMng_c();
    new dInfo_c();

    dScStage_c::setTitleReplayRandomTable();
    dGameCom::initGame();

    m_104 = false;
    m_105 = false;

    VIEnableDimming(true);

    return SUCCEEDED;
}

void dScBoot_c::deleteReady() {
    dGraph_c::ms_Instance = nullptr;
}

int dScBoot_c::doDelete() {
    if (myBackGround_Phase.callMethod(this) != sPhase_c::DONE) {
        return NOT_READY;
    }

    dReset::Manage_c::GetInstance()->BootComplete();
    dRemoconMng_c::dConnect_c::m_isBoot = 0;
    setFadeInFrame(30);
    setFadeOutFrame(30);

    if (!dDyl::Unlink(fProfile::YES_NO_WINDOW)) {
        return NOT_READY;
    }
    if (!dDyl::Unlink(fProfile::CONTROLLER_INFORMATION)) {
        return NOT_READY;
    }

    dSystem::createEffectManagerPhase2(nullptr);

    return SUCCEEDED;
}

int dScBoot_c::preExecute() {
    if (dScene_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    dReset::Manage_c::GetInstance()->PermitSoftReset(true);
    return SUCCEEDED;
}

int dScBoot_c::execute() {
    myBackGround_Phase.callMethod(this);
    mStateMgr2.executeState();
    if (!mSkipFirstPhase) {
        mStateMgr1.executeState();
    }
    return SUCCEEDED;
}

int dScBoot_c::draw() {
    return SUCCEEDED;
}

void dScBoot_c::initializeState_ResetWait() {
    mSkipFirstPhase = false;
    mpWiiStrapScreen->mLayout.mpAnimGroup->setAndUpdate(0.0f);
}

void dScBoot_c::executeState_ResetWait() {
    if (dReset::Manage_c::GetInstance()->mSoftResetStateMaybe != 1) {
        return;
    }
    dHbm::Manage_c::GetInstance()->mFlags |= 0x40;
    mStateMgr2.changeState(StateID_ResetFadeOut);
}

void dScBoot_c::finalizeState_ResetWait() {}

void dScBoot_c::initializeState_ResetFadeOut() {
    mSkipFirstPhase = true;
    dScene_c::m_isAutoFadeIn = false;
    dFader_c::setFader(false);
    m_104 = dFader_c::startFadeOut(30);
    m_8d = false;
}

void dScBoot_c::executeState_ResetFadeOut() {
    if (!m_104) {
        m_104 = dFader_c::startFadeOut(30);
        return;
    }
    if (!m_8d && dReset::Manage_c::GetInstance()->IsFaderBlank()) {
        m_8d = true;
    }
    if (!m_8d && dFader_c::mFader->getStatus() == mFaderBase_c::OPAQUE) {
        if (mpControllerInformation != nullptr && mpControllerInformation->mVisible) {
            if (dWarningManager_c::m_instance->m_b00 > 0) {
                dWarningManager_c::m_instance->AllWarningEnd(false);
            }
        }
        if (!m_105) {
            finalizeState_ResetFadeIn();
            mStateMgr1.changeState(StateID_ProcEnd);
            dSelectCursor_c::m_instance->Cancel(0);
            mpYesNoWindow->mIsActive = false;
            mpControllerInformation->mVisible = false;
            mStateMgr2.changeState(StateID_ResetWait);
        } else {
            mStateMgr2.changeState(StateID_ResetFadeIn);
        }
    }
}

#include <game/bases/d_s_boot.hpp>
#include <game/bases/d_system.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_graph.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_reset.hpp>
#include <game/bases/d_remocon_manager.hpp>
#include <game/bases/d_dylink.hpp>
#include <game/bases/d_hbm.hpp>
#include <game/bases/d_fader.hpp>
#include <game/bases/d_wipe_kuppa.hpp>
#include <game/bases/d_wipe_dokan.hpp>
#include <game/bases/d_wipe_mario.hpp>
#include <game/bases/d_wipe_circle.hpp>
#include <game/bases/d_nand_thread.hpp>
#include <game/bases/d_s_restart_crsin.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/bases/d_warning_manager.hpp>
#include <game/mLib/m_pad.hpp>
#include <revolution/VI.h>

BASE_PROFILE(BOOT, dScBoot_c);

dScBoot_c *dScBoot_c::m_instance;

void painter() {
    d2d::draw();
}

namespace {

dDynamicModuleControl s_DBasesDMC("d_bases", nullptr);
dDynamicModuleControl s_EnemiesDMC("d_enemies", nullptr);
dDynamicModuleControl s_En_BossDMC("d_en_boss", nullptr);

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0a(void *) {
    if (s_DBasesDMC.load_async()) {
        s_DBasesDMC.link();
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0b(void *) {
    if (s_EnemiesDMC.load_async()) {
        s_EnemiesDMC.link();
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0c(void *) {
    if (s_En_BossDMC.load_async()) {
        s_En_BossDMC.link();
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2a(void *) {
    if (
        dDyl::LinkASync(fProfile::CRSIN) == 1 &&
        dDyl::LinkASync(fProfile::STAGE) == 1 &&
        dDyl::LinkASync(fProfile::RESTART_CRSIN) == 1 &&
        dDyl::LinkASync(fProfile::GAMEOVER) == 1 &&
        dDyl::LinkASync(fProfile::RESULT) == 1 &&
        dDyl::LinkASync(fProfile::MOVIE) == 1
    ) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2b(void *) {
    if (dDyl::LinkASync(fProfile::BOOT) == 1) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2c(void *) {
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2d(void *) {
    if (dDyl::LinkASync(fProfile::WORLD_9_DEMO) == 1) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_2e(void *) {
    if (dDyl::LinkASync(fProfile::WORLD_MAP) == 1) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_3a(void *) {
    if (dDyl::LinkASync(fProfile::GAME_SETUP) == 1) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_3b(void *) {
    if (dDyl::LinkASync(fProfile::MULTI_PLAY_COURSE_SELECT) == 1) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

} // anonymous namespace

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_Controller(void *) {
    if (dDyl::LinkASync(fProfile::YES_NO_WINDOW) != 1) {
        return sPhase_c::WAIT;
    }
    if (dDyl::LinkASync(fProfile::CONTROLLER_INFORMATION) != 1) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_WiiStrap(void *thisPtr) {
    dScBoot_c *self = (dScBoot_c *) thisPtr;
    dWiiStrapScreen_c *strapScreen = (dWiiStrapScreen_c *) fBase_c::createChild(fProfile::WII_STRAP, self, 0, 0);
    self->mpWiiStrapScreen = strapScreen;
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myCreate_LogoScreen(void *thisPtr) {
    dScBoot_c *self = (dScBoot_c *) thisPtr;
    if (!self->mpWiiStrapScreen->mHasLoadedLayout) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myCreate_ExtensionMng(void *) {
    dWarningManager_c::CreateWarningManager();
    dWarningManager_c::addWarningForbid();
    return sPhase_c::OK;
}

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_Painter(void *) {
    dGraph_c::ms_Instance->mpPainterFunc = painter;
    return sPhase_c::OK;
}

sPhase_c::phaseMethod myCreate_PhaseMethod[] = {
    myCreate_WiiStrap,
    myCreate_LogoScreen,
    myCreate_Painter
};

/// @brief This phase is run via dScene_c::mpPhase, so before dScBoot_c::execute() is run.
sPhase_c myCreate_InitPhase(myCreate_PhaseMethod, ARRAY_SIZE(myCreate_PhaseMethod));

mDvd_callback_c *s_dvdcb;

void *BootSound(void *) {
    dAudio::boot();
    return (void *) 1;
}

sPhase_c::METHOD_RESULT_e myReadArc_BootSound(void *) {
    s_dvdcb = mDvd_callback_c::createOrDie(BootSound, nullptr);
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myReadArc_Sound(void *) {
    if (!s_dvdcb->mDone) {
        return sPhase_c::WAIT;
    }
    s_dvdcb->destroy();
    s_dvdcb = nullptr;
    dAudio::FUN_8006a6a0(true);
    dAudio::loadSceneSnd();
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myReadArc_resSndWait(void *thisPtr) {
    dScBoot_c *self = (dScBoot_c *) thisPtr;
    if (dAudio::isLoadedSceneSnd()) {
        self->mSceneSoundLoaded = true;
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myCreate_HbmManage(void *) {
    sPhase_c::METHOD_RESULT_e res = sPhase_c::WAIT;
    if (dHbm::Manage_c::GetInstance()->Load()) {
        res = sPhase_c::OK;
    }
    return res;
}

sPhase_c::METHOD_RESULT_e myReadArc_SaveBanner(void *) {
    const char *saveBannerPath = "/EU/save_banner";
    static const char saveBannerName[] = "save_banner"; ///< @unofficial
    dResMng_c::m_instance->setRes(saveBannerPath, saveBannerName, nullptr);
    return sPhase_c::OK;
}

/// @unofficial
const char *l_env_names[] = {
    "Env_world",
    "Env_course",
    "Env_movie"
};

sPhase_c::METHOD_RESULT_e myReadArc_RegRes(void *) {
    dResMng_c::m_instance->setRes("Env", l_env_names, ARRAY_SIZE(l_env_names), nullptr);
    return sPhase_c::OK;
}


sPhase_c::METHOD_RESULT_e myReadArc_resWait(void *) {
    if (dResMng_c::m_instance->mRes.syncAllRes()) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myReadArc_WipeKuppa(void *) {
    if (dWipeKuppa_c::m_instance != nullptr && dWipeKuppa_c::m_instance->createLayout()) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}
sPhase_c::METHOD_RESULT_e myReadArc_WipeDokan(void *) {
    if (dWipeDokan_c::m_instance != nullptr && dWipeDokan_c::m_instance->createLayout()) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}
sPhase_c::METHOD_RESULT_e myReadArc_WipeMario(void *) {
    if (dWipeMario_c::m_instance != nullptr && dWipeMario_c::m_instance->createLayout()) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}
sPhase_c::METHOD_RESULT_e myReadArc_WipeCircle(void *) {
    if (dWipeCircle_c::m_instance != nullptr && dWipeCircle_c::m_instance->createLayout()) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_CreateYesNoWindow(void *selfPtr) {
    dScBoot_c *self = (dScBoot_c *) selfPtr;
    dYesNoWindow_c *ynWindow = (dYesNoWindow_c *) fBase_c::createChild(fProfile::YES_NO_WINDOW, self, 0, 0);
    self->mpYesNoWindow = ynWindow;
    dSelectCursor_c *selectCursor = (dSelectCursor_c *) fBase_c::createChild(fProfile::SELECT_CURSOR, self, 0, 0);
    self->mpSelectCursor = selectCursor;
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myReadArc_ExtensionMng(void *) {
    if (!dWarningManager_c::m_Created) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myReadArc_MakeControllerInformation(void *selfPtr) {
    dScBoot_c *self = (dScBoot_c *) selfPtr;
    dControllerInformation_c *controllerInfo = (dControllerInformation_c *) fBase_c::createChild(fProfile::CONTROLLER_INFORMATION, self, 0, 0);
    self->mpControllerInformation = controllerInfo;
    return sPhase_c::OK;
}

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_ControllerInformationReady(void *selfPtr) {
    dScBoot_c *self = (dScBoot_c *) selfPtr;
    if (!self->mpControllerInformation->mIsCreated) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

/// @unofficial
sPhase_c::METHOD_RESULT_e myCreate_YesNoWindowReady(void *selfPtr) {
    dScBoot_c *self = (dScBoot_c *) selfPtr;
    dYesNoWindow_c *ynWindow = self->mpYesNoWindow;
    dSelectCursor_c *selectCursor = self->mpSelectCursor;
    if (!ynWindow->mHasLoadedLayout || !selectCursor->mHasLoadedLayout) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

sPhase_c::phaseMethod myBackGround_PhaseMethod[] = {
    myCreate_Controller,
    dSystem::createFontManagerPhase,
    dSystem::createMessageManagerPhase,
    myCreate_CreateYesNoWindow,
    myCreate_YesNoWindowReady,
    myCreate_ExtensionMng,
    myReadArc_ExtensionMng,
    myCreate_HbmManage,
    myReadArc_MakeControllerInformation,
    myCreate_ControllerInformationReady,
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
    myReadArc_resWait,
    myReadArc_WipeKuppa,
    myReadArc_WipeDokan,
    myReadArc_WipeMario,
    myReadArc_WipeCircle,
    dSystem::createEffectManagerPhase1
};

/// @brief This phase is run directly inside of dScBoot_c::execute() each frame.
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
    mStateMgr(*this, StateID_FadeOutWait),
    mResetStateMgr(*this, StateID_ResetWait)
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

    mResetFadeOutStarted = false;
    mSaveDataCreatedMessageShown = false;

    VIEnableDimming(true);

    return SUCCEEDED;
}

void dScBoot_c::deleteReady() {
    dGraph_c::ms_Instance->mpPainterFunc = nullptr;
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
    mResetStateMgr.executeState();
    if (!mIsResetting) {
        mStateMgr.executeState();
    }
    return SUCCEEDED;
}

int dScBoot_c::draw() {
    return SUCCEEDED;
}

// [Unsure how one could get rid of this pragma...]
#pragma push
#pragma pool_data off

void dScBoot_c::initializeState_ResetWait() {
    mIsResetting = false;
    mpWiiStrapScreen->mLayout.mpAnimGroup->setAndUpdate(0.0f);
}

void dScBoot_c::executeState_ResetWait() {
    if (dReset::Manage_c::GetInstance()->mSoftResetStateMaybe != 1) {
        return;
    }
    dHbm::Manage_c::GetInstance()->mFlags |= 0x40;
    mResetStateMgr.changeState(StateID_ResetFadeOut);
}

void dScBoot_c::finalizeState_ResetWait() {}

void dScBoot_c::initializeState_ResetFadeOut() {
    mIsResetting = true;
    dScene_c::m_isAutoFadeIn = false;
    dFader_c::setFader(dFader_c::FADER_FADE);
    mResetFadeOutStarted = dFader_c::startFadeOut(30);
    mResetFaderDone = false;
}

void dScBoot_c::executeState_ResetFadeOut() {
    if (!mResetFadeOutStarted) {
        mResetFadeOutStarted = dFader_c::startFadeOut(30);
        return;
    }
    if (!mResetFaderDone && dReset::Manage_c::GetInstance()->IsFaderBlank()) {
        mResetFaderDone = true;
    }
    if (mResetFaderDone && dFader_c::isStatus(mFaderBase_c::OPAQUE)) {
        if (mpControllerInformation != nullptr && mpControllerInformation->mIsCreated) {
            if (dWarningManager_c::m_instance->m_b00 >= 1) {
                dWarningManager_c::m_instance->AllWarningEnd(false);
            }
            if (mSaveDataCreatedMessageShown) {
                // Hide the yes/no window so that it doesn't show up again after reset
                finalizeState_ResetFadeIn();
                mStateMgr.changeState(StateID_ProcEnd);
                dYesNoWindow_c *ynWindow = mpYesNoWindow;
                dSelectCursor_c::m_instance->Cancel(0);
                ynWindow->mIsActive = false;
                mpControllerInformation->mVisible = false;
                mResetStateMgr.changeState(StateID_ResetWait);
            } else {
                mResetStateMgr.changeState(StateID_ResetFadeIn);
            }
        }
    }
}

void dScBoot_c::finalizeState_ResetFadeOut() {
    dReset::Manage_c::GetInstance()->SetSoftResetFinish();
    if (isState(StateID_WiiStrapDispEndWait) || isState(StateID_ControllerInformationDispEndWait)) {
        mResetFaderDone = false;
        if (isState(StateID_WiiStrapDispEndWait)) {
            dWarningManager_c::m_instance->AllWarningEnd(false);
            mpControllerInformation->mVisible = true;
            mpWiiStrapScreen->mVisible = false;
            mStateMgr.changeState(StateID_ControllerInformationSoundWait);
        } else {
            mpControllerInformation->mVisible = false;
            mStateMgr.changeState(StateID_NandCommandEndWait);
        }
        mAutoAdvanceTimer = 0;
        mMinWaitTimer = 0;
    }
    mResetFadeOutStarted = false;
}

void dScBoot_c::initializeState_ResetFadeIn() {
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(true);
    dFader_c::setFader(dFader_c::FADER_FADE);
    dFader_c::startFadeIn(30);
    mpWiiStrapScreen->mLayout.mpAnimGroup->setAndUpdate(0.0f);
}

void dScBoot_c::executeState_ResetFadeIn() {
    if (mFader_c::isStatus(mFaderBase_c::HIDDEN)) {
        mResetStateMgr.changeState(StateID_ResetWait);
    }
}

void dScBoot_c::finalizeState_ResetFadeIn() {
    m_isAutoFadeIn = true;
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
    if (isState(StateID_ErrorWindowOnStageWait) || isState(StateID_ErrorInfinityDisp)) {
        mStateMgr.changeState(StateID_GoToErrorFadeOut);
    }
    if (isState(StateID_ControllerInformationKeyWait) || isState(StateID_ControllerInformationDispEndWait)) {
        mAutoAdvanceTimer = 0;
        mMinWaitTimer = 0;
    } else {
        mAutoAdvanceTimer = 1200;
        mMinWaitTimer = 60;
    }
    mResetFaderDone = false;
    mIsResetting = false;
}

void dScBoot_c::initializeState_FadeOutWait() {}

void dScBoot_c::executeState_FadeOutWait() {
    if (mFader_c::isStatus(mFaderBase_c::HIDDEN)) {
        mStateMgr.changeState(StateID_WiiStrapKeyWait);
    }
}

void dScBoot_c::finalizeState_FadeOutWait() {}

#pragma pop

void dScBoot_c::initializeState_WiiStrapKeyWait() {
    mAutoAdvanceTimer = 1200;
    mMinWaitTimer = 60;
    mpWiiStrapScreen->mLayout.mpAnimGroup->setAndUpdate(0.0f);
}

void dScBoot_c::executeState_WiiStrapKeyWait() {
    mAutoAdvanceTimer--;
    mMinWaitTimer--;
    if (mMinWaitTimer <= 0) {
        mStateMgr.changeState(StateID_WiiStrapDispEndWait);
    }
}

void dScBoot_c::finalizeState_WiiStrapKeyWait() {}

void dScBoot_c::initializeState_WiiStrapDispEndWait() {}

void dScBoot_c::executeState_WiiStrapDispEndWait() {
    mAutoAdvanceTimer--;
    ulong buttons = (
        WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT | WPAD_BUTTON_DOWN | WPAD_BUTTON_UP |
        WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_1 | WPAD_BUTTON_2 |
        WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS |
        WPAD_BUTTON_FS_Z | WPAD_BUTTON_FS_C
    );
    if (mAutoAdvanceTimer <= 0 || mPad::g_currentCore->downTrigger(buttons) || dInfo_c::mGameFlag & 0x80000) {
        mStateMgr.changeState(StateID_WiiStrapFadeOut);
    }
}

void dScBoot_c::finalizeState_WiiStrapDispEndWait() {}

void dScBoot_c::initializeState_WiiStrapFadeOut() {
    dScene_c::m_isAutoFadeIn = false;
    dFader_c::setFader(dFader_c::FADER_FADE);
    dFader_c::startFadeOut(30);
    dHbm::Manage_c::GetInstance()->mFlags |= 0x40;
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(true);
}

void dScBoot_c::executeState_WiiStrapFadeOut() {
    if (mFader_c::isStatus(mFaderBase_c::OPAQUE)) {
        if (mpControllerInformation != nullptr && mpControllerInformation->mIsCreated) {
            mpWiiStrapScreen->mVisible = false;
            mStateMgr.changeState(StateID_ControllerInformationFadeIn);
        }
    }
}

void dScBoot_c::finalizeState_WiiStrapFadeOut() {
    dWarningManager_c::m_instance->AllWarningEnd(false);
}

void dScBoot_c::initializeState_ControllerInformationFadeIn() {
    dFader_c::setFader(dFader_c::FADER_FADE);
    dFader_c::startFadeIn(30);
    mpControllerInformation->mVisible = true;
}

void dScBoot_c::executeState_ControllerInformationFadeIn() {
    if (mFader_c::isStatus(mFaderBase_c::HIDDEN)) {
        mStateMgr.changeState(StateID_ControllerInformationSoundWait);
    }
}

void dScBoot_c::finalizeState_ControllerInformationFadeIn() {
    dScene_c::m_isAutoFadeIn = true;
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
    dWarningManager_c::subWarningForbid();
}

void dScBoot_c::initializeState_ControllerInformationSoundWait() {
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x08;
    mAutoAdvanceTimer = 300;
    mMinWaitTimer = 30;
}

void dScBoot_c::executeState_ControllerInformationSoundWait() {
    if (mAutoAdvanceTimer > 0) {
        mAutoAdvanceTimer--;
    }
    if (mMinWaitTimer > 0) {
        mMinWaitTimer--;
    }
    if (mSceneSoundLoaded) {
        mStateMgr.changeState(StateID_ControllerInformationKeyWait);
    }
}

void dScBoot_c::finalizeState_ControllerInformationSoundWait() {}

void dScBoot_c::initializeState_ControllerInformationKeyWait() {}

void dScBoot_c::executeState_ControllerInformationKeyWait() {
    if (mAutoAdvanceTimer > 0) {
        mAutoAdvanceTimer--;
    }
    mMinWaitTimer--;
    if (mMinWaitTimer <= 0) {
        mpControllerInformation->mState = dControllerInformation_c::SHOW_IN;
        mStateMgr.changeState(StateID_ControllerInformationDispEndWait);
    }
}

void dScBoot_c::finalizeState_ControllerInformationKeyWait() {}

void dScBoot_c::initializeState_ControllerInformationDispEndWait() {}

void dScBoot_c::executeState_ControllerInformationDispEndWait() {
    if (mAutoAdvanceTimer != 0) {
        mAutoAdvanceTimer--;
    }
    ulong buttons = (
        WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT | WPAD_BUTTON_DOWN | WPAD_BUTTON_UP |
        WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_1 | WPAD_BUTTON_2 |
        WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS |
        WPAD_BUTTON_FS_Z | WPAD_BUTTON_FS_C
    );
    if (mPad::g_currentCore->downTrigger(buttons) || dInfo_c::mGameFlag & 0x80000) {
        mpControllerInformation->mState = dControllerInformation_c::END;
        mAutoAdvanceTimer = 0;
    }
    if (mAutoAdvanceTimer <= 0) {
        mStateMgr.changeState(StateID_NandCommandEndWait);
    }
}

void dScBoot_c::finalizeState_ControllerInformationDispEndWait() {}

void dScBoot_c::initializeState_NandCommandEndWait() {}

void dScBoot_c::executeState_NandCommandEndWait() {
    if (dNandThread_c::m_instance->mState == 0) {
        if (!dReset::Manage_c::GetInstance()->isSafetyWait()) {
            mStateMgr.changeState(StateID_ExistFileCheck);
        }
    }
}

void dScBoot_c::finalizeState_NandCommandEndWait() {}

void dScBoot_c::initializeState_ExistFileCheck() {
    dHbm::Manage_c::GetInstance()->mFlags |= 0x40;
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(true);
    dNandThread_c::m_instance->cmdExistCheck();
    mNewSaveFileCreated = false;
}

void dScBoot_c::finalizeState_ExistFileCheck() {}

void dScBoot_c::executeState_ExistFileCheck() {
    if (dSaveMng_c::m_instance->isNandBusy()) {
        return;
    }
    if (dNandThread_c::m_instance->mError != 0) {
        mStateMgr.changeState(StateID_GoToErrorFadeOut);
    } else if (dNandThread_c::m_instance->mFileExists) {
        mStateMgr.changeState(StateID_Load);
    } else {
        mStateMgr.changeState(StateID_NandSpaceCheck);
    }
}

void dScBoot_c::initializeState_NandSpaceCheck() {
    dNandThread_c::m_instance->cmdSpaceCheck();
}

void dScBoot_c::finalizeState_NandSpaceCheck() {}

void dScBoot_c::executeState_NandSpaceCheck() {
    if (dSaveMng_c::m_instance->isNandBusy()) {
        return;
    }
    if (dNandThread_c::m_instance->mError != 0) {
        mStateMgr.changeState(StateID_GoToErrorFadeOut);
    } else {
        const char *save_icon = "save_icon.bti";
        const char *save_banner = "save_banner";
        if (dResMng_c::m_instance->mRes.getRes(save_banner, save_icon).IsValid()) {
            mStateMgr.changeState(StateID_CreateFile);
        }
    }
}

void dScBoot_c::initializeState_CreateFile() {
    dSaveMng_c *saveMng = dSaveMng_c::m_instance;
    saveMng->mHeader.initialize();
    for (int i = 0; i < SAVE_SLOT_COUNT; i++) {
        saveMng->getSaveGame(i)->initialize();
        saveMng->getTempGame(i)->initialize();
    }
    saveMng->calcCRC();
    saveMng->startNandSave();
}

void dScBoot_c::executeState_CreateFile() {
    if (dSaveMng_c::m_instance->isNandBusy()) {
        return;
    }
    if (dNandThread_c::m_instance->mError == 0) {
        mNewSaveFileCreated = true;
    }
    dScene_c::m_isAutoFadeIn = false;
    mStateMgr.changeState(StateID_GoToErrorFadeIn);
}

void dScBoot_c::finalizeState_CreateFile() {}

void dScBoot_c::initializeState_Load() {
    dSaveMng_c::m_instance->startNandLoad();
}

void dScBoot_c::executeState_Load() {
    if (dSaveMng_c::m_instance->isNandBusy()) {
        return;
    }
    if (dNandThread_c::m_instance->mError != 0) {
        mStateMgr.changeState(StateID_GoToErrorFadeOut);
        return;
    }
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
    mStateMgr.changeState(StateID_ProcEnd);
}

void dScBoot_c::finalizeState_Load() {}

void dScBoot_c::initializeState_ProcEnd() {}

void dScBoot_c::executeState_ProcEnd() {
    dScRestartCrsin_c::startTitle(0, false);
}

void dScBoot_c::finalizeState_ProcEnd() {}

void dScBoot_c::initializeState_NewSaveFileDisp() {
    mpYesNoWindow->mType = dYesNoWindow_c::SAVE_DATA_CREATED;
    mpYesNoWindow->mIsActive = true;
}

void dScBoot_c::executeState_NewSaveFileDisp() {
    if (!mpYesNoWindow->mIsAnimating) {
        mStateMgr.changeState(StateID_ButtonInputWait);
    }
}

void dScBoot_c::finalizeState_NewSaveFileDisp() {
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
}

void dScBoot_c::initializeState_ButtonInputWait() {
    mSaveDataCreatedMessageShown = true;
}

void dScBoot_c::executeState_ButtonInputWait() {
    if (mPad::g_currentCore->downTrigger(WPAD_BUTTON_A | WPAD_BUTTON_2) || dInfo_c::mGameFlag & 0x80000) {
        dReset::Manage_c::GetInstance()->ActiveSaveWindow(true);
        mStateMgr.changeState(StateID_WindowExitWait);
    }
}

void dScBoot_c::finalizeState_ButtonInputWait() {}

void dScBoot_c::initializeState_WindowExitWait() {
    mpYesNoWindow->mHitButton = true;
}

void dScBoot_c::executeState_WindowExitWait() {
    if (!mpYesNoWindow->mIsAnimating) {
        dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
        dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
        mNewSaveFileCreated = false;
        mStateMgr.changeState(StateID_ProcEnd);
    }
}

void dScBoot_c::finalizeState_WindowExitWait() {}

void dScBoot_c::initializeState_GoToErrorFadeOut() {
    dScene_c::m_isAutoFadeIn = false;
    dFader_c::setFader(dFader_c::FADER_FADE);
    dFader_c::startFadeOut(30);
    dReset::Manage_c::GetInstance()->ActiveSaveWindow(true);
}

void dScBoot_c::executeState_GoToErrorFadeOut() {
    if (mFader_c::isStatus(mFaderBase_c::OPAQUE)) {
        mpWiiStrapScreen->mVisible = false;
        mpControllerInformation->mVisible = false;
        mStateMgr.changeState(StateID_GoToErrorFadeIn);
    }
}

void dScBoot_c::finalizeState_GoToErrorFadeOut() {}

void dScBoot_c::initializeState_GoToErrorFadeIn() {
    dFader_c::setFader(dFader_c::FADER_FADE);
    dFader_c::startFadeIn(30);
}

void dScBoot_c::executeState_GoToErrorFadeIn() {
    if (mFader_c::isStatus(mFaderBase_c::HIDDEN)) {
        if (mNewSaveFileCreated) {
            dScene_c::m_isAutoFadeIn = true;
            mStateMgr.changeState(StateID_NewSaveFileDisp);
        } else {
            dScene_c::m_isAutoFadeIn = true;
            dReset::Manage_c::GetInstance()->ActiveSaveWindow(false);
            mStateMgr.changeState(StateID_ErrorWindowOnStageWait);
        }
    }
}

void dScBoot_c::finalizeState_GoToErrorFadeIn() {
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
}

void dScBoot_c::initializeState_ErrorWindowOnStageWait() {
    dWarningManager_c::m_instance->setError(dNandThread_c::m_instance->mError);
}

void dScBoot_c::executeState_ErrorWindowOnStageWait() {
    dWarningManager_c *warningMgr = dWarningManager_c::m_instance;
    dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
    if (warningMgr->mErrorID == 6) {
        if (!warningMgr->m_b09) {
            dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
            mStateMgr.changeState(StateID_NandCommandEndWait);
        }
    } else if (!dWarningManager_c::isError()) {
        dHbm::Manage_c::GetInstance()->mFlags &= ~0x40;
        mStateMgr.changeState(StateID_ErrorInfinityDisp);
    }
}

void dScBoot_c::finalizeState_ErrorWindowOnStageWait() {}

void dScBoot_c::initializeState_ErrorInfinityDisp() {}

void dScBoot_c::executeState_ErrorInfinityDisp() {}

void dScBoot_c::finalizeState_ErrorInfinityDisp() {}

#include <game/bases/d_scene.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_fader.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_reset.hpp>
#include <game/bases/d_s_restart_crsin.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_WarningManager.hpp>
#include <game/mLib/m_fader.hpp>

u32 dScene_c::mPara;
ProfileName dScene_c::m_nextScene = fProfile::INVALID;
ProfileName dScene_c::m_nowScene = fProfile::INVALID;
ProfileName dScene_c::m_oldScene = fProfile::INVALID;
bool dScene_c::m_otherSceneFlg = true;

u16 dScene_c::m_fadeInFrame = 30;
u16 dScene_c::m_fadeOutFrame = 30;
bool dScene_c::m_isAutoFadeIn = true;

dScene_c::dScene_c() {
    setProcControlFlag(ROOT_DISABLE_EXECUTE);
    setProcControlFlag(ROOT_DISABLE_DRAW);

    mpPhase = nullptr;
    EffectManager_c::m_firstCalc = false;
    dGameCom::clearGameStop();

    dActor_c::mExecStopReq = 0;
    dActor_c::mDrawStopReq = 0;
    dActor_c::mExecStop = 0;
    dActor_c::mDrawStop = 0;
}

dScene_c::~dScene_c() {
    if (dWarningManager_c::m_WarningForbid > 0) {
        dWarningManager_c::m_WarningForbid--;
    }
    dAudio::destroySceneSnd();
}

int dScene_c::preCreate() {
    if (dBase_c::preCreate() == NOT_READY) {
        return NOT_READY;
    }

    m_isAutoFadeIn = true;
    return SUCCEEDED;
}

void dScene_c::postCreate(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        dReset::Manage_c::GetInstance()->SetSoftResetFinish();
        dAudio::prepareSceneSnd();
        fBase_c::createChild(fProfile::LASTACTOR, this, 0, fBase_c::OTHER);
    }
    dBase_c::postCreate(status);
}

int dScene_c::preDelete() {
    if (dBase_c::preDelete() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dScene_c::postDelete(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        m_otherSceneFlg = false;
        dReset::Manage_c::GetInstance()->PostDeleteScene();
    }
    dBase_c::postDelete(status);
}

int dScene_c::preExecute() {
    if (dBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (mpPhase != nullptr && mpPhase->callMethod(this) != sPhase_c::DONE) {
        // Phase not finished yet
        return NOT_READY;
    }

    if (m_nextScene != fProfile::INVALID) {
        // Prepare to go to next scene
        if (mFader_c::isStatus(mFaderBase_c::HIDDEN)) {
            dFader_c::startFadeOut(m_fadeOutFrame); // Initiate fade out if not started yet
        } else if (mFader_c::isStatus(mFaderBase_c::OPAQUE)) {
            deleteRequest(); // Delete this scene if the fade has completed
        }
        return NOT_READY;
    }

    if (isProcControlFlag(ROOT_DISABLE_EXECUTE)) {
        if (!checkChildProcessCreateState()) {
            // We may run the execute and draw steps of this base and its children from now on
            clearProcControlFlag(ROOT_DISABLE_EXECUTE);
            clearProcControlFlag(ROOT_DISABLE_DRAW);
        }
        return NOT_READY;
    }

    if (mFader_c::isStatus(mFaderBase_c::OPAQUE) && m_isAutoFadeIn) {
        // Begin fade in if needed, also initialize sound objects
        dFader_c::startFadeIn(m_fadeInFrame);
        dAudio::requestStartScene(m_nowScene);
    }

    return SUCCEEDED;
}

void dScene_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status != NOT_READY) {
        dReset::Manage_c::GetInstance()->SetSoftResetScene();
    }
    dBase_c::postExecute(status);
}

int dScene_c::preDraw() {
    if (dBase_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dScene_c::postDraw(fBase_c::MAIN_STATE_e status) {
    dBase_c::postDraw(status);
}

void dScene_c::setStartScene() {
    m_nextScene = fProfile::BOOT;
    m_nowScene = fProfile::BOOT;
    m_oldScene = fProfile::BOOT;
    m_otherSceneFlg = false;
    dAudio::setNextScene(fProfile::BOOT, 0);
}

void dScene_c::setResetScene() {
    dScRestartCrsin_c::startTitle(0, true);
}

dScene_c *dScene_c::createNextScene() {
    if (m_otherSceneFlg || m_nextScene == fProfile::INVALID) {
        return nullptr;
    }

    dScene_c *newScene = (dScene_c *) createRoot(m_nextScene, mPara, SCENE);
    if (newScene != nullptr) {
        m_oldScene = m_nowScene;
        m_nowScene = m_nextScene;
        m_nextScene = fProfile::INVALID;
        m_otherSceneFlg = true;
        return newScene;
    }

    return nullptr;
}

void dScene_c::setNextScene(ProfileName nextScene, unsigned long param, bool forceChange) {
    if (m_nextScene != fProfile::INVALID) {
        return;
    }

    dReset::Manage_c *resetMng = dReset::Manage_c::GetInstance();
    bool isChangeDisallowed =
        ((resetMng->mModeInit == dReset::Manage_c::SOFT_RESET) ||
        (resetMng->mModeProc == dReset::Manage_c::SOFT_RESET)) ||
        ((resetMng->mModeInit == dReset::Manage_c::SAFETY_WAIT) ||
        (resetMng->mModeProc == dReset::Manage_c::SAFETY_WAIT));

    if (!isChangeDisallowed || forceChange) {
        if (m_nowScene == fProfile::STAGE && nextScene != fProfile::STAGE) {
            dScStage_c::m_isCourseOut = true;
        }
        dAudio::setNextScene(nextScene, param);
        m_nextScene = nextScene;
        mPara = param;
    }
}

void dScene_c::setFadeInFrame(unsigned short length) {
    m_fadeInFrame = length;
}

void dScene_c::setFadeOutFrame(unsigned short length) {
    m_fadeOutFrame = length;
}

void dScene_c::setFadeInOutFrame(unsigned short length) {
    m_fadeInFrame = length;
    m_fadeOutFrame = length;
}

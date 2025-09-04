#pragma once

#include <game/bases/d_scene.hpp>
#include <game/bases/d_wii_strap_screen.hpp>
#include <game/bases/d_controller_information.hpp>
#include <game/bases/d_cursor_select.hpp>
#include <game/bases/d_yes_no_window.hpp>
#include <game/sLib/s_State.hpp>

class dScBoot_c : public dScene_c {
public:
    dScBoot_c(); ///< @copydoc dScene_c::dScene_c
    virtual ~dScBoot_c(); ///< @copydoc dScene_c::~dScene_c
    virtual int create();
    virtual int execute();
    virtual int preExecute();
    virtual int draw();
    virtual int doDelete();
    virtual void deleteReady();

private:
    STATE_FUNC_DECLARE(dScBoot_c, ResetWait);
    STATE_FUNC_DECLARE(dScBoot_c, ResetFadeOut);
    STATE_FUNC_DECLARE(dScBoot_c, ResetFadeIn);
    STATE_FUNC_DECLARE(dScBoot_c, FadeOutWait);
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapKeyWait);
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapDispEndWait);
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapFadeOut);
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationFadeIn);
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationSoundWait);
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationKeyWait);
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationDispEndWait);
    STATE_FUNC_DECLARE(dScBoot_c, NandCommandEndWait);
    STATE_FUNC_DECLARE(dScBoot_c, ExistFileCheck);
    STATE_FUNC_DECLARE(dScBoot_c, NandSpaceCheck);
    STATE_FUNC_DECLARE(dScBoot_c, CreateFile);
    STATE_FUNC_DECLARE(dScBoot_c, Load);
    STATE_FUNC_DECLARE(dScBoot_c, GoToErrorFadeOut);
    STATE_FUNC_DECLARE(dScBoot_c, GoToErrorFadeIn);
    STATE_FUNC_DECLARE(dScBoot_c, NewSaveFileDisp);
    STATE_FUNC_DECLARE(dScBoot_c, ButtonInputWait);
    STATE_FUNC_DECLARE(dScBoot_c, WindowExitWait);
    STATE_FUNC_DECLARE(dScBoot_c, ProcEnd);
    STATE_FUNC_DECLARE(dScBoot_c, ErrorWindowOnStageWait);
    STATE_FUNC_DECLARE(dScBoot_c, ErrorInfinityDisp);

    sFStateMgr_c<dScBoot_c, sStateMethodUsr_FI_c> mStateMgr1;
    sFStateMgr_c<dScBoot_c, sStateMethodUsr_FI_c> mStateMgr2;

    bool isState(const sStateIDIf_c &state) const {
        return *mStateMgr1.getStateID() == state;
    }

    u8 mPad1[2];
    dWiiStrapScreen_c *mpWiiStrapScreen;
    dYesNoWindow_c *mpYesNoWindow;
    dSelectCursor_c *mpSelectCursor;
    dControllerInformation_c *mpControllerInformation;

    bool m_8c;
    bool m_8d;
    bool m_8e;
    bool mSkipFirstPhase;

    bool m_90;
    bool m_91;
    int m_94;
    int m_98;

public:
    static dScBoot_c *m_instance;
};

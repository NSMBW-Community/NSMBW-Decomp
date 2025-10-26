#pragma once

#include <game/bases/d_scene.hpp>
#include <game/bases/d_wii_strap_screen.hpp>
#include <game/bases/d_controller_information.hpp>
#include <game/bases/d_cursor_select.hpp>
#include <game/bases/d_yes_no_window.hpp>
#include <game/sLib/s_State.hpp>

/// @brief This scene handles the game's boot process.
/// It does the following:
/// - Shows the Wii strap information screen.
/// - Shows the controller information screen.
/// - Checks for existing save data, creating new save data if none exists.
/// - Shows save data-related error messages
/// - Finally, transitions to the title screen.
/// @statetable
/// @ingroup bases
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

    bool isState(const sStateIDIf_c &state) const {
        return *mStateMgr.getStateID() == state;
    }

    /// @brief Deadstripped dummy function to force weak function order
    void DUMMY_FUNCTION_ORDER() {
        mStateMgr.initializeState();
        mStateMgr.finalizeState();
        mStateMgr.getOldStateID();
    }

private:
    STATE_FUNC_DECLARE(dScBoot_c, ResetWait); ///< Waiting for a reset event.
    STATE_FUNC_DECLARE(dScBoot_c, ResetFadeOut); ///< Fading out the game for the reset.
    STATE_FUNC_DECLARE(dScBoot_c, ResetFadeIn); ///< Fading back in after the reset.

    STATE_FUNC_DECLARE(dScBoot_c, FadeOutWait); ///< Waiting for fade out to complete.
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapKeyWait); ///< Forced delay before key press is accepted.
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapDispEndWait); ///< Waiting for key press to continue.
    STATE_FUNC_DECLARE(dScBoot_c, WiiStrapFadeOut); ///< The Wii strap screen is fading out.
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationFadeIn); ///< The controller information screen is fading in.
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationSoundWait); ///< Waiting for the scene sound to load.
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationKeyWait); ///< Forced delay before key press is accepted.
    STATE_FUNC_DECLARE(dScBoot_c, ControllerInformationDispEndWait); ///< Waiting for key press to continue.
    STATE_FUNC_DECLARE(dScBoot_c, NandCommandEndWait); ///< Waiting for the system to be ready for a NAND command.
    STATE_FUNC_DECLARE(dScBoot_c, ExistFileCheck); ///< Checking for existing save data.
    STATE_FUNC_DECLARE(dScBoot_c, NandSpaceCheck); ///< Checking for sufficient NAND space for the save data.
    STATE_FUNC_DECLARE(dScBoot_c, CreateFile); ///< Creating new save data.
    STATE_FUNC_DECLARE(dScBoot_c, Load); ///< Loading existing save data.
    STATE_FUNC_DECLARE(dScBoot_c, GoToErrorFadeOut); ///< Fading out to show an error message.
    STATE_FUNC_DECLARE(dScBoot_c, GoToErrorFadeIn); ///< Fading in the error message.
    STATE_FUNC_DECLARE(dScBoot_c, NewSaveFileDisp); ///< Animating in the "new save data created" message.
    STATE_FUNC_DECLARE(dScBoot_c, ButtonInputWait); ///< Waiting for button input after creating new save data.
    STATE_FUNC_DECLARE(dScBoot_c, WindowExitWait); ///< Waiting for the "save data created" window to close.
    STATE_FUNC_DECLARE(dScBoot_c, ProcEnd); ///< Going to the title screen.
    STATE_FUNC_DECLARE(dScBoot_c, ErrorWindowOnStageWait); ///< Showing an error message.
    STATE_FUNC_DECLARE(dScBoot_c, ErrorInfinityDisp); ///< Showing an irrecoverable error message.

    sFStateMgr_c<dScBoot_c, sStateMethodUsr_FI_c> mStateMgr; ///< The main state manager for the boot scene.
    sFStateMgr_c<dScBoot_c, sStateMethodUsr_FI_c> mResetStateMgr; ///< The state manager for the ResetXXX states.

    u8 mPad; ///< @unused
public:
    dWiiStrapScreen_c *mpWiiStrapScreen; ///< The Wii strap screen instance.
    dYesNoWindow_c *mpYesNoWindow; ///< The yes/no window instance.
    dSelectCursor_c *mpSelectCursor; ///< The select cursor instance.
    dControllerInformation_c *mpControllerInformation; ///< The controller information instance.

private:
    bool mNewSaveFileCreated; ///< Whether new save data was created during boot.
    bool mResetFaderDone; ///< Whether the reset fade-out has completed.
public:
    bool mSceneSoundLoaded; ///< Whether the sound (for the button press) has finished loading.
private:
    bool mIsResetting; ///< Whether the game is currently resetting.

    bool mResetFadeOutStarted; ///< Whether the reset fade-out has started.
    bool mSaveDataCreatedMessageShown; ///< Whether the "new save data created" message has been shown.
    int mAutoAdvanceTimer; ///< The number of frames after which the scene will automatically advance.
    int mMinWaitTimer; ///< The minimum number of frames to wait before a button may be pressed to advance the scene.

public:
    static dScBoot_c *m_instance; ///< The singleton instance of the boot scene.
};

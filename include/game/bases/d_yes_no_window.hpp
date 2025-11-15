#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/sLib/s_State.hpp>

/// @brief Displays a confirmation prompt.
/// @details This appears, for example, when the game asks the player if their progress should be saved.
/// @ingroup bases
class dYesNoWindow_c : public dBase_c {
public:

    /// @brief The prompt type.
    enum WINDOW_TYPE_e {
        SAVE_DATA_CREATED,
        SAVE,
        SAVED,
        EXIT_FREE_MODE,
        WATCH_UNLOCKED_HINT_MOVIE,
        SKIP_COURSE,
        WATCH_HINT_MOVIE,
        GOT_ALL_STAR_COINS,
        GOT_ALL_STAR_COINS_MENTION_W9,
        BOWSERJR_SINGLEPLAYER,
        RETURN_TITLE,
        BOWSERJR_MULTIPLAYER,
        RETURN_TO_MAP,
        BACK_TO_COURSE_SELECTION,
        GOT_ALL_STAR_COINS_ALL_WORLDS,
        QUICK_SAVE,
        QUICK_SAVED,
        ALL_TOAD_HOUSES_RETURNED,
        ITEM_INFO,
        COURSE_CLEAR_WITH_SUPER_GUIDE,
        SKIP_COURSE_CONFIRM,
        COMPLETED_EVERYTHING,
        PROGRESS_AUTOSAVE_AFTER_W9_UNLOCK,
        BLOCK_TRANSFORMATION,
        BLOCK_TRANSFORMATION_REVERSE,
        EXIT_COIN_BATTLE,
        RETURN_TO_MAP2,
        PEACH_CASTLE_HINT1,
        PEACH_CASTLE_HINT2,
        WINDOW_TYPE_COUNT
    };

    /// @brief The possible cursor positions.
    enum CURSOR_POS_e {
        POS_YES,
        POS_NO,
        POS_OK,
        POS_COUNT
    };

    /// @brief The sound effect types used for the window.
    enum SOUND_e {
        SOUND_CLOSE,
        SOUND_WAIT,
        SOUND_CURSOR_MOVE,
        SOUND_OPEN_ANIME,
        SOUND_COUNT
    };

    /// @brief The animation names used for the window.
    enum ANIM_NAME_e {
        inWindow,
        loopWindow,
        inYesNoButton,
        onYesNoButton,
        idleYesNoButton,
        hitYesNoButton,
        offYesNoButton,
        inBG,
        outBG,
        outWindow,
        ANIM_NAME_COUNT
    };

    /// @brief The animations used for the window.
    enum ANIMS_e {
        ANIM_IN_WINDOW,
        ANIM_IN_QUICK_SAVE,
        ANIM_LOOP_WINDOW,
        ANIM_LOOP_QUICK_SAVE,
        ANIM_IN_YES,
        ANIM_IN_NO,
        ANIM_IN_OK,
        ANIM_ON_YES,
        ANIM_ON_NO,
        ANIM_ON_OK,
        ANIM_IDLE_YES,
        ANIM_IDLE_NO,
        ANIM_IDLE_OK,
        ANIM_HIT_YES,
        ANIM_HIT_NO,
        ANIM_HIT_OK,
        ANIM_HIT_QUICK_SAVE,
        ANIM_OFF_YES,
        ANIM_OFF_NO,
        ANIM_OFF_OK,
        ANIM_IN_BG,
        ANIM_OUT_BG,
        ANIM_OUT_WINDOW,
        ANIM_COUNT
    };

    /// @brief The picture panes used for the window.
    enum P_PANE_e {
        P_yesBase_00,
        P_noBase_00,
        P_centerBase_00,
        P_COUNT
    };

    /// @brief The fixed text boxes used for the window.
    enum T_FIXED_PANE_e {
        T_center_00,
        T_center_01,
        T_otehonTextS_00,
        T_otehonText_00,
        T_FIXED_COUNT
    };

    /// @brief The text boxes used for the window.
    enum T_PANE_e {
        T_questionS_00,
        T_question_00,
        T_otehonTextS_01,
        T_otehonText_01,
        T_needCoinX_00,
        T_needCoin_00,
        T_yes_00,
        T_yes_01,
        T_no_00,
        T_no_01,
        T_COUNT
    };

    /// @brief The null panes used for the window.
    enum N_PANE_e {
        N_otehonText_00,
        N_saveIcon_00,
        N_COUNT
    };

    dYesNoWindow_c();
    virtual ~dYesNoWindow_c();
    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

private:
    /// @brief Fills out the window content according to the settings.
    /// @details This includes:
    /// - Filling out the text boxes
    /// - Setting the visibility of the buttons
    /// - Showing the quick save balloon if necessary
    void populateLayout();

    STATE_FUNC_DECLARE(dYesNoWindow_c, InitWait); ///< Initial state. Switches to OpenAnimeStartWait immediately.
    STATE_FUNC_DECLARE(dYesNoWindow_c, OpenAnimeEndWait); ///< Waiting for the window to finish the opening animation.
    STATE_FUNC_DECLARE(dYesNoWindow_c, ButtonOnStageAnimeEndWait); ///< Plays the button appearing animation and waits for it to finish.
    STATE_FUNC_DECLARE(dYesNoWindow_c, ButtonChangeAnimeEndWait); ///< Playing the button change animations.
    STATE_FUNC_DECLARE(dYesNoWindow_c, SelectWait); ///< Waiting for either a button change or button hit to occur.
    STATE_FUNC_DECLARE(dYesNoWindow_c, HitAnimeEndWait); ///< Waiting for the button to finish the hit animation.
    STATE_FUNC_DECLARE(dYesNoWindow_c, ClouseAnimeEndWait); ///< Waiting for the window to finish the closing animation.
    STATE_FUNC_DECLARE(dYesNoWindow_c, HitAnimeAfterWait); ///< Waiting in an infinite loop after the button was pressed.

    LytBase_c mLayout; ///< The layout for the window.

    /// @brief The state manager for the window.
    sFStateMgr_c<dYesNoWindow_c, sStateMethodUsr_FI_c> mStateMgr;

    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the window.
    nw4r::lyt::Picture *mpPictures[P_COUNT]; ///< The picture panes.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The text boxes.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes.

    int mCursorPos; ///< The current cursor position.
    int mPrevCursorPos; ///< The previous cursor position.

public:
    /// @brief The type of the window.
    /// @details This determines the text and the layout of the window.
    WINDOW_TYPE_e mType;

    /// @brief The number of star coins required to unlock a hint movie.
    /// @details This is only to be used with the WATCH_UNLOCKED_HINT_MOVIE type.
    int mStarCoinsRequired;

    bool mHasLoadedLayout; ///< Whether the layout has been loaded.
    bool mIsActive; ///< Whether the window is currently open.
    bool mHitButton; ///< Whether a button has been pressed.
    bool mIsAnimating; ///< Whether an animation is currently playing.
    bool mKeepOpen; ///< Whether the window should stay open after a button has been pressed.
    bool mCancel; ///< Whether the window has been cancelled (and closed).
    bool mHideBG; ///< Whether to hide the translucent background that fills the screen.
};

#pragma once
#include <dol/bases/d_base.hpp>
#include <dol/bases/d_lytbase.hpp>
#include <dol/sLib/s_State.hpp>

/// @brief Displays a yes/no window.
/// @details This appears, for example, in the pause menu or when you press + on the world map.
/// @ingroup bases
class dYesNoWindow_c : public dBase_c {
public:
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
        NUM_WINDOW_TYPES
    };

    enum CURSOR_POS_e {
        POS_YES,
        POS_NO,
        POS_OK
    };

    dYesNoWindow_c();
    virtual ~dYesNoWindow_c();
    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

private:
    /// @brief Fills in the layout of the window.
    /// @details This includes:
    /// - Filling out the text boxes
    /// - Setting the visibility of the buttons
    /// - Conditionally showing the quick save balloon
    void populateLayout();

    STATE_FUNC_DECLARE(dYesNoWindow_c, InitWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, OpenAnimeEndWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, ButtonOnStageAnimeEndWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, ButtonChangeAnimeEndWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, SelectWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, HitAnimeEndWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, ClouseAnimeEndWait);
    STATE_FUNC_DECLARE(dYesNoWindow_c, HitAnimeAfterWait);
    
    LytBase_c mLayout; ///< @brief The layout for the window.

    /// @brief The state manager for the window.
    sFStateMgr_c<dYesNoWindow_c, sStateMethodUsr_FI_c> mStateMgr;

    nw4r::lyt::Pane *mpRootPane; ///< @brief The root pane of the window.

    nw4r::lyt::Picture *P_yesBase_00,
                       *P_noBase_00,
                       *P_centerBase_00;

    /// @brief Gets the n-th picture pane.
    nw4r::lyt::Picture *getPicturePane(int n) { return (&P_yesBase_00)[n]; }

    LytTextBox_c *T_questionS_00,
                 *T_question_00,
                 *T_otehonTextS_01,
                 *T_otehonText_01,
                 *T_needCoinX_00,
                 *T_needCoin_00,
                 *T_yes_00,
                 *T_yes_01,
                 *T_no_00,
                 *T_no_01;

    nw4r::lyt::Pane *N_otehonText_00,
                    *N_saveIcon_00;

    int mCursorPos; ///< @brief The current cursor position.
    int mPrevCursorPos; ///< @brief The previous cursor position.

    /// @brief The type of the window.
    /// @details This determines the text and the layout of the window.
    WINDOW_TYPE_e mType;

public:
    /// @brief The number of star coins required to unlock a hint movie.
    /// @details This is only to be used with the WATCH_UNLOCKED_HINT_MOVIE type.
    int mStarCoinsRequired;

private:
    bool mHasLoadedLayout; ///< @brief Whether the layout has been loaded.
    bool mIsActive; ///< @brief Whether the window is currently open.

public:
    bool mHitButton; ///< @brief Whether a button has been pressed.
    bool mIsAnimating; ///< @brief Whether an animation is currently playing.
    bool mKeepOpen; ///< @brief Whether the window should stay open after a button has been pressed.
    bool mCancel; ///< @brief Whether the window has been cancelled (and closed).
    bool mHasBG; ///< @brief Whether the window has a picture background.
};

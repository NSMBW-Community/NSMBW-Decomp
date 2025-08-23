#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/sLib/s_State.hpp>

/// @brief Display a menu that appears when the game is paused inside a course.
/// @details Note that this is not responsible for the confirmation menu,
/// just for the two menu buttons "Continue" and "Exit".
/// @statetable
/// @ingroup bases
class Pausewindow_c : public dBase_c {
public:

    /// @brief The text boxes used in the layout.
    /// @unofficial
    enum T_PANE_e {
        T_tuzukeru_00, T_tuzukeru_01,
        T_modoru_00, T_modoru_01,
        T_worldNum_00,
        T_corseNum_00,
        T_corsePic_00,
        T_osusumeText_00,
        T_COUNT
    };

    /// @brief The null panes used in the layout.
    /// @unofficial
    enum N_PANE_e {
        N_worldText_00,
        N_osusumeText_00,
        N_multiText_00,
        N_COUNT
    };

    /// @brief The picture panes used in the layout.
    /// @unofficial
    enum P_PANE_e {
        P_SBBase_00,
        P_SBBase_02,
        P_shadowBlack,
        P_coin_00,
        P_COUNT
    };

    /// @brief The window panes used in the layout.
    /// @unofficial
    enum W_PANE_e {
        W_N_pauseMenu_00,
        W_COUNT
    };

    /// @brief The animation names used in the layout.
    /// @unofficial
    enum ANIM_NAME_e {
        inWindow,
        loopWindow,
        outWindow,
        onButton,
        idleButton,
        offButon,
        hitButton,
        ANIM_NAME_COUNT
    };

    /// @brief The animations used for the layout.
    /// @unofficial
    enum ANIM_e {
        ANIM_IN_WINDOW, ANIM_LOOP_WINDOW, ANIM_OUT_WINDOW,
        ANIM_ON_TUZUKU, ANIM_ON_MENU,
        ANIM_IDLE_TUZUKU, ANIM_IDLE_MENU,
        ANIM_OFF_TUZUKU, ANIM_OFF_MENU,
        ANIM_HIT_TUZUKU, ANIM_HIT_MENU,
        ANIM_COUNT
    };

    Pausewindow_c();
    virtual ~Pausewindow_c();
    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

private:
    void setWorldCourseWrite(); ///< Sets the world number and course number / icon.
    void TitleDispChkWrite(); ///< Sets the visibility of the panes based on the game mode.
    void SelectCursorSetup(); ///< Sets up the cursor for the buttons.

    STATE_FUNC_DECLARE(Pausewindow_c, InitWait); ///< Initial state. Switches to OpenAnimeEndWait immediately.
    STATE_FUNC_DECLARE(Pausewindow_c, OpenAnimeEndWait); ///< Waiting for the window to finish the opening animation.
    STATE_FUNC_DECLARE(Pausewindow_c, ButtonChangeAnimeEndWait); ///< Waiting for the button change animation to finish.
    STATE_FUNC_DECLARE(Pausewindow_c, PauseDisp); ///< Showing the pause window.
    STATE_FUNC_DECLARE(Pausewindow_c, HitAnimeEndWait); ///< Waiting for the button hit animation to finish and goes to ClouseAnimeEndWait after.
    STATE_FUNC_DECLARE(Pausewindow_c, ClouseAnimeEndWait); ///< Waiting for the close animation to finish. Goes back to InitWait.

    LytBase_c mLayout; ///< The layout for the window.

    /// @brief The state manager for the window.
    sFStateMgr_c<Pausewindow_c, sStateMethodUsr_FI_c> mStateMgr;

    nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.
    nw4r::lyt::Window *mpWindowPanes[W_COUNT]; ///< The window panes of the view.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The textboxes of the view.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.

public:
    int mNextButton; ///< The button to be activated next (when the cursor moves).
private:
    int mActiveButton; ///< The button currently active.
    int mColor; ///< The color of the window. Use a value from 0-3 (one per player).
    bool mHasLoadedLayout; ///< Whether the layout has been loaded.
    bool mIsActive; ///< Whether the window is currently open.

public:
    bool mClose; ///< Whether the window was closed via the back button.
    bool mButtonHit; ///< Whether one of the buttons was hit.
    bool mIsAnimating; ///< Whether an animation is currently playing.
    bool mToTitle; ///< Whether to show "Title Screen" instead of "Exit". [This seems to never be set in-game].

    static Pausewindow_c *m_instance; ///< The static instance of the pause window.
};

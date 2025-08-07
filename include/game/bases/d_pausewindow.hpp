#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/sLib/s_State.hpp>

/// @ingroup bases
class Pausewindow_c : public dBase_c {
public:

    /// @brief The text boxes used in the layout.
    /// @unofficial
    enum T_PANE_e {
        T_tuzukeru_00,
        T_tuzukeru_01,
        T_modoru_00,
        T_modoru_01,
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
    void setWorldCourseWrite();
    void TitleDispChkWrite();
    void SelectCursorSetup();

    STATE_FUNC_DECLARE(Pausewindow_c, InitWait); ///< @brief
    STATE_FUNC_DECLARE(Pausewindow_c, OpenAnimeEndWait); ///< @brief
    STATE_FUNC_DECLARE(Pausewindow_c, ButtonChangeAnimeEndWait); ///< @brief
    STATE_FUNC_DECLARE(Pausewindow_c, PauseDisp); ///< @brief
    STATE_FUNC_DECLARE(Pausewindow_c, HitAnimeEndWait); ///< @brief
    STATE_FUNC_DECLARE(Pausewindow_c, ClouseAnimeEndWait); ///< @brief

    LytBase_c mLayout; ///< @brief The layout for the window.

    /// @brief The state manager for the window.
    sFStateMgr_c<Pausewindow_c, sStateMethodUsr_FI_c> mStateMgr;

    nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.
    nw4r::lyt::Window *mpWindowPanes[W_COUNT]; ///< The window panes of the view.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The textboxes of the view.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.

private:
    int mCursor;
    int mCursor2;
    int mColor;
    bool mHasLoadedLayout; ///< @brief Whether the layout has been loaded.
    bool mIsActive; ///< @brief Whether the window is currently open.

public:
    bool m_292;
    bool m_293;
    bool m_294;
    bool m_295;

    static Pausewindow_c *m_instance; ///< The static instance of the pause window.
};

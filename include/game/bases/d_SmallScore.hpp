#pragma once
#include <game/bases/d_2d.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/lyt.h>

/**
 * @brief Displays a small popup score indicator.
 * @ingroup bases
 * @details The class handles the creation, animation, and rendering of a single small score popup,
 * which appears when performing actions such as stomping enemies, collecting coins, or earning 1-UPs.
 */
class dSmallScore_c {
public:

    /// @brief The possible states of the score popup.
    enum STATE_e {
        STATE_MAKE_START,
        STATE_UP_MOVE,
        STATE_DISP_WAIT,
        STATE_GOAL_DISP,
        STATE_NONE,
        STATE_COUNT = STATE_NONE,
    };

    /// @brief The different types of the small score popups.
    enum POPUP_TYPE_e {
        POPUP_TYPE_100, ///< The number 100.
        POPUP_TYPE_200, ///< The number 200.
        POPUP_TYPE_400, ///< The number 400.
        POPUP_TYPE_800, ///< The number 800.
        POPUP_TYPE_1000, ///< The number 1000.
        POPUP_TYPE_2000, ///< The number 2000.
        POPUP_TYPE_4000, ///< The number 4000.
        POPUP_TYPE_8000, ///< The number 8000.
        POPUP_TYPE_1, ///< The number 1, used for red coins and toad balloons.
        POPUP_TYPE_2, ///< The number 2, used for red coins and toad balloons.
        POPUP_TYPE_3, ///< The number 3, used for red coins and toad balloons.
        POPUP_TYPE_4, ///< The number 4, used for red coins and toad balloons.
        POPUP_TYPE_5, ///< The number 5, used for red coins and toad balloons.
        POPUP_TYPE_6, ///< The number 6, used for red coins and toad balloons.
        POPUP_TYPE_7, ///< The number 7, used for red coins and toad balloons.
        POPUP_TYPE_8, ///< The number 8, used for red coins and toad balloons.
        POPUP_TYPE_1UP, ///< The text "1-UP".
        POPUP_TYPE_2UP, ///< The text "2-UP".
        POPUP_TYPE_3UP, ///< The text "3-UP".
        POPUP_TYPE_4UP, ///< The text "4-UP".
        POPUP_TYPE_1UP_COLOR_CHANGE, ///< The text "1-UP", but changes color (see chgColor). Used when a 1-UP is awarded to multiple players.
        POPUP_TYPE_COIN_2, ///< [Coin icon]x2. Used in Coin Battle.
        POPUP_TYPE_COIN_3, ///< [Coin icon]x3. Used in Coin Battle.
        POPUP_TYPE_COIN_5, ///< [Coin icon]x5. Used in Coin Battle.
        POPUP_TYPE_COIN_10, ///< [Coin icon]x10. Used in Coin Battle.
        POPUP_TYPE_COIN_15, ///< [Coin icon]x15. Used in Coin Battle.
        POPUP_TYPE_COIN_20, ///< [Coin icon]x20. Used in Coin Battle.
        POPUP_TYPE_INVALID = -1 ///< Invalid popup type.
    };

    /// @brief The text boxes used for the layout.
    enum T_PANE_e {
        T_100_00,
        T_1000_00,
        T_red2_00,
        T_1UP_00,
        T_coin_x_00,
        T_coinPoint_00,
        T_COUNT
    };

    /// @brief The null panes used for the layout.
    enum N_PANE_e {
        N_coin_00,
        N_COUNT
    };

    typedef void (dSmallScore_c::*ProcFunc)();

    dSmallScore_c(); ///< Constructs a new score popup creator.
    virtual ~dSmallScore_c(); ///< Destroys the score popup creator.

    bool createLayout(d2d::ResAccMultLoader_c *res);
    void execute();
    void draw();
    bool doDelete();

    void setPlayer1upColor(int);
    void setPlayer1000Color(int);
    void setPlayer100Color(int);
    void chgColor();

    void setNormalOrBlueColor();

    void ScissorMaskSet();
    void BigSmallAnime();

    void MakeStart();
    void UpMove();
    void DispWait();
    void GoalScoreDisp();

    void PositionSet();
    void CreateSmallScore(const mVec3_c &, int, int);
    void PosSet(const mVec3_c &);

    void setClipScale(mVec2_c v) { mClipScale = v; }

    LytBase_c mLayout; ///< The layout for the score popup.
    mVec2_c mPos;
    mVec2_c mPosDelta;
    mVec2_c mPosDeceleration;
    mVec2_c mPosOffset;
    mVec2_c mScale;
    mVec2_c mClipScale;
    mVec2_c mAnimScale;
    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the layout.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The text boxes used for the layout.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes used for the layout.
    float mMaxHeight; ///< The maximum height the score popup can be displayed at.
    STATE_e mState; ///< The state the score popup is in.
    int mPopupType; ///< The popup type. Value is a POPUP_TYPE_e.
    int mDispWaitCounter; ///< The amount of frames the @p DispWait state has been active.
    int mDispWaitTime; ///< The number of frames to wait in the @p DispWait state.
    int mPlayerType;
    int mChgColorCounter; ///< Counter that is incremented every call to chgColor(), which ensures that the 1-UP color is only set once every 10 calls.
    u32 mPlayerColor;
    int mAnimCounter;
    u32 mCurTextbox; ///< The textbox currently displayed. Value is a T_PANE_e.
    bool mIsGoalScore;
    bool mInitialized; ///< Whether the layout has been initialized.
    bool mEnableColorChange;
    bool mEnableBigSmallAnim;
    bool mAnimIsShrinking;
    bool mHasBlueColor; ///< Whether counter type score popups should be colored blue instead of red.

    static dSmallScore_c *m_instance; ///< The instance of the score popup creator. @unused
};

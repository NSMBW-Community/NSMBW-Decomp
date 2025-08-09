#pragma once
#include <game/bases/d_2d.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/lyt.h>


class dSmallScore_c {
public:
    enum STATE_e {
        STATE_MAKE_START = 0,
        STATE_UP_MOVE = 1,
        STATE_DISP_WAIT = 2,
        STATE_GOAL_DISP = 3,
        STATE_NONE = 4
    };

    enum POPUP_TYPE_e {
        POPUP_TYPE_100,
        POPUP_TYPE_200,
        POPUP_TYPE_400,
        POPUP_TYPE_800,
        POPUP_TYPE_1000,
        POPUP_TYPE_2000,
        POPUP_TYPE_4000,
        POPUP_TYPE_8000,
        POPUP_TYPE_ONE,
        POPUP_TYPE_TWO,
        POPUP_TYPE_THREE,
        POPUP_TYPE_FOUR,
        POPUP_TYPE_FIVE,
        POPUP_TYPE_SIX,
        POPUP_TYPE_SEVEN,
        POPUP_TYPE_EIGHT,
        POPUP_TYPE_1UP,
        POPUP_TYPE_2UP,
        POPUP_TYPE_3UP,
        POPUP_TYPE_4UP,
        POPUP_TYPE_1UP_DUPE,
        POPUP_TYPE_COIN_2,
        POPUP_TYPE_COIN_3,
        POPUP_TYPE_COIN_5,
        POPUP_TYPE_COIN_10,
        POPUP_TYPE_COIN_15,
        POPUP_TYPE_COIN_20
    };

    typedef void (dSmallScore_c::*ProcFunc)();

    dSmallScore_c();
    virtual ~dSmallScore_c();

    bool createLayout(d2d::ResAccMultLoader_c *);
    void execute();
    void draw();
    void doDelete();

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

    LytBase_c mLayout;
    mVec2_c mPos;
    mVec2_c mPosDelta;
    mVec2_c mPosDeceleration;
    mVec2_c mPosOffset;
    mVec2_c mScale;
    mVec2_c mClipScale;
    mVec2_c mAnimScale;
    nw4r::lyt::Pane *mpRootPane;
    LytTextBox_c *T_100_00;
    LytTextBox_c *T_1000_00;
    LytTextBox_c *T_red2_00;
    LytTextBox_c *T_1UP_00;
    LytTextBox_c *T_coin_x_00;
    LytTextBox_c *T_coinPoint_00;
    nw4r::lyt::Pane *N_coin_00;
    float mMaxHeight;
    STATE_e mState; ///< Determines the state the score popup is in
    int mPopupType;
    int mDispWaitCounter; ///< Counter that is incremented every frame while in the 'DispWait' state
    int mDispWaitTime; ///< Number of frames to wait in 'DispWait' (unless mPlayerType is 4, in which case the default value is 60)
    int mPlayerType;
    int mChgColorCounter; ///< Counter that is incremented every call to dSmallScore_c::chgColor(), which ensures that the 1-up color is only set once every 10 calls.
    u32 mPlayerColor;
    int mAnimCounter;
    u32 mCurTextbox;
    bool mIsGoalScore;
    bool mInitialized;
    bool mEnableColorChange;
    bool mEnableBigSmallAnim;
    bool mAnimIsShrinking;
    bool mHasBlueColor;

    /// @brief Gets the n-th text box.
    LytTextBox_c *getTextBox(int n) { return (&T_100_00)[n]; }

    static dSmallScore_c *m_instance;

};

#pragma once
#include <game/bases/d_resource.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/nw4r/lyt/lyt_pane.hpp>


class dSmallScore_c {

public:
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
    nw4r::lyt::Pane * mpRootPane;
    LytTextBox_c * T_100_00;
    LytTextBox_c * T_1000_00;
    LytTextBox_c * T_red2_00;
    LytTextBox_c * T_1UP_00;
    LytTextBox_c * T_coin_x_00;
    LytTextBox_c * T_coinPoint_00;
    nw4r::lyt::Pane * N_coin_00;
    float mMaxHeight;
    int mState; ///< Determines the state - 0=MakeStart / 1=UpMove / 2=DispWait / 3=GoalScoreDisp / 4=None
    int mPopupType;
    int mDispWaitCounter; ///< Counter that is incremented every frame while in the 'DispWait' state
    int mDispWaitTime; ///< Number of frames to wait in 'DispWait' (unless mPlayerType is 4, in which case the default value is 60)
    int mPlayerType;
    int mChgColorCounter; ///< Counter that is incremented every call to dSmallScore_c::chgColor(), which ensures that the 1-up color is only set once every 10 calls.
    unsigned int mPlayerColour;
    int mAnimCounter;
    unsigned int mCurTextbox;
    bool mIsGoalScore;
    bool mInitialized;
    bool mEnableColorChange;
    bool mEnableBigSmallAnim;
    bool mAnimIsShrinking;
    bool mHasBlueColor;
    // unsigned char padding[2];

    /// @brief Gets the n-th text box.
    LytTextBox_c *getTextBox(int n) { return (&T_100_00)[n]; }

    static dSmallScore_c * m_instance;

};

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
    float _1a4; // mVec2 maybe
    float _1a8;
    unsigned char u_1ac[4];
    float _1b0;
    float _1b4;
    float _1b8;
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
    float _1f4;
    int _1f8;
    int _1fc;
    int _200;
    int _204;
    int _208;
    int _20c;
    unsigned int _210;
    int mAnimCounter;
    unsigned int _218;
    unsigned char _21c;
    bool mInitialized;
    unsigned char _21e;
    unsigned char _21f;
    bool mAnimIsShrinking;
    unsigned char _221;
    unsigned char u_222[2];

    /// @brief Gets the n-th text box.
    LytTextBox_c *getTextBox(int n) { return (&T_100_00)[n]; }

    static dSmallScore_c * m_instance;

};

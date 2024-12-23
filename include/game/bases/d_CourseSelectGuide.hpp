#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/sLib/s_State.hpp>
#include <game/sLib/s_StateMethodUsr_FI.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_info.hpp>

class dCourseSelectGuide_c {
public:
    dCourseSelectGuide_c();
    virtual ~dCourseSelectGuide_c();

private:
    bool createLayout();
    void ScissorMaskSet();
    void RestNumberDisp();
    void RestAlphaDisp();
    void execute();
    void draw();
    void doDelete();

    void CollectionCoinSet();

    void FUN_80010690();
    void ControllerConnectCheck();
    bool FUN_80010f40(int);
    void FUN_800125c0(short, dWmLib::CourseType_e);
    void FUN_80010b50(dWmLib::CourseType_e);

    void dostuff(u8 wNo, u8 cNo);

    u8 mUnk;

    LytBase_c mLayout; ///< @brief The layout for the window.

    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrWorldCourse;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrGuide;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrUp;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrDown;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrLeft;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrRight;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrShadow;
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrScrollGuide;

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, WorldCourseOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, WorldCourseOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, WorldCourseDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, WorldCourseExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, GuideOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, GuideOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, GuideDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, GuideExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, UpOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, UpOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, UpDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, UpExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, DownOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, DownOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, DownDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, DownExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, LeftOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, LeftOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, LeftDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, LeftExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, RightOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, RightOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, RightDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, RightExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ShadowOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ShadowOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ShadowDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ShadowExitAnimeEndCheck);

    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ScrollGuideOnStageWait);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ScrollGuideOnStageAnimeEndCheck);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ScrollGuideDisp);
    STATE_FUNC_DECLARE(dCourseSelectGuide_c, ScrollGuideExitAnimeEndCheck);

    nw4r::lyt::Pane *mpRootPane; ///< @brief The root pane of the window.

    nw4r::lyt::Pane *N_IconPos1P_00,
                    *N_IconPos2P_00,
                    *N_IconPos3P_00,
                    *N_IconPos4P_00,
                    *N_mapArrow_00,
                    *N_proportionL_00,
                    *N_proportionR_00,
                    *N_proportionC_00,
                    *N_guideViewC_00,
                    *N_guideViewR_01,
                    *N_left_00;

    LytTextBox_c *T_worldNum_00,
                 *T_cSelect_00,
                 *T_cSelect_pic,
                 *T_lifeNumber_00,
                 *T_lifeNumber_01,
                 *T_lifeNumber_02,
                 *T_lifeNumber_03,
                 *T_guideViewLS_00,
                 *T_guideViewL_01;

    nw4r::lyt::Picture *P_cC_1_00,
                       *P_cC_2_00,
                       *P_cC_3_00,
                       *P_cC_1s_00,
                       *P_cC_2s_00,
                       *P_cC_3s_00,
                       *P_flagSkull_00,
                       *P_marioFace_00,
                       *P_luigiFace_00,
                       *P_BkinoFace_00,
                       *P_YkinoFace_00,
                       *P_bgShadow_00;

    Remocon::EXTENSION_TYPE_e mExtension;
    int mWorldNo;
    int mCourseNo;
    int mCourseType;
    int mRest[4];
    int m424;
    int m428;
    int mAlpha;
    int mAlphaTarget;
    u8 m434;
    bool mInitialized;
    bool m436;
    bool m437;
    bool m438;
    bool m439;
    bool m43a;
    bool m43b;
    bool m43c;
    bool m43d;
    bool m43e;
    bool m43f;
    bool m440;
    bool m441;
    bool m442;
    bool m443;
    bool m444;
    bool mInScrollMode;
    bool m446;
    bool m447;
    bool m448;
    bool m449;
    bool m44a;
    bool m44b;
    bool m44c;
    bool m44d;
    bool mShowRestNumber;

    /// @brief Gets the n-th pane.
    nw4r::lyt::Pane *getPane(int n) { return (&N_IconPos1P_00)[n]; }

    /// @brief Gets the n-th picture pane.
    nw4r::lyt::Picture *getPicturePane(int n) { return (&P_cC_1_00)[n]; }

    /// @brief Gets the n-th text box.
    LytTextBox_c *getTextBox(int n) { return (&T_worldNum_00)[n]; }

public:
    static inline int getPaneNum(int i) {
        static const int paneNums[] = { 0, 1, 3, 2 };
        return paneNums[i];
    }

    void thing(u8 wNo, u8 cNo);

    inline void hideFlag() { P_flagSkull_00->setVisible(false); }
    inline void showFlag() { P_flagSkull_00->setVisible(true); }
    inline nw4r::lyt::Picture *getFlag() { return P_flagSkull_00; }

    static dCourseSelectGuide_c *m_instance;

    static const int c_DISP_WAIT_TIMER;

private:
    // [Some unused class - but needed here because it has a static initializer.]

    class NumberHolder {
    public:
        NumberHolder() {
            if (!msIsInit) {
                a = 20;
                b = 70;
                msIsInit = true;
            }
        }

        u16 a, b;
    };
    static NumberHolder msUnk;
    static char msIsInit;
};

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
#include <game/sLib/s_GlobalData.hpp>

class dCourseSelectGuide_c {

    enum T_PANE_e {
        T_worldNum_00,
        T_cSelect_00,
        T_cSelect_pic,
        T_lifeNumber_00,
        T_lifeNumber_01,
        T_lifeNumber_02,
        T_lifeNumber_03,
        T_guideViewLS_00,
        T_guideViewL_01,
        T_COUNT
    };

    enum N_PANE_e {
        N_IconPos1P_00,
        N_IconPos2P_00,
        N_IconPos3P_00,
        N_IconPos4P_00,
        N_mapArrow_00,
        N_proportionL_00,
        N_proportionR_00,
        N_proportionC_00,
        N_guideViewC_00,
        N_guideViewR_01,
        N_left_00,
        N_COUNT
    };

    enum P_PANE_e {
        P_cC_1_00,
        P_cC_2_00,
        P_cC_3_00,
        P_cC_1s_00,
        P_cC_2s_00,
        P_cC_3s_00,
        P_flagSkull_00,
        P_marioFace_00,
        P_luigiFace_00,
        P_BkinoFace_00,
        P_YkinoFace_00,
        P_bgShadow_00,
        P_COUNT
    };

    enum ANIM_NAME_e {
        inInfo,
        loopInfo,
        outInfo,
        inMapArrow,
        loopArrow,
        outMapArrow,
        inShadow,
        outShadow,
        ANIM_NAME_COUNT
    };

    /// @brief The animations used for the layout.
    enum ANIM_e {
        ANIM_IN_UNDER, ANIM_IN_ZANKI, ANIM_IN_COURSE_INFO,
        ANIM_LOOP_UNDER, ANIM_LOOP_ZANKI, ANIM_LOOP_COURSE_INFO,
        ANIM_OUT_UNDER, ANIM_OUT_ZANKI, ANIM_OUT_COURSE_INFO,
        ANIM_IN_ARROW_R, ANIM_IN_ARROW_L, ANIM_IN_ARROW_U, ANIM_IN_ARROW_D,
        ANIM_LOOP_ARROW_R, ANIM_LOOP_ARROW_L, ANIM_LOOP_ARROW_U, ANIM_LOOP_ARROW_D,
        ANIM_OUT_ARROW_R, ANIM_OUT_ARROW_L, ANIM_OUT_ARROW_U, ANIM_OUT_ARROW_D,
        ANIM_IN_SHADOW,
        ANIM_OUT_SHADOW,
    };

    struct GlobalData_t {
        short mDispWaitTimer, mHideAlpha;
    };

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

    void FUN_80010690(); ///< @unofficial
    void ControllerConnectCheck();
    void CourseSelectIconDisp(dWmLib::CourseType_e); ///< @unofficial
    bool FUN_80010f40(int); ///< @unofficial
    void FUN_800125c0(short, dWmLib::CourseType_e); ///< @unofficial

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

    nw4r::lyt::Pane *mpNullPanes[N_COUNT];
    LytTextBox_c *mpTextBoxes[T_COUNT];
    nw4r::lyt::Picture *mpPicturePanes[P_COUNT];

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

public:
    static inline int getPaneNum(int i) {
        static const int paneNums[] = {
            N_IconPos1P_00,
            N_IconPos2P_00,
            N_IconPos4P_00,
            N_IconPos3P_00
        };
        return paneNums[i];
    }

    static dCourseSelectGuide_c *m_instance;

    static const int c_DISP_WAIT_TIMER;

private:
    // [Some unused class - but needed here because it has a static initializer.]

};

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

/**
 * @brief The world map HUD.
 */
class dCourseSelectGuide_c {

    /// @brief The text boxes used in the layout.
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

    /// @brief The null panes used in the layout.
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

    /// @brief The picture panes used in the layout.
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

    /// @brief The animation names used in the layout.
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
        ANIM_COUNT
    };

    struct GlobalData_t {
        short mDispWaitTimer;
        short mUnk; ///< @unused
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

    void PlayerIconSet(); ///< @unofficial
    void ControllerConnectCheck();
    void CourseSelectSet(dWmLib::CourseType_e); ///< @unofficial
    bool IsDirectionAllowed(int); ///< @unofficial
    void UpdateGuide(short, dWmLib::CourseType_e); ///< @unofficial

    u8 mUnk; ///< @unused

    LytBase_c mLayout; ///< The layout for the window.

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

    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the window.

    nw4r::lyt::Pane *mpNullPanes[N_COUNT];
    LytTextBox_c *mpTextBoxes[T_COUNT];
    nw4r::lyt::Picture *mpPicturePanes[P_COUNT];

    Remocon::EXTENSION_TYPE_e mExtension;
    int mWorldNo;
    int mCourseNo; ///< The current course number. Set to a negative value if it's a course with an icon.
    int mCourseType;
    int mRest[4]; ///< The remaining lives for each player. Set to -1 if the player is not present.
    int mWorldCourseOnStageTimer;
    int mGuideOnStageTimer;
    int mAlpha;
    int mAlphaTarget;
    u8 mMoveDirection;
    bool mInitialized;
    bool mSkipOutCourseAnim;
    bool mSkipInCourseAnim;
    bool mIsExitingCourse;
    bool mNoAnimInCourseInfo;
    bool mBeginGuide;
    bool mEndGuide;
    bool mDisableGuide;
    bool mSkipGuideAnim;
    bool mFreeMove;
    bool mDisableArrows;
    bool mScrollEnabled;
    bool mEnabled;
    bool mHideHUD;
    bool mShowShadow;
    bool mStopShadowDisp;
    bool mInScrollMode;
    bool mShowScrollGuide;
    bool mEndScrollGuide;
    bool mUnderAnim;
    bool mCourseInfoAnim;
    bool mUpAnim;
    bool mDownAnim;
    bool mLeftAnim;
    bool mRightAnim;
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
};

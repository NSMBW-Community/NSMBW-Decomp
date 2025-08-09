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

/// @brief The world map HUD.
class dCourseSelectGuide_c {

    /// @brief The text boxes used in the layout.
    /// @unofficial
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
    /// @unofficial
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
    /// @unofficial
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
    /// @unofficial
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
    /// @unofficial
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

    /// @brief The fixed panes used in the layout.
    /// @unofficial
    enum T_FIXED_PANE_e {
        T_guideViewC_00,
        T_guideViewR_00,
        T_world_00,
        T_FIXED_COUNT
    };

public:
    /// @unofficial
    struct GlobalData_t {
        short mDispWaitTimer; ///< @copydoc dCourseSelectGuide_c::c_DISP_WAIT_TIMER
        short mUnk; ///< @unused
    };

    dCourseSelectGuide_c(); ///< Constructs a new course select guide instance.
    virtual ~dCourseSelectGuide_c(); ///< Destroys the instance.

private:
    bool createLayout(); ///< Creates the layout and initializes the class state.
    void ScissorMaskSet(); ///< Clips the layout to the maximum allowed size.
    void RestNumberDisp(); ///< Fills in the text boxes showing the remaining lives count.
    void RestAlphaDisp(); ///< Sets the opacity of the remaining lives count.
    void execute(); ///< The main loop of the view.
    void draw(); ///< Draws the view.
    void doDelete(); ///< Prepares the view for deletion.

    void CollectionCoinSet(); ///< Updates the star coin icons.

    /// @brief Updates the player icons in the remaining lives info to the active player.
    /// @unofficial
    void PlayerIconSet();

    /**
     * @brief Updates the textbox that displays the current course.
     * @param type The point type.
     *
     * @unofficial
     */
    void CourseSelectSet(dWmLib::PointType_e type);

    bool IsDirectionAllowed(int dir); ///< Returns whether the arrow for direction @p dir should be shown. @unofficial

    /**
     * @brief Updates the course-related parts of the course info guide.
     * @param courseNo The course number.
     * @param type The point type.
     *
     * The updated elements include the course name, the star coins collected and the checkpoint status.
     * @unofficial
     */
    void UpdateGuide(short courseNo, dWmLib::PointType_e type);

    /// @brief Checks for attached controller extensions and updates the button hints with the correct icon.
    /// @details The updated elements include the "View Map" and "Items" button legends.
    void ControllerConnectCheck();

    u32 mUnk; ///< @unused

    LytBase_c mLayout; ///< The layout for the window.

    /// @brief The state manager for the course info (world / course name).
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrWorldCourse;
    /// @brief The state manager for the button legends and remaining lives display.
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrGuide;
    /// @brief The state manager for the up arrow for the map view.
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrUp;
    /// @brief The state manager for the down arrow for the map view.
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrDown;
    /// @brief The state manager for the left arrow for the map view.
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrLeft;
    /// @brief The state manager for the right arrow for the map view.
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrRight;
    /// @brief The state manager for darkening the course view. [Unused?]
    sFStateMgr_c<dCourseSelectGuide_c, sStateMethodUsr_FI_c> mStateMgrShadow;
    /// @brief The state manager for the button legends during the map view.
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

    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the view.

    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The textboxes of the view.
    nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.

    Remocon::EXTENSION_TYPE_e mExtension; ///< The currently attached Wii remote extension.
    int mWorldNo; ///< The current world number.
    int mCourseNo; ///< The current course number. Set to a negative value if it's a course with an icon.
    int mCourseType; ///< The current course type. @todo Document this enum.

    /**
     * @brief The remaining lives for each player.
     *
     * Set to -1 if the player is not present. This is stored here so that the game doesn't have to
     * recalculate the textbox each frame.
     */
    int mRest[PLAYER_COUNT];

    int mWorldCourseOnStageTimer; ///< Cooldown for showing the course info.
    int mGuideOnStageTimer; ///< Cooldown for showing the remaining guide HUD.

    /**
     * @brief The current transparency for the player remaining lives HUD.
     *
     * This should be changed via mRestAlphaTarget, which will cause mRestAlpha to smoothly
     * transition to mRestAlphaTarget. @decompnote{This transition effect goes unused, because even
     * though stepping on Peach's castle performs this transition, it happens before the
     * remaining lives counter appears again on-screen.}
     */
    int mRestAlpha;
    int mRestAlphaTarget; ///< The target transparency for mRestAlpha.

    u8 mMoveDirection; ///< A bitmask describing which arrows should be shown in the free move view.

    bool mInitialized; ///< Whether the layout has been fully initialized.

    bool mShowCourseInfo; ///< Set this to @p true to show the course info.
    bool mHideCourseInfo; ///< Set this to @p true to hide the course info.
    bool mIsCourseInfoOutAnime; ///< Automatically set to @p false after the course info has been hidden.
    bool mNoAnimCourseInfoIn; ///< Whether the course info should simply appear instead of sliding in.

    bool mBeginGuide; ///< Set this to @p true to show the remaining guide.
    bool mEndGuide; ///< Set this to @p true to hide the remaining guide.
    bool mGuideRelated; ///< @todo Figure out what this does.
    bool mNoHUDAppearAnim; ///< Whether the HUD should be shown without an animation.

    bool mEnableAllArrows; ///< Whether to show all map view arrows. Gets reset after 1 frame.
    bool mDisableArrows; ///< Whether to stop updating the map view arrows.
    bool mMapView; ///< Whether the HUD should process the map view elements.

    bool mEnabled; ///< Controls whether the HUD is updated every frame.
    bool mHideHUD; ///< Set this to true to hide the entire HUD.

    bool mShowShadow; ///< Set this to @p true to darken the world map view.
    bool mHideShadow; ///< Set this to @p true to remove the darkening of the world map view.

    bool mInMapView; ///< Whether the HUD is in the map view mode.

    bool mShowScrollGuide; ///< Set this to @p true to show the button legends for the map view mode.
    bool mEndScrollGuide; ///< Set this to @p true to hide the button legends for the map view mode.
    bool mScrollGuideRelated; ///< @todo Figure out what this does.

    bool mCourseInfoAnim; ///< Whether the course information is in an animation.
    bool mUpAnim; ///< Whether the up arrow is in an animation.
    bool mDownAnim; ///< Whether the down arrow is in an animation.
    bool mLeftAnim; ///< Whether the left arrow is in an animation.
    bool mRightAnim; ///< Whether the right arrow is in an animation.

    bool mShowRestNumber; ///< Whether to update the remaining lives count.

public:
    static dCourseSelectGuide_c *m_instance; ///< The instance of the view.

    /// @brief The speed at which the opacity of the remaining lives counter is changed.
    static const int c_DISP_WAIT_TIMER;
};

#include <game/bases/d_CourseSelectGuide.hpp>
#include <game/bases/d_a_player_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_game_display.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/mLib/m_video.hpp>
#include <constants/message_list.h>

dCourseSelectGuide_c *dCourseSelectGuide_c::m_instance;

template <>
dCourseSelectGuide_c::GlobalData_t sGlobalData_c<dCourseSelectGuide_c>::mData = {
    dCourseSelectGuide_c::c_DISP_WAIT_TIMER, dCourseSelectGuide_c::c_DISP_WAIT_TIMER + 50
};

const int dCourseSelectGuide_c::c_DISP_WAIT_TIMER = 20;

dCourseSelectGuide_c::dCourseSelectGuide_c() :
    mStateMgrWorldCourse(*this, StateID_WorldCourseOnStageWait),
    mStateMgrGuide(*this, StateID_GuideOnStageWait),
    mStateMgrUp(*this, StateID_UpOnStageWait),
    mStateMgrDown(*this, StateID_DownOnStageWait),
    mStateMgrLeft(*this, StateID_LeftOnStageWait),
    mStateMgrRight(*this, StateID_RightOnStageWait),
    mStateMgrShadow(*this, StateID_ShadowOnStageWait),
    mStateMgrScrollGuide(*this, StateID_ScrollGuideOnStageWait)
{
    mVec3_c dummy; // [Unused - generate a constructor for mVec3_c in this TU].
    mInitialized = false;
    m_instance = this;
}

STATE_DEFINE(dCourseSelectGuide_c, WorldCourseOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, WorldCourseOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, WorldCourseDisp);
STATE_DEFINE(dCourseSelectGuide_c, WorldCourseExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, GuideOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, GuideOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, GuideDisp);
STATE_DEFINE(dCourseSelectGuide_c, GuideExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, UpOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, UpOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, UpDisp);
STATE_DEFINE(dCourseSelectGuide_c, UpExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, DownOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, DownOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, DownDisp);
STATE_DEFINE(dCourseSelectGuide_c, DownExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, LeftOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, LeftOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, LeftDisp);
STATE_DEFINE(dCourseSelectGuide_c, LeftExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, RightOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, RightOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, RightDisp);
STATE_DEFINE(dCourseSelectGuide_c, RightExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, ShadowOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, ShadowOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, ShadowDisp);
STATE_DEFINE(dCourseSelectGuide_c, ShadowExitAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, ScrollGuideOnStageWait);
STATE_DEFINE(dCourseSelectGuide_c, ScrollGuideOnStageAnimeEndCheck);
STATE_DEFINE(dCourseSelectGuide_c, ScrollGuideDisp);
STATE_DEFINE(dCourseSelectGuide_c, ScrollGuideExitAnimeEndCheck);

dCourseSelectGuide_c::~dCourseSelectGuide_c() {
    m_instance = nullptr;
}

bool dCourseSelectGuide_c::createLayout() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "corseSelectUIGuide_37_inInfo.brlan",
        "corseSelectUIGuide_37_loopInfo.brlan",
        "corseSelectUIGuide_37_outInfo.brlan",
        "corseSelectUIGuide_37_inMapArrow.brlan",
        "corseSelectUIGuide_37_loopArrow.brlan",
        "corseSelectUIGuide_37_outMapArrow.brlan",
        "corseSelectUIGuide_37_inShadow.brlan",
        "corseSelectUIGuide_37_outShadow.brlan"
    };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = {
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "C00_shadow",
        "C00_shadow"
    };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = {
        inInfo, inInfo, inInfo,
        loopInfo, loopInfo, loopInfo,
        outInfo, outInfo, outInfo,
        inMapArrow, inMapArrow, inMapArrow, inMapArrow,
        loopArrow, loopArrow, loopArrow, loopArrow,
        outMapArrow, outMapArrow, outMapArrow, outMapArrow,
        inShadow,
        outShadow
    };

    static const char *NPANE_NAME_DT[N_COUNT] = {
        "N_IconPos1P_00", "N_IconPos2P_00", "N_IconPos3P_00", "N_IconPos4P_00",
        "N_mapArrow_00",
        "N_proportionL_00", "N_proportionR_00", "N_proportionC_00",
        "N_guideViewC_00", "N_guideViewR_01",
        "N_left_00"
    };

    static const char *T_PANE_NAME_TBL[T_COUNT] = {
        "T_worldNum_00",
        "T_cSelect_00", "T_cSelect_pic",
        "T_lifeNumber_00", "T_lifeNumber_01", "T_lifeNumber_02", "T_lifeNumber_03",
        "T_guideViewLS_00", "T_guideViewL_01"
    };

    static const int MESSAGE_DATA_TBL[T_FIXED_COUNT] = {
        MSG_CS_SELECT_WORLD,
        MSG_CS_MENU,
        MSG_CS_WORLD
    };
    static const char *T_PANE_FIXED_NAME_TBL[T_FIXED_COUNT] = {
        "T_guideViewC_00",
        "T_guideViewR_00",
        "T_world_00"
    };

    static const char *P_PANE_NAME_TBL[P_COUNT] = {
        "P_cC_1_00",
        "P_cC_2_00",
        "P_cC_3_00",
        "P_cC_1s_00",
        "P_cC_2s_00",
        "P_cC_3s_00",
        "P_flagSkull_00",
        "P_marioFace_00",
        "P_luigiFace_00",
        "P_BkinoFace_00",
        "P_YkinoFace_00",
        "P_bgShadow_00"
    };

    if (mInitialized) {
        return true;
    }
    if (!mLayout.ReadResource("corseSelectUIGuide/corseSelectUIGuide.arc", false)) {
        return false;
    }
    mLayout.build("corseSelectUIGuide_37.brlyt", 0);
    mLayout.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mpRootPane = mLayout.getRootPane();
    mLayout.NPaneRegister(NPANE_NAME_DT, &mpNullPanes[N_IconPos1P_00], N_COUNT);
    mLayout.TPaneRegister(T_PANE_NAME_TBL, &mpTextBoxes[T_worldNum_00], T_COUNT);
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL, MESSAGE_DATA_TBL, BMG_CATEGORY_COURSE_SELECT_GUIDE, T_FIXED_COUNT);
    mLayout.PPaneRegister(P_PANE_NAME_TBL, &mpPicturePanes[P_cC_1_00], P_COUNT);

    mLayout.mDrawOrder = 2;
    mpRootPane->SetVisible(true);
    mpNullPanes[N_mapArrow_00]->SetVisible(true);
    mpPicturePanes[P_flagSkull_00]->SetVisible(false);

    for (int i = 0; i < ARRAY_SIZE(mRest); i++) {
        mRest[i] = -1;
    }

    RestNumberDisp();

    mExtension = Remocon::EXTENSION_UNKNOWN;
    mInitialized = true;

    mWorldNo = dScWMap_c::getWorldNo();
    mCourseNo = -10;
    mCourseType = 11;

    mLayout.AllAnimeEndSetup();

    mEnabled = true;

    mShowCourseInfo = false;
    mHideCourseInfo = true;

    mIsCourseInfoOutAnime = false;
    mNoAnimCourseInfoIn = false;

    mBeginGuide = false;
    mEndGuide = true;
    mGuideRelated = false;
    mNoHUDAppearAnim = false;

    mMapView = false;
    mDisableArrows = false;
    mHideHUD = false;

    mShowShadow = false;
    mHideShadow = false;

    mShowScrollGuide = false;
    mEndScrollGuide = false;

    mScrollGuideRelated = false;
    mCourseInfoAnim = false;
    mUpAnim = false;
    mDownAnim = false;
    mLeftAnim = false;
    mRightAnim = false;

    mShowRestNumber = false;

    mWorldCourseOnStageTimer = 0;
    mGuideOnStageTimer = 0;

    mRestAlpha = 255;
    mRestAlphaTarget = 255;

    mLayout.ReverseAnimeStartSetup(ANIM_OUT_UNDER, false);
    mLayout.ReverseAnimeStartSetup(ANIM_OUT_ZANKI, false);
    mLayout.ReverseAnimeStartSetup(ANIM_OUT_COURSE_INFO, false);
    mLayout.AnimePlay();
    mLayout.calc();
    PlayerIconSet();
    mLayout.ReverseAnimeStartSetup(ANIM_IN_SHADOW, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_UNDER, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_ZANKI, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_COURSE_INFO, false);
    mLayout.AnimePlay();
    mLayout.calc();

    mVec2_c scale;
    dGameCom::DispSizeScale(scale);
    mpNullPanes[N_proportionL_00]->SetScale(scale);
    mpNullPanes[N_proportionR_00]->SetScale(scale);
    mpNullPanes[N_proportionC_00]->SetScale(scale);

    ScissorMaskSet();

    return true;
}

void dCourseSelectGuide_c::ScissorMaskSet() {
    d2d::ClipSettings clipSettings;
    if (dGameCom::GetAspectRatio() == 0) {
        clipSettings.mPos.y = mVideo::getSmth(330.0f);
        clipSettings.mSize.x = 640.0f;
        clipSettings.mSize.y = 330.0f;
        clipSettings.mEnabled = true;
    }
    mLayout.mClipSettings = clipSettings;
}

void dCourseSelectGuide_c::PlayerIconSet() {
    static const int picPaneNums[PLAYER_COUNT] = { P_marioFace_00, P_luigiFace_00, P_BkinoFace_00, P_YkinoFace_00 };

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mpPicturePanes[P_marioFace_00 + i]->SetVisible(false);
    }

    int currPane = N_IconPos1P_00;
    int playerCount = -1;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        PLAYER_CHARACTER_e playerType = daPyCom_c::sc_PLAYER_ORDER[i];
        int playerIndex = daPyMng_c::getPlayerIndex(playerType);
        if (dGameCom::PlayerEnterCheck(playerIndex)) {
            mVec3_c translate = mpNullPanes[currPane]->GetTranslate();
            mpPicturePanes[picPaneNums[playerType]]->SetTranslate(translate);
            mpPicturePanes[picPaneNums[playerType]]->SetVisible(true);
            currPane++;
            playerCount++;
        }
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mpNullPanes[N_IconPos1P_00 + i]->SetVisible(false);
    }
    mpNullPanes[N_IconPos1P_00 + playerCount]->SetVisible(true);
}

void dCourseSelectGuide_c::RestNumberDisp() {
    static const int textBoxIdxs[PLAYER_COUNT] = { T_lifeNumber_00, T_lifeNumber_01, T_lifeNumber_03, T_lifeNumber_02 };

    for (int i = 0; i < PLAYER_COUNT; i++) {
        PLAYER_CHARACTER_e playerType = daPyCom_c::sc_PLAYER_ORDER[i];
        int playerIndex = daPyMng_c::getPlayerIndex(playerType);
        int rest = daPyMng_c::getRest(playerType);
        if (mRest[i] != rest) {
            mRest[i] = rest;
            int textIdx = textBoxIdxs[i];
            LytTextBox_c *textBox = mpTextBoxes[textIdx];
            dGameCom::LayoutDispNumber(rest, dGameDisplay_c::c_PLAYNUM_DIGIT, textBox, true);
        }
    }
    mShowRestNumber = false;
}

void dCourseSelectGuide_c::RestAlphaDisp() {
    if (mRestAlpha == mRestAlphaTarget) {
        return;
    }
    if (mRestAlphaTarget != 0) {
        mRestAlpha += c_DISP_WAIT_TIMER;
        if (mRestAlpha >= 255) {
            mRestAlpha = 255;
        }
    } else {
        mRestAlpha -= c_DISP_WAIT_TIMER;
        if (mRestAlpha < 0) {
            mRestAlpha = 0;
        }
    }

    u8 alpha = mRestAlpha;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        mpNullPanes[N_IconPos1P_00 + i]->SetAlpha(alpha);
        mpPicturePanes[P_marioFace_00 + i]->SetAlpha(alpha);
    }
}

void dCourseSelectGuide_c::execute() {
    if (mInitialized && mEnabled) {
        if (mShowRestNumber) {
            RestNumberDisp();
        }

        mStateMgrWorldCourse.executeState();
        mStateMgrGuide.executeState();

        if (mMapView) {
            mStateMgrUp.executeState();
            mStateMgrDown.executeState();
            mStateMgrLeft.executeState();
            mStateMgrRight.executeState();

            mMoveDirection = 0;
            if (mEnableAllArrows) {
                mEnableAllArrows = false;
            }
            if (mDisableArrows) {
                if (
                    !mLayout.isAnime(ANIM_OUT_ARROW_U) || !mLayout.isAnime(ANIM_OUT_ARROW_D) ||
                    !mLayout.isAnime(ANIM_OUT_ARROW_L) || !mLayout.isAnime(ANIM_OUT_ARROW_R)
                ) {
                    mDisableArrows = false;
                    mMapView = false;
                }
            }
        }

        mStateMgrScrollGuide.executeState();
        mStateMgrShadow.executeState();
        mLayout.AnimePlay();
        ControllerConnectCheck();
        RestAlphaDisp();
        mLayout.calc();
    }
}

void dCourseSelectGuide_c::draw() {
    if (mInitialized && mEnabled) {
        mLayout.entry();
    }
}

void dCourseSelectGuide_c::doDelete() {
    mLayout.doDelete();
}

void dCourseSelectGuide_c::CourseSelectSet(dWmLib::PointType_e type) {
    static const int sc_startPointIcons[WORLD_USED_COUNT] = {
        MSG_CS_ICON_START_RIGHT, // W1
        MSG_CS_ICON_START_UP,    // W2
        MSG_CS_ICON_START_RIGHT, // W3
        MSG_CS_ICON_START_UP,    // W4
        MSG_CS_ICON_START_RIGHT, // W5
        MSG_CS_ICON_START_UP,    // W6
        MSG_CS_ICON_START_RIGHT, // W7
        MSG_CS_ICON_START_UP,    // W8
        MSG_CS_ICON_START_UP,    // W9
    };
    dInfo_c *info = dInfo_c::m_instance;
    MsgRes_c *msgRes = dMessage_c::getMesRes();
    int actualWNo = mWorldNo + 1;
    int actualLevelNo;
    if (type == dWmLib::POINT_TYPE_PATH) {
        mCourseNo = -2;
        return;
    } else if (type == dWmLib::POINT_TYPE_REGULAR_COURSE || type == dWmLib::POINT_TYPE_OTHER) {
        actualLevelNo = mCourseNo + 1;
    } else if (type == dWmLib::POINT_TYPE_START_NODE) {
        mCourseType = dWmLib::COURSE_TYPE_INVALID; /// [Why not use COURSE_TYPE_KINOKO_START here?]
    } else {
        mCourseType = dWmLib::COURSE_TYPE_JUNCTION;
    }
    info->field_3b4 = actualWNo;
    mpTextBoxes[T_worldNum_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_CURR_WORLD, 0);
    mRestAlphaTarget = 255;
    int messageID;
    switch (mCourseType) {
        default:
            info->field_3b8 = actualLevelNo;
            mpTextBoxes[T_cSelect_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_COURSE_NUM, 0);
            mpTextBoxes[T_cSelect_00]->SetVisible(true);
            mpTextBoxes[T_cSelect_pic]->SetVisible(false);
            return;
        case dWmLib::COURSE_TYPE_GHOST: messageID = MSG_CS_ICON_GHOST_HOUSE; break;
        case dWmLib::COURSE_TYPE_TOWER: messageID = MSG_CS_ICON_TOWER; break;
        case dWmLib::COURSE_TYPE_CASTLE:
            if (mWorldNo != WORLD_8) {
                messageID = MSG_CS_ICON_CASTLE;
            } else {
                messageID = MSG_CS_ICON_W8_CASTLE;
            }
            break;
        case dWmLib::COURSE_TYPE_KINOKO:
            if (dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_1UP)) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_1UP;
            } else if (dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_STAR)) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_STAR;
            } else {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_RED;
            }
            break;
        case dWmLib::COURSE_TYPE_JUNCTION: messageID = MSG_CS_ICON_JUNCTION; break;
        case dWmLib::COURSE_TYPE_CANNON: messageID = MSG_CS_ICON_CANNON; break;
        case dWmLib::COURSE_TYPE_KOOPA_SHIP:
            if (dWmLib::isKoopaShipAnchor()) {
                messageID = MSG_CS_ICON_ANCHOR;
            } else {
                messageID = MSG_CS_ICON_AIRSHIP;
            }
            break;
        case dWmLib::COURSE_TYPE_PEACH_CASTLE:
            mRestAlphaTarget = 0;
            messageID = MSG_CS_ICON_PEACH_CASTLE;
            break;
        case dWmLib::COURSE_TYPE_INVALID:
            if (dWmLib::getStartPointKinokoHouseKindNum() == 0 || dWmLib::IsCourseClear(mWorldNo, STAGE_START_KINOKO_HOUSE)) {
                messageID = sc_startPointIcons[mWorldNo];
                break;
            }
            if (dWmLib::isStartPointKinokoHouseStar()) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_STAR;
            } else if (dWmLib::isStartPointKinokoHouseRed()) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_RED;
            } else {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_1UP;
            }
            break;
    }
    mpTextBoxes[T_cSelect_pic]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, messageID, 0);
    mpTextBoxes[T_cSelect_00]->SetVisible(false);
    mpTextBoxes[T_cSelect_pic]->SetVisible(true);
}

void dCourseSelectGuide_c::CollectionCoinSet() {
    dMj2dGame_c *saveGame = dSaveMng_c::m_instance->getSaveGame(-1);
    dCyuukan_c *checkpoint = dInfo_c::m_instance->getCyuukan();
    for (unsigned int i = 0; i < STAR_COIN_COUNT; i++) {
        mpPicturePanes[P_cC_1s_00 + i]->SetVisible(true);
        mpPicturePanes[P_cC_1s_00 + i]->SetAlpha(255);
        mpPicturePanes[P_cC_1_00 + i]->SetAlpha(255);
        if (mCourseNo < 0 || !dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_NO_STAR_COINS)) {
            mpPicturePanes[P_cC_1s_00 + i]->SetVisible(false);
        } else {
            u8 collectCoin = saveGame->isCollectCoin(mWorldNo, mCourseNo, i);
            if (collectCoin != 0) {
                mpPicturePanes[P_cC_1_00 + i]->SetVisible(true);
            } else if (
                mWorldNo == checkpoint->mWorldNo
                && mCourseNo == checkpoint->mCourseNo
                && checkpoint->mCoinCollection[i] != 4
            ) {
                mpPicturePanes[P_cC_1s_00 + i]->SetAlpha(0);
                mpPicturePanes[P_cC_1_00 + i]->SetVisible(true);
                mpPicturePanes[P_cC_1_00 + i]->SetAlpha(70);
            } else {
                mpPicturePanes[P_cC_1_00 + i]->SetVisible(false);
            }
        }
    }
}

bool dCourseSelectGuide_c::IsDirectionAllowed(int dir) {
    static const u8 flags[] = {
        BIT_FLAG(DIR_UP),
        BIT_FLAG(DIR_DOWN),
        BIT_FLAG(DIR_LEFT),
        BIT_FLAG(DIR_RIGHT)
    };
    return (mMoveDirection & flags[dir]) != 0;
}

void dCourseSelectGuide_c::initializeState_WorldCourseOnStageWait() {}
void dCourseSelectGuide_c::executeState_WorldCourseOnStageWait() {
    if (mScrollGuideRelated || mHideHUD || mHideCourseInfo) {
        return;
    }

    if (mWorldCourseOnStageTimer > 0) {
        mWorldCourseOnStageTimer--;
        return;
    }
    mWorldCourseOnStageTimer = 0;

    if (mShowCourseInfo) {
        mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseOnStageWait() {}

void dCourseSelectGuide_c::initializeState_WorldCourseOnStageAnimeEndCheck() {
    mCourseInfoAnim = true;
    mShowCourseInfo = false;
    mLayout.AnimeStartSetup(ANIM_IN_COURSE_INFO, mNoAnimCourseInfoIn);
}
void dCourseSelectGuide_c::executeState_WorldCourseOnStageAnimeEndCheck() {
    if (mHideCourseInfo) {
        mLayout.AnimeEndSetup(ANIM_IN_COURSE_INFO);
        mStateMgrWorldCourse.changeState(StateID_WorldCourseExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_COURSE_INFO)) {
        mStateMgrWorldCourse.changeState(StateID_WorldCourseDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseOnStageAnimeEndCheck() {
    mCourseInfoAnim = false;
}

void dCourseSelectGuide_c::initializeState_WorldCourseDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_COURSE_INFO);
}
void dCourseSelectGuide_c::executeState_WorldCourseDisp() {
    if (mHideHUD) {
        return;
    }
    mShowCourseInfo = false;
    if (mHideCourseInfo) {
        mStateMgrWorldCourse.changeState(StateID_WorldCourseExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseDisp() {}

void dCourseSelectGuide_c::initializeState_WorldCourseExitAnimeEndCheck() {
    mCourseInfoAnim = true;
    mHideCourseInfo = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_COURSE_INFO);
    mLayout.AnimeStartSetup(ANIM_OUT_COURSE_INFO, false);
}
void dCourseSelectGuide_c::executeState_WorldCourseExitAnimeEndCheck() {
    if (mShowCourseInfo) {
        mLayout.AnimeEndSetup(ANIM_OUT_COURSE_INFO);
        mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_COURSE_INFO)) {
        mIsCourseInfoOutAnime = false;
        mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseExitAnimeEndCheck() {
    mCourseInfoAnim = false;
}

void dCourseSelectGuide_c::initializeState_GuideOnStageWait() {}
void dCourseSelectGuide_c::executeState_GuideOnStageWait() {
    if (mScrollGuideRelated || mHideHUD || mEndGuide) {
        return;
    }

    if (mGuideOnStageTimer > 0) {
        mGuideOnStageTimer--;
        return;
    }
    mGuideOnStageTimer = 0;

    if (mBeginGuide) {
        mStateMgrGuide.changeState(StateID_GuideOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_GuideOnStageWait() {
    PlayerIconSet();
}

void dCourseSelectGuide_c::initializeState_GuideOnStageAnimeEndCheck() {
    mBeginGuide = false;
    mLayout.AnimeStartSetup(ANIM_IN_UNDER, mNoHUDAppearAnim);
    mLayout.AnimeStartSetup(ANIM_IN_ZANKI, mNoHUDAppearAnim);
}
void dCourseSelectGuide_c::executeState_GuideOnStageAnimeEndCheck() {
    if (mEndGuide) {
        mLayout.AnimeEndSetup(ANIM_IN_UNDER);
        mLayout.AnimeEndSetup(ANIM_IN_ZANKI);
        mStateMgrGuide.changeState(StateID_GuideExitAnimeEndCheck);
    } else {
        if (mLayout.isAnime(ANIM_IN_UNDER) || mLayout.isAnime(ANIM_IN_ZANKI)) {
            return;
        }
        mStateMgrGuide.changeState(StateID_GuideDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_GuideOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_GuideDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_UNDER);
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ZANKI);
}
void dCourseSelectGuide_c::executeState_GuideDisp() {
    if (!mHideHUD) {
        mBeginGuide = false;
        if (mEndGuide) {
            mStateMgrGuide.changeState(StateID_GuideExitAnimeEndCheck);
        }
    }
}
void dCourseSelectGuide_c::finalizeState_GuideDisp() {}

void dCourseSelectGuide_c::initializeState_GuideExitAnimeEndCheck() {
    mEndGuide = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_UNDER);
    mLayout.AnimeEndSetup(ANIM_LOOP_ZANKI);
    mLayout.AnimeStartSetup(ANIM_OUT_UNDER, false);
    mLayout.AnimeStartSetup(ANIM_OUT_ZANKI, false);
}
void dCourseSelectGuide_c::executeState_GuideExitAnimeEndCheck() {
    if (mBeginGuide) {
        mLayout.AnimeEndSetup(ANIM_OUT_UNDER);
        mLayout.AnimeEndSetup(ANIM_OUT_ZANKI);
        mStateMgrGuide.changeState(StateID_GuideOnStageAnimeEndCheck);
    } else {
        if (mLayout.isAnime(ANIM_OUT_UNDER) || mLayout.isAnime(ANIM_OUT_ZANKI)) {
            return;
        }
        mGuideRelated = false;
        mStateMgrGuide.changeState(StateID_GuideOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_GuideExitAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_UpOnStageWait() {}
void dCourseSelectGuide_c::executeState_UpOnStageWait() {
    if (!mDisableArrows && (IsDirectionAllowed(DIR_UP) || mEnableAllArrows)) {
        mStateMgrUp.changeState(StateID_UpOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_UpOnStageWait() {}

void dCourseSelectGuide_c::initializeState_UpOnStageAnimeEndCheck() {
    mUpAnim = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_U, false);
}
void dCourseSelectGuide_c::executeState_UpOnStageAnimeEndCheck() {
    if (!IsDirectionAllowed(DIR_UP) || mDisableArrows) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_U);
        mStateMgrUp.changeState(StateID_UpExitAnimeEndCheck);
    } else {
        if (mLayout.isAnime(ANIM_IN_ARROW_U) || mLayout.isAnime(ANIM_IN_UNDER)) {
            return;
        }
        mStateMgrUp.changeState(StateID_UpDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_UpOnStageAnimeEndCheck() {
    mUpAnim = false;
}

void dCourseSelectGuide_c::initializeState_UpDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_U);
}
void dCourseSelectGuide_c::executeState_UpDisp() {
    if (!IsDirectionAllowed(DIR_UP) || mDisableArrows) {
        mStateMgrUp.changeState(StateID_UpExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_UpDisp() {}

void dCourseSelectGuide_c::initializeState_UpExitAnimeEndCheck() {
    mUpAnim = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_U);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_U, false);
}
void dCourseSelectGuide_c::executeState_UpExitAnimeEndCheck() {
    if (IsDirectionAllowed(DIR_UP)) {
        mStateMgrUp.changeState(StateID_UpOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_U)) {
        mStateMgrUp.changeState(StateID_UpOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_UpExitAnimeEndCheck() {
    mUpAnim = false;
}

void dCourseSelectGuide_c::initializeState_DownOnStageWait() {}
void dCourseSelectGuide_c::executeState_DownOnStageWait() {
    if (!mDisableArrows && (IsDirectionAllowed(DIR_DOWN) || mEnableAllArrows)) {
        mStateMgrDown.changeState(StateID_DownOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_DownOnStageWait() {}

void dCourseSelectGuide_c::initializeState_DownOnStageAnimeEndCheck() {
    mDownAnim = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_D, false);
}
void dCourseSelectGuide_c::executeState_DownOnStageAnimeEndCheck() {
    if (!IsDirectionAllowed(DIR_DOWN) || mDisableArrows) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_D);
        mStateMgrDown.changeState(StateID_DownExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_D)) {
        mStateMgrDown.changeState(StateID_DownDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_DownOnStageAnimeEndCheck() {
    mDownAnim = false;
}

void dCourseSelectGuide_c::initializeState_DownDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_D);
}
void dCourseSelectGuide_c::executeState_DownDisp() {
    if (!IsDirectionAllowed(DIR_DOWN) || mDisableArrows) {
        mStateMgrDown.changeState(StateID_DownExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_DownDisp() {}

void dCourseSelectGuide_c::initializeState_DownExitAnimeEndCheck() {
    mDownAnim = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_D);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_D, false);
}
void dCourseSelectGuide_c::executeState_DownExitAnimeEndCheck() {
    if (IsDirectionAllowed(DIR_DOWN)) {
        mStateMgrDown.changeState(StateID_DownOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_D)) {
        mStateMgrDown.changeState(StateID_DownOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_DownExitAnimeEndCheck() {
    mDownAnim = false;
}

void dCourseSelectGuide_c::initializeState_LeftOnStageWait() {}
void dCourseSelectGuide_c::executeState_LeftOnStageWait() {
    if (!mDisableArrows && (IsDirectionAllowed(DIR_LEFT) || mEnableAllArrows)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftOnStageWait() {}

void dCourseSelectGuide_c::initializeState_LeftOnStageAnimeEndCheck() {
    mLeftAnim = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_L, false);
}
void dCourseSelectGuide_c::executeState_LeftOnStageAnimeEndCheck() {
    if (!IsDirectionAllowed(DIR_LEFT) || mDisableArrows) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_L);
        mStateMgrLeft.changeState(StateID_LeftExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_L)) {
        mStateMgrLeft.changeState(StateID_LeftDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftOnStageAnimeEndCheck() {
    mLeftAnim = false;
}

void dCourseSelectGuide_c::initializeState_LeftDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_L);
}
void dCourseSelectGuide_c::executeState_LeftDisp() {
    if (!IsDirectionAllowed(DIR_LEFT) || mDisableArrows) {
        mStateMgrLeft.changeState(StateID_LeftExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftDisp() {}

void dCourseSelectGuide_c::initializeState_LeftExitAnimeEndCheck() {
    mLeftAnim = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_L);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_L, false);
}
void dCourseSelectGuide_c::executeState_LeftExitAnimeEndCheck() {
    if (IsDirectionAllowed(DIR_LEFT)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_L)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftExitAnimeEndCheck() {
    mLeftAnim = false;
}

void dCourseSelectGuide_c::initializeState_RightOnStageWait() {}
void dCourseSelectGuide_c::executeState_RightOnStageWait() {
    if (!mDisableArrows && (IsDirectionAllowed(DIR_RIGHT) || mEnableAllArrows)) {
        mStateMgrRight.changeState(StateID_RightOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_RightOnStageWait() {}

void dCourseSelectGuide_c::initializeState_RightOnStageAnimeEndCheck() {
    mRightAnim = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_R, false);
}
void dCourseSelectGuide_c::executeState_RightOnStageAnimeEndCheck() {
    if (!IsDirectionAllowed(DIR_RIGHT) || mDisableArrows) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_R);
        mStateMgrRight.changeState(StateID_RightExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_R)) {
        mStateMgrRight.changeState(StateID_RightDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_RightOnStageAnimeEndCheck() {
    mRightAnim = false;
}

void dCourseSelectGuide_c::initializeState_RightDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_R);
}
void dCourseSelectGuide_c::executeState_RightDisp() {
    if (!IsDirectionAllowed(DIR_RIGHT) || mDisableArrows) {
        mStateMgrRight.changeState(StateID_RightExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_RightDisp() {}

void dCourseSelectGuide_c::initializeState_RightExitAnimeEndCheck() {
    mRightAnim = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_R);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_R, false);
}
void dCourseSelectGuide_c::executeState_RightExitAnimeEndCheck() {
    if (IsDirectionAllowed(DIR_RIGHT)) {
        mStateMgrRight.changeState(StateID_RightOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_R)) {
        mStateMgrRight.changeState(StateID_RightOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_RightExitAnimeEndCheck() {
    mRightAnim = false;
}

void dCourseSelectGuide_c::initializeState_ShadowOnStageWait() {}
void dCourseSelectGuide_c::executeState_ShadowOnStageWait() {
    if (mShowShadow) {
        mStateMgrShadow.changeState(StateID_ShadowOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_ShadowOnStageWait() {
    mShowShadow = false;
}

void dCourseSelectGuide_c::initializeState_ShadowOnStageAnimeEndCheck() {
    mLayout.AnimeStartSetup(ANIM_IN_SHADOW, false);
}
void dCourseSelectGuide_c::executeState_ShadowOnStageAnimeEndCheck() {
    if (!mLayout.isAnime(-1)) {
        mStateMgrShadow.changeState(StateID_ShadowDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_ShadowOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ShadowDisp() {}
void dCourseSelectGuide_c::executeState_ShadowDisp() {
    if (mHideShadow) {
        mStateMgrShadow.changeState(StateID_ShadowExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_ShadowDisp() {
    mHideShadow = false;
}

void dCourseSelectGuide_c::initializeState_ShadowExitAnimeEndCheck() {
    mLayout.AnimeStartSetup(ANIM_OUT_SHADOW, false);
}
void dCourseSelectGuide_c::executeState_ShadowExitAnimeEndCheck() {
    if (!mLayout.isAnime(-1)) {
        mStateMgrShadow.changeState(StateID_ShadowOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_ShadowExitAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideOnStageWait() {
    mScrollGuideRelated = false;
}
void dCourseSelectGuide_c::executeState_ScrollGuideOnStageWait() {
    if (mEndScrollGuide || mGuideRelated) {
        mEndScrollGuide = false;
        mScrollGuideRelated = false;
        mGuideRelated = false;
        mInMapView = false;
#if (defined(VERSION_K) || defined(VERSION_W))
    // Don't transition to the next state until the animation is finished.
    // Otherwise, the "View Map" text would change to "Back to Mario" too early.
    } else if (mShowScrollGuide && !mLayout.isAnime(ANIM_OUT_UNDER)) {
#else
    } else if (mShowScrollGuide) {
#endif
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideOnStageWait() {
    mShowScrollGuide = false;
}

void dCourseSelectGuide_c::initializeState_ScrollGuideOnStageAnimeEndCheck() {
    mExtension = Remocon::EXTENSION_UNKNOWN;
    mInMapView = true;
    mpNullPanes[N_guideViewC_00]->SetVisible(false);
    mpNullPanes[N_guideViewR_01]->SetVisible(false);
    mpNullPanes[N_left_00]->SetVisible(false);
    mLayout.AnimeStartSetup(ANIM_IN_UNDER, false);
    mScrollGuideRelated = true;
}
void dCourseSelectGuide_c::executeState_ScrollGuideOnStageAnimeEndCheck() {
    if (mEndScrollGuide) {
        mLayout.AnimeEndSetup(ANIM_IN_UNDER);
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideExitAnimeEndCheck);
    } else if (!mLayout.isAnime(-1)) {
        // [Bug: The above should probably be !mLayout.isAnime(ANIM_IN_UNDER).
        // But since ScrollGuideDisp doesn't really do anything, no difference is visible.]
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_UNDER);
}
void dCourseSelectGuide_c::executeState_ScrollGuideDisp() {
    if (mEndScrollGuide) {
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideDisp() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideExitAnimeEndCheck() {
    mEndScrollGuide = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_UNDER);
    mLayout.AnimeStartSetup(ANIM_OUT_UNDER, false);
}
void dCourseSelectGuide_c::executeState_ScrollGuideExitAnimeEndCheck() {
    if (!mLayout.isAnime(-1)) {
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideExitAnimeEndCheck() {
    mExtension = Remocon::EXTENSION_UNKNOWN;
    mScrollGuideRelated = false;
    mpNullPanes[N_guideViewC_00]->SetVisible(true);
    mpNullPanes[N_guideViewR_01]->SetVisible(true);
    mpNullPanes[N_left_00]->SetVisible(true);
    mInMapView = false;
}

void dCourseSelectGuide_c::UpdateGuide(short courseNo, dWmLib::PointType_e type) {
    if (mCourseNo == courseNo) {
        return;
    }
    mCourseType = dWmLib::GetCourseTypeFromCourseNo(courseNo);
    mCourseNo = courseNo;
    CourseSelectSet(type);
    if (type != dWmLib::POINT_TYPE_PATH) {
        CollectionCoinSet();
    }
    dInfo_c *info = dInfo_c::m_instance;
    dCyuukan_c *checkpoint = &info->mCyuukan;
    u8 wNo = mWorldNo;
    u8 cNo = courseNo;
    mpPicturePanes[P_flagSkull_00]->SetVisible(false);
    for (int i = 0; i < CHECKPOINT_COUNT; i++) {
        if (checkpoint->isCyuukanStart(i, wNo, cNo)) {
            mpPicturePanes[P_flagSkull_00]->SetVisible(true);
            break;
        }
    }
}

void dCourseSelectGuide_c::ControllerConnectCheck() {
    Remocon::EXTENSION_TYPE_e attachedExtension = dGameKey_c::m_instance->mRemocon[0]->mAttachedExtension;
    if (attachedExtension != mExtension) {
        mExtension = attachedExtension;

        MsgRes_c *msgRes = dMessage_c::getMesRes();
        bool attached = true;
        if (attachedExtension == Remocon::EXTENSION_NONE) {
            attached = false;
        }
        dInfo_c::m_instance->mExtensionAttached = attached;
        if (mInMapView) {
            mpTextBoxes[T_guideViewLS_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_BACK_TO_MARIO, 0);
        } else {
            mpTextBoxes[T_guideViewLS_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_VIEW_MAP, 0);
        }
        mpTextBoxes[T_guideViewL_01]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_ITEMS, 0);
    }
}

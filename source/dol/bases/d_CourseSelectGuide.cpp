#include <game/bases/d_CourseSelectGuide.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_game_display.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/mLib/m_video.hpp>
#include <constants/message_list.h>

dCourseSelectGuide_c *dCourseSelectGuide_c::m_instance;
dCourseSelectGuide_c::NumberHolder dCourseSelectGuide_c::msUnk;
char dCourseSelectGuide_c::msIsInit;

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

    static const char *GROUP_NAME_DT[] = {
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "A00_underGuide", "A01_zanki", "A02_courseInfo",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "B00_ArrowRight", "B01_ArrowLeft", "B02_ArrowUp", "B03_ArrowUnder",
        "C00_shadow",
        "C00_shadow"
    };

    static const int ANIME_INDEX_TBL[] = {
        inInfo, inInfo, inInfo,
        loopInfo, loopInfo, loopInfo,
        outInfo, outInfo, outInfo,
        inMapArrow, inMapArrow, inMapArrow, inMapArrow,
        loopArrow, loopArrow, loopArrow, loopArrow,
        outMapArrow, outMapArrow, outMapArrow, outMapArrow,
        inShadow,
        outShadow
    };

    static const char *NPANE_NAME_DT[] = {
        "N_IconPos1P_00", "N_IconPos2P_00", "N_IconPos3P_00", "N_IconPos4P_00",
        "N_mapArrow_00",
        "N_proportionL_00", "N_proportionR_00", "N_proportionC_00",
        "N_guideViewC_00", "N_guideViewR_01",
        "N_left_00"
    };

    static const char *T_PANE_NAME_TBL[] = {
        "T_worldNum_00",
        "T_cSelect_00", "T_cSelect_pic",
        "T_lifeNumber_00", "T_lifeNumber_01", "T_lifeNumber_02", "T_lifeNumber_03",
        "T_guideViewLS_00", "T_guideViewL_01"
    };

    static const int MESSAGE_DATA_TBL[] = {
        MSG_CS_SELECT_WORLD,
        MSG_CS_MENU,
        MSG_CS_WORLD
    };
    static const char *T_PANE_FIXED_NAME_TBL[] = {
        "T_guideViewC_00",
        "T_guideViewR_00",
        "T_world_00"
    };

    static const char *P_PANE_NAME_TBL[] = {
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
    mLayout.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(ANIME_INDEX_TBL));
    mpRootPane = mLayout.getRootPane();
    mLayout.NPaneRegister(NPANE_NAME_DT, &mpNullPanes[N_IconPos1P_00], ARRAY_SIZE(NPANE_NAME_DT));
    mLayout.TPaneRegister(T_PANE_NAME_TBL, &mpTextBoxes[T_worldNum_00], ARRAY_SIZE(T_PANE_NAME_TBL));
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL, MESSAGE_DATA_TBL, BMG_CATEGORY_COURSE_SELECT_GUIDE, ARRAY_SIZE(MESSAGE_DATA_TBL));
    mLayout.PPaneRegister(P_PANE_NAME_TBL, &mpPicturePanes[P_cC_1_00], ARRAY_SIZE(P_PANE_NAME_TBL));

    mLayout.mDrawOrder = 2;
    mpRootPane->setVisible(true);
    mpNullPanes[N_mapArrow_00]->setVisible(true);
    mpPicturePanes[P_flagSkull_00]->setVisible(false);

    for (int i = 0; i < 4; i++) {
        mRest[i] = -1;
    }

    RestNumberDisp();

    mExtension = Remocon::EXTENSION_THREE;
    mInitialized = true;

    mWorldNo = dScWMap_c::getWorldNo();
    mCourseNo = -10;
    mCourseType = 11;

    mLayout.AllAnimeEndSetup();

    m441 = true;
    m436 = false;
    m437 = true;
    m438 = false;
    m439 = false;
    m43a = false;
    m43b = true;
    m43c = false;
    m43d = false;
    m440 = false;
    m43f = false;
    m442 = false;
    m443 = false;
    m444 = false;
    m446 = false;
    m447 = false;
    m448 = false;
    m449 = false;
    m44a = false;
    m44b = false;
    m44c = false;
    m44d = false;
    mShowRestNumber = false;
    m424 = 0;
    m428 = 0;
    mAlpha = 255;
    mAlphaTarget = 255;


    mLayout.ReverseAnimeStartSetup(ANIM_OUT_UNDER, false);
    mLayout.ReverseAnimeStartSetup(ANIM_OUT_ZANKI, false);
    mLayout.ReverseAnimeStartSetup(ANIM_OUT_COURSE_INFO, false);
    mLayout.AnimePlay();
    mLayout.calc();
    FUN_80010690();
    mLayout.ReverseAnimeStartSetup(ANIM_IN_SHADOW, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_UNDER, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_ZANKI, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_COURSE_INFO, false);
    mLayout.AnimePlay();
    mLayout.calc();

    mVec2_c scale;
    dGameCom::DispSizeScale(scale);
    mpNullPanes[N_proportionL_00]->setScale(scale);
    mpNullPanes[N_proportionR_00]->setScale(scale);
    mpNullPanes[N_proportionC_00]->setScale(scale);

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

void dCourseSelectGuide_c::FUN_80010690() {
    static const int picPaneNums[] = { P_marioFace_00, P_luigiFace_00, P_BkinoFace_00, P_YkinoFace_00 };

    for (int i = 0; i < 4; i++) {
        mpPicturePanes[P_marioFace_00 + i]->setVisible(false);
    }

    int currPane = N_IconPos1P_00;
    int playerCount = -1;
    for (int i = 0; i < 4; i++) {
        daPyMng_c::PyType paneIdx = (daPyMng_c::PyType) getPaneNum(i);
        int cvtPaneIdx = daPyMng_c::getPlayerIndex(paneIdx);
        if (dGameCom::PlayerEnterCheck(cvtPaneIdx)) {
            mVec3_c pos = mpNullPanes[currPane]->mPos;
            mpPicturePanes[picPaneNums[paneIdx]]->mPos = pos;
            mpPicturePanes[picPaneNums[paneIdx]]->setVisible(true);
            currPane++;
            playerCount++;
        }
    }

    for (int i = 0; i < 4; i++) {
        mpNullPanes[N_IconPos1P_00 + i]->setVisible(false);
    }
    mpNullPanes[N_IconPos1P_00 + playerCount]->setVisible(true);
}

inline int getRest(int i) {
    return daPyMng_c::mRest[i];
}

void dCourseSelectGuide_c::RestNumberDisp() {
    static const int textBoxIdxs[] = { T_lifeNumber_00, T_lifeNumber_01, T_lifeNumber_03, T_lifeNumber_02 };

    for (int i = 0; i < 4; i++) {
        daPyMng_c::PyType paneIdx = (daPyMng_c::PyType) getPaneNum(i);
        int cvtPaneIdx = daPyMng_c::getPlayerIndex(paneIdx);
        int rest = getRest(paneIdx);
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
    if (mAlpha == mAlphaTarget) {
        return;
    }
    if (mAlphaTarget != 0) {
        mAlpha += 20;
        if (mAlpha >= 255) {
            mAlpha = 255;
        }
    } else {
        mAlpha -= 20;
        if (mAlpha < 0) {
            mAlpha = 0;
        }
    }

    u8 alpha = mAlpha;
    mpNullPanes[N_IconPos1P_00]->setAlpha(alpha);
    mpPicturePanes[P_marioFace_00]->setAlpha(alpha);
    mpNullPanes[N_IconPos2P_00]->setAlpha(alpha);
    mpPicturePanes[P_luigiFace_00]->setAlpha(alpha);
    mpNullPanes[N_IconPos3P_00]->setAlpha(alpha);
    mpPicturePanes[P_BkinoFace_00]->setAlpha(alpha);
    mpNullPanes[N_IconPos4P_00]->setAlpha(alpha);
    mpPicturePanes[P_YkinoFace_00]->setAlpha(alpha);
}

void dCourseSelectGuide_c::execute() {
    if (mInitialized && m441) {
        if (mShowRestNumber) {
            RestNumberDisp();
        }

        mStateMgrWorldCourse.executeState();
        mStateMgrGuide.executeState();

        if (m440) {
            mStateMgrUp.executeState();
            mStateMgrDown.executeState();
            mStateMgrLeft.executeState();
            mStateMgrRight.executeState();

            m434 = false;
            if (m43e) {
                m43e = false;
            }
            if (m43f) {
                if (
                    !mLayout.isAnime(ANIM_OUT_ARROW_U) || !mLayout.isAnime(ANIM_OUT_ARROW_D) ||
                    !mLayout.isAnime(ANIM_OUT_ARROW_L) || !mLayout.isAnime(ANIM_OUT_ARROW_R)
                ) {
                    m43f = false;
                    m440 = false;
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
    if (mInitialized && m441) {
        mLayout.entry();
    }
}

void dCourseSelectGuide_c::doDelete() {
    mLayout.doDelete();
}

void dCourseSelectGuide_c::CourseSelectIconDisp(dWmLib::CourseType_e type) {
    static const int sc_startPointIcons[] = {
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
    if (type == 1) {
        mCourseNo = -2;
        return;
    }
    if (type == 3 || type == 4) {
        type = (dWmLib::CourseType_e) (mCourseNo + 1);
    } else if (type == 2) {
        mCourseType = 11;
    } else {
        mCourseType = 5;
    }
    info->field_3b4 = actualWNo;
    mpTextBoxes[T_worldNum_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_CURR_WORLD, 0);
    mAlphaTarget = 255;
    int messageID;
    switch(mCourseType) {
        default:
            info->field_3b8 = type;
            mpTextBoxes[T_cSelect_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_COURSE_NUM, 0);
            mpTextBoxes[T_cSelect_00]->setVisible(true);
            mpTextBoxes[T_cSelect_pic]->setVisible(false);
            return;
        case 1: messageID = MSG_CS_ICON_GHOST_HOUSE; break;
        case 2: messageID = MSG_CS_ICON_TOWER; break;
        case 3:
            if (mWorldNo != WORLD_8) {
                messageID = MSG_CS_ICON_CASTLE;
            } else {
                messageID = MSG_CS_ICON_W8_CASTLE;
            }
            break;
        case 4:
            if (dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_1UP)) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_1UP;
            } else if (dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_STAR)) {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_STAR;
            } else {
                messageID = MSG_CS_ICON_KINOKO_HOUSE_RED;
            }
            break;
        case 5: messageID = MSG_CS_ICON_JUNCTION; break;
        case 6: messageID = MSG_CS_ICON_CANNON; break;
        case 8:
            if (dWmLib::isKoopaShipAnchor()) {
                messageID = MSG_CS_ICON_ANCHOR;
            } else {
                messageID = MSG_CS_ICON_AIRSHIP;
            }
            break;
        case 10:
            mAlphaTarget = 0;
            messageID = MSG_CS_ICON_PEACH_CASTLE;
            break;
        case 11:
            if (dWmLib::getStartPointKinokoHouseKindNum() == 0 || dWmLib::IsCourseClear(mWorldNo, STAGE_RESCUE)) {
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
    mpTextBoxes[T_cSelect_00]->setVisible(false);
    mpTextBoxes[T_cSelect_pic]->setVisible(true);
}

void dCourseSelectGuide_c::CollectionCoinSet() {
    dMj2dGame_c *saveGame = dSaveMng_c::m_instance->getSaveGame(-1);
    dCyuukan_c *checkpoint = dInfo_c::m_instance->getCyuukan();
    for (unsigned int i = 0; i < 3; i++) {
        mpPicturePanes[P_cC_1s_00 + i]->setVisible(true);
        mpPicturePanes[P_cC_1s_00 + i]->setAlpha(255);
        mpPicturePanes[P_cC_1_00 + i]->setAlpha(255);
        if (mCourseNo < 0 || !dScWMap_c::IsCourseType(mWorldNo, mCourseNo, dScWMap_c::COURSE_TYPE_2)) {
            mpPicturePanes[P_cC_1s_00 + i]->setVisible(false);
        } else {
            u8 collectCoin = saveGame->isCollectCoin(mWorldNo, mCourseNo, i);
            if (collectCoin != 0) {
                mpPicturePanes[P_cC_1_00 + i]->setVisible(true);
            } else {
                if (mWorldNo == checkpoint->mWorldNo
                && mCourseNo == checkpoint->mCourseNo
                && checkpoint->mCoinCollection[i] != 4) {
                    mpPicturePanes[P_cC_1s_00 + i]->setAlpha(0);
                    mpPicturePanes[P_cC_1_00 + i]->setVisible(true);
                    mpPicturePanes[P_cC_1_00 + i]->setAlpha(70);
                } else {
                    mpPicturePanes[P_cC_1_00 + i]->setVisible(false);
                }
            }
        }
    }
}

bool dCourseSelectGuide_c::FUN_80010f40(int type) {
    static const u8 flags[] = { 1, 2, 4, 8 };
    return (m434 & flags[type]) != 0;
}

void dCourseSelectGuide_c::initializeState_WorldCourseOnStageWait() {}
void dCourseSelectGuide_c::executeState_WorldCourseOnStageWait() {
    if (m448 || m442 || m437) {
        return;
    }

    if (m424 > 0) {
        m424--;
        return;
    }
    m424 = 0;

    if (!m436) {
        return;
    }

    mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_WorldCourseOnStageWait() {}

void dCourseSelectGuide_c::initializeState_WorldCourseOnStageAnimeEndCheck() {
    m449 = true;
    m436 = false;
    mLayout.AnimeStartSetup(ANIM_IN_COURSE_INFO, m439);
}
void dCourseSelectGuide_c::executeState_WorldCourseOnStageAnimeEndCheck() {
    if (m437) {
        mLayout.AnimeEndSetup(ANIM_IN_COURSE_INFO);
        mStateMgrWorldCourse.changeState(StateID_WorldCourseExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_COURSE_INFO)) {
        mStateMgrWorldCourse.changeState(StateID_WorldCourseDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseOnStageAnimeEndCheck() {
    m449 = false;
}

void dCourseSelectGuide_c::initializeState_WorldCourseDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_COURSE_INFO);
}
void dCourseSelectGuide_c::executeState_WorldCourseDisp() {
    if (m442) {
        return;
    }
    m436 = false;
    if (!m437) {
        return;
    }
    mStateMgrWorldCourse.changeState(StateID_WorldCourseExitAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_WorldCourseDisp() {}

void dCourseSelectGuide_c::initializeState_WorldCourseExitAnimeEndCheck() {
    m449 = true;
    m437 = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_COURSE_INFO);
    mLayout.AnimeStartSetup(ANIM_OUT_COURSE_INFO, false);
}
void dCourseSelectGuide_c::executeState_WorldCourseExitAnimeEndCheck() {
    if (m436) {
        mLayout.AnimeEndSetup(ANIM_OUT_COURSE_INFO);
        mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_COURSE_INFO)) {
        m438 = false;
        mStateMgrWorldCourse.changeState(StateID_WorldCourseOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_WorldCourseExitAnimeEndCheck() {
    m449 = false;
}

void dCourseSelectGuide_c::initializeState_GuideOnStageWait() {}
void dCourseSelectGuide_c::executeState_GuideOnStageWait() {
    if (m448 || m442 || m43b) {
        return;
    }
    if (m428 > 0) {
        m428--;
        return;
    }
    m428 = 0;
    if (!m43a) {
        return;
    }
    mStateMgrGuide.changeState(StateID_GuideOnStageAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_GuideOnStageWait() {
    FUN_80010690();
}

void dCourseSelectGuide_c::initializeState_GuideOnStageAnimeEndCheck() {
    m43a = false;
    mLayout.AnimeStartSetup(ANIM_IN_UNDER, m43d);
    mLayout.AnimeStartSetup(ANIM_IN_ZANKI, m43d);
}
void dCourseSelectGuide_c::executeState_GuideOnStageAnimeEndCheck() {
    if (m43b) {
        mLayout.AnimeEndSetup(ANIM_IN_UNDER);
        mLayout.AnimeEndSetup(ANIM_IN_ZANKI);
        mStateMgrGuide.changeState(StateID_GuideExitAnimeEndCheck);
        return;
    }
    if (mLayout.isAnime(ANIM_IN_UNDER) || mLayout.isAnime(ANIM_IN_ZANKI)) {
        return;
    }
    mStateMgrGuide.changeState(StateID_GuideDisp);
}
void dCourseSelectGuide_c::finalizeState_GuideOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_GuideDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_UNDER);
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ZANKI);
}
void dCourseSelectGuide_c::executeState_GuideDisp() {
    if (m442) {
        return;
    }
    m43a = false;
    if (!m43b) {
        return;
    }
    mStateMgrGuide.changeState(StateID_GuideExitAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_GuideDisp() {}

void dCourseSelectGuide_c::initializeState_GuideExitAnimeEndCheck() {
    m43b = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_UNDER);
    mLayout.AnimeEndSetup(ANIM_LOOP_ZANKI);
    mLayout.AnimeStartSetup(ANIM_OUT_UNDER, false);
    mLayout.AnimeStartSetup(ANIM_OUT_ZANKI, false);
}
void dCourseSelectGuide_c::executeState_GuideExitAnimeEndCheck() {
    if (m43a) {
        mLayout.AnimeEndSetup(ANIM_OUT_UNDER);
        mLayout.AnimeEndSetup(ANIM_OUT_ZANKI);
        mStateMgrGuide.changeState(StateID_GuideOnStageAnimeEndCheck);
        return;
    }
    if (mLayout.isAnime(ANIM_OUT_UNDER) || mLayout.isAnime(ANIM_OUT_ZANKI)) {
        return;
    }
    m43c = false;
    mStateMgrGuide.changeState(StateID_GuideOnStageWait);
}
void dCourseSelectGuide_c::finalizeState_GuideExitAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_UpOnStageWait() {}
void dCourseSelectGuide_c::executeState_UpOnStageWait() {
    if (!m43f && (FUN_80010f40(0) || m43e)) {
        mStateMgrUp.changeState(StateID_UpOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_UpOnStageWait() {}

void dCourseSelectGuide_c::initializeState_UpOnStageAnimeEndCheck() {
    m44a = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_U, false);
}
void dCourseSelectGuide_c::executeState_UpOnStageAnimeEndCheck() {
    if (!FUN_80010f40(0) || m43f) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_U);
        mStateMgrUp.changeState(StateID_UpExitAnimeEndCheck);
        return;
    }
    if (mLayout.isAnime(ANIM_IN_ARROW_U) || mLayout.isAnime(ANIM_IN_UNDER)) {
        return;
    }
    mStateMgrUp.changeState(StateID_UpDisp);
}
void dCourseSelectGuide_c::finalizeState_UpOnStageAnimeEndCheck() {
    m44a = false;
}

void dCourseSelectGuide_c::initializeState_UpDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_U);
}
void dCourseSelectGuide_c::executeState_UpDisp() {
    if (!FUN_80010f40(0) || m43f) {
        mStateMgrUp.changeState(StateID_UpExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_UpDisp() {}

void dCourseSelectGuide_c::initializeState_UpExitAnimeEndCheck() {
    m44a = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_U);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_U, false);
}
void dCourseSelectGuide_c::executeState_UpExitAnimeEndCheck() {
    if (FUN_80010f40(0)) {
        mStateMgrUp.changeState(StateID_UpOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_U)) {
        mStateMgrUp.changeState(StateID_UpOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_UpExitAnimeEndCheck() {
    m44a = false;
}

void dCourseSelectGuide_c::initializeState_DownOnStageWait() {}
void dCourseSelectGuide_c::executeState_DownOnStageWait() {
    if (!m43f && (FUN_80010f40(1) || m43e)) {
        mStateMgrDown.changeState(StateID_DownOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_DownOnStageWait() {}

void dCourseSelectGuide_c::initializeState_DownOnStageAnimeEndCheck() {
    m44b = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_D, false);
}
void dCourseSelectGuide_c::executeState_DownOnStageAnimeEndCheck() {
    if (!FUN_80010f40(1) || m43f) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_D);
        mStateMgrDown.changeState(StateID_DownExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_D)) {
        mStateMgrDown.changeState(StateID_DownDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_DownOnStageAnimeEndCheck() {
    m44b = false;
}

void dCourseSelectGuide_c::initializeState_DownDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_D);
}
void dCourseSelectGuide_c::executeState_DownDisp() {
    if (!FUN_80010f40(1) || m43f) {
        mStateMgrDown.changeState(StateID_DownExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_DownDisp() {}

void dCourseSelectGuide_c::initializeState_DownExitAnimeEndCheck() {
    m44b = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_D);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_D, false);
}
void dCourseSelectGuide_c::executeState_DownExitAnimeEndCheck() {
    if (FUN_80010f40(1)) {
        mStateMgrDown.changeState(StateID_DownOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_D)) {
        mStateMgrDown.changeState(StateID_DownOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_DownExitAnimeEndCheck() {
    m44b = false;
}

void dCourseSelectGuide_c::initializeState_LeftOnStageWait() {}
void dCourseSelectGuide_c::executeState_LeftOnStageWait() {
    if (!m43f && (FUN_80010f40(2) || m43e)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftOnStageWait() {}

void dCourseSelectGuide_c::initializeState_LeftOnStageAnimeEndCheck() {
    m44c = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_L, false);
}
void dCourseSelectGuide_c::executeState_LeftOnStageAnimeEndCheck() {
    if (!FUN_80010f40(2) || m43f) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_L);
        mStateMgrLeft.changeState(StateID_LeftExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_L)) {
        mStateMgrLeft.changeState(StateID_LeftDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftOnStageAnimeEndCheck() {
    m44c = false;
}

void dCourseSelectGuide_c::initializeState_LeftDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_L);
}
void dCourseSelectGuide_c::executeState_LeftDisp() {
    if (!FUN_80010f40(2) || m43f) {
        mStateMgrLeft.changeState(StateID_LeftExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftDisp() {}

void dCourseSelectGuide_c::initializeState_LeftExitAnimeEndCheck() {
    m44c = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_L);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_L, false);
}
void dCourseSelectGuide_c::executeState_LeftExitAnimeEndCheck() {
    if (FUN_80010f40(2)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_L)) {
        mStateMgrLeft.changeState(StateID_LeftOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_LeftExitAnimeEndCheck() {
    m44c = false;
}

void dCourseSelectGuide_c::initializeState_RightOnStageWait() {}
void dCourseSelectGuide_c::executeState_RightOnStageWait() {
    if (!m43f && (FUN_80010f40(3) || m43e)) {
        mStateMgrRight.changeState(StateID_RightOnStageAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_RightOnStageWait() {}

void dCourseSelectGuide_c::initializeState_RightOnStageAnimeEndCheck() {
    m44d = true;
    mLayout.AnimeStartSetup(ANIM_IN_ARROW_R, false);
}
void dCourseSelectGuide_c::executeState_RightOnStageAnimeEndCheck() {
    if (!FUN_80010f40(3) || m43f) {
        mLayout.AnimeEndSetup(ANIM_IN_ARROW_R);
        mStateMgrRight.changeState(StateID_RightExitAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_IN_ARROW_R)) {
        mStateMgrRight.changeState(StateID_RightDisp);
    }
}
void dCourseSelectGuide_c::finalizeState_RightOnStageAnimeEndCheck() {
    m44d = false;
}

void dCourseSelectGuide_c::initializeState_RightDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_ARROW_R);
}
void dCourseSelectGuide_c::executeState_RightDisp() {
    if (!FUN_80010f40(3) || m43f) {
        mStateMgrRight.changeState(StateID_RightExitAnimeEndCheck);
    }
}
void dCourseSelectGuide_c::finalizeState_RightDisp() {}

void dCourseSelectGuide_c::initializeState_RightExitAnimeEndCheck() {
    m44d = true;
    mLayout.AnimeEndSetup(ANIM_LOOP_ARROW_R);
    mLayout.AnimeStartSetup(ANIM_OUT_ARROW_R, false);
}
void dCourseSelectGuide_c::executeState_RightExitAnimeEndCheck() {
    if (FUN_80010f40(3)) {
        mStateMgrRight.changeState(StateID_RightOnStageAnimeEndCheck);
    } else if (!mLayout.isAnime(ANIM_OUT_ARROW_R)) {
        mStateMgrRight.changeState(StateID_RightOnStageWait);
    }
}
void dCourseSelectGuide_c::finalizeState_RightExitAnimeEndCheck() {
    m44d = false;
}

void dCourseSelectGuide_c::initializeState_ShadowOnStageWait() {}
void dCourseSelectGuide_c::executeState_ShadowOnStageWait() {
    if (!m443) {
        return;
    }
    mStateMgrShadow.changeState(StateID_ShadowOnStageAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_ShadowOnStageWait() {
    m443 = false;
}

void dCourseSelectGuide_c::initializeState_ShadowOnStageAnimeEndCheck() {
    mLayout.AnimeStartSetup(ANIM_IN_SHADOW, false);
}
void dCourseSelectGuide_c::executeState_ShadowOnStageAnimeEndCheck() {
    if (mLayout.isAnime(-1)) {
        return;
    }
    mStateMgrShadow.changeState(StateID_ShadowDisp);
}
void dCourseSelectGuide_c::finalizeState_ShadowOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ShadowDisp() {}
void dCourseSelectGuide_c::executeState_ShadowDisp() {
    if (!m444) {
        return;
    }
    mStateMgrShadow.changeState(StateID_ShadowExitAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_ShadowDisp() {
    m444 = false;
}

void dCourseSelectGuide_c::initializeState_ShadowExitAnimeEndCheck() {
    mLayout.AnimeStartSetup(ANIM_OUT_SHADOW, false);
}
void dCourseSelectGuide_c::executeState_ShadowExitAnimeEndCheck() {
    if (mLayout.isAnime(-1)) {
        return;
    }
    mStateMgrShadow.changeState(StateID_ShadowOnStageWait);
}
void dCourseSelectGuide_c::finalizeState_ShadowExitAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideOnStageWait() {
    m448 = false;
}
void dCourseSelectGuide_c::executeState_ScrollGuideOnStageWait() {
    if (m447 || m43c) {
        m447 = false;
        m448 = false;
        m43c = false;
        mInScrollMode = false;
        return;
    }
    if (!m446) {
        return;
    }
    mStateMgrScrollGuide.changeState(StateID_ScrollGuideOnStageAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideOnStageWait() {
    m446 = false;
}

void dCourseSelectGuide_c::initializeState_ScrollGuideOnStageAnimeEndCheck() {
    mExtension = Remocon::EXTENSION_THREE;
    mInScrollMode = true;
    mpNullPanes[N_guideViewC_00]->setVisible(false);
    mpNullPanes[N_guideViewR_01]->setVisible(false);
    mpNullPanes[N_left_00]->setVisible(false);
    mLayout.AnimeStartSetup(ANIM_IN_UNDER, false);
    m448 = true;
}
void dCourseSelectGuide_c::executeState_ScrollGuideOnStageAnimeEndCheck() {
    if (m447) {
        mLayout.AnimeEndSetup(ANIM_IN_UNDER);
        mStateMgrScrollGuide.changeState(StateID_ScrollGuideExitAnimeEndCheck);
        return;
    }
    if (mLayout.isAnime(-1)) {
        return;
    }
    mStateMgrScrollGuide.changeState(StateID_ScrollGuideDisp);
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideOnStageAnimeEndCheck() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideDisp() {
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_UNDER);
}
void dCourseSelectGuide_c::executeState_ScrollGuideDisp() {
    if (!m447) {
        return;
    }
    mStateMgrScrollGuide.changeState(StateID_ScrollGuideExitAnimeEndCheck);
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideDisp() {}

void dCourseSelectGuide_c::initializeState_ScrollGuideExitAnimeEndCheck() {
    m447 = false;
    mLayout.AnimeEndSetup(ANIM_LOOP_UNDER);
    mLayout.AnimeStartSetup(ANIM_OUT_UNDER, false);
}
void dCourseSelectGuide_c::executeState_ScrollGuideExitAnimeEndCheck() {
    if (mLayout.isAnime(-1)) {
        return;
    }
    mStateMgrScrollGuide.changeState(StateID_ScrollGuideOnStageWait);
}
void dCourseSelectGuide_c::finalizeState_ScrollGuideExitAnimeEndCheck() {
    mExtension = Remocon::EXTENSION_THREE;
    m448 = false;
    mpNullPanes[N_guideViewC_00]->setVisible(true);
    mpNullPanes[N_guideViewR_01]->setVisible(true);
    mpNullPanes[N_left_00]->setVisible(true);
    mInScrollMode = false;
}

void dCourseSelectGuide_c::FUN_800125c0(short courseNo, dWmLib::CourseType_e type) {
    if (mCourseNo == courseNo) {
        return;
    }
    mCourseType = dWmLib::GetCourseTypeFromCourseNo(courseNo);
    mCourseNo = courseNo;
    CourseSelectIconDisp(type);
    if (type != 1) {
        CollectionCoinSet();
    }
    dInfo_c *info = dInfo_c::m_instance;
    dCyuukan_c *checkpoint;
    u8 wNo, cNo;
    cNo = courseNo;
    wNo = mWorldNo;
    mpPicturePanes[P_flagSkull_00]->setVisible(false);
    checkpoint = &info->mCyuukan;
    for (int i = 0; i < 2; i++) {
        if (checkpoint->isCyuukanStart(i, wNo, cNo)) {
            mpPicturePanes[P_flagSkull_00]->setVisible(true);
            break;
        }
    }
}

void dCourseSelectGuide_c::ControllerConnectCheck() {
    Remocon::EXTENSION_TYPE_e attachedExtension = dGameKey_c::m_instance->mRemocon[0]->mAttachedExtension;
    if (attachedExtension != mExtension) {
        mExtension = attachedExtension;

        MsgRes_c *msgRes = dMessage_c::getMesRes();
        int attached = 1;
        if (attachedExtension == 0) {
            attached = 0;
        }
        dInfo_c::m_instance->mExtensionAttached = attached;
        if (mInScrollMode) {
            mpTextBoxes[T_guideViewLS_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_BACK_TO_MARIO, 0);
        } else {
            mpTextBoxes[T_guideViewLS_00]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_VIEW_MAP, 0);
        }
        mpTextBoxes[T_guideViewL_01]->setMessage(msgRes, BMG_CATEGORY_COURSE_SELECT_GUIDE, MSG_CS_ITEMS, 0);
    }
}

#include <game/bases/d_pausewindow.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_scene.hpp>
#include <game/bases/d_cursor_select.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/snd/snd_audio_mgr.hpp>
#include <constants/game_constants.h>
#include <constants/sound_list.h>
#include <constants/message_list.h>

ACTOR_PROFILE(PAUSEWINDOW, Pausewindow_c, 0);

STATE_DEFINE(Pausewindow_c, InitWait);
STATE_DEFINE(Pausewindow_c, OpenAnimeEndWait);
STATE_DEFINE(Pausewindow_c, ButtonChangeAnimeEndWait);
STATE_DEFINE(Pausewindow_c, PauseDisp);
STATE_DEFINE(Pausewindow_c, HitAnimeEndWait);
STATE_DEFINE(Pausewindow_c, ClouseAnimeEndWait);

Pausewindow_c *Pausewindow_c::m_instance = nullptr;

Pausewindow_c::Pausewindow_c() :
    mStateMgr(*this, StateID_InitWait),
    mHasLoadedLayout(false)
{
    Pausewindow_c::m_instance = this;
}

Pausewindow_c::~Pausewindow_c() {}


int Pausewindow_c::create() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "pauseMenu_16_inWindow.brlan",
        "pauseMenu_16_loopWindow.brlan",
        "pauseMenu_16_outWindow.brlan",
        "pauseMenu_16_onButton.brlan",
        "pauseMenu_16_idleButton.brlan",
        "pauseMenu_16_offButon.brlan",
        "pauseMenu_16_hitButton.brlan"
    };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = {
        "A00_inWindow", "A00_inWindow", "A00_inWindow",
        "B00_tuzukuButton", "B02_menuButton",
        "B00_tuzukuButton", "B02_menuButton",
        "B00_tuzukuButton", "B02_menuButton",
        "B00_tuzukuButton", "B02_menuButton"
    };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = {
        inWindow, loopWindow, outWindow,
        onButton, onButton,
        idleButton, idleButton,
        offButon, offButon,
        hitButton, hitButton
    };

    static const char *textBox[] = {
        "T_tuzukeru_00", "T_tuzukeru_01",
        "T_modoru_00", "T_modoru_01"
    };
    static const int msgIdTbl[ARRAY_SIZE(textBox)] = {
        MSG_PAUSE_CONTINUE, MSG_PAUSE_CONTINUE,
        MSG_PAUSE_EXIT, MSG_PAUSE_EXIT
    };

    static const char *textBox2[] = {
        "T_world_00", "T_hyphen_00", "T_multiText_00"
    };
    static const int msgIdTbl2[ARRAY_SIZE(textBox2)] = {
        MSG_WM_WORLD, MSG_WM_HYPHEN, MSG_WM_FREE_MODE
    };

    static const char *PPANE_NAME_DT[P_COUNT] = {
        "P_SBBase_00",
        "P_SBBase_02",
        "P_shadowBlack",
        "P_coin_00"
    };

    static const char *WPANE_NAME_DT[W_COUNT] = {
        "W_N_pauseMenu_00"
    };

    static const char *T_PANE_NAME_TBL[T_COUNT] = {
        "T_tuzukeru_00", "T_tuzukeru_01",
        "T_modoru_00", "T_modoru_01",
        "T_worldNum_00", "T_corseNum_00",
        "T_corsePic_00",
        "T_osusumeText_00"
    };

    static const char *NPANE_NAME_DT[N_COUNT] = {
        "N_worldText_00",
        "N_osusumeText_00",
        "N_multiText_00"
    };

    if (mHasLoadedLayout) {
        return SUCCEEDED;
    }

    if (!mLayout.ReadResource("pauseMenu/pauseMenu.arc", false)) {
        return NOT_READY;
    }

    mLayout.build("pauseMenu_16.brlyt", nullptr);
    mLayout.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mLayout.TPaneNameRegister(textBox, msgIdTbl, BMG_CATEGORY_PAUSE_WINDOW, ARRAY_SIZE(textBox));
    mLayout.TPaneNameRegister(textBox2, msgIdTbl2, BMG_CATEGORY_WORLD_MAP, ARRAY_SIZE(textBox2));
    mLayout.PPaneRegister(PPANE_NAME_DT, mpPicturePanes, P_COUNT);
    mLayout.WPaneRegister(WPANE_NAME_DT, mpWindowPanes, W_COUNT);
    mLayout.TPaneRegister(T_PANE_NAME_TBL, mpTextBoxes, T_COUNT);
    mLayout.NPaneRegister(NPANE_NAME_DT, mpNullPanes, N_COUNT);

    mLayout.mDrawOrder = m2d::DRAW_ORDER_PAUSE_WINDOW;
    mIsActive = false;
    mHasLoadedLayout = true;
    mIsAnimating = false;
    mColor = 0;
    mToTitle = false;
    mButtonHit = false;

    if (
        dGameCom::isNowCourseClear() ||
        dScene_c::m_nowScene != fProfile::STAGE ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_CANNON
    ) {
        mpPicturePanes[P_shadowBlack]->SetVisible(false);
    } else {
        mpPicturePanes[P_shadowBlack]->SetVisible(true);
    }
    if (
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY_2 ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY_3
    ) {
        mpPicturePanes[P_shadowBlack]->SetVisible(true);
    }
    TitleDispChkWrite();

    mLayout.AllAnimeEndSetup();
    return SUCCEEDED;
}

int Pausewindow_c::execute() {
    if (mHasLoadedLayout && mIsActive) {
        mStateMgr.executeState();
        mLayout.AnimePlay();
        mLayout.calc();
    }
    return SUCCEEDED;
}

int Pausewindow_c::draw() {
    if (mHasLoadedLayout && mIsActive) {
        mLayout.entry();
    }
    return SUCCEEDED;
}

int Pausewindow_c::doDelete() {
    if (!mLayout.doDelete()) {
        return NOT_READY;
    }
    m_instance = nullptr;
    return SUCCEEDED;
}

void Pausewindow_c::setWorldCourseWrite() {
    MsgRes_c *msgRes = dMessage_c::getMesRes();
    dInfo_c *info = dInfo_c::getInstance();
    u8 world = info->getWorld();
    u8 course = info->getCourse();
    if (info->getWorld() > WORLD_USED_COUNT) {
        world = WORLD_USED_COUNT;
    }
    info->mDisplayCourseWorld = world + 1;
    mpTextBoxes[T_worldNum_00]->setMessage(msgRes, BMG_CATEGORY_WORLD_MAP, MSG_WM_WORLD_NUM, 0);
    int msgID;
    switch (course) {
        case STAGE_GHOST_HOUSE:
            msgID = MSG_WM_ICON_GHOST_HOUSE;
            break;
        case STAGE_TOWER:
        case STAGE_TOWER_2:
            msgID = MSG_WM_ICON_TOWER;
            break;
        case STAGE_CASTLE:
        case STAGE_CASTLE_2:
            if (world != LAST_WORLD) {
                msgID = MSG_WM_ICON_CASTLE;
            } else {
                msgID = MSG_WM_ICON_LAST_WORLD_CASTLE;
            }
            break;
        case STAGE_KINOKO_HOUSE:
        case STAGE_KINOKO_HOUSE_2:
        case STAGE_KINOKO_HOUSE_3:
        case STAGE_KINOKO_HOUSE_4:
        case STAGE_KINOKO_HOUSE_5:
        case STAGE_KINOKO_HOUSE_6:
        case STAGE_KINOKO_HOUSE_7:
            if (dScWMap_c::IsCourseType(world, course, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_1UP)) {
                msgID = MSG_WM_ICON_KINOKO_HOUSE_1UP;
            } else if (dScWMap_c::IsCourseType(world, course, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_STAR)) {
                msgID = MSG_WM_ICON_KINOKO_HOUSE_STAR;
            } else {
                msgID = MSG_WM_ICON_KINOKO_HOUSE;
            }
            break;
        case STAGE_START_KINOKO_HOUSE:
            if (dWmLib::isStartPointKinokoHouseStar()) {
                msgID = MSG_WM_ICON_KINOKO_HOUSE_STAR;
            } else if (dWmLib::isStartPointKinokoHouseRed()) {
                msgID = MSG_WM_ICON_KINOKO_HOUSE;
            } else {
                msgID = MSG_WM_ICON_KINOKO_HOUSE_1UP;
            }
            break;
        case STAGE_ENEMY:
        case STAGE_ENEMY_2:
        case STAGE_ENEMY_3:
            msgID = MSG_WM_ICON_RESCUE;
            break;
        case STAGE_CANNON:
            msgID = MSG_WM_ICON_CANNON;
            break;
        case STAGE_PEACH_CASTLE:
            msgID = MSG_WM_ICON_PEACH_CASTLE;
            break;
        case STAGE_UNK37:
            msgID = MSG_WM_ICON_ANCHOR;
            break;
        case STAGE_DOOMSHIP:
            if (dWmLib::isKoopaShipAnchor()) {
                msgID = MSG_WM_ICON_ANCHOR;
            } else {
                msgID = MSG_WM_ICON_AIRSHIP;
            }
            break;
        default:
            info->mDisplayCourseNum = course + 1;
            mpTextBoxes[T_corseNum_00]->SetVisible(true);
            mpTextBoxes[T_corseNum_00]->setMessage(msgRes, BMG_CATEGORY_WORLD_MAP, MSG_WM_COURSE_NUM, 0);
            mpTextBoxes[T_corsePic_00]->SetVisible(false);
            return;
    }
    mpTextBoxes[T_corseNum_00]->SetVisible(false);
    mpTextBoxes[T_corsePic_00]->SetVisible(true);
    mpTextBoxes[T_corsePic_00]->setMessage(msgRes, BMG_CATEGORY_WORLD_MAP, msgID, 0);
}

void Pausewindow_c::TitleDispChkWrite() {
    mpNullPanes[N_worldText_00]->SetVisible(true);
    mpNullPanes[N_osusumeText_00]->SetVisible(false);
    mpNullPanes[N_multiText_00]->SetVisible(false);
    MsgRes_c *msgRes = dMessage_c::getMesRes();
    dInfo_c *info = dInfo_c::m_instance;
    int pageIndex = info->mCourseSelectIndexInPage;
    if (pageIndex >= 10) {
        pageIndex -= 10;
    }
    if (
        dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_IS_COIN_COURSE &&
        info->mCourseSelectPageNum == 0 &&
        pageIndex < 5
    ) {
        mpTextBoxes[T_corseNum_00]->SetVisible(true);
        mpTextBoxes[T_corsePic_00]->SetVisible(false);
        // [Bug: This should also do the following so that the world number isn't shown too:]
        // mpTextBoxes[T_worldNum_00]->SetVisible(false);
        mpPicturePanes[P_coin_00]->SetVisible(true);
        info->mDisplayCourseNum = pageIndex + 1;
        mpTextBoxes[T_corseNum_00]->setMessage(msgRes, BMG_CATEGORY_WORLD_MAP, MSG_WM_COURSE_NUM, 0);
    } else {
        mpPicturePanes[P_coin_00]->SetVisible(false);
        setWorldCourseWrite();
    }
}

void Pausewindow_c::SelectCursorSetup() {
    static const int PANE_IDX_TBL[] = {
        P_SBBase_00,
        P_SBBase_02
    };
    dGameCom::SelectCursorSetup(mpPicturePanes[PANE_IDX_TBL[mNextButton]], 0, false);
}

void Pausewindow_c::initializeState_InitWait() {}
void Pausewindow_c::executeState_InitWait() {
    mNextButton = 0;
    mIsActive = true;
    dGameCom::WindowPaneColorSet(mpWindowPanes[W_N_pauseMenu_00], mColor);
    if (mToTitle) {
        MsgRes_c *msgRes = dMessage_c::getMesRes();
        mpTextBoxes[T_tuzukeru_00]->setMessage(msgRes, BMG_CATEGORY_MAIN_MENU, MSG_MAIN_MENU_CONTINUE, 0);
        mpTextBoxes[T_tuzukeru_01]->setMessage(msgRes, BMG_CATEGORY_MAIN_MENU, MSG_MAIN_MENU_CONTINUE, 0);
        mpTextBoxes[T_modoru_00]->setMessage(msgRes, BMG_CATEGORY_MAIN_MENU, MSG_MAIN_MENU_TITLE_SCREEN, 0);
        mpTextBoxes[T_modoru_01]->setMessage(msgRes, BMG_CATEGORY_MAIN_MENU, MSG_MAIN_MENU_TITLE_SCREEN, 0);
    }
    mStateMgr.changeState(StateID_OpenAnimeEndWait);
}
void Pausewindow_c::finalizeState_InitWait() {}

void Pausewindow_c::initializeState_OpenAnimeEndWait() {
    mIsAnimating = true;
    mLayout.AllAnimeEndSetup();
    mLayout.ReverseAnimeStartSetup(ANIM_ON_TUZUKU, false);
    mLayout.ReverseAnimeStartSetup(ANIM_ON_MENU, false);
    mLayout.AnimeStartSetup(ANIM_IN_WINDOW, false);
}
void Pausewindow_c::executeState_OpenAnimeEndWait() {
    if (!mLayout.isAllAnime()) {
        mStateMgr.changeState(StateID_ButtonChangeAnimeEndWait);
    }
}
void Pausewindow_c::finalizeState_OpenAnimeEndWait() {
    mIsAnimating = false;
    mClose = false;
    mButtonHit = false;
    mActiveButton = -1;
}

void Pausewindow_c::initializeState_ButtonChangeAnimeEndWait() {
    mLayout.AllAnimeEndSetup();
    mIsAnimating = true;
    if (mActiveButton >= 0) {
        mLayout.AnimeStartSetup(ANIM_OFF_TUZUKU + mActiveButton, false);
    }
    mActiveButton = mNextButton;
    mLayout.AnimeStartSetup(ANIM_ON_TUZUKU + mNextButton, false);
}
void Pausewindow_c::executeState_ButtonChangeAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mStateMgr.changeState(StateID_PauseDisp);
    }
}
void Pausewindow_c::finalizeState_ButtonChangeAnimeEndWait() {
    mIsAnimating = false;
}

void Pausewindow_c::initializeState_PauseDisp() {
    SelectCursorSetup();
}
void Pausewindow_c::executeState_PauseDisp() {
    if (mClose) {
        mStateMgr.changeState(StateID_ClouseAnimeEndWait);
    } else if (mButtonHit) {
        mStateMgr.changeState(StateID_HitAnimeEndWait);
    } else {
        if (mActiveButton != mNextButton) {
            SndAudioMgr::sInstance->startSystemSe(SE_SYS_CURSOR, 1);
            mStateMgr.changeState(StateID_ButtonChangeAnimeEndWait);
        }
    }
}
void Pausewindow_c::finalizeState_PauseDisp() {
    dSelectCursor_c::m_instance->Cancel(0);
}

void Pausewindow_c::initializeState_HitAnimeEndWait() {
    mIsAnimating = true;
    mLayout.AnimeStartSetup(ANIM_HIT_TUZUKU + mNextButton, false);
}
void Pausewindow_c::executeState_HitAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mStateMgr.changeState(StateID_ClouseAnimeEndWait);
    }
}
void Pausewindow_c::finalizeState_HitAnimeEndWait() {
    mIsAnimating = false;
}

void Pausewindow_c::initializeState_ClouseAnimeEndWait() {
    mIsAnimating = true;
    mLayout.AnimeStartSetup(ANIM_OUT_WINDOW, false);
}
void Pausewindow_c::executeState_ClouseAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mIsActive = false;
        mStateMgr.changeState(StateID_InitWait);
    }
}
void Pausewindow_c::finalizeState_ClouseAnimeEndWait() {
    mIsAnimating = false;
}

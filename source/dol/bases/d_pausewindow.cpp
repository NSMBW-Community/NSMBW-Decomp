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

    static const int MESSAGE_DATA_TBL_1[] = {
        0, 0,
        1, 1
    };
    static const char *T_PANE_FIXED_NAME_TBL_1[] = {
        "T_tuzukeru_00", "T_tuzukeru_01",
        "T_modoru_00", "T_modoru_01"
    };

    static const int MESSAGE_DATA_TBL_2[] = {
        0x50, 0x60, 0x62
    };
    static const char *T_PANE_FIXED_NAME_TBL_2[] = {
        "T_world_00",
        "T_hyphen_00",
        "T_multiText_00"
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
        "T_tuzukeru_00",
        "T_tuzukeru_01",
        "T_modoru_00",
        "T_modoru_01",
        "T_worldNum_00",
        "T_corseNum_00",
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
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL_1, MESSAGE_DATA_TBL_1, 1, ARRAY_SIZE(T_PANE_FIXED_NAME_TBL_1));
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL_2, MESSAGE_DATA_TBL_2, 2, ARRAY_SIZE(T_PANE_FIXED_NAME_TBL_2));
    mLayout.PPaneRegister(PPANE_NAME_DT, mpPicturePanes, P_COUNT);
    mLayout.WPaneRegister(WPANE_NAME_DT, mpWindowPanes, W_COUNT);
    mLayout.TPaneRegister(T_PANE_NAME_TBL, mpTextBoxes, T_COUNT);
    mLayout.NPaneRegister(NPANE_NAME_DT, mpNullPanes, N_COUNT);

    mLayout.mDrawOrder = 141;
    mIsActive = false;
    mHasLoadedLayout = true;
    m_294 = false;
    mColor = 0;
    m_295 = false;
    m_293 = false;

    if (
        dGameCom::isNowCourseClear() ||
        dScene_c::m_nowScene != fProfile::STAGE ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_CANNON
    ) {
        mpPicturePanes[P_shadowBlack]->setVisible(false);
    } else {
        mpPicturePanes[P_shadowBlack]->setVisible(true);
    }
    if (
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY_2 ||
        dInfo_c::m_startGameInfo.mLevel1 == STAGE_ENEMY_3
    ) {
        mpPicturePanes[P_shadowBlack]->setVisible(true);
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
    u8 w = info->getWorld();
    u8 c = info->getCourse();
    if (info->getWorld() > WORLD_USED_COUNT) {
        w = WORLD_USED_COUNT;
    }
    info->field_3b4 = w + 1;
    mpTextBoxes[T_worldNum_00]->setMessage(msgRes, 2, 0x51, 0);
    int icon;
    switch (c) {
        case STAGE_GHOST_HOUSE:
            icon = 0x55;
            break;
        case STAGE_TOWER:
        case STAGE_TOWER_2:
            icon = 0x54;
            break;
        case STAGE_CASTLE:
        case STAGE_CASTLE_2:
            if (w != WORLD_8) {
                icon = 0x53;
            } else {
                icon = 0x5d;
            }
            break;
        case STAGE_KINOKO_HOUSE:
        case STAGE_KINOKO_HOUSE_2:
        case STAGE_KINOKO_HOUSE_3:
        case STAGE_KINOKO_HOUSE_4:
        case STAGE_KINOKO_HOUSE_5:
        case STAGE_KINOKO_HOUSE_6:
        case STAGE_KINOKO_HOUSE_7:
            if (dScWMap_c::IsCourseType(w, c, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_1UP)) {
                icon = 0x5b;
            } else if (dScWMap_c::IsCourseType(w, c, dScWMap_c::COURSE_TYPE_KINOKO_HOUSE_STAR)) {
                icon = 0x5c;
            } else {
                icon = 0x58;
            }
            break;
        case STAGE_START_KINOKO_HOUSE:
            if (dWmLib::isStartPointKinokoHouseStar()) {
                icon = 0x5c;
            } else if (dWmLib::isStartPointKinokoHouseRed()) {
                icon = 0x58;
            } else {
                icon = 0x5b;
            }
            break;
        case STAGE_ENEMY:
        case STAGE_ENEMY_2:
        case STAGE_ENEMY_3:
            icon = 0x5e;
            break;
        case STAGE_CANNON:
            icon = 0x56;
            break;
        case STAGE_PEACH_CASTLE:
            icon = 0x5a;
            break;
        case STAGE_UNK37:
            icon = 0x5f;
            break;
        case STAGE_DOOMSHIP:
            if (dWmLib::isKoopaShipAnchor()) {
                icon = 0x5f;
            } else {
                icon = 0x57;
            }
            break;
        default:
            info->field_3b8 = c + 1;
            mpTextBoxes[T_corseNum_00]->setVisible(true);
            mpTextBoxes[T_corseNum_00]->setMessage(msgRes, 2, 0x52, 0);
            mpTextBoxes[T_corsePic_00]->setVisible(false);
            return;
    }
    mpTextBoxes[T_corseNum_00]->setVisible(false);
    mpTextBoxes[T_corsePic_00]->setVisible(true);
    mpTextBoxes[T_corsePic_00]->setMessage(msgRes, 2, icon, 0);
}

void Pausewindow_c::TitleDispChkWrite() {
    mpNullPanes[N_worldText_00]->setVisible(true);
    mpNullPanes[N_osusumeText_00]->setVisible(false);
    mpNullPanes[N_multiText_00]->setVisible(false);
    MsgRes_c *msgRes = dMessage_c::getMesRes();
    dInfo_c *info = dInfo_c::m_instance;
    int tmp = info->field_3e8;
    if (tmp >= 10) {
        tmp -= 10;
    }
    if (
        (dInfo_c::mGameFlag & 0x40) != 0 &&
        info->field_3e4 == 0 &&
        tmp < 5
    ) {
        mpTextBoxes[T_corseNum_00]->setVisible(true);
        mpTextBoxes[T_corsePic_00]->setVisible(false);
        mpPicturePanes[P_coin_00]->setVisible(true);
        info->field_3b8 = tmp + 1;
        mpTextBoxes[T_corseNum_00]->setMessage(msgRes, 2, 0x52, 0);
    } else {
        mpPicturePanes[P_coin_00]->setVisible(false);
        setWorldCourseWrite();
    }
}

void Pausewindow_c::SelectCursorSetup() {
    static const int idxs[] = {
        P_SBBase_00,
        P_SBBase_02
    };
    dGameCom::SelectCursorSetup(mpPicturePanes[idxs[mCursor]], 0, false);
}

void Pausewindow_c::initializeState_InitWait() {}
void Pausewindow_c::executeState_InitWait() {
    mCursor = 0;
    mIsActive = true;
    dGameCom::WindowPaneColorSet(mpWindowPanes[W_N_pauseMenu_00], mColor);
    if (m_295) {
        MsgRes_c *msgRes = dMessage_c::getMesRes();
        mpTextBoxes[T_tuzukeru_00]->setMessage(msgRes, 0x32, 0x17, 0);
        mpTextBoxes[T_tuzukeru_01]->setMessage(msgRes, 0x32, 0x17, 0);
        mpTextBoxes[T_modoru_00]->setMessage(msgRes, 0x32, 0x18, 0);
        mpTextBoxes[T_modoru_01]->setMessage(msgRes, 0x32, 0x18, 0);
    }
    mStateMgr.changeState(StateID_OpenAnimeEndWait);
}
void Pausewindow_c::finalizeState_InitWait() {}

void Pausewindow_c::initializeState_OpenAnimeEndWait() {
    m_294 = true;
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
    m_294 = false;
    m_292 = false;
    m_293 = false;
    mCursor2 = -1;
}

void Pausewindow_c::initializeState_ButtonChangeAnimeEndWait() {
    mLayout.AllAnimeEndSetup();
    m_294 = true;
    if (mCursor2 >= 0) {
        mLayout.AnimeStartSetup(ANIM_OFF_TUZUKU + mCursor2, false);
    }
    mCursor2 = mCursor;
    mLayout.AnimeStartSetup(ANIM_ON_TUZUKU + mCursor, false);
}
void Pausewindow_c::executeState_ButtonChangeAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mStateMgr.changeState(StateID_PauseDisp);
    }
}
void Pausewindow_c::finalizeState_ButtonChangeAnimeEndWait() {
    m_294 = false;
}

void Pausewindow_c::initializeState_PauseDisp() {
    SelectCursorSetup();
}
void Pausewindow_c::executeState_PauseDisp() {
    if (m_292) {
        mStateMgr.changeState(StateID_ClouseAnimeEndWait);
    } else if (m_293) {
        mStateMgr.changeState(StateID_HitAnimeEndWait);
    } else {
        if (mCursor2 != mCursor) {
            SndAudioMgr::sInstance->startSystemSe(SE_SYS_CURSOR, 1);
            mStateMgr.changeState(StateID_ButtonChangeAnimeEndWait);
        }
    }
}
void Pausewindow_c::finalizeState_PauseDisp() {
    dSelectCursor_c::m_instance->Cancel(0);
}

void Pausewindow_c::initializeState_HitAnimeEndWait() {
    m_294 = true;
    mLayout.AnimeStartSetup(ANIM_HIT_TUZUKU + mCursor, false);
}
void Pausewindow_c::executeState_HitAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mStateMgr.changeState(StateID_ClouseAnimeEndWait);
    }
}
void Pausewindow_c::finalizeState_HitAnimeEndWait() {
    m_294 = false;
}

void Pausewindow_c::initializeState_ClouseAnimeEndWait() {
    m_294 = true;
    mLayout.AnimeStartSetup(ANIM_OUT_WINDOW, false);
}
void Pausewindow_c::executeState_ClouseAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mIsActive = false;
        mStateMgr.changeState(StateID_InitWait);
    }
}
void Pausewindow_c::finalizeState_ClouseAnimeEndWait() {
    m_294 = false;
}

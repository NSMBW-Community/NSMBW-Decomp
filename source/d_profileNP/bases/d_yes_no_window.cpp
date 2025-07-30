#include <game/bases/d_yes_no_window.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_cursor_select.hpp>
#include <game/snd/snd_audio_mgr.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <constants/message_list.h>
#include <constants/game_constants.h>
#include <constants/sound_list.h>
/// @file

STATE_DEFINE(dYesNoWindow_c, InitWait);
STATE_DEFINE(dYesNoWindow_c, OpenAnimeEndWait);
STATE_DEFINE(dYesNoWindow_c, ButtonOnStageAnimeEndWait);
STATE_DEFINE(dYesNoWindow_c, ButtonChangeAnimeEndWait);
STATE_DEFINE(dYesNoWindow_c, SelectWait);
STATE_DEFINE(dYesNoWindow_c, HitAnimeEndWait);
STATE_DEFINE(dYesNoWindow_c, ClouseAnimeEndWait);
STATE_DEFINE(dYesNoWindow_c, HitAnimeAfterWait);

BASE_PROFILE(YES_NO_WINDOW, dYesNoWindow_c);

dYesNoWindow_c::dYesNoWindow_c() :
    mStateMgr(*this, StateID_InitWait),
    mHasLoadedLayout(false) {}

dYesNoWindow_c::~dYesNoWindow_c() {}

int dYesNoWindow_c::create() {
    static const char *AnmNameTbl[] = {
        "yesnoWindow_11_inWindow.brlan",
        "yesnoWindow_11_loopWindow.brlan",
        "yesnoWindow_11_inYesNoButton.brlan",
        "yesnoWindow_11_onYesNoButton.brlan",
        "yesnoWindow_11_idleYesNoButton.brlan",
        "yesnoWindow_11_hitYesNoButton.brlan",
        "yesnoWindow_11_offYesNoButton.brlan",
        "yesnoWindow_11_inBG.brlan",
        "yesnoWindow_11_outBG.brlan",
        "yesnoWindow_11_outWindow.brlan"
    };

    static const int ANIME_INDEX_TBL[] = {0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 8, 9, 0};
    static const char *GROUP_NAME_DT[] = {
        "A00_inWindow",
        "D00_save00",
        "A00_inWindow",
        "D00_save00",
        "B00_yesButton_00",
        "B01_noButton_00",
        "B02_centerButton",
        "B00_yesButton_00",
        "B01_noButton_00",
        "B02_centerButton",
        "B00_yesButton_00",
        "B01_noButton_00",
        "B02_centerButton",
        "B00_yesButton_00",
        "B01_noButton_00",
        "B02_centerButton",
        "D00_save00",
        "B00_yesButton_00",
        "B01_noButton_00",
        "B02_centerButton",
        "C00_BG_00",
        "C00_BG_00",
        "A00_inWindow"
    };

    static const int MESSAGE_DATA_TBL[] = {MSG_OK, MSG_OK, MSG_WATCH_HINT_MOVIE, MSG_WATCH_HINT_MOVIE};
    static const char *T_PANE_FIXED_NAME_TBL[] = {
        "T_center_00",
        "T_center_01",
        "T_otehonTextS_00",
        "T_otehonText_00"
    };
    static const char *PPANE_NAME_DT[] = {
        "P_yesBase_00",
        "P_noBase_00",
        "P_centerBase_00"
    };
    static const char *T_PANE_NAME_TBL[] = {
        "T_questionS_00",
        "T_question_00",
        "T_otehonTextS_01",
        "T_otehonText_01",
        "T_needCoinX_00",
        "T_needCoin_00",
        "T_yes_00",
        "T_yes_01",
        "T_no_00",
        "T_no_01"
    };
    static const char *NPANE_NAME_DT[] = {
        "N_otehonText_00",
        "N_saveIcon_00"
    };

    if (mHasLoadedLayout) {
        return SUCCEEDED;
    }

    if (!mLayout.ReadResource("yesnoWindow/yesnoWindow.arc", false)) {
        return NOT_READY;
    }

    mLayout.build("yesnoWindow_11.brlyt", nullptr);
    mLayout.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(GROUP_NAME_DT));
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL, MESSAGE_DATA_TBL, BMG_CATEGORY_YES_NO_WINDOW, ARRAY_SIZE(T_PANE_FIXED_NAME_TBL));

    mLayout.mDrawOrder = 143;
    mpRootPane = mLayout.getRootPane();

    mLayout.PPaneRegister(PPANE_NAME_DT, &P_yesBase_00, ARRAY_SIZE(PPANE_NAME_DT));
    mLayout.TPaneRegister(T_PANE_NAME_TBL, &T_questionS_00, ARRAY_SIZE(T_PANE_NAME_TBL));
    mLayout.NPaneRegister(NPANE_NAME_DT, &N_otehonText_00, ARRAY_SIZE(NPANE_NAME_DT));

    mIsActive = false;
    mHasLoadedLayout = true;
    mKeepOpen = false;
    mStarCoinsRequired = 0;
    mCancel = false;
    mHideBG = false;

    mpRootPane->mFlags &= ~1;

    mLayout.AllAnimeEndSetup();
    mLayout.ReverseAnimeStartSetup(ANIM_IN_BG, false);
    mLayout.AnimePlay();
    mLayout.calc();
    return SUCCEEDED;
}

int dYesNoWindow_c::execute() {
    if (mHasLoadedLayout && mIsActive) {
        mStateMgr.executeState();
        mLayout.AnimePlay();
        mLayout.calc();
    } else if (mHideBG) {
        mLayout.calc();
    }
    return SUCCEEDED;
}

int dYesNoWindow_c::draw() {
    if (mHasLoadedLayout && mIsActive) {
        mLayout.entry();
    } else if (mHideBG) {
        mLayout.entry();
    }
    return SUCCEEDED;
}

int dYesNoWindow_c::doDelete() {
    return mLayout.doDelete();
}

/// @brief The prompt message for each type.
static const int MainMsgIDs[] = {
    MSG_SAVE_DATA_CREATED,          // SAVE_DATA_CREATED
    MSG_ASK_SAVE,                   // SAVE
    MSG_PROGRESS_SAVED,             // SAVED
    MSG_EXIT_FREE_MODE,             // EXIT_FREE_MODE
    MSG_WATCH_HINT_MOVIE,           // WATCH_UNLOCKED_HINT_MOVIE
    MSG_SKIP_COURSE,                // SKIP_COURSE
    MSG_WATCH_HINT_MOVIE,           // WATCH_HINT_MOVIE
    MSG_GOT_ALL_STAR_COINS_NO_W9,   // GOT_ALL_STAR_COINS
    MSG_GOT_ALL_STAR_COINS_W9,      // GOT_ALL_STAR_COINS_MENTION_W9
    MSG_TOADS_CAPTURED,             // BOWSERJR_SINGLEPLAYER
    MSG_RETURN_TO_TITLE,            // RETURN_TITLE
    MSG_TOADS_CAPTURED_MULTIPLAYER, // BOWSERJR_MULTIPLAYER
    MSG_BACK_TO_MAP,                // RETURN_TO_MAP
    MSG_BACK_TO_COURSE_SELECTION,   // BACK_TO_COURSE_SELECTION
    MSG_GOT_ALL_STAR_COINS_WORLD_X, // GOT_ALL_STAR_COINS_ALL_WORLDS
    MSG_RETURN_TO_TITLE_QUICK_SAVE, // QUICK_SAVE
    MSG_SAVED_RETURNING_TO_TITLE,   // QUICK_SAVED
    MSG_TOAD_HOUSES_BACK,           // ALL_TOAD_HOUSES_RETURNED
    MSG_ITEM_HINT,                  // ITEM_INFO
    MSG_TRY_WITHOUT_SUPER_GUIDE,    // COURSE_CLEAR_WITH_SUPER_GUIDE
    MSG_SKIP_COURSE,                // SKIP_COURSE_CONFIRM
    MSG_COMPLETED_EVERYTHING,       // COMPLETED_EVERYTHING
    MSG_PROGRESS_SAVED_FIRST_TIME,  // PROGRESS_AUTOSAVE_AFTER_W9_UNLOCK
    MSG_SWITCH_FIRST_TIME,          // BLOCK_TRANSFORMATION
    MSG_SWITCH,                     // BLOCK_TRANSFORMATION_REVERSE
    MSG_EXIT_COIN_BATTLE,           // EXIT_COIN_BATTLE
    MSG_BACK_TO_MAP2,               // RETURN_TO_MAP2
    MSG_PEACH_HINT,                 // PEACH_CASTLE_HINT1
    MSG_PEACH_HINT2                 // PEACH_CASTLE_HINT2
};

#define OPTIONS_YES_NO { true, true, false }
#define OPTIONS_OK { false, false, true }

/// @brief Which buttons are visible in the Yes/No window.
static const bool PicVisible[][3] = {
    OPTIONS_OK,     // SAVE_DATA_CREATED
    OPTIONS_YES_NO, // SAVE
    OPTIONS_OK,     // SAVED
    OPTIONS_YES_NO, // EXIT_FREE_MODE
    OPTIONS_YES_NO, // WATCH_UNLOCKED_HINT_MOVIE
    OPTIONS_OK,     // SKIP_COURSE
    OPTIONS_YES_NO, // WATCH_HINT_MOVIE
    OPTIONS_OK,     // GOT_ALL_STAR_COINS
    OPTIONS_OK,     // GOT_ALL_STAR_COINS_MENTION_W9
    OPTIONS_OK,     // BOWSERJR_SINGLEPLAYER
    OPTIONS_YES_NO, // RETURN_TITLE
    OPTIONS_OK,     // BOWSERJR_MULTIPLAYER
    OPTIONS_YES_NO, // RETURN_TO_MAP
    OPTIONS_YES_NO, // BACK_TO_COURSE_SELECTION
    OPTIONS_OK,     // GOT_ALL_STAR_COINS_ALL_WORLDS
    OPTIONS_YES_NO, // QUICK_SAVE
    OPTIONS_OK,     // QUICK_SAVED
    OPTIONS_OK,     // ALL_TOAD_HOUSES_RETURNED
    OPTIONS_OK,     // ITEM_INFO
    OPTIONS_YES_NO, // COURSE_CLEAR_WITH_SUPER_GUIDE
    OPTIONS_YES_NO, // SKIP_COURSE_CONFIRM
    OPTIONS_OK,     // COMPLETED_EVERYTHING
    OPTIONS_OK,     // PROGRESS_AUTOSAVE_AFTER_W9_UNLOCK
    OPTIONS_OK,     // BLOCK_TRANSFORMATION
    OPTIONS_OK,     // BLOCK_TRANSFORMATION_REVERSE
    OPTIONS_YES_NO, // EXIT_COIN_BATTLE
    OPTIONS_YES_NO, // RETURN_TO_MAP2
    OPTIONS_OK,     // PEACH_CASTLE_HINT1
    OPTIONS_OK      // PEACH_CASTLE_HINT2
};

/// @brief Starting cursor positions for the Yes/No window.
static const int StartingCursorPositions[] = {
    dYesNoWindow_c::POS_OK,  // SAVE_DATA_CREATED
    dYesNoWindow_c::POS_YES, // SAVE
    dYesNoWindow_c::POS_OK,  // SAVED
    dYesNoWindow_c::POS_NO,  // EXIT_FREE_MODE
    dYesNoWindow_c::POS_NO,  // WATCH_UNLOCKED_HINT_MOVIE
    dYesNoWindow_c::POS_OK,  // SKIP_COURSE
    dYesNoWindow_c::POS_NO,  // WATCH_HINT_MOVIE
    dYesNoWindow_c::POS_OK,  // GOT_ALL_STAR_COINS
    dYesNoWindow_c::POS_OK,  // GOT_ALL_STAR_COINS_MENTION_W9
    dYesNoWindow_c::POS_OK,  // BOWSERJR_SINGLEPLAYER
    dYesNoWindow_c::POS_NO,  // RETURN_TITLE
    dYesNoWindow_c::POS_OK,  // BOWSERJR_MULTIPLAYER
    dYesNoWindow_c::POS_NO,  // RETURN_TO_MAP
    dYesNoWindow_c::POS_NO,  // BACK_TO_COURSE_SELECTION
    dYesNoWindow_c::POS_OK,  // GOT_ALL_STAR_COINS_ALL_WORLDS
    dYesNoWindow_c::POS_YES, // QUICK_SAVE
    dYesNoWindow_c::POS_OK,  // QUICK_SAVED
    dYesNoWindow_c::POS_OK,  // ALL_TOAD_HOUSES_RETURNED
    dYesNoWindow_c::POS_OK,  // ITEM_INFO
    dYesNoWindow_c::POS_YES, // COURSE_CLEAR_WITH_SUPER_GUIDE
    dYesNoWindow_c::POS_NO,  // SKIP_COURSE_CONFIRM
    dYesNoWindow_c::POS_OK,  // COMPLETED_EVERYTHING
    dYesNoWindow_c::POS_OK,  // PROGRESS_AUTOSAVE_AFTER_W9_UNLOCK
    dYesNoWindow_c::POS_OK,  // BLOCK_TRANSFORMATION
    dYesNoWindow_c::POS_OK,  // BLOCK_TRANSFORMATION_REVERSE
    dYesNoWindow_c::POS_NO,  // EXIT_COIN_BATTLE
    dYesNoWindow_c::POS_NO,  // RETURN_TO_MAP2
    dYesNoWindow_c::POS_OK,  // PEACH_CASTLE_HINT1
    dYesNoWindow_c::POS_OK   // PEACH_CASTLE_HINT2
};

const int SoundEffects[] = { SE_SYS_BACK, SE_SYS_DECIDE, SE_SYS_CURSOR, SE_SYS_DIALOGUE_IN };

void dYesNoWindow_c::populateLayout() {
    static int fillLeftReqStarCoins = 2;
    MsgRes_c *msgRes = dMessage_c::getMesRes();

    int mainMsgID = MainMsgIDs[mType];
    if (mType == WATCH_HINT_MOVIE) {
        dGameCom::LayoutDispNumber(mStarCoinsRequired, fillLeftReqStarCoins, T_needCoin_00, true);

        T_otehonTextS_01->setMessage(msgRes, BMG_CATEGORY_HINT_MOVIES, MSG_STAR_COINS_REQUIRED, 0);
        T_otehonText_01->setMessage(msgRes, BMG_CATEGORY_HINT_MOVIES, MSG_STAR_COINS_REQUIRED, 0);
        T_needCoinX_00->setMessage(msgRes, BMG_CATEGORY_HINT_MOVIES, MSG_STAR_COINS_REQUIRED_X, 0);

        T_questionS_00->setVisible(false);
        N_otehonText_00->setVisible(true);
    } else {
        if (mType == GOT_ALL_STAR_COINS_ALL_WORLDS) {
            dInfo_c::m_instance->field_3b4 = dScWMap_c::m_WorldNo + 1;
        }
        if (mType == SKIP_COURSE_CONFIRM &&
            dInfo_c::m_startGameInfo.mWorld1 == WORLD_8 &&
            dInfo_c::m_startGameInfo.mLevel1 == STAGE_CASTLE)
        {
            mainMsgID = MSG_SKIP_FINAL_COURSE;
        }

        T_questionS_00->setVisible(true);
        N_otehonText_00->setVisible(false);
        T_questionS_00->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, mainMsgID, 0);
        T_question_00->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, mainMsgID, 0);
    }

    // Set visibility of Yes/No/OK buttons
    for (int i = 0; i < 3;) {
        bool isVisible = PicVisible[mType][i];
        getPicturePane(i++)->setVisible(isVisible);
    }

    mCursorPos = StartingCursorPositions[mType];

    // Set text in Yes/No/OK buttons
    int bmgIDForYesButton, bmgIDForNoButton;
    if (mType == SAVE) {
        bmgIDForYesButton = MSG_SAVE;
        bmgIDForNoButton = MSG_DONT_SAVE;
    } else if (mType == QUICK_SAVE) {
        bmgIDForYesButton = MSG_QUICK_SAVE;
        bmgIDForNoButton = MSG_DONT_SAVE;
    } else if (mType == COURSE_CLEAR_WITH_SUPER_GUIDE) {
        bmgIDForYesButton = MSG_OK_TRY;
        bmgIDForNoButton = MSG_NO_SKIP;
    } else if (mType == SKIP_COURSE_CONFIRM) {
        bmgIDForYesButton = MSG_YES_IM_SURE;
        bmgIDForNoButton = MSG_NO_WAIT;

        // Show a different message if the player wants to skip the final course
        if (dInfo_c::m_startGameInfo.mWorld1 == WORLD_8 && dInfo_c::m_startGameInfo.mLevel1 == STAGE_CASTLE) {
            bmgIDForYesButton = MSG_WATCH_ENDING;
        }
    } else {
        bmgIDForNoButton = MSG_CANCEL;
        if (mType == WATCH_UNLOCKED_HINT_MOVIE || mType == WATCH_HINT_MOVIE) {
            bmgIDForYesButton = MSG_SAVED_OK;
        } else {
            bmgIDForYesButton = MSG_OK;
        }
    }

    T_yes_00->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, bmgIDForYesButton, false);
    T_yes_01->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, bmgIDForYesButton, false);
    T_no_00->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, bmgIDForNoButton, false);
    T_no_01->setMessage(msgRes, BMG_CATEGORY_YES_NO_WINDOW, bmgIDForNoButton, false);

    if (mType == QUICK_SAVED) {
        N_saveIcon_00->setVisible(true);
    } else {
        N_saveIcon_00->setVisible(false);
    }
}

// ----------------
// StateID_InitWait
// ----------------

void dYesNoWindow_c::initializeState_InitWait() {
    if (mHideBG) {
        return;
    }
    mpRootPane->mFlags &= ~1;
}

void dYesNoWindow_c::executeState_InitWait() {
    mIsActive = true;
    mIsAnimating = false;
    mKeepOpen = false;

    mStateMgr.changeState(StateID_OpenAnimeEndWait);
}

void dYesNoWindow_c::finalizeState_InitWait() {}

// ------------------------
// StateID_OpenAnimeEndWait
// ------------------------

void dYesNoWindow_c::initializeState_OpenAnimeEndWait() {
    mCancel = false;
    mpRootPane->setVisible(true);
    mIsAnimating = true;

    mLayout.AllAnimeEndSetup();
    mLayout.ReverseAnimeStartSetup(ANIM_IN_YES, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_NO, false);
    mLayout.ReverseAnimeStartSetup(ANIM_IN_OK, false);

    switch (mType) {
        case SAVE:
        case SAVED:
        case EXIT_FREE_MODE:
        case WATCH_UNLOCKED_HINT_MOVIE:
        case WATCH_HINT_MOVIE:
        case RETURN_TITLE:
        case QUICK_SAVE:
        case QUICK_SAVED:
        case ITEM_INFO:
        case EXIT_COIN_BATTLE:
        case PEACH_CASTLE_HINT1:
        case PEACH_CASTLE_HINT2:
            if (!mHideBG) {
                mLayout.AnimeStartSetup(ANIM_IN_BG, false);
            }
            break;
    }
    if (mType == QUICK_SAVED) {
        mLayout.AnimeStartSetup(ANIM_IN_QUICK_SAVE, false);
    }
    mLayout.AnimeStartSetup(ANIM_IN_WINDOW, false);
    SndAudioMgr::sInstance->startSystemSe(SoundEffects[SOUND_OPEN_ANIME], 1);

    populateLayout();
}

void dYesNoWindow_c::executeState_OpenAnimeEndWait() {
    if (!mLayout.isAllAnime()) {
        mHitButton = false;
        mStateMgr.changeState(StateID_ButtonOnStageAnimeEndWait);
    }
}

void dYesNoWindow_c::finalizeState_OpenAnimeEndWait() {
    mIsAnimating = false;
    mHideBG = false;
}

// ---------------------------------
// StateID_ButtonOnStageAnimeEndWait
// ---------------------------------

void dYesNoWindow_c::initializeState_ButtonOnStageAnimeEndWait() {
    mIsAnimating = true;
    mLayout.AnimeStartSetup(ANIM_IN_YES, false);
    mLayout.AnimeStartSetup(ANIM_IN_NO, false);
    mLayout.AnimeStartSetup(ANIM_IN_OK, false);
}
void dYesNoWindow_c::executeState_ButtonOnStageAnimeEndWait() {
    if (!mLayout.isAllAnime()) {
        mStateMgr.changeState(StateID_ButtonChangeAnimeEndWait);
    }
}
void dYesNoWindow_c::finalizeState_ButtonOnStageAnimeEndWait() {
    mPrevCursorPos = -1;
    mIsAnimating = false;
}

// --------------------------------
// StateID_ButtonChangeAnimeEndWait
// --------------------------------

void dYesNoWindow_c::initializeState_ButtonChangeAnimeEndWait() {
    mIsAnimating = true;

    mLayout.AllAnimeEndSetup();
    if (mPrevCursorPos >= 0) {
        // ID 17 is the exit animation for button 0 (Yes)
        mLayout.AnimeStartSetup(ANIM_OFF_YES + mPrevCursorPos, false);
    }
    mPrevCursorPos = mCursorPos;

    // ID 7 is the in animation for button 0 (Yes)
    mLayout.AnimeStartSetup(ANIM_ON_YES + mCursorPos, false);
}

void dYesNoWindow_c::executeState_ButtonChangeAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mStateMgr.changeState(StateID_SelectWait);
    }
}

void dYesNoWindow_c::finalizeState_ButtonChangeAnimeEndWait() {
    mIsAnimating = false;
}

// ------------------
// StateID_SelectWait
// ------------------

void dYesNoWindow_c::initializeState_SelectWait() {
    if (mType == QUICK_SAVED) {
        mLayout.LoopAnimeStartSetup(ANIM_LOOP_QUICK_SAVE);
    }
    mLayout.LoopAnimeStartSetup(ANIM_LOOP_WINDOW);
    dGameCom::updateSelectCursor(getPicturePane(mCursorPos), 0, false);
}

void dYesNoWindow_c::executeState_SelectWait() {
    if (mCancel) {
        if (mType == QUICK_SAVED) {
            mLayout.AllAnimeEndSetup();
            mLayout.AnimeStartSetup(ANIM_HIT_QUICK_SAVE, false);
        }
        mStateMgr.changeState(StateID_ClouseAnimeEndWait);
        return;
    }

    if (mHitButton) {
        if (mType == RETURN_TO_MAP || mType == RETURN_TO_MAP2 || mType == BACK_TO_COURSE_SELECTION) {
            if (mCursorPos == POS_YES) {
                // Stop game music
                dAudio::pauseOffGameWithReset();
            }
        }
        mStateMgr.changeState(StateID_HitAnimeEndWait);
        return;
    }

    if (mPrevCursorPos != mCursorPos) {
        // Cursor moved

        SndAudioMgr::sInstance->startSystemSe(SoundEffects[SOUND_CURSOR_MOVE], 1);
        mStateMgr.changeState(StateID_ButtonChangeAnimeEndWait);
    }
}

void dYesNoWindow_c::finalizeState_SelectWait() {
    dSelectCursor_c::m_instance->Cancel(0);
}

// -----------------------
// StateID_HitAnimeEndWait
// -----------------------

void dYesNoWindow_c::initializeState_HitAnimeEndWait() {
    mLayout.AllAnimeEndSetup();
    mIsAnimating = true;
    mHitButton = false;

    if (mType == QUICK_SAVED) {
        mLayout.AnimeStartSetup(ANIM_HIT_QUICK_SAVE, false);
    }
    mLayout.AnimeStartSetup(ANIM_HIT_YES + mCursorPos, false);
    SndAudioMgr::sInstance->startSystemSe(SoundEffects[SOUND_WAIT], 1);
}

void dYesNoWindow_c::executeState_HitAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        if (mKeepOpen) {
            mStateMgr.changeState(StateID_HitAnimeAfterWait);
        } else {
            mStateMgr.changeState(StateID_ClouseAnimeEndWait);
        }
    }
}

void dYesNoWindow_c::finalizeState_HitAnimeEndWait() {
    mIsAnimating = false;
}

// --------------------------
// StateID_ClouseAnimeEndWait
// --------------------------

void dYesNoWindow_c::initializeState_ClouseAnimeEndWait() {
    mIsAnimating = true;
    mLayout.AllAnimeEndSetup();
    switch (mType) {
        case SAVE:
        case SAVED:
        case EXIT_FREE_MODE:
        case WATCH_UNLOCKED_HINT_MOVIE:
        case WATCH_HINT_MOVIE:
        case RETURN_TITLE:
        case QUICK_SAVE:
        case QUICK_SAVED:
        case ITEM_INFO:
        case EXIT_COIN_BATTLE:
        case PEACH_CASTLE_HINT1:
        case PEACH_CASTLE_HINT2:
            if (!mHideBG) {
                mLayout.AnimeStartSetup(ANIM_OUT_BG, false);
            }
            break;
    }
    SndAudioMgr::sInstance->startSystemSe(SoundEffects[SOUND_CLOSE], 1);
    mLayout.AnimeStartSetup(ANIM_OUT_WINDOW, false);
}

void dYesNoWindow_c::executeState_ClouseAnimeEndWait() {
    if (!mLayout.isAnime(-1)) {
        mIsActive = false;
        mStateMgr.changeState(StateID_InitWait);
    }
}

void dYesNoWindow_c::finalizeState_ClouseAnimeEndWait() {
    mIsAnimating = false;
}

// -------------------------
// StateID_HitAnimeAfterWait
// -------------------------

void dYesNoWindow_c::initializeState_HitAnimeAfterWait() {
    mHitButton = false;
}

void dYesNoWindow_c::executeState_HitAnimeAfterWait() {}

void dYesNoWindow_c::finalizeState_HitAnimeAfterWait() {}

#include <game/bases/d_gamedisplay.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <constants/sound_list.h>
#include <game/snd/snd_audio_mgr.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/mLib/m_fader.hpp>

BASE_PROFILE(GAMEDISPLAY, dGameDisplay_c)

STATE_DEFINE(dGameDisplay_c, ProcMainGame);
STATE_DEFINE(dGameDisplay_c, ProcMainPause);
STATE_DEFINE(dGameDisplay_c, ProcGoalSettleUp);
STATE_DEFINE(dGameDisplay_c, ProcGoalEnd);

dGameDisplay_c* dGameDisplay_c::m_instance;
const int dGameDisplay_c::c_COINNUM_DIGIT = 2;
const int dGameDisplay_c::c_PLAYNUM_DIGIT = 2;
const int dGameDisplay_c::c_TIME_DIGIT = 3;
const int dGameDisplay_c::c_SCORE_DIGIT = 8;


dGameDisplay_c::~dGameDisplay_c() {
    m_instance = nullptr;
}

dGameDisplay_c::dGameDisplay_c() :
    mStateMgr(*this, StateID_ProcMainGame),
    m_452(1),
    mHasLoadedLayout(false)
{
    m_instance = this;
}

int dGameDisplay_c::create() {
    if (mHasLoadedLayout) {
        return 1;
    }

    if (! createLayout()) {
        return 0;
    }

    mLayout.mDrawOrder = 0x11;

    mPlayNum[0] = -1;
    m_424[0] = 0;
    m_454 = 0;
    mPlayNum[1] = -1;
    m_424[1] = 0;
    m_455 = 0;
    mPlayNum[2] = -1;
    m_424[2] = 0;
    m_456 = 0;
    mPlayNum[3] = -1;
    m_424[3] = 0;

    m_457 = 0;
    mScore = 1;
    m_3E4 = 1;
    mCoins = 1;
    mTimer = -1;
    m_434 = 2;

    setPlayNum(mPlayNum);
    setCoinNum(0);
    setTime(0);
    setScore(0);

    m_438 = 0;
    m_440 = 0;
    m_3FC = -1;
    m_3F8 = -1;
    m_444 = 0;
    m_453 = 0;
    m_400 = 0;
    m_404 = 0x1e;
    m_408 = 0x1e;
    m_40C = 0;
    m_43C = 0;
    m_540 = 0;
    m_53C = 0;
    m_538 = 0;

    RestDispSetup();
    fn_801585c0();
    mLayout.AllAnimeEndSetup();

    if ((dScStage_c::m_gameMode == 2) || (dScStage_c::m_gameMode == 3) || dScStage_c::m_miniGame || (dInfo_c::m_startGameInfo.mLevel1 == STAGE_PEACH_CASTLE) || dScStage_c::m_isStaffCredit) {
        mpRootPane->setVisible(false);
    } else {
        mpRootPane->setVisible(true);
    }

    if (dScStage_c::m_gameMode == 1) {
        N_otasukeInfo_00->setVisible(true);
    } else {
        N_otasukeInfo_00->setVisible(false);
    }

    if (PauseManager_c::m_OtasukeAfter) {
        m_448 = 1;
        N_otasukeChu_00->setVisible(true);
        N_left_00->setVisible(false);
        mLayout.ReverseAnimeStartSetup(0, false);
    } else {
        m_448 = 0;
        N_otasukeChu_00->setVisible(false);
        N_left_00->setVisible(true);
    }

    if (dInfo_c::m_startGameInfo.mLevel1 == STAGE_CANNON) {
        N_collection_00->setVisible(false);
        N_score_00->setVisible(false);
        N_time_00->setVisible(false);
    } else {
        N_collection_00->setVisible(true);
        N_score_00->setVisible(true);
        N_time_00->setVisible(true);
    }

    if (dInfo_c::mGameFlag & 0x40) {
        N_score_00->setVisible(false);
    }


    dMj2dGame_c *save = dSaveMng_c::m_instance->getSaveGame(-1);
    u32 w = dInfo_c::m_startGameInfo.mWorld1;
    u32 l = dInfo_c::m_startGameInfo.mLevel1;
    if (w > 9) {
        w = 0;
    }
    if (l > STAGE_STAFFROLL) {
        l = STAGE_1;
    }

    if (!(dInfo_c::mGameFlag & 0x10) && !(dInfo_c::mGameFlag & 0x80000000) && !dScWMap_c::IsCourseType(w, l, dScWMap_c::COURSE_TYPE_NO_STAR_COINS)) {
        N_collection_00->setVisible(false);
    }

    for (u32 i = 0; i < 3; i++) {
        m_3EC[i] = 3;
        if ((dScStage_c::mCollectionCoin[i] != 4) || save->isCollectCoin(w, l, i)) {
            (&P_collectOff_00)[2 * i]->setVisible(false);
            (&P_collection_00)[2 * i]->setVisible(true);
            m_3EC[i] = 2;
        }
    }

    mVec2_c disp_scale;
    dGameCom::DispSizeScale(disp_scale);
    N_proportionL_00->setScale(disp_scale);
    N_proportionR_00->setScale(disp_scale);

    nw4r::lyt::Pane * a = N_proportionL_00;
    nw4r::lyt::Pane * b = N_proportionR_00;

    m_544.x = a->mPos.x;
    m_544.y = a->mPos.y;
    m_544.z = a->mPos.z;
    m_550.x = b->mPos.x;
    m_550.y = b->mPos.y;
    m_550.z = b->mPos.z;
    m_55C.x = N_otasukeChu_00->mPos.x;
    m_55C.y = N_otasukeChu_00->mPos.y;
    m_55C.z = N_otasukeChu_00->mPos.z;
    m_568.x = N_areaZanki_00->mPos.x;
    m_568.y = N_areaZanki_00->mPos.y;
    m_568.z = N_areaZanki_00->mPos.z;
    m_574.x = N_otasukeInfo_00->mPos.x;
    m_574.y = N_otasukeInfo_00->mPos.y;
    m_574.z = N_otasukeInfo_00->mPos.z;
    m_580.x = N_areaCoin_00->mPos.x;
    m_580.y = N_areaCoin_00->mPos.y;
    m_580.z = N_areaCoin_00->mPos.z;
    m_58C.x = N_areaScore_00->mPos.x;
    m_58C.y = N_areaScore_00->mPos.y;
    m_58C.z = N_areaScore_00->mPos.z;
    m_598.x = N_time_00->mPos.x;
    m_598.y = N_time_00->mPos.y;
    m_598.z = N_time_00->mPos.z;

    OtehonPosChange();

    m_449 = 0;
    m_44A = 0;
    mHasLoadedLayout = true;
    m_44C = 0;
    mAreaZankiAlpha = 0xFF;
    m_44D = 0;
    mAreaCoinAlpha = 0xFF;
    m_44E = 0;
    mAreaScoreAlpha = 0xFF;
    m_44F = 0;
    m_450 = 0;
    m_451 = 0;

    mLayout.calc();

    return 1;
}

int dGameDisplay_c::execute() {
    if (mHasLoadedLayout) {

        RestCoinAnimeCheck();
        mStateMgr.executeState();

        if (mLayout.isAllAnime() || m_450 || m_451) {
            mLayout.AnimePlay();
            mLayout.calc();
        }

        AreaCheck();
        AlphaEnterAndExit();

        // This doesn't match... the assembly seems to imply that this is some
        // kind of int[2][2], but that also doesn't match...
        if (m_424[0]) {
            m_424[0]--;
        }
        if (m_424[1]) {
            m_424[1]--;
        }
        if (m_424[2]) {
            m_424[2]--;
        }
        if (m_424[3]) {
            m_424[3]--;
        }
    }

    return 1;
}

int dGameDisplay_c::draw() {
    if (! m_452) {
        return 1;
    }

    if (mHasLoadedLayout) {
        mLayout.entry();
    }

    return 1;
}

int dGameDisplay_c::doDelete() {
    return mLayout.doDelete();
}

// Doesn't match - weird float trickery
void dGameDisplay_c::OtehonPosChange() {
    if (dScStage_c::m_gameMode != 4) {
        return;
    }

    float a = 0.0f;
    float b = 16.0f;
    float c = 23.0f;

    float d = a - c; // -23.0f
    float e = a - b; // -16.0f

    N_proportionL_00->mPos = mVec3_c(b, a, a) + m_544;

    // N_proportionL_00->mPos.x = m_544.x + b;
    // N_proportionL_00->mPos.y = m_544.y + a;
    // N_proportionL_00->mPos.z = m_544.z + a;

    N_proportionR_00->mPos.x = m_550.x + e;
    N_proportionR_00->mPos.y = m_550.y + a;
    N_proportionR_00->mPos.z = m_550.z + a;
    N_otasukeChu_00->mPos.x = m_55C.x + a;
    N_otasukeChu_00->mPos.y = m_55C.y + d;
    N_otasukeChu_00->mPos.z = m_55C.z + a;
    N_areaZanki_00->mPos.x = m_568.x + a;
    N_areaZanki_00->mPos.y = m_568.y + d;
    N_areaZanki_00->mPos.z = m_568.z + a;
    N_otasukeInfo_00->mPos.x = m_574.x + a;
    N_otasukeInfo_00->mPos.y = m_574.y + c;
    N_otasukeInfo_00->mPos.z = m_574.z + a;
    N_areaCoin_00->mPos.x = m_580.x + a;
    N_areaCoin_00->mPos.y = m_580.y + d;
    N_areaCoin_00->mPos.z = m_580.z + a;
    N_areaScore_00->mPos.x = m_58C.x + a;
    N_areaScore_00->mPos.y = m_58C.y + d;
    N_areaScore_00->mPos.z = m_58C.z + a;
    N_time_00->mPos.x = m_598.x + a;
    N_time_00->mPos.y = m_598.y + d;
    N_time_00->mPos.z = m_598.z + a;
}

// ???
void dGameDisplay_c::AreaSetup(int a, int b) {
    // ...
    m_458[a].left = (&N_otasukeInfo_00)[b]->m_8c;
    m_458[a].top = (&N_otasukeInfo_00)[b]->m_8c;
    m_458[a].right = (&N_otasukeInfo_00)[b]->m_9c;
    m_458[a].bottom = (&N_otasukeInfo_00)[b]->m_9c;
    // ...
    float w = (&N_otasukeInfo_00)[b]->width;
    float h = (&N_otasukeInfo_00)[b]->height;
    switch ((u8)((&N_otasukeInfo_00)[b]->mOriginType % 3)) {
        case 0:
            m_458[a].right += w;
            break;
        case 1:
            m_458[a].right -= w * 0.5f;
            m_458[a].bottom += w * 0.5f;
            break;
        case 2:
            m_458[a].right -= w;
            break;
    }

    switch ((u8)((&N_otasukeInfo_00)[b]->mOriginType / 3)) {
        case 0:
            m_458[a].top -= h;
            break;
        case 1:
            m_458[a].left += h * 0.5f;
            m_458[a].top -= h * 0.5f;
            break;
        case 2:
            m_458[a].top += h;
            break;
    }
}



void dGameDisplay_c::fn_801585c0() {
    for (int i = 0; i < 3; i++) {
        AreaSetup(i, i + 6);
    }

    if (m_414 != 6) {
        nw4r::lyt::Pane * pane = (&P_collectOff_00)[m_414];
        m_458[0].right = pane->m_8c + pane->width * 0.5f + 60.0f;
    }
}


void dGameDisplay_c::RestDispSetup() {
    mVec3_c iconPos[4];

    static const PLAYER_CHARACTER_e lbl_802F5C28[PLAYER_COUNT] = {
        PLAYER_MARIO,
        PLAYER_LUIGI,
        PLAYER_BLUE_TOAD,
        PLAYER_YELLOW_TOAD,
    };

    static const u32 lbl_802F5C38[PLAYER_COUNT] = {
        6, 7, 9, 8
    };

    iconPos[PLAYER_MARIO] = P_marioIcon_00->mPos;
    P_marioIcon_00->setVisible(false);
    iconPos[PLAYER_LUIGI] = P_luijiIcon_00->mPos;
    P_luijiIcon_00->setVisible(false);
    iconPos[PLAYER_BLUE_TOAD] = P_kinoB_00->mPos;
    P_kinoB_00->setVisible(false);
    iconPos[PLAYER_YELLOW_TOAD] = P_kinoY_00->mPos;
    P_kinoY_00->setVisible(false);

    int iconPosIdx = 0;

    for (int i = 0; i < PLAYER_COUNT; i++) {
        int idx = daPyMng_c::getPlayerIndex(lbl_802F5C28[i]);
        if (! daPyMng_c::mPlayerEntry[idx]) {
            continue;
        }

        (&P_collectOff_00)[lbl_802F5C38[i]]->setVisible(true);
        (&P_collectOff_00)[lbl_802F5C38[i]]->mPos = iconPos[iconPosIdx];
        iconPosIdx++;
        m_414 = i + 6;
    }
}

// Almost matches - up to static local offsets
bool dGameDisplay_c::createLayout() {
    static const char * AnmNameTbl[] = { "gameScene_37_inMarioCoin.brlan" };
    static const char * GROUP_NAME_DT[] = { "C00_inMarioCoin" };
    static const int MESSAGE_DATA_TBL[] = { 0x14, 0x14, 0x13, 0x13 };
    static const int ANIME_INDEX_TBL[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 6, 7, 8, 9,
    }; // ???
    static const char * NPANE_NAME_DT[] = {
        "N_otasukeInfo_00", "N_otasukeChu_00", "N_left_00", "N_coin_00",
        "N_collection_00", "N_score_00", "N_areaZanki_00", "N_areaCoin_00",
        "N_areaScore_00", "N_marioIcon_00", "N_luigiIcon_00", "N_kinoB_00",
        "N_kinoY_00", "N_coin_01", "N_time_00", "N_proportionL_00",
        "N_proportionR_00", "N_coin1st_00", "N_coin2nd_00", "N_coin3rd_00",
    };
    static const char * PPANE_NAME_DT[] = {
        "P_collectOff_00", "P_collection_00",
        "P_collectOff_01", "P_collection_01",
        "P_collectOff_02", "P_collection_02",
        "P_marioIcon_00", "P_luijiIcon_00",
        "P_kinoB_00", "P_kinoY_00",
    };
    static const char * T_PANE_NAME_DT[] = { "T_left_00", "T_left_01", "T_left_02", "T_left_03", "T_coin_00", "T_time_00", "T_score_00" };
    static const char * T_PANE_NAME_TBL[] = { "T_otaChuS_00", "T_otaChu_01", "T_InfoS_00", "T_Info_00" };

    bool ok = mLayout.ReadResource("gameScene/gameScene.arc", false);
    if (! ok) {
        return false;
    }

    mLayout.build("gameScene_37.brlyt", nullptr);
    mLayout.AnimeResRegister(AnmNameTbl, 1);
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, 1);

    mpRootPane = mLayout.getRootPane();
    mLayout.NPaneRegister(NPANE_NAME_DT, &N_otasukeInfo_00, 20);
    mLayout.PPaneRegister(PPANE_NAME_DT, &P_collectOff_00, 10);

    P_collectOff_00->setVisible(true);
    P_collection_00->setVisible(false);
    P_collectOff_01->setVisible(false); // [???]
    P_collectOff_01->setVisible(true);
    P_collection_01->setVisible(false);
    P_collectOff_02->setVisible(false); // [???]
    P_collectOff_02->setVisible(true);
    P_collection_02->setVisible(false);

    P_marioIcon_00->setVisible(false);

    mLayout.TPaneRegister(T_PANE_NAME_DT, mpTextBoxes, 11);
    mLayout.TPaneNameRegister(T_PANE_NAME_TBL, MESSAGE_DATA_TBL, 1, 4);

    return true;
}


void dGameDisplay_c::RestCoinAnimeCheck() {
    if (m_44A) {
        m_449 = 0;
    }
    if (! m_449) {
        return;
    }

    m_449 = 0;
    m_44A = 1;
    mLayout.AnimeStartSetup(0, false);

    if (dActorCreateMng_c::m_instance->m_bcb) {
        PauseManager_c::m_instance->mDisablePause = true;
    }
}


void dGameDisplay_c::AreaCheck() {
    if (! (mpRootPane->mFlags & 1)) {
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (! daPyMng_c::checkPlayer(i)) {
            continue;
        }

        dAcPy_c * player = daPyMng_c::getPlayer(i);
        if (! player) {
            continue;
        }

        mVec3_c pos = player->getCenterPos();
        dGameCom::getGlbPosToLyt(pos);

        if (!m_44D && m_458[0].right < pos.x && m_458[0].bottom > pos.x && m_458[0].left > pos.y && m_458[0].top < pos.y) {
            m_44D = 1;
            m_450 = 1;
        }

        if (!m_44E && m_458[1].right < pos.x && m_458[1].bottom > pos.x && m_458[1].left > pos.y && m_458[1].top < pos.y) {
            m_44E = 1;
            m_450 = 1;
        }

        if (!m_44F && m_458[2].right < pos.x && m_458[2].bottom > pos.x && m_458[2].left > pos.y && m_458[2].top < pos.y) {
            m_44F = 1;
            m_450 = 1;
        }
    }
}


void dGameDisplay_c::AlphaEnterAndExit() {
    if (! (mpRootPane->mFlags & 1)) {
        return;
    }

    int x = 0;
    int a = N_areaZanki_00->mAlpha;
    if (m_44D) {
        if (a <= 70) {
            x++;
        }
    } else if (a >= 0xFF) {
        x++;
    }

    a = N_areaCoin_00->mAlpha;
    if (m_44E) {
        if (a <= 70) {
            x++;
        }
    } else if (a >= 0xFF) {
        x++;
    }

    a = N_areaScore_00->mAlpha;
    if (m_44F) {
        if (a <= 70) {
            x++;
        }
    } else if (a >= 0xFF) {
        x++;
    }

    if (x >= 3) {
        m_450 = 0;
    } else {
        m_450 = 1;
    }

    int max_alpha = 0xFF;
    int min_alpha = 70;
    int step = 12;

    // Doesn't match - maybe uses an inline function?
    int d = step;
    if (m_44D) {
        d = -d;
    }
    mAreaZankiAlpha += d;
    if (mAreaZankiAlpha <= min_alpha) {
        mAreaZankiAlpha = min_alpha;
    }
    if (mAreaZankiAlpha >= max_alpha) {
        mAreaZankiAlpha = max_alpha;
    }

    N_areaZanki_00->mAlpha = mAreaZankiAlpha;
    m_44D = 0;

    int e = step;
    if (m_44E) {
        e = -e;
    }
    mAreaCoinAlpha += e;
    if (mAreaCoinAlpha <= min_alpha) {
        mAreaCoinAlpha = min_alpha;
    }
    if (mAreaCoinAlpha >= max_alpha) {
        mAreaCoinAlpha = max_alpha;
    }

    N_areaCoin_00->mAlpha = mAreaCoinAlpha;
    m_44E = 0;

    int f = step;
    if (m_44F) {
        f = -f;
    }
    mAreaScoreAlpha += f;
    if (mAreaScoreAlpha <= min_alpha) {
        mAreaScoreAlpha = min_alpha;
    }
    if (mAreaScoreAlpha >= max_alpha) {
        mAreaScoreAlpha = max_alpha;
    }

    N_areaScore_00->mAlpha = mAreaScoreAlpha;
    m_44F = 0;
}

bool dGameDisplay_c::NormalSettle() {
    short t = ((int)(dStageTimer_c::m_instance->mPreciseTime + 0xFFF) >> 12);
    bool ret;
    if (t > 0) {
        int x = m_40C;
        int score = 0;
        while (x > 0) {
            t--;
            score += m_410;
            if (t <= 0) {
                break;
            }
            x--;
        }

        dStageTimer_c::m_instance->setTimer((short) t);

        if (! (dInfo_c::mGameFlag & 0x40)) {
            SndAudioMgr::sInstance->holdSystemSe(SE_SYS_SCORE_COUNT, 1);
        }

        daPyMng_c::addScore(score, -1);
        ret = false;
    } else {
        ret = true;
    }

    return ret;
}

bool dGameDisplay_c::OtasukeSettle() {
    dStageTimer_c * timer = dStageTimer_c::m_instance;
    short t = ((int)(timer->mPreciseTime + 0xFFF) >> 12);
    int score = daPyMng_c::mScore;

    if (!t && !score && !mCoins) {
        return true;
    }

    if (mCoins) {
        mCoins += -3;
        if (mCoins < 0) {
            mCoins = 0;
        }
    }

    dGameCom::LayoutDispNumber(mCoins, c_COINNUM_DIGIT, mpCoinTextBox, false);

    if (t) {
        t -= m_40C;
        if (t < 0) {
            t = 0;
        }
    }
    timer->setTimer(t);

    if (score) {
        score -= m_410 * m_40C;
        if (score < 0) {
            score = 0;
        }
    }
    daPyMng_c::mScore = score;

    SndAudioMgr::sInstance->holdSystemSe(SE_SYS_SCORE_COUNT, 1);

    return false;


}

// ----------------
// StateID_ProcMainGame
// ----------------

void dGameDisplay_c::initializeState_ProcMainGame() {
    m_43C = 0;
    m_451 = 0;
    m_453 = 0;
}

void dGameDisplay_c::executeState_ProcMainGame() {
    if (m_438 != 1) {
        if (m_44C) {
            mStateMgr.changeState(StateID_ProcGoalSettleUp);
        }
        return;
    } else {
        m_440 = 0;
        mStateMgr.changeState(StateID_ProcMainPause);
    }
}

void dGameDisplay_c::finalizeState_ProcMainGame() {
    m_451 = 1;
}

// ----------------
// StateID_ProcMainPause
// ----------------

void dGameDisplay_c::initializeState_ProcMainPause() {
    m_43C = 1;
    m_438 = 0;
}

void dGameDisplay_c::executeState_ProcMainPause() {
    if (m_440 == 1) {
        mStateMgr.changeState(StateID_ProcMainGame);
    }
}
void dGameDisplay_c::finalizeState_ProcMainPause() {
    m_440 = 0;
}

// ----------------
// StateID_ProcGoalSettleUp
// ----------------

void dGameDisplay_c::initializeState_ProcGoalSettleUp() {
    m_40C = 10;
    m_410 = 0x32;
    m_444 = 1;
    m_453 = 1;
    m_400 = 0;

    dMultiMng_c::mspInstance->m_04 = (short)((int)(dStageTimer_c::m_instance->mPreciseTime + 0xFFF) >> 12);

    if (PauseManager_c::m_OtasukeAfter) {
        EffectCollectionCoinClear();
    }
}

void dGameDisplay_c::executeState_ProcGoalSettleUp() {
    if (m_400 < m_404) {
        m_400++;
    } else if (PauseManager_c::m_OtasukeAfter) {
        if (OtasukeSettle()) {
            m_400 = 0;
            SndAudioMgr::sInstance->startSystemSe(SE_SYS_SCORE_COUNT_FINISH, 1);
            mStateMgr.changeState(StateID_ProcGoalEnd);
        }
    } else {
        if (NormalSettle()) {
            m_400 = 0;
            if (!(dInfo_c::mGameFlag & 0x40)) {
                SndAudioMgr::sInstance->startSystemSe(SE_SYS_SCORE_COUNT_FINISH, 1);
            }
            mStateMgr.changeState(StateID_ProcGoalEnd);
        }
    }
}

void dGameDisplay_c::finalizeState_ProcGoalSettleUp() {}

// ----------------
// StateID_ProcGoalEnd
// ----------------

void dGameDisplay_c::initializeState_ProcGoalEnd() {}

void dGameDisplay_c::executeState_ProcGoalEnd() {
    if (m_400 < m_408) {
        m_400++;
    } else {
        m_444 = 0;
    }
}

void dGameDisplay_c::finalizeState_ProcGoalEnd() {}


void dGameDisplay_c::Effect1UP(int a) {
    mVec3_c vec;
    if (
        (mFader_c::mFader->getStatus() == mFaderBase_c::HIDDEN) &&
        !PauseManager_c::m_OtasukeAfter &&
        !dScStage_c::m_miniGame
    ) {
        if (dInfo_c::m_startGameInfo.mScreenType == 3) {

        } else if (m_424[a] == 0) {
            m_424[a] = 0xF;

            nw4r::lyt::Pane * icon = &N_otasukeInfo_00[9 + a];

            vec.x = icon->m_8c;
            vec.y = icon->m_9c;
            vec.z = 0.0f;

            // TODO: useless matrix copy?

            float b = vec.x;
            float c = vec.y;
            dGameCom::fn_800B37E0(vec, false);
            mEf::createEffect("Wm_2d_1up01", 0, &vec, nullptr, nullptr);

            vec.x = b;
            vec.y = c;
            vec.z = 0.0f;
            dGameCom::fn_800B37E0(vec, true);
            mEf::createEffect("Wm_2d_1up02", 0, &vec, nullptr, nullptr);
        }
    }
}

void dGameDisplay_c::GrayColorSet(int) {
    // ??? Ghidra hangs
}

void dGameDisplay_c::ReturnGrayColorSet(int) {
    // ??? Ghidra hangs
}

void dGameDisplay_c::EffectCollectionCoinClear() {
    static const int lbl_802f5cb8[] = { 0, 2, 4 };
    static const int lbl_802f5cc4[] = { 1, 3, 5 };

    for (int i = 0; i < 3; i++) {
        (&P_collectOff_00)[lbl_802f5cb8[i]]->setVisible(true);
        if ((&P_collectOff_00)[lbl_802f5cc4[i]]->mFlags & 1) { // isVisible
            nw4r::lyt::Pane * icon = &N_coin1st_00[i];

            mVec3_c vec;
            vec.x = icon->m_8c;
            vec.y = icon->m_9c;
            vec.z = 0.0f;

            // TODO: useless matrix copy?

            float b = vec.x;
            float c = vec.y;
            dGameCom::fn_800B37E0(vec, false);
            mEf::createEffect("Wm_2d_starcoinvanish", 0, &vec, nullptr, nullptr);
        }
    }
}

void dGameDisplay_c::EffectCollectionCoinGet(int i) {
    nw4r::lyt::Pane * icon = &N_coin1st_00[i];

    mVec3_c vec;
    vec.x = icon->m_8c;
    vec.y = icon->m_9c;
    vec.z = 0.0f;

    // TODO: useless matrix copy?

    float b = vec.x;
    float c = vec.y;
    dGameCom::fn_800B37E0(vec, false);
    mEf::createEffect("Wm_2d_starcoinget", 0, &vec, nullptr, nullptr);
}

void dGameDisplay_c::setPlayNum(int* life_nums) {
    static const int PANE_INDEX_TBL[PLAYER_COUNT] = { 0, 2, 4, 6 };

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (mPlayNum[i] == life_nums[i]) {
            continue;
        }

        if (mPlayNum[i] < life_nums[i]) {
            Effect1UP(i);
        }
        if (life_nums[i] == 0) {
            GrayColorSet(i);
        } else if (mPlayNum[i] == 0) {
            ReturnGrayColorSet(i);
        }

        mPlayNum[i] = life_nums[i];
        dGameCom::LayoutDispNumber(mPlayNum[i], c_PLAYNUM_DIGIT, mpTextBoxes[PANE_INDEX_TBL[i]], false);
    }
}

void dGameDisplay_c::setCoinNum(int num_coins) {
    if (!m_453 && (mCoins != num_coins)) {
        mCoins = num_coins;
        dGameCom::LayoutDispNumber(mCoins, c_COINNUM_DIGIT, mpTextBoxes[8], false);

        // The fader status check doesn't match...
        if (!num_coins && !m_444 && (mFader_c::mFader->getStatus() == mFaderBase_c::HIDDEN)) {
            if (dInfo_c::m_startGameInfo.mScreenType == 2 || dInfo_c::m_startGameInfo.mScreenType == 3) {
                return;
            }

            nw4r::lyt::Pane * pane = N_coin_01;
            mVec3_c vec;

            vec.x = pane->m_8c;
            vec.y = pane->m_9c;
            vec.z = 0.0f;

            // TODO: useless matrix copy?

            float b = vec.x;
            float c = vec.y;
            dGameCom::fn_800B37E0(vec, false);
            mEf::createEffect("Wm_2d_coin100", 0, &vec, nullptr, nullptr);
        }
    }
}

void dGameDisplay_c::setTime(int time) {
    if (mTimer == time) {
        return;
    }

    mTimer = time;
    dGameCom::LayoutDispNumber(mTimer, c_TIME_DIGIT, mpTextBoxes[9], true);
}

void dGameDisplay_c::setCollect() {
    if (m_453) {
        return;
    }

    dMj2dGame_c *save = dSaveMng_c::m_instance->getSaveGame(-1);

    u8 w = dScStage_c::m_instance->mCurrWorld;
    u8 l = dScStage_c::m_instance->mCurrCourse;
    if ((w >= 10) || (l >= STAGE_COUNT)) {
        return;
    }

    if ((dInfo_c::m_startGameInfo.mScreenType == 2) || (dInfo_c::m_startGameInfo.mScreenType == 3)) {
        return;
    }

    for (u32 coin_id = 0; coin_id < 3; coin_id++) {
        if (save->isCollectCoin(w, l, coin_id) && (dInfo_c::m_startGameInfo.mScreenType == 0)) {
            (&P_collectOff_00)[coin_id]->setVisible(false);
            (&P_collection_00)[coin_id]->setVisible(true);
            if (m_3EC[coin_id] != 2) {
                m_3EC[coin_id] = 2;
            }
        } else if (dScStage_c::mCollectionCoin[coin_id] != 4) {
            (&P_collectOff_00)[coin_id]->setVisible(false);
            (&P_collection_00)[coin_id]->setVisible(true);
            if (m_3EC[coin_id] != 2) {
                m_3EC[coin_id] = 2;
                EffectCollectionCoinGet(coin_id);
            }
        } else {
            (&P_collectOff_00)[coin_id]->setVisible(true);
            (&P_collection_00)[coin_id]->setVisible(false);
            if (m_3EC[coin_id] != 0) {
                m_3EC[coin_id] = 0;
            }
        }
    }
}

#define MAX_SCORE       99999950
void dGameDisplay_c::setScore(int score) {
    if ((mScore == score) || (mScore >= MAX_SCORE)) {
        return;
    }

    // Only let 1 in 3 calls to 'setScore' actually update the scrore.
    if (++m_434 < 2) {
        return;
    }

    m_434 = 0;
    mScore = score;
    if (score >= MAX_SCORE) {
        mScore = MAX_SCORE;
    }

    dGameCom::LayoutDispNumber(mScore, c_SCORE_DIGIT, mpTextBoxes[10], true);
}

void dGameDisplay_c::RestCoinAnimeSetup() {
    if (PauseManager_c::m_OtasukeAfter) {
        m_449 = 1;
    }
}

#include <game/bases/d_gamedisplay.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_com.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <constants/sound_list.h>
#include <game/snd/snd_audio_mgr.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/mLib/m_fader.hpp>

ACTOR_PROFILE(GAMEDISPLAY, dGameDisplay_c, 0);

STATE_DEFINE(dGameDisplay_c, ProcMainGame);
STATE_DEFINE(dGameDisplay_c, ProcMainPause);
STATE_DEFINE(dGameDisplay_c, ProcGoalSettleUp);
STATE_DEFINE(dGameDisplay_c, ProcGoalEnd);

dGameDisplay_c *dGameDisplay_c::m_instance;
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

    if (!createLayout()) {
        return 0;
    }

    mLayout.mDrawOrder = 0x11;

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mPlayNum[i] = -1;
        m_424[i] = 0;
        m_454[i] = 0;
    }

    m_454[3] = 0;
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
        mpRootPane->SetVisible(false);
    } else {
        mpRootPane->SetVisible(true);
    }

    if (dScStage_c::m_gameMode == 1) {
        mpNullPanes[N_otasukeInfo_00]->SetVisible(true);
    } else {
        mpNullPanes[N_otasukeInfo_00]->SetVisible(false);
    }

    if (PauseManager_c::m_OtasukeAfter) {
        m_448 = 1;
        mpNullPanes[N_otasukeChu_00]->SetVisible(true);
        mpNullPanes[N_left_00]->SetVisible(false);
        mLayout.ReverseAnimeStartSetup(0, false);
    } else {
        m_448 = 0;
        mpNullPanes[N_otasukeChu_00]->SetVisible(false);
        mpNullPanes[N_left_00]->SetVisible(true);
    }

    if (dInfo_c::m_startGameInfo.mLevel1 == STAGE_CANNON) {
        mpNullPanes[N_collection_00]->SetVisible(false);
        mpNullPanes[N_score_00]->SetVisible(false);
        mpNullPanes[N_time_00]->SetVisible(false);
    } else {
        mpNullPanes[N_collection_00]->SetVisible(true);
        mpNullPanes[N_score_00]->SetVisible(true);
        mpNullPanes[N_time_00]->SetVisible(true);
    }

    if (dInfo_c::mGameFlag & 0x40) {
        mpNullPanes[N_score_00]->SetVisible(false);
    }


    dMj2dGame_c *save = dSaveMng_c::m_instance->getSaveGame(-1);
    u32 w = dInfo_c::m_startGameInfo.mWorld1;
    u32 l = dInfo_c::m_startGameInfo.mLevel1;
    if (w > WORLD_USED_COUNT) {
        w = 0;
    }
    if (l > STAGE_STAFFROLL) {
        l = STAGE_1;
    }

    if (!(dInfo_c::mGameFlag & 0x10) && !(dInfo_c::mGameFlag & 0x80000000) && !dScWMap_c::IsCourseType(w, l, dScWMap_c::COURSE_TYPE_NO_STAR_COINS)) {
        mpNullPanes[N_collection_00]->SetVisible(false);
    }

    for (u32 i = 0; i < 3; i++) {
        m_3EC[i] = 3;
        if ((dScStage_c::mCollectionCoin[i] != 4) || save->isCollectCoin(w, l, i)) {
            mpPicturePanes[P_collectOff_00 + 2 * i]->SetVisible(false);
            mpPicturePanes[P_collection_00 + 2 * i]->SetVisible(true);
            m_3EC[i] = 2;
        }
    }

    mVec2_c disp_scale;
    dGameCom::DispSizeScale(disp_scale);
    mpNullPanes[N_proportionL_00]->SetScale(disp_scale);
    mpNullPanes[N_proportionR_00]->SetScale(disp_scale);

    m_544 = mpNullPanes[N_proportionL_00]->GetTranslate();
    m_550 = mpNullPanes[N_proportionR_00]->GetTranslate();
    m_55C = mpNullPanes[N_otasukeChu_00]->GetTranslate();
    m_568 = mpNullPanes[N_areaZanki_00]->GetTranslate();
    m_574 = mpNullPanes[N_otasukeInfo_00]->GetTranslate();
    m_580 = mpNullPanes[N_areaCoin_00]->GetTranslate();
    m_58C = mpNullPanes[N_areaScore_00]->GetTranslate();
    m_598 = mpNullPanes[N_time_00]->GetTranslate();

    OtehonPosChange();

    m_449 = 0;
    m_44A = 0;
    mHasLoadedLayout = true;
    m_44C = 0;
    for (int i = 0; i < 3; i++) {
        mAreaAlpha[i] = 0xFF;
        m_44D[i] = 0;
    }
    m_450 = 0;
    m_451 = 0;

    mLayout.calc();

    return SUCCEEDED;
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

        for (int i = 0; i < PLAYER_COUNT; i++) {
            if (m_424[i]) {
                m_424[i]--;
            }
        }
    }

    return SUCCEEDED;
}

int dGameDisplay_c::draw() {
    if (!m_452) {
        return 1;
    }

    if (mHasLoadedLayout) {
        mLayout.entry();
    }

    return SUCCEEDED;
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
    float offs1 = 16.0f;
    float offs2 = 23.0f;

    float x = a + offs1; // 16.0f
    float e = a - offs1; // -16.0f

    float d = a - offs2; // -23.0f
    float f = a + offs2; // 23.0f

    mVec3_c v;

    v.setToShifted(m_544, x, 0.0f, 0.0f);
    mpNullPanes[N_proportionL_00]->SetTranslate(v);

    v.setToShifted(m_550, e, 0.0f, 0.0f);
    mpNullPanes[N_proportionR_00]->SetTranslate(v);

    v.setToShifted(m_55C, 0.0f, d, 0.0f);
    mpNullPanes[N_otasukeChu_00]->SetTranslate(v);

    v.setToShifted(m_568, 0.0f, d, 0.0f);
    mpNullPanes[N_areaZanki_00]->SetTranslate(v);

    v.setToShifted(m_574, 0.0f, f, 0.0f);
    mpNullPanes[N_otasukeInfo_00]->SetTranslate(v);

    v.setToShifted(m_580, 0.0f, d, 0.0f);
    mpNullPanes[N_areaCoin_00]->SetTranslate(v);

    v.setToShifted(m_58C, 0.0f, d, 0.0f);
    mpNullPanes[N_areaScore_00]->SetTranslate(v);

    v.setToShifted(m_598, 0.0f, d, 0.0f);
    mpNullPanes[N_time_00]->SetTranslate(v);

    mLayout.calc();
}

void dGameDisplay_c::AreaSetup(int a, int areaPane) {
    nw4r::math::MTX34 mtx = mpNullPanes[areaPane]->GetGlobalMtx();
    m_458[a].right = mtx._03;
    m_458[a].bottom = mtx._03;
    m_458[a].left = mtx._13;
    m_458[a].top = mtx._13;
    nw4r::lyt::Size size = mpNullPanes[areaPane]->GetSize();

    float w = size.width;
    switch (mpNullPanes[areaPane]->GetBasePositionH()) {
        case 0:
            m_458[a].bottom += w;
            break;
        case 1:
            w /= 2;
            m_458[a].right -= w;
            m_458[a].bottom += w;
            break;
        case 2:
            m_458[a].right -= w;
            break;
    }

    float h = size.height;
    switch (mpNullPanes[areaPane]->GetBasePositionV()) {
        case 0:
            m_458[a].top -= h;
            break;
        case 1:
            h /= 2;
            m_458[a].left += h;
            m_458[a].top -= h;
            break;
        case 2:
            m_458[a].left += h;
            break;
    }
}



void dGameDisplay_c::fn_801585c0() {
    for (int i = 0; i < 3; i++) {
        AreaSetup(i, N_areaZanki_00 + i);
    }

    if (m_414 != P_marioIcon_00) {
        nw4r::lyt::Pane *pane = mpPicturePanes[m_414];
        nw4r::math::MTX34 mtx = pane->GetGlobalMtx();
        nw4r::lyt::Size size = pane->GetSize();

        float f = mtx._03 + size.width * 0.5f;
        f += 60.0f;
        m_458[0].bottom = f;
    }
}


void dGameDisplay_c::RestDispSetup() {
    mVec3_c iconPos[4];

    static const u32 lbl_802F5C38[PLAYER_COUNT] = {
        P_marioIcon_00, P_luijiIcon_00, P_kinoY_00, P_kinoB_00,
    };

    iconPos[PLAYER_MARIO] = mpPicturePanes[P_marioIcon_00]->GetTranslate();
    mpPicturePanes[P_marioIcon_00]->SetVisible(false);
    iconPos[PLAYER_LUIGI] = mpPicturePanes[P_luijiIcon_00]->GetTranslate();
    mpPicturePanes[P_luijiIcon_00]->SetVisible(false);
    iconPos[PLAYER_YELLOW_TOAD] = mpPicturePanes[P_kinoB_00]->GetTranslate();
    mpPicturePanes[P_kinoB_00]->SetVisible(false);
    iconPos[PLAYER_BLUE_TOAD] = mpPicturePanes[P_kinoY_00]->GetTranslate();
    mpPicturePanes[P_kinoY_00]->SetVisible(false);

    int iconPosIdx = 0;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        int idx = daPyMng_c::getPlayerIndex(daPyCom_c::sc_PLAYER_ORDER[i]);
        if (!daPyMng_c::mPlayerEntry[idx]) {
            continue;
        }

        mpPicturePanes[lbl_802F5C38[i]]->SetVisible(true);
        mpPicturePanes[lbl_802F5C38[i]]->SetTranslate(iconPos[iconPosIdx]);
        iconPosIdx++;
        m_414 = P_marioIcon_00 + i;
    }
}

bool dGameDisplay_c::createLayout() {
    static const char *AnmNameTbl[] = {
        "gameScene_37_inMarioCoin.brlan"
    };

    static const char *GROUP_NAME_DT[] = {
        "C00_inMarioCoin"
    };
    static const int ANIME_INDEX_TBL[] = {
        0
    };

    static const int MESSAGE_DATA_TBL[] = {
        0x14, 0x14, 0x13, 0x13
    };

    static const char *NPANE_NAME_DT[] = {
        "N_otasukeInfo_00", "N_otasukeChu_00",
        "N_left_00",
        "N_coin_00",
        "N_collection_00",
        "N_score_00",
        "N_areaZanki_00", "N_areaCoin_00", "N_areaScore_00",
        "N_marioIcon_00", "N_luigiIcon_00", "N_kinoB_00", "N_kinoY_00",
        "N_coin_01",
        "N_time_00",
        "N_proportionL_00", "N_proportionR_00",
        "N_coin1st_00", "N_coin2nd_00", "N_coin3rd_00"
    };

    static const char *PPANE_NAME_DT[] = {
        "P_collectOff_00", "P_collection_00",
        "P_collectOff_01", "P_collection_01",
        "P_collectOff_02", "P_collection_02",
        "P_marioIcon_00", "P_luijiIcon_00",
        "P_kinoB_00", "P_kinoY_00"
    };

    static const char *T_PANE_NAME_DT[] = {
        "T_left_00", "T_x_01",
        "T_left_01", "T_x_02",
        "T_left_02", "T_x_03",
        "T_left_03", "T_x_04",
        "T_coin_00",
        "T_time_00",
        "T_score_00"
    };

    static const char *T_PANE_NAME_TBL[] = {
        "T_otaChuS_00", "T_otaChu_01",
        "T_InfoS_00", "T_Info_00"
    };

    if (!mLayout.ReadResource("gameScene/gameScene.arc", false)) {
        return false;
    }

    mLayout.build("gameScene_37.brlyt", nullptr);
    mLayout.AnimeResRegister(AnmNameTbl, 1);
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, 1);

    mpRootPane = mLayout.getRootPane();
    mLayout.NPaneRegister(NPANE_NAME_DT, mpNullPanes, N_COUNT);
    mLayout.PPaneRegister(PPANE_NAME_DT, mpPicturePanes, P_COUNT);

    mpPicturePanes[P_collectOff_00]->SetVisible(true);
    mpPicturePanes[P_collection_00]->SetVisible(false);
    mpPicturePanes[P_collectOff_01]->SetVisible(false); // [???]
    mpPicturePanes[P_collectOff_01]->SetVisible(true);
    mpPicturePanes[P_collection_01]->SetVisible(false);
    mpPicturePanes[P_collectOff_02]->SetVisible(false); // [???]
    mpPicturePanes[P_collectOff_02]->SetVisible(true);
    mpPicturePanes[P_collection_02]->SetVisible(false);

    mpPicturePanes[P_marioIcon_00]->SetVisible(false);

    mLayout.TPaneRegister(T_PANE_NAME_DT, mpTextBoxes, T_COUNT);
    mLayout.TPaneNameRegister(T_PANE_NAME_TBL, MESSAGE_DATA_TBL, 1, 4);

    return true;
}


void dGameDisplay_c::RestCoinAnimeCheck() {
    if (m_44A) {
        m_449 = 0;
    }
    if (!m_449) {
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
    if (!mpRootPane->IsVisible()) {
        return;
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (!daPyMng_c::checkPlayer(i)) {
            continue;
        }

        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player == nullptr) {
            continue;
        }

        mVec3_c pos = player->getCenterPos();
        dGameCom::getGlbPosToLyt(pos);

        for (int i = 0; i < 3; i++) {
            if (!m_44D[i] && m_458[i].right < pos.x && m_458[i].bottom > pos.x && m_458[i].left > pos.y && m_458[i].top < pos.y) {
                m_44D[i] = 1;
                m_450 = 1;
            }
        }
    }
}


void dGameDisplay_c::AlphaEnterAndExit() {
    if (!mpRootPane->IsVisible()) {
        return;
    }

    int x = 0;
    for (int i = 0; i < 3; i++) {
        int a = mpNullPanes[N_areaZanki_00 + i]->GetAlpha();
        if (m_44D[i]) {
            if (a <= 70) {
                x++;
            }
        } else if (a >= 0xFF) {
            x++;
        }
    }

    if (x >= 3) {
        m_450 = 0;
    } else {
        m_450 = 1;
    }

    int max_alpha = 0xFF;
    int min_alpha = 70;
    int step = 12;

    for (int i = 0; i < 3; i++) {
        int d = step;
        if (m_44D[i]) {
            d *= -1;
        }
        mAreaAlpha[i] += d;
        if (mAreaAlpha[i] <= min_alpha) {
            mAreaAlpha[i] = min_alpha;
        }
        if (mAreaAlpha[i] >= max_alpha) {
            mAreaAlpha[i] = max_alpha;
        }

        mpNullPanes[N_areaZanki_00 + i]->SetAlpha(mAreaAlpha[i]);
        m_44D[i] = 0;
    }
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

        if (!(dInfo_c::mGameFlag & 0x40)) {
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
    dStageTimer_c *timer = dStageTimer_c::m_instance;
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

    dGameCom::LayoutDispNumber(mCoins, c_COINNUM_DIGIT, mpTextBoxes[N_areaScore_00], false);

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
    if (
        !mFader_c::mFader->isHidden() ||
        PauseManager_c::m_OtasukeAfter ||
        dScStage_c::m_miniGame ||
        dInfo_c::m_startGameInfo.mScreenType == 3
    ) {
        return;
    }
    if (m_424[a] == 0) {
        m_424[a] = 0xF;

        nw4r::lyt::Pane *icon = mpNullPanes[N_marioIcon_00 + a];

        nw4r::math::MTX34 mtx = icon->GetGlobalMtx();

        mVec3_c tmp;
        tmp.x = mtx._03;
        tmp.y = mtx._13;
        tmp.z = 0.0f;
        dGameCom::fn_800B37E0(tmp, false);
        mEf::createEffect("Wm_2d_1up01", 0, &tmp, nullptr, nullptr);

        tmp.x = mtx._03;
        tmp.y = mtx._13;
        tmp.z = 0.0f;
        dGameCom::fn_800B37E0(tmp, true);
        mEf::createEffect("Wm_2d_1up02", 0, &tmp, nullptr, nullptr);
    }
}

void dGameDisplay_c::GrayColorSet(int player) {
    if (m_454[player] == 0) {
        static nw4r::ut::Color GrayColor(160, 160, 160, 255);
        static const int TPANE_IDX_TBL[] = {
            T_left_00, T_x_01,
            T_left_01, T_x_02,
            T_left_02, T_x_03,
            T_left_03, T_x_04
        };
        static const int PPANE_IDX_TBL[] = { P_marioIcon_00, P_luijiIcon_00, P_kinoB_00, P_kinoY_00 };

        m_454[player] = true;
        nw4r::lyt::Pane *icon1 = mpPicturePanes[PPANE_IDX_TBL[player]];
        nw4r::lyt::Material *mat1 = icon1->GetMaterial();
        mColorBackup[0][player] = mat1->GetTevColor(1);
        mat1->SetTevColor(1, nw4r::g3d::detail::GetRGBAS10(GrayColor));
        nw4r::lyt::Pane *icon2 = mpTextBoxes[TPANE_IDX_TBL[player * 2]];
        nw4r::lyt::Material *mat2 = icon2->GetMaterial();
        mColorBackup[1][player] = mat2->GetTevColor(1);
        mat2->SetTevColor(1, nw4r::g3d::detail::GetRGBAS10(GrayColor));
        nw4r::lyt::Pane *icon3 = mpTextBoxes[TPANE_IDX_TBL[player * 2 + 1]];
        nw4r::lyt::Material *mat3 = icon3->GetMaterial();
        mColorBackup[2][player] = mat3->GetTevColor(1);
        mat3->SetTevColor(1, nw4r::g3d::detail::GetRGBAS10(GrayColor));
    }
}

void dGameDisplay_c::ReturnGrayColorSet(int player) {
    if (m_454[player] != 0) {
        static const int TPANE_IDX_TBL[] = {
            T_left_00, T_x_01,
            T_left_01, T_x_02,
            T_left_02, T_x_03,
            T_left_03, T_x_04
        };
        static const int PPANE_IDX_TBL[] = { P_marioIcon_00, P_luijiIcon_00, P_kinoB_00, P_kinoY_00 };

        m_454[player] = 0;
        nw4r::lyt::Pane *icon1 = mpPicturePanes[PPANE_IDX_TBL[player]];
        icon1->GetMaterial()->SetTevColor(1, mColorBackup[0][player]);
        nw4r::lyt::Pane *icon2 = mpTextBoxes[TPANE_IDX_TBL[player * 2]];
        icon2->GetMaterial()->SetTevColor(1, mColorBackup[1][player]);
        nw4r::lyt::Pane *icon3 = mpTextBoxes[TPANE_IDX_TBL[player * 2 + 1]];
        icon3->GetMaterial()->SetTevColor(1, mColorBackup[2][player]);
    }
}

void dGameDisplay_c::EffectCollectionCoinClear() {
    static const int lbl_802f5cb8[] = { 0, 2, 4 };
    static const int lbl_802f5cc4[] = { 1, 3, 5 };

    for (int i = 0; i < 3; i++) {
        mpPicturePanes[lbl_802f5cb8[i]]->SetVisible(true);
        if (mpPicturePanes[lbl_802f5cc4[i]]->IsVisible()) {
            nw4r::lyt::Pane *icon = mpNullPanes[N_coin1st_00 + i];

            nw4r::math::MTX34 mtx = icon->GetGlobalMtx();

            mVec3_c tmp;
            tmp.x = mtx._03;
            tmp.y = mtx._13;
            tmp.z = 0.0f;
            dGameCom::fn_800B37E0(tmp, false);
            mEf::createEffect("Wm_2d_starcoinvanish", 0, &tmp, nullptr, nullptr);
        }
        mpPicturePanes[lbl_802f5cc4[i]]->SetVisible(false);
    }
}

void dGameDisplay_c::EffectCollectionCoinGet(int i) {
    nw4r::lyt::Pane *icon = mpNullPanes[N_coin1st_00 + i];

    nw4r::math::MTX34 mtx = icon->GetGlobalMtx();

    mVec3_c tmp;
    tmp.x = mtx._03;
    tmp.y = mtx._13;
    tmp.z = 0.0f;
    dGameCom::fn_800B37E0(tmp, false);
    mEf::createEffect("Wm_2d_starcoinget", 0, &tmp, nullptr, nullptr);
}

void dGameDisplay_c::setPlayNum(int *life_nums) {
    static const int PANE_INDEX_TBL[PLAYER_COUNT] = {
        T_left_00, T_left_01, T_left_02, T_left_03
    };

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
        dGameCom::LayoutDispNumber(mCoins, c_COINNUM_DIGIT, mpTextBoxes[T_coin_00], false);

        if (num_coins || m_444 || !mFader_c::mFader->isHidden()) {
            return;
        }
        if (dInfo_c::m_startGameInfo.mScreenType == 2 || dInfo_c::m_startGameInfo.mScreenType == 3) {
            return;
        }

        nw4r::lyt::Pane *pane = mpNullPanes[N_coin_01];

        nw4r::math::MTX34 mtx = pane->GetGlobalMtx();

        mVec3_c tmp;
        tmp.x = mtx._03;
        tmp.y = mtx._13;
        tmp.z = 0.0f;

        dGameCom::fn_800B37E0(tmp, false);
        mEf::createEffect("Wm_2d_coin100", 0, &tmp, nullptr, nullptr);
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
    u8 l = dScStage_c::m_instance->getCourse();
    u8 w = dScStage_c::m_instance->getWorld();
    if (w >= WORLD_COUNT || l >= STAGE_COUNT) {
        return;
    }

    if (dInfo_c::m_startGameInfo.mScreenType == 2 || dInfo_c::m_startGameInfo.mScreenType == 3) {
        return;
    }

    for (u32 coin_id = 0; coin_id < 3; coin_id++) {
        if (save->isCollectCoin(w, l, coin_id) && (dInfo_c::m_startGameInfo.mScreenType == 0)) {
            mpPicturePanes[P_collectOff_00 + 2 * coin_id]->SetVisible(false);
            mpPicturePanes[P_collection_00 + 2 * coin_id]->SetVisible(true);
            if (m_3EC[coin_id] != 2) {
                m_3EC[coin_id] = 2;
            }
        } else if (dScStage_c::mCollectionCoin[coin_id] != 4) {
            mpPicturePanes[P_collectOff_00 + 2 * coin_id]->SetVisible(false);
            mpPicturePanes[P_collection_00 + 2 * coin_id]->SetVisible(true);
            if (m_3EC[coin_id] != 2) {
                m_3EC[coin_id] = 2;
                EffectCollectionCoinGet(coin_id);
            }
        } else {
            mpPicturePanes[P_collectOff_00 + 2 * coin_id]->SetVisible(true);
            mpPicturePanes[P_collection_00 + 2 * coin_id]->SetVisible(false);
            if (m_3EC[coin_id] != 0) {
                m_3EC[coin_id] = 0;
            }
        }
    }
    mLayout.calc();
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

    dGameCom::LayoutDispNumber(mScore, c_SCORE_DIGIT, mpTextBoxes[T_score_00], true);
}

void dGameDisplay_c::RestCoinAnimeSetup() {
    if (PauseManager_c::m_OtasukeAfter) {
        m_449 = 1;
    }
}

#include <game/bases/d_s_restart_crsin.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_s_crsin_static.hpp>

dInfo_c::StartGameInfo_s dScRestartCrsin_c::m_startGameInfo;

void dScRestartCrsin_c::startTitle(unsigned char isReplay, bool forceChange) {
    static const u8 c_stage[15][2] = {
        {WORLD_1, STAGE_4},
        {WORLD_1, STAGE_6},
        {WORLD_2, STAGE_1},
        {WORLD_2, STAGE_4},
        {WORLD_3, STAGE_1},
        {WORLD_3, STAGE_5},
        {WORLD_3, STAGE_TOWER},
        {WORLD_4, STAGE_1},
        {WORLD_4, STAGE_5},
        {WORLD_5, STAGE_4},
        {WORLD_5, STAGE_5},
        {WORLD_5, STAGE_GHOST_HOUSE},
        {WORLD_6, STAGE_CASTLE},
        {WORLD_7, STAGE_6},
        {WORLD_8, STAGE_7}
    };

    if (!isReplay) {
        m_startGameInfo.mWorld1 = WORLD_1;
        m_startGameInfo.mLevel1 = STAGE_TITLE;
        m_startGameInfo.mGameMode = dInfo_c::GAME_MODE_TITLE;
        m_startGameInfo.mIsReplay = 0;
    } else {
        m_startGameInfo.mWorld1 = c_stage[dScStage_c::m_titleRandomTable[dScStage_c::m_titleCount]][0];
        m_startGameInfo.mLevel1 = c_stage[dScStage_c::m_titleRandomTable[dScStage_c::m_titleCount]][1];
        m_startGameInfo.mGameMode = dInfo_c::GAME_MODE_TITLE_REPLAY;
        m_startGameInfo.mIsReplay = 1;
    }

    m_startGameInfo.mWorld2 = m_startGameInfo.mWorld1;
    m_startGameInfo.mLevel2 = m_startGameInfo.mLevel1;
    m_startGameInfo.mArea = 0;
    m_startGameInfo.mEntrance = 0xFF;

    dFader_c::setFader(dFader_c::FADER_FADE);
    dScCrsin_c::m_isDispOff = true;
    dScene_c::setNextScene(fProf::RESTART_CRSIN, forceChange, forceChange);
}

void dScRestartCrsin_c::reStartPeachCastle() {
    m_startGameInfo.mGameMode = dInfo_c::GAME_MODE_NORMAL;
    m_startGameInfo.mIsReplay = false;
    m_startGameInfo.mArea = 0;
    m_startGameInfo.mEntrance = 0xFF;
    m_startGameInfo.mWorld1 = WORLD_1;
    m_startGameInfo.mLevel1 = STAGE_PEACH_CASTLE;
    m_startGameInfo.mWorld2 = WORLD_1;
    m_startGameInfo.mLevel2 = STAGE_PEACH_CASTLE;

    dFader_c::setFader(dFader_c::FADER_CIRCLE_MIDDLE);
    dScCrsin_c::m_isDispOff = true;
    dScStage_c::m_exitMode = dScStage_c::EXIT_2;

    dScene_c::setNextScene(fProf::RESTART_CRSIN, 0, false);
}

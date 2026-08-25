#include <game/bases/d_s_restart_crsin.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_WarningManager.hpp>
#include <game/bases/d_fader.hpp>
#include <game/bases/d_system.hpp>

BASE_PROFILE(RESTART_CRSIN, dScRestartCrsin_c);

dScRestartCrsin_c::dScRestartCrsin_c() {}

dScRestartCrsin_c::~dScRestartCrsin_c() {}

int dScRestartCrsin_c::create() {
    if (m_startGameInfo.mGameMode == dInfo_c::GAME_MODE_TITLE) {
        dGameCom::initGame();
    }

    dWarningManager_c::m_WarningForbid++;
    dFader_c::setFader(dFader_c::FADER_FADE);
    dScene_c::setFadeInOutFrame(1);

    nw4r::ut::Color color;
    color = 0xFF;
    dSys_c::setClearColor(color);

    return SUCCEEDED;
}

int dScRestartCrsin_c::preExecute() {
    return dScene_c::preExecute() != 0;
}

int dScRestartCrsin_c::execute() {
    dInfo_c::m_instance->startGame(m_startGameInfo);

    return SUCCEEDED;
}

int dScRestartCrsin_c::doDelete() {
    dScene_c::setFadeInFrame(30);
    dScene_c::setFadeOutFrame(30);

    return SUCCEEDED;
}

int dScRestartCrsin_c::draw() {
    return SUCCEEDED;
}

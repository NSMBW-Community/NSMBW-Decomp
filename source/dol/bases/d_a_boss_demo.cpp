#include <game/bases/d_a_boss_demo.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/sLib/s_GlobalData.hpp>

STATE_VIRTUAL_DEFINE(daBossDemo_c, Ready);
STATE_VIRTUAL_DEFINE(daBossDemo_c, BattleStDemo);
STATE_VIRTUAL_DEFINE(daBossDemo_c, BattleIn);
STATE_VIRTUAL_DEFINE(daBossDemo_c, BattleEdDemo);

GLOBAL_DATA_DEF(daBossDemo_c, {
    false
});

int daBossDemo_c::create() {
    if (dActorMng_c::m_instance->mpCurrBossDemo != nullptr) {
        deleteRequest();
        return NOT_READY;
    }

    dActorMng_c::m_instance->mpCurrBossDemo = this;
    initialize();
    mStateMgr.changeState(StateID_Ready);
    mStateMgr.refreshState();
    return SUCCEEDED;
}

int daBossDemo_c::execute() {
    mStateMgr.executeState();
    return SUCCEEDED;
}

int daBossDemo_c::draw() {
    return SUCCEEDED;
}

void daBossDemo_c::deleteReady() {}

int daBossDemo_c::doDelete() {
    return SUCCEEDED;
}

void daBossDemo_c::setBattleResult() {
    dMultiMng_c *multiMng = dMultiMng_c::mspInstance;
    if (multiMng == nullptr) {
        return;
    }

    multiMng->setCollectionCoin();
    for (int i = 0; i < PLAYER_COUNT; i++) {
        int rest = daPyMng_c::getRest(daPyMng_c::getPlayerType(i));
        multiMng->setRest(rest, i);
    }
}

void daBossDemo_c::setMultiCourseClear() {
    if (daPyMng_c::getNum() == daPyMng_c::getEntryNum()) {
        dGameCom::setMultiCourseClear(3);
    } else {
        dGameCom::setMultiCourseClear(1);
    }
}

bool daBossDemo_c::abandonRetryAfterOtehonClear() {
    dScStage_c::setNextScene(fProf::WORLD_MAP, 0, dScStage_c::EXIT_0, dFader_c::FADER_MARIO);
    return false;
}

void daBossDemo_c::retryAfterOtehonClear() {
    dScStage_c::setNextScene(fProf::WORLD_MAP, 0, dScStage_c::EXIT_2, dFader_c::FADER_CIRCLE_MIDDLE);
}

bool daBossDemo_c::checkStartRetry(bool b) {
    if (GLOBAL_DATA.mForceOtehonClear) {
        dOtehonClear_c *otehon = dScStage_c::m_OtehonClear_p;
        otehon->m_b9 = b;
        otehon->m_b8 = true;
        otehon->m_b5 = true;
        return true;
    }

    if (isOtehonClear()) {
        dOtehonClear_c *otehon = dScStage_c::m_OtehonClear_p;
        otehon->m_b9 = b;
        otehon->m_b8 = true;
        otehon->m_b5 = true;
        return true;
    }

    return false;
}

bool daBossDemo_c::isOtehonClear() {
    if (GLOBAL_DATA.mForceOtehonClear) {
        return true;
    }

    bool res = false;
    int world = dScStage_c::m_instance->mCurrWorld;
    int course = dScStage_c::m_instance->mCurrCourse;
    if (
        dInfo_c::m_startGameInfo.mGameMode == dInfo_c::GAME_MODE_SUPER_GUIDE &&
        !dWmLib::IsCourseClear(world, course)
    ) {
        res = true;
    }
    return res;
}

void daBossDemo_c::initializeState_Ready() {}
void daBossDemo_c::finalizeState_Ready() {}
void daBossDemo_c::executeState_Ready() {
    if (mBossID == BASE_ID_NULL) {
        dEnBoss_c *boss = bossSearch();
        if (boss != nullptr) {
            mBossID = boss->getID();
        }
    }

    if (mBossID != BASE_ID_NULL && checkBattleStDemo() && demoScroll()) {
        mStateMgr.changeState(StateID_BattleStDemo);
    }
}

void daBossDemo_c::initializeState_BattleStDemo() {}
void daBossDemo_c::finalizeState_BattleStDemo() {}
void daBossDemo_c::executeState_BattleStDemo() {}

void daBossDemo_c::initializeState_BattleIn() {
    daPyDemoMng_c::mspInstance->releaseDemoMode(1);

    dEnBoss_c *boss = (dEnBoss_c *) fManager_c::searchBaseByID(mBossID);
    if (boss != nullptr) {
        boss->setBattleReady();
    }

    mBattleEnd = false;
}
void daBossDemo_c::finalizeState_BattleIn() {}
void daBossDemo_c::executeState_BattleIn() {
    if (checkBattleEdDemo()) {
        mStateMgr.changeState(StateID_BattleEdDemo);
    }
}

void daBossDemo_c::initializeState_BattleEdDemo() {}
void daBossDemo_c::finalizeState_BattleEdDemo() {}
void daBossDemo_c::executeState_BattleEdDemo() {}

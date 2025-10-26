#include <game/bases/d_cyuukan.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>

dCyuukan_c::dCyuukan_c() {
    clear();
}

void dCyuukan_c::clear() {
    mAmbushType = 0;
    mWorldNo = -1;
    mCourseNo = -1;
    mFileNo = -1;
    mNextGoto = -1;

    // [Yes, we set it later again... this is to fix a regswap]
    mHitPlayer[0] = PLAYER_COUNT;

    mIsKinopioInChuukan = false;
    mPlayerSetPos.x = 0.0f;
    mPlayerSetPos.y = 0.0f;
    mActiveCheckpoint = -1;

    for (int i = 0; i < CHECKPOINT_COUNT; i++) {
        mHitPlayer[i] = PLAYER_COUNT;
    }
    for (int i = 0; i < ARRAY_SIZE(mCoinCollection); i++) {
        mCoinCollection[i] = PLAYER_COUNT;
    }
}


void dCyuukan_c::courseIN() {
    dInfo_c *info = dInfo_c::m_instance;
    u8 world = dInfo_c::m_startGameInfo.mWorld1;
    u8 course = dInfo_c::m_startGameInfo.mLevel1;
    if (STAGE_KINOKO_HOUSE <= course && course <= STAGE_KINOKO_HOUSE_7 || course == STAGE_PEACH_CASTLE) {
        return;
    }
    if (
        info->m_3b3 != 0 ||
        dInfo_c::m_startGameInfo.mScreenType != 0 ||
        course != mCourseNo || world != mWorldNo
    ) {
        clear();
        return;
    }
    if (!checkEntry()) {
        clear();
    }
}

bool dCyuukan_c::checkEntry() {
    for (int i = 0; i < 2; i++) {
        if (mHitPlayer[i] != PLAYER_COUNT) {
            int idx = daPyMng_c::getPlayerIndex(mHitPlayer[i]);
            if (!dGameCom::PlayerEnterCheck(idx)) {
                return false;
            }
        }
    }
    return true;
}

void dCyuukan_c::setCyuukanData(int checkpointIndex, u8 nextGoto, s8 player, ulong ambushType) {
    dScStage_c *stage = dScStage_c::getInstance();
    PLAYER_TYPE_e hitPlayer = daPyMng_c::getPlayerType(player);
    mWorldNo = stage->mCurrWorld;
    mCourseNo = stage->mCurrCourse;
    mFileNo = stage->mCurrFile;
    mNextGoto = nextGoto;
    mHitPlayer[checkpointIndex] = hitPlayer;
    mAmbushType = ambushType & 0xFF;
    mActiveCheckpoint = checkpointIndex;
    setPos(daPyMng_c::getPlayerSetPos(stage->mCurrFile, nextGoto));
    for (int i = 0; i < ARRAY_SIZE(mCoinCollection); i++) {
        mCoinCollection[i] = dScStage_c::mCollectionCoin[i];
    }
}

bool dCyuukan_c::isCyuukanStart(int idx, u8 world, u8 course) {
    if (mActiveCheckpoint == -1) {
        return false;
    }
    if (world == 0xff || course == 0xff) {
        return false;
    }
    if (mHitPlayer[idx] == PLAYER_COUNT || course != mCourseNo || world != mWorldNo) {
        return false;
    }
    return true;
}

s8 dCyuukan_c::getPlrNo(int idx) const {
    if (mActiveCheckpoint == -1) {
        return -1;
    }
    if (mHitPlayer[idx] == PLAYER_COUNT) {
        return -1;
    }
    return daPyMng_c::getPlayerIndex(mHitPlayer[idx]);
}

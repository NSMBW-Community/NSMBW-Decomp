#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_SmallScore.hpp>
#include <constants/sound_list.h>

dMultiMng_c *dMultiMng_c::mspInstance;

dMultiMng_c::dMultiMng_c() {
    mspInstance = this;
}

dMultiMng_c::~dMultiMng_c() {}

void dMultiMng_c::initStage() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        mScore[i] = 0;
        mCoin[i] = 0;
        mBattleCoin[i] = 0;
        mEnemyDown[i] = 0;
        mCollectionCoin[i] = 0;
    }
}

void dMultiMng_c::setClapSE() {
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    mVec2_c pos(
        bgParam->xStart() + bgParam->xSize() / 2,
        bgParam->yStart() - bgParam->ySize() / 2
    );
    dAudio::SoundEffectID_t(SE_SYS_NICE_S).playMapSound(pos, 0);
}

void dMultiMng_c::setRest(int rest, int playerNo) {
    mRest[playerNo] = rest;
}

void dMultiMng_c::addScore(int value, int plrNo) {
    daPyMng_c::changeItemKinopioPlrNo(plrNo);
    mScore[plrNo] += value;
    if (mScore[plrNo] > MAX_EXTRA_MODE_SCORE) {
        mScore[plrNo] = MAX_EXTRA_MODE_SCORE;
    }
}

void dMultiMng_c::incCoin(int plrNo) {
    daPyMng_c::changeItemKinopioPlrNo(plrNo);
    mCoin[plrNo]++;
}

void dMultiMng_c::incEnemyDown(int plrNo) {
    daPyMng_c::changeItemKinopioPlrNo(plrNo);
    mEnemyDown[plrNo]++;
}

void dMultiMng_c::setBattleCoin(int plrNo, int value) {
    if (
        (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_4) &&
        (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_IS_COIN_COURSE)
    ) {
        mBattleCoin[plrNo] += value;

        dSmallScore_c::POPUP_TYPE_e popupType = dSmallScore_c::POPUP_TYPE_INVALID;
        switch (value) {
            case 2: popupType = dSmallScore_c::POPUP_TYPE_COIN_2; break;
            case 3: popupType = dSmallScore_c::POPUP_TYPE_COIN_3; break;
            case 5: popupType = dSmallScore_c::POPUP_TYPE_COIN_5; break;
            case 10: popupType = dSmallScore_c::POPUP_TYPE_COIN_10; break;
            case 15: popupType = dSmallScore_c::POPUP_TYPE_COIN_15; break;
            case 20: popupType = dSmallScore_c::POPUP_TYPE_COIN_20; break;
        }
        if (popupType != dSmallScore_c::POPUP_TYPE_INVALID) {
            dAcPy_c *player = daPyMng_c::getCtrlPlayer(plrNo);
            if (player == nullptr) {
                return;
            }
            mVec3_c popupPos = player->mPos;
            popupPos.y += player->mSomeYOffset;
            dGameCom::CreateSmallScore(popupPos, popupType, plrNo, false);
        }
    }
}

void dMultiMng_c::setCollectionCoin() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        mCollectionCoin[i] = 0;
    }

    /// @unofficial
    static const int masks[STAR_COIN_COUNT] = { 1, 2, 4 };
    for (int i = 0; i < STAR_COIN_COUNT; i++) {
        int coin = dScStage_c::mCollectionCoin[i];
        if (coin != PLAYER_COUNT) {
            mCollectionCoin[coin] |= masks[i];
        }
    }
}

#include <game/bases/d_a_nice_coin.hpp>
#include <game/bases/d_a_nice_coin_regular.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_multi_manager.hpp>

ACTOR_PROFILE(AC_NICE_COIN, daNiceCoin_c, 0);

ACTOR_PROFILE(AC_NICE_COIN_REGULAR, daNiceCoinRegular_c, 0);

STATE_DEFINE(daNiceCoin_c, Search);
STATE_DEFINE(daNiceCoin_c, EndWait);

int daNiceCoin_c::create() {
    mNiceType = TYPE_NICE_COIN;
    if (mProfName == fProf::AC_NICE_COIN_REGULAR) {
        mNiceType = TYPE_NICE_COIN_REGULAR;
    }

    if (dBg_c::m_bg_p->CoinGetBitCheck(mPos.x, -mPos.y, m_mbgchoice_keep)) {
        return FAILED;
    }

    u32 param = mParam;
    mCollectType = TYPE_NORMAL;

    mCollectionID = ACTOR_PARAM_LOCAL(param, CollectionID);
    mMaxCoinNum = ACTOR_PARAM_LOCAL(param, CoinNum);
    mMaxBlueCoinNum = ACTOR_PARAM_LOCAL(param, BlueCoinNum);

    if ((mMaxCoinNum != 0) && (mMaxBlueCoinNum != 0)) {
        mCollectType = TYPE_BOTH;
    }
    if ((mMaxCoinNum == 0) && (mMaxBlueCoinNum != 0)) {
        mCollectType = TYPE_BLUE;
    }

    mStateMgr.changeState(StateID_Search);
    return SUCCEEDED;
}

int daNiceCoin_c::execute() {
    mStateMgr.executeState();
    return SUCCEEDED;
}

int daNiceCoin_c::draw() {
    return SUCCEEDED;
}

int daNiceCoin_c::doDelete() {
    return SUCCEEDED;
}

void daNiceCoin_c::initializeState_Search() {}
void daNiceCoin_c::finalizeState_Search() {}
void daNiceCoin_c::executeState_Search() {
    int type = mCollectType;
    bool notCompleted = false;

    if (((type == TYPE_NORMAL) || (type == TYPE_BOTH)) && (mCollectedCoinNum < mMaxCoinNum)) {
        notCompleted = true;
    }
    if (((!notCompleted) && (type - 1U <= 1)) && (mCollectedBlueCoinNum < mMaxBlueCoinNum)) {
        notCompleted = true;
    }

    if (!notCompleted) {
        dMultiMng_c::mspInstance->setClapSE();
        dBg_c::m_bg_p->CoinGetBitSet(mPos.x, -mPos.y, mLayer);
        mStateMgr.changeState(StateID_EndWait);
    }
}

void daNiceCoin_c::initializeState_EndWait() {}
void daNiceCoin_c::finalizeState_EndWait() {}
void daNiceCoin_c::executeState_EndWait() {}

daNiceCoin_c::~daNiceCoin_c() {}

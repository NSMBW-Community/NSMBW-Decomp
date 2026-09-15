#include <game/bases/d_a_nice_coin.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_multi_manager.hpp>

typedef daNiceCoin_c daNiceCoinRegular_c;

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
        // Already activated, don't spawn the actor.
        return FAILED;
    }

    mCollectionID = ACTOR_PARAM(CollectionID);
    mReqCoinNum = ACTOR_PARAM(CoinNum);
    mReqBlueCoinNum = ACTOR_PARAM(BlueCoinNum);

    mCollectType = TYPE_NORMAL;
    if (mReqCoinNum != 0 && mReqBlueCoinNum != 0) {
        mCollectType = TYPE_BOTH;
    }
    if (mReqCoinNum == 0 && mReqBlueCoinNum != 0) {
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
    bool notCompleted = false;

    if ((mCollectType == TYPE_NORMAL || mCollectType == TYPE_BOTH) && mCollectedCoinNum < mReqCoinNum) {
        notCompleted = true;
    }
    if (!notCompleted && (mCollectType == TYPE_BLUE || mCollectType == TYPE_BOTH) && mCollectedBlueCoinNum < mReqBlueCoinNum) {
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

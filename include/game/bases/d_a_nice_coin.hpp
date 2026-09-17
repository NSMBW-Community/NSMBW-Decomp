#pragma once
#include <game/bases/d_actor_state.hpp>

/// @brief A tracker for collected coins and blue coins.
/// @details Once all the paired coins are collected, a clap sound will be played.
/// @paramtable
/// @statetable
/// @ingroup bases
class daNiceCoin_c : public dActorState_c {
public:

    /// @brief The possible collection types.
    enum COLLECT_TYPE_e {
        TYPE_NORMAL, ///< Normal coins.
        TYPE_BLUE, ///< Blue coins.
        TYPE_BOTH, ///< Both normal and blue coins.
    };

    /// @brief The "Nice Coin" type of this actor.
    enum NICE_TYPE_e {
        TYPE_NICE_COIN, ///< AC_NICE_COIN
        TYPE_NICE_COIN_REGULAR, ///< AC_NICE_COIN_REGULAR
    };

    daNiceCoin_c() {} ///< @copydoc dActorState_c::dActorState_c
    virtual ~daNiceCoin_c() {} ///< @copydoc dActorState_c::~dActorState_c

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();

    STATE_FUNC_DECLARE(daNiceCoin_c, Search); ///< Checking if coins are collected.
    STATE_FUNC_DECLARE(daNiceCoin_c, EndWait); ///< Empty state after the coins were collected.

    int mCollectionID; ///< The ID used to check for collected coins.
    COLLECT_TYPE_e mCollectType; ///< The type of coins being tracked.
    NICE_TYPE_e mNiceType; ///< The type of this actor.

    int mReqCoinNum; ///< The number of coins that need to be collected.
    int mReqBlueCoinNum; ///< The number of blue coins that need to be collected.

    int mCollectedCoinNum; ///< The number of coins collected so far.
    int mCollectedBlueCoinNum; ///< The number of blue coins collected so far.

    ACTOR_PARAM_CONFIG(CollectionID, 0, 4); ///< The ID to pair coins with.
    ACTOR_PARAM_CONFIG(CoinNum, 8, 8); ///< The number of coins to collect.
    ACTOR_PARAM_CONFIG(BlueCoinNum, 16, 8); ///< The number of blue coins to collect.
};

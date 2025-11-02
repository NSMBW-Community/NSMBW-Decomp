#pragma once

#include <types.h>
#include <constants/game_constants.h>

/// @brief Multiplayer manager.
/// Keeps track of statistics for each player,
/// used for Coin Battle and Free Mode.
class dMultiMng_c {
public:
    dMultiMng_c(); ///< Creates a new multiplayer manager.
    virtual ~dMultiMng_c(); ///< Destroys the multiplayer manager.

    void initStage(); ///< Initializes all counters to zero.
    void setClapSE(); ///< Plays the clap sound effect.
    void setRest(int rest, int plrNo); ///< Sets the remaining lives for a player.
    void addScore(int value, int plrNo); ///< Adds to the score of a player.
    void incCoin(int plrNo); ///< Increments the coin count for a player.
    void incEnemyDown(int plrNo); ///< Increments the enemy kill count for a player.
    void setBattleCoin(int plrNo, int value);  ///< Rewards a battle coin to a player.
    void setCollectionCoin(); ///< Loads the star coin collection information.

    int m_04; ///< @unused
    int mRest[PLAYER_COUNT]; ///< The number of remaining lives for each player.
    int mScore[PLAYER_COUNT]; ///< The score of each player.
    int mCoin[PLAYER_COUNT]; ///< The coin count for each player.
    int mEnemyDown[PLAYER_COUNT]; ///< The enemy kill count for each player.
    int mBattleCoin[PLAYER_COUNT]; ///< The battle coin count for each player.
    /// @brief The star coin collection status for each player.
    /// Each byte is a bitfield representing whether each star coin has been collected
    /// by the corresponding player.
    /// 0x1 is the first star coin, 0x2 is the second, and 0x4 is the third.
    u8 mCollectionCoin[PLAYER_COUNT];

    static dMultiMng_c *mspInstance; ///< The instance of this class.
};

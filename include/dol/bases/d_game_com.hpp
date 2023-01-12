#pragma once
#include <types.h>

/// @brief A collection of various functions used throughout the game.
/// @details Name stands for Game Common.
namespace dGameCom {
    u32 getRandomSeed(); ///< Gets a seed used for randomness.
    void clearGameStop();
}

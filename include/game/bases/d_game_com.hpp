#pragma once
#include <types.h>
#include <lib/nw4r/lyt/lyt_picture.hpp>

/// @brief A collection of various functions used throughout the game.
/// @details Name stands for Game Common.
/// @ingroup bases
namespace dGameCom {
    u32 getRandomSeed(); ///< Gets a seed used for randomness.
    void clearGameStop();
    float getDispCenterY();

    bool isGameStop(unsigned long);
    
    /// @unofficial
    void updateSelectCursor(nw4r::lyt::Picture *pic, int index, bool useSpecialDraw);
}

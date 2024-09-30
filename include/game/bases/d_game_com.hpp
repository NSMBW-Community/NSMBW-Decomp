#pragma once
#include <types.h>
#include <lib/nw4r/lyt/lyt_picture.hpp>
#include <game/mLib/m3d/m3d.hpp>

/// @brief A collection of various functions used throughout the game.
/// @details Name stands for Game Common.
/// @ingroup bases
namespace dGameCom {
    u32 getRandomSeed(); ///< Gets a seed used for randomness.
    void clearGameStop();
    float getDispCenterY();

    bool isGameStop(unsigned long);
    void SetSoftLight_Player(m3d::bmdl_c&, int);
    void SetSoftLight_Map(m3d::bmdl_c&, int);
    void SetSoftLight_Boss(m3d::bmdl_c&, int);
    void SetSoftLight_Enemy(m3d::bmdl_c&, int);
    void SetSoftLight_MapObj(m3d::bmdl_c&, int);
    void SetSoftLight_Item(m3d::bmdl_c&, int);

    /// @unofficial
    void updateSelectCursor(nw4r::lyt::Picture *pic, int index, bool useSpecialDraw);
}

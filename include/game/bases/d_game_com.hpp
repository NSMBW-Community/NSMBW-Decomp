#pragma once
#include <types.h>
#include <lib/nw4r/lyt/lyt_picture.hpp>
#include <game/mLib/m_3d.hpp>

/// @brief A collection of various functions used throughout the game.
/// @details Name stands for Game Common.
/// @ingroup bases
namespace dGameCom {

    //////////////////////////////////
    // Random Number Generation API //
    //////////////////////////////////

    u32 getRandomSeed(); ///< Gets a seed used for randomness.

    ////////////////////
    // Game Pause API //
    ////////////////////

    /// @brief The game stopping conditions.
    enum GAME_STOP_e {
        GAME_STOP_PAUSE = BIT_FLAG(0), ///< The pause menu is open.
        GAME_STOP_WARNING = BIT_FLAG(1), ///< An error occurred.
        GAME_STOP_OTASUKE_PAUSE = BIT_FLAG(2), ///< The pause menu is open during super guide/hint movie playback.
        GAME_STOP_HOME_MENU = BIT_FLAG(3), ///< The Home Menu is open.
        GAME_STOP_ANY = -1, ///< The game is stopped for any unspecified reason.
    };

    /// @brief Determines if gameplay is currently stopped for the specified reason(s).
    /// @param flag The pause reasons. Value is a ::GAME_STOP_e.
    bool isGameStop(ulong flag);
    void clearGameStop(); ///< Resets the game stop state.

    ////////////////////////
    // Model Lighting API //
    ////////////////////////

    void SetSoftLight_Player(m3d::bmdl_c&, int); ///< Sets the soft light effect for players.
    void SetSoftLight_Map(m3d::bmdl_c&, int); ///< Sets the soft light effect for map actors.
    void SetSoftLight_Boss(m3d::bmdl_c&, int); ///< Sets the soft light effect for bosses.
    void SetSoftLight_Enemy(m3d::bmdl_c&, int); ///< Sets the soft light effect for enemies.
    void SetSoftLight_MapObj(m3d::bmdl_c&, int); ///< Sets the soft light effect for map objects.
    void SetSoftLight_Item(m3d::bmdl_c&, int); ///< Sets the soft light effect for items.

    ////////////////
    // Other APIs //
    ////////////////

    bool someCheck(mVec3_c *a, AreaBound *b);
    void CreateSmallScore(const mVec3_c &, int, int, bool);
    u8 GetAspectRatio();
    bool PlayerEnterCheck(int);
    void Player1upColor(LytTextBox_c *, int);
    void getGlbPosToLyt(mVec3_c &);

    /// @unofficial
    void updateSelectCursor(nw4r::lyt::Picture *pic, int index, bool useSpecialDraw);
    float getDispCenterY();
}

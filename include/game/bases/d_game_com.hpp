#pragma once
#include <types.h>
#include <nw4r/lyt.h>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_lyttextBox.hpp>

/// @brief A collection of various functions used throughout the game.
/// @details Name stands for Game Common.
/// @ingroup bases
namespace dGameCom {

    //////////////////////////////////
    // Random Number Generation API //
    //////////////////////////////////

    u32 getRandomSeed(); ///< Gets a seed used for randomness.
    int rndInt(size_t max);

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
    void setGameStop();

    ////////////////////////
    // Model Lighting API //
    ////////////////////////

    void SetSoftLight_Player(m3d::bmdl_c&, int); ///< Sets the soft light effect for players.
    void SetSoftLight_Map(m3d::bmdl_c&, int); ///< Sets the soft light effect for map actors.
    void SetSoftLight_Boss(m3d::bmdl_c&, int); ///< Sets the soft light effect for bosses.
    void SetSoftLight_Enemy(m3d::bmdl_c&, int); ///< Sets the soft light effect for enemies.
    void SetSoftLight_MapObj(m3d::bmdl_c&, int); ///< Sets the soft light effect for map objects.
    void SetSoftLight_Item(m3d::bmdl_c&, int); ///< Sets the soft light effect for items.

    ////////////////////
    // Fukidashi APIs //
    ////////////////////

    /// @brief Displays the given action prompt to the given player.
    /// @unofficial
    /// @param playerId The player to display the action prompt to.
    /// @param fukidashiAction The action prompt to display.
    void showFukidashi(int playerId, int fukidashiAction);

    /// @brief Temporarily hides the given action prompt for the given player.
    /// @unofficial
    /// @param playerId The player to hide the action prompt for.
    /// @param fukidashiAction The action prompt type to be hidden.
    /// @param param3 Unknown.
    void hideFukidashiTemporarily(int playerId, int fukidashiAction, int param3);

    /// @brief Hides the given action prompt for the given player for the rest of the current level.
    /// @unofficial
    /// @param playerId The player to hide the action prompt for.
    /// @param fukidashiAction The action prompt type to be hidden.
    /// @param param3 Unknown.
    void hideFukidashiForLevel(int playerId, int fukidashiAction, int param3);

    /// @brief Hides the given action prompt for the given player for the rest of the current session.
    /// @details The action prompt remains hidden until the player returns to the main menu.
    /// @unofficial
    /// @param playerId The player to hide the action prompt for.
    /// @param fukidashiAction The action prompt type to be hidden.
    void hideFukidashiForSession(int playerId, int fukidashiAction);

    ////////////////
    // Other APIs //
    ////////////////

    void CreateSmallScore(const mVec3_c &, int, int, bool);
    u8 GetAspectRatio();
    bool PlayerEnterCheck(int);
    void Player1upColor(LytTextBox_c *, int);
    void getGlbPosToLyt(mVec3_c &);
    bool someCheck(mVec3_c *a, mBoundBox *b); ///< @unofficial

    bool checkRectangleOverlap(mVec3_c *, mVec3_c *, mVec3_c *, mVec3_c *, float); ///< @unofficial

    void updateSelectCursor(nw4r::lyt::Picture *pic, int index, bool useSpecialDraw); ///< @unofficial
    float getDispCenterY();

    void DispSizeScale(nw4r::math::VEC2 &scale);

    void LayoutDispNumber(const int &value, const int &fillLeft, LytTextBox_c *textBox, bool fillWidth);

    bool isNowCourseClear();
    void SelectCursorSetup();
    void SelectCursorSetup(nw4r::lyt::Pane *, int, bool);
    void WindowPaneColorSet(nw4r::lyt::Window *, int);
}

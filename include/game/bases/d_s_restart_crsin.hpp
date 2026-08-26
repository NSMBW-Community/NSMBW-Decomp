#pragma once
#include <game/bases/d_scene.hpp>
#include <game/bases/d_info.hpp>

/**
 * @brief This scene is used to "restart" levels.
 * @ingroup bases
 * @details It is used for the following purposes:
 * - Returning to Peach's Castle after a Hint Movie ends.
 * - Starting the title screen stage.
 */
class dScRestartCrsin_c : public dScene_c {
public:
    dScRestartCrsin_c(); ///< @copydoc dScene_c::dScene_c
    virtual ~dScRestartCrsin_c(); ///< @copydoc dScene_c::~dScene_c

    virtual int create();
    virtual int doDelete();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();

    /// @brief Starts the title screen stage or a title replay.
    /// @param isReplay Whether it will start a random replay or the title screen.
    /// @param forceChange If the scene transition should always be performed.
    static void startTitle(u8 isReplay, bool forceChange);

    /// @brief Starts the Peach's Castle stage.
    static void reStartPeachCastle();

    static dInfo_c::StartGameInfo_s m_startGameInfo; ///< Info used to start a level.
};

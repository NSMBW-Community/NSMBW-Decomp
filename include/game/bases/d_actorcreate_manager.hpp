#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_actor_data.hpp>
#include <game/bases/d_cd.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

#define GROUP_ID(param) (param >> 24)

/// @unofficial
struct sMapActorCreateBounds {
    enum BOUND_SCROLL_DIR_X_e {
        BOUND_SCROLL_X_RIGHT = -1,
        BOUND_SCROLL_X_LEFT,
        BOUND_SCROLL_X_NONE,
    };

    enum BOUND_SCROLL_DIR_Y_e {
        BOUND_SCROLL_Y_DOWN = -1,
        BOUND_SCROLL_Y_UP,
        BOUND_SCROLL_Y_NONE,
    };

    int mScrollBoundaryTileX, mScrollBoundaryTileY;
    int mTileLeft, mTileRight, mTileTop, mTileBottom;
    BOUND_SCROLL_DIR_X_e mScrollDirX;
    BOUND_SCROLL_DIR_Y_e mScrollDirY;
};

/// @brief Manager for map actor creation.
/// @decompnote{Map actors are the equivalent of sprites in Reggie terminology.}
/// @ingroup bases
class dActorCreateMng_c {
public:
    void ActorCreateInfoClear(); ///< Resets all temporary data for map actor spawning.

    /**
     * @brief Checks whether an actor should be spawned based on the camera's current scroll position.
     * @details This check succeeds if the actor spawn area's edge lies exactly on the screen
     * boundary's edge and the area overlaps with the boundary on the perpendicular axis.
     * @param boundsEdge The coordinate of the screen boundary edge (on the axis of movement), in tiles.
     * @param areaEdge The coordinate of the actor's corresponding spawn area edge, in tiles.
     * @param boundMin The minimum coordinate of the boundary on the perpendicular axis, in tiles.
     * @param boundMax The maximum coordinate of the boundary on the perpendicular axis, in tiles.
     * @param areaMin The minimum coordinate of the spawn area on the perpendicular axis, in tiles.
     * @param areaMax The maximum coordinate of the spawn area on the perpendicular axis, in tiles.
     * @returns @p true if the actor should be spawned, @p false otherwise.
     */
    bool ScroolAreaInCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax);

    /// @brief Checks whether an actor should be spawned due to being close to a screen edge (for looping stages).
    /// @note In the current implementation, this is a direct wrapper for ScroolAreaInCheck().
    bool ScroolAreaInLoopCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax);

    void setMapActorCreate(); ///< Initializes map actors on first area load.
    void setMapActorCreate_next(); ///< Initializes map actors on area reload.
    void MapActorInital_set(); ///< Triggers actor creation on first area load.
    void MapActorInital_next(); ///< Triggers actor creation on area reload.

    /// @brief Checks whether the profile associated to the map actor is allowed to ignore screen bounds on spawn.
    /// @unofficial
    /// @returns @p true if the actor is in the "global" whitelist.
    bool GlobalActorCheck(sMapActorData *data);

    /**
     * @brief Adds a group-based map actor to the group manager.
     * @unofficial
     * @param data The map actor to be added.
     * @param file The course file the map actor is in.
     * @param mapActorIdx The index of the map actor in the level data.
     */
    void processGroupId(sMapActorData *data, u8 file, u32 mapActorIdx);

    /// @brief Checks whether the provided actor parameters contain a group ID definition.
    /// @returns @p true if the actor does NOT use groups, @p false otherwise.
    bool GroupIdCheck(ulong param);

    /**
     * @brief Manages actor spawning when entering a level or area.
     * @unofficial
     * @param levelData The map actor data for the file/area.
     * @param deleteVal A pointer to the delete value array for the file/area.
     * @param spawnFlags A pointer to the spawn flags array for the file/area.
     * @param mapActorCount The amount of map actors to process.
     * @param isAreaReload Whether the area is being reloaded (e.g. next goto that points to the same area).
     */
    void MapActorInitialCreate(sMapActorData *levelData, u16 *deleteVal, u8 *spawnFlags, int mapActorCount, bool isAreaReload);

    /// @brief Checks for screen scrolling and zoom changes to trigger spawning of nearby actors.
    void MapActorScroolCreateCheck();

    /// @brief Performs spawning of nearby actors as they enter the screen via scrolling or zoom changes.
    /// @unofficial
    /// @param createBounds The boundary data required to perform the screen-based checks.
    /// @param isScreenScroll Whether the actor spawning is triggered by screen scrolling (1) or zoom changes (0).
    void MapActorScrollCreate(sMapActorCreateBounds *createBounds, int isScreenScroll);

    void incZposCount(); ///< Updates the Z-position index for the next spawned actor.
    float addZposCount(); ///< Gets the Z position for the next spawned actor.
    void incZposCount_layer2(); ///< Updates the Z-position index for the next spawned actor on layer 2.
    float addZposCount_layer2(); ///< Gets the Z position for the next spawned actor on layer 2.
    void incMapObjZposCount(); ///< Updates the Z-position index for the next spawned non-enemy actor.
    float addMapObjZposCount(); ///< Gets the Z position for the next spawned non-enemy actor.
    void incMapObjZposCount_layer2(); ///< Updates the Z-position index for the next spawned non-enemy actor on layer 2.
    float addMapObjZposCount_layer2(); ///< Gets the Z position for the next spawned non-enemy actor on layer 2.

    /**
     * @brief Spawns an actor from the corresponding map actor data.
     * @unofficial
     * @param data The map actor data.
     * @param info The map actor info.
     * @param spawnFlags A pointer to the actor's spawn flags.
     * @param deleteVal A pointer to the actor's delete value, or @p nullptr .
     * @param areaNo The zone the actor will be linked to.
     * @returns The spawned actor, or @p nullptr if it's a dummy actor or the actor creation fails.
     */
    dActor_c *mapActorSpawn(sMapActorData *data, dActorData_c *info, u8 *spawnFlags, u16 *deleteVal, u8 areaNo);

    int mZPosCount; ///< The Z position index for the next spawned actor.
    int mZPosCountLayer2; ///< The Z position index for the next spawned actor on layer 2.
    int mMapObjZPosCount; ///< The Z position index for the next spawned non-enemy actor.
    int mMapObjZPosCountLayer2; ///< The Z position index for the next spawned non-enemy actor on layer 2.

    u16 mDeleteVals[MAX_MAP_ACTOR_COUNT]; ///< A value that each map actor can write to when it is deleted. @unused
    u8 mSpawnFlags[MAX_MAP_ACTOR_COUNT]; ///< Flags indicating each map actor' spawn status. See dActor_c::ACTOR_SPAWN_FLAG_e.
    u16 mYoshiColor; ///< The color of the next spawned Yoshi (egg).

    bool mIsEndingDemo; ///< Whether an end-of-level cutscene is active.
    bool mIsGoalPoleDemo; ///< Whether a goal flagpole cutscene is active.

    int mScreenLeftPos; ///< The screen's left edge.
    int mScreenTopPos; ///< The screen's top edge.
    int mPrevScrollBoundaryTileX; ///< The previous scroll boundary on the X axis, used to check if the screen has scrolled.
    int mPrevScrollBoundaryTileY; ///< The previous scroll boundary on the Y axis, used to check if the screen has scrolled.

    static const ProfileName l_GoAheadEnemy[]; ///< The profiles allowed to spawn outside of the screen.
    static dActorCreateMng_c *m_instance; ///< The static instance of the map actor creation manager.
};

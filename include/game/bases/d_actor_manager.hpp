#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_cd.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

#define GROUP_ID(param) (param >> 24)

class dActorMng_c {
public:
    void createUpCoin(const mVec3_c &pos, u8 dir, u8 count, u8 layer);

    static dActorMng_c *m_instance;
};

/// @unofficial
enum MAP_ACTOR_INFO_FLAG_e {
    ACTOR_CREATE_GLOBAL = BIT_FLAG(1), ///< Spawn even if not inside the screen bounds.
    ACTOR_CREATE_MAPOBJ = BIT_FLAG(3), ///< ["Map objects" seem to be actors that are not e.g. enemies, but part of the environment (blocks, rolling hills, etc.)]
    ACTOR_CREATE_GROUP_0 = BIT_FLAG(4) ///< Only spawn if the map actor has group 0.
};

/// @unofficial
struct sMapActorInfo {
    u16 mProfileName;
    int mSpawnOffsetX;
    int mSpawnOffsetY;
    int mSpawnRangeOffsetX;
    int mSpawnRangeOffsetY;
    int mSpawnRangeWidth;
    int mSpawnRangeHeight;
    u16 mSpawnMarginTop;
    u16 mSpawnMarginBottom;
    u16 mSpawnMarginLeft;
    u16 mSpawnMarginRight;
    u16 mFlags; /// See MAP_ACTOR_INFO_FLAG_e.
};


/// @unofficial
namespace dActorCreate {
    extern sMapActorInfo sMapActors[];

    inline sMapActorInfo *getMapActorInfo(int idx) { return &sMapActors[idx]; }
}

enum SCROLL_DIR_X_e {
    SCROLL_X_LEFT,
    SCROLL_X_NONE,
    SCROLL_X_RIGHT = -1
};

enum SCROLL_DIR_Y_e {
    SCROLL_Y_UP,
    SCROLL_Y_NONE,
    SCROLL_Y_DOWN = -1
};

/// @unofficial
struct sMapActorCreateBounds {
    int mEdgeTileX, mEdgeTileY;
    int mTileLeft, mTileRight, mTileTop, mTileBottom;
    SCROLL_DIR_X_e mScrollDirX;
    SCROLL_DIR_Y_e mScrollDirY;
};

/// @brief Manager for map actor [Reggie terminology: sprite] creation.
class dActorCreateMng_c {
public:
    /// @brief Resets all temporary data for map actor spawning.
    void ActorCreateInfoClear();

    /// @brief Checks whether the spawn area for a map actor is within bounds.
    /// This check succeeds if the area edge in one direction lies on the bounds edge
    /// and the area is contained within the bounds in the other direction.
    /// @param boundsEdge The edge the area must lie on.
    /// @param areaEdge The edge of the area.
    /// @param boundMin The minimum coordinate of the bound in the perpendicular direction.
    /// @param areaMin The minimum coordinate of the area in the perpendicular direction.
    /// @param boundMax The maximum coordinate of the bound in the perpendicular direction.
    /// @param areaMax The maximum coordinate of the area in the perpendicular direction.
    bool ScroolAreaInCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax);
    /// @brief Checks whether the spawn area for a map actor is within bounds for a looping stage.
    /// Is actually just a stub of ScroolAreaInCheck().
    bool ScroolAreaInLoopCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax);

    void setMapActorCreate(); ///< Initializes map actors on first area load.
    void setMapActorCreate_next(); ///< Initialize map actors on area reload.
    void MapActorInital_set();
    void MapActorInital_next();

    /// @unofficial
    bool GlobalActorCheck(sMapActorData *data);

    /// @unofficial
    void processGroupId(sMapActorData *data, u8 file, u32 idx);

    bool GroupIdCheck(ulong param);

    /// @unofficial
    void MapActorInitialCreate(sMapActorData *data, u16 *deleteNum, u8 *spawnFlags, int count, bool isAreaReload);

    void MapActorScroolCreateCheck();
    /// @unofficial
    void MapActorScrollCreate(sMapActorCreateBounds *s, int noDispScaleChange);

    void incZposCount();
    float addZposCount();
    void incZposCount_layer2();
    float addZposCount_layer2();
    void incMapObjZposCount();
    float addMapObjZposCount();
    void incMapObjZposCount_layer2();
    float addMapObjZposCount_layer2();

    /// @unofficial
    dActor_c *mapActorSpawn(sMapActorData *data, sMapActorInfo *info, u8 *spawnFlags, u16 *deleteNum, u8 areaNo);

    int mZPosCount; ///< The Z position index for the next actor.
    int mZPosCountLayer2; ///< The Z position index for the next actor on layer 2.
    int mMapObjZPosCount;
    int mMapObjZPosCountLayer2;
    u16 mDeleteNums[MAX_MAP_ACTOR_COUNT]; ///< Number that can be written to by the map actor when deleting.
    u8 mSpawnFlags[MAX_MAP_ACTOR_COUNT]; ///< Flags indicating map actor spawn status. See dActor_c::ACTOR_SPAWN_FLAG_e.
    u16 mYoshiColor; ///< The color of the next spawned yoshi (egg).
    bool mNoSpawnOnReload; ///< Do not spawn new actors if the area is reloaded. [(Is this ever even triggered? Maybe used in playtesting.)]
    bool mNoSpawnOnScroll; ///< Do not spawn new actors in the current area.
    int mPosX, mPosY;
    int mPrevEdgeTileX, mPrevEdgeTileY;

    static dActorCreateMng_c *m_instance;
};

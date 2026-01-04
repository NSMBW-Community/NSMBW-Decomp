#pragma once

#include <types.h>
#include <game/framework/f_profile.hpp>

/// @unofficial
enum MAP_ACTOR_INFO_FLAG_e {
    ACTOR_CREATE_GLOBAL = BIT_FLAG(1), ///< The actor spawns regardless of its distance to the camera.
    ACTOR_CREATE_UNK_4 = BIT_FLAG(2), ///< [Seemingly only used for sprites that mess with the camera (Zoom, Scroll Limiters).]
    ACTOR_CREATE_MAPOBJ = BIT_FLAG(3), ///< Identifies the actor as a map object (blocks, surfaces and decorations) rather than an enemy.
    ACTOR_CREATE_GROUPABLE = BIT_FLAG(4) ///< Allows the actor to be managed by a Group Controller if a valid Group ID is provided.
};

/// @brief The configuration for a map actor.
class dActorData_c {
public:
    ProfileName mProfileName; ///< The profile spawned by the map actor.
    int mSpawnOffsetX; ///< The horizontal offset from the map actor to the actor's spawn position.
    int mSpawnOffsetY; ///< The vertical offset from the map actor to the actor's spawn position.
    int mSpawnRangeOffsetX; ///< The horizontal offset of the actor's bounding box from the actor's position.
    int mSpawnRangeOffsetY; ///< The vertical offset of the actor's bounding box from the actor's position.
    int mSpawnRangeHalfWidth; ///< The half-width of the actor's bounding box.
    int mSpawnRangeHalfHeight; ///< The half-height of the actor's bounding box.
    u16 mSpawnMarginTop;    ///< The additional padding added to the top of the bounding box to prevent pop-in.
    u16 mSpawnMarginBottom; ///< The additional padding added to the bottom of the bounding box to prevent pop-in.
    u16 mSpawnMarginLeft;   ///< The additional padding added to the left of the bounding box to prevent pop-in.
    u16 mSpawnMarginRight;  ///< The additional padding added to the right of the bounding box to prevent pop-in.
    u16 mFlags; ///< Various flags that control map actor spawning behaviour. Value is a bitfield from MAP_ACTOR_INFO_FLAG_e.

    static dActorData_c l_actor_data_tbl[]; ///< The configuration for each map actor.
    static inline dActorData_c *getMapActorInfo(int idx) { return &l_actor_data_tbl[idx]; }
};

#pragma once
#include <types.h>

/// @file
// Please place the defines/enums in alphabetical order!

/// @addtogroup constants
/// @{

#define AMBUSH_ENEMY_COUNT 4 ///< The maximum amount of ambush enemies per world map.

#define CHECKPOINT_COUNT 2 ///< The maximum number of checkpoints supported.

#define HINT_MOVIE_COUNT 70 ///< The total hint movie count.

#define MAX_LIVES 99 ///< The maximum life count.

#define MAX_STOCK_ITEM 99 ///< The maximum inventory amount per item.

#define SAVE_SLOT_COUNT 3 ///< The total save slot count.

#define SMALL_SCORE_COUNT 30 ///< The maximum number of small score popups visible simultaneously.

#define STAR_COIN_COUNT 3 ///< The amount of Star Coins per level.

#define STARTING_LIVES_COUNT 5 ///< The amount of starting lives.

#define SUPER_GUIDE_DEATH_COUNT 8 ///< The amount of deaths required for the Super Guide to show.

#define WM_ENEMY_PATH_MAX_POINT_COUNT 64 ///< The maximum number of keypoints for world map enemy paths.

#define WM_PATH_MAX_POINT_COUNT 32 ///< The maximum number of keypoints for generic world map paths.

enum DIRECTION_e {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

enum PATH_DIRECTION_e {
    PATH_DIR_NORMAL,
    PATH_DIR_REVERSE,
    PATH_DIR_INITIAL,
};

/// @brief The identifiers for each character.
enum PLAYER_CHARACTER_e {
    PLAYER_MARIO,
    PLAYER_LUIGI,
    PLAYER_YELLOW_TOAD,
    PLAYER_BLUE_TOAD,
    PLAYER_COUNT
};

/// @brief Various flags applied to the player on occasion.
enum PLAYER_CREATE_ITEM_e {
    CREATE_ITEM_NONE        = BIT_FLAG(-1),
    CREATE_ITEM_STAR_POWER  = BIT_FLAG(0), ///< Gives the player Star Power.
    CREATE_ITEM_YOSHI       = BIT_FLAG(1), ///< Spawns a Yoshi along with the player.
    CREATE_ITEM_BUBBLE      = BIT_FLAG(2), ///< Spawns the player inside a bubble.
    CREATE_ITEM_RESCUE_TOAD = BIT_FLAG(3) ///< Spawns a Rescue Toad instead of the regular player character.
};

/// @brief The identifiers for each powerup.
enum PLAYER_POWERUP_e {
    POWERUP_NONE,
    POWERUP_MUSHROOM,
    POWERUP_FIRE_FLOWER,
    POWERUP_MINI_MUSHROOM,
    POWERUP_PROPELLER_SHROOM,
    POWERUP_PENGUIN_SUIT,
    POWERUP_ICE_FLOWER,
    POWERUP_COUNT
};

/// @brief The numbers corresponding to each level type.
enum STAGE_e {
    STAGE_1,
    STAGE_2,
    STAGE_3,
    STAGE_4,
    STAGE_5,
    STAGE_6,
    STAGE_7,
    STAGE_8,
    STAGE_9,
    STAGE_COIN_BATTLE = 19,
    STAGE_GHOST_HOUSE,
    STAGE_TOWER,
    STAGE_TOWER_2, ///< @unused
    STAGE_CASTLE,
    STAGE_CASTLE_2, ///< @unused
    STAGE_KINOKO_HOUSE,
    STAGE_KINOKO_HOUSE_2,
    STAGE_KINOKO_HOUSE_3,
    STAGE_KINOKO_HOUSE_4,
    STAGE_KINOKO_HOUSE_5,
    STAGE_KINOKO_HOUSE_6,
    STAGE_KINOKO_HOUSE_7,
    STAGE_ENEMY,
    STAGE_ENEMY_2,
    STAGE_ENEMY_3,
    STAGE_CANNON,
    STAGE_UNK37,
    STAGE_DOOMSHIP,
    STAGE_START_KINOKO_HOUSE, ///< The toad house on the starting node of each world.
    STAGE_TITLE,
    STAGE_PEACH_CASTLE,
    STAGE_STAFFROLL,
    STAGE_COUNT
};

/// @brief The numbers corresponding to each world.
enum WORLD_e {
    WORLD_1,
    WORLD_2,
    WORLD_3,
    WORLD_4,
    WORLD_5,
    WORLD_6,
    WORLD_7,
    WORLD_8,
    WORLD_9,
    WORLD_10, ///< @unused
    WORLD_COUNT,

    LAST_WORLD = WORLD_8,
    LAST_SPECIAL_WORLD = WORLD_9,
    NORMAL_WORLD_COUNT = LAST_WORLD + 1,
    SPECIAL_WORLD_COUNT = LAST_SPECIAL_WORLD - LAST_WORLD,
    WORLD_USED_COUNT = NORMAL_WORLD_COUNT + SPECIAL_WORLD_COUNT
};

/// @}

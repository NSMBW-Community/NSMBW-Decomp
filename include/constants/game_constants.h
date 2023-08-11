#pragma once
/// @file
// Please place the defines/enums in alphabetical order!

/// @addtogroup constants
/// @{

#define AMBUSH_ENEMY_COUNT 4 ///< The maximum amount of ambush enemies per world map.

#define HINT_MOVIE_COUNT 70 ///< The total hint movie count.

#define MAX_LIVES 99 ///< The maximum life count.

#define MAX_STOCK_ITEM 99 ///< The maximum inventory amount per item.

#define SAVE_SLOT_COUNT 3 ///< The total save slot count.

#define STAR_COIN_COUNT 3 ///< The amount of Star Coins per level.

#define STARTING_LIVES_COUNT 5 ///< The amount of starting lives.

#define SUPER_GUIDE_DEATH_COUNT 8 ///< The amount of deaths required for the Super Guide to show.

/// @brief The identifiers for each character.
enum PLAYER_CHARACTER_e {
    PLAYER_MARIO,
    PLAYER_LUIGI,
    PLAYER_BLUE_TOAD,
    PLAYER_YELLOW_TOAD,
    PLAYER_COUNT
};

/// @brief Various flags applied to the player on occasion.
enum PLAYER_CREATE_ITEM_e {
    CREATE_ITEM_NONE = 0x0,
    CREATE_ITEM_STAR_POWER = 0x1, ///< Gives the player Star Power.
    CREATE_ITEM_YOSHI = 0x2, ///< Spawns a Yoshi along with the player.
    CREATE_ITEM_BUBBLE = 0x4, ///< Spawns the player inside a bubble.
    CREATE_ITEM_RESCUE_TOAD = 0x8 ///< Spawns a Rescue Toad instead of the regular player character.
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
    STAGE_ENEMY = 32,
    STAGE_ENEMY_2,
    STAGE_ENEMY_3,
    STAGE_CANNON,
    STAGE_DOOMSHIP = 37,
    STAGE_RESCUE, ///< When used, it loads the Toad Rescue level for the corresponding world.
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
    SPECIAL_WORLD_COUNT = LAST_SPECIAL_WORLD - LAST_WORLD
};

/// @}

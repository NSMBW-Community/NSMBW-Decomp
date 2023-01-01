#pragma once
/// @file
// Please place the defines/enums in alphabetical order!

#define AMBUSH_ENEMY_COUNT 4 ///< The maximum amount of ambush enemies per world map.

#define HINT_MOVIE_COUNT 70 ///< The total hint movie count.

#define MAX_LIVES 99 ///< The maximum life count.

#define MAX_STOCK_ITEM 99 ///< The maximum inventory amount per item.

#define SAVE_SLOT_COUNT 3 ///< The total save slot count.

#define STAR_COIN_COUNT 3 ///< The amount of Star Coins per level.

#define STARTING_LIVES_COUNT 5 ///< The amount of starting lives.

#define SUPER_GUIDE_DEATH_COUNT 8 ///< The amount of deaths required for the Super Guide to show.

/// @brief The identifiers for each character.
enum PlayerCharacter {
    MARIO,
    LUIGI,
    BLUE_TOAD,
    YELLOW_TOAD,
    PLAYER_COUNT
};

/// @brief Various flags applied to the player on occasion.
enum PlayerCreateItem {
    NO_CREATE_ITEM = 0x0,
    STAR_POWER = 0x1, ///< Gives the player Star Power.
    RIDING_YOSHI = 0x2, ///< Spawns a Yoshi along with the player.
    BUBBLED = 0x4, ///< Spawns the player inside a bubble.
    RESCUE_TOAD = 0x8 ///< Spawns a Rescue Toad instead of the regular player character.
};

/// @brief The identifiers for each powerup.
enum PlayerPowerup {
    NO_POWERUP,
    MUSHROOM,
    FIRE_FLOWER,
    MINI_MUSHROOM,
    PROPELLER_SHROOM,
    PENGUIN_SUIT,
    ICE_FLOWER,
    POWERUP_COUNT
};

/// @brief The numbers corresponding to each level type.
enum Stage {
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
    STAGE_TOWER_2, ///< [Unused].
    STAGE_CASTLE,
    STAGE_CASTLE_2, ///< [Unused].
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
enum World {
    WORLD_1,
    WORLD_2,
    WORLD_3,
    WORLD_4,
    WORLD_5,
    WORLD_6,
    WORLD_7,
    WORLD_8,
    WORLD_9,
    WORLD_10, ///< [Unused].
    WORLD_COUNT,

    LAST_WORLD = WORLD_8,
    NORMAL_WORLD_COUNT = LAST_WORLD + 1,
    SPECIAL_WORLD_COUNT = WORLD_COUNT - NORMAL_WORLD_COUNT - 1
};

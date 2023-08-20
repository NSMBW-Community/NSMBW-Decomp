#pragma once
/// @file

/// @addtogroup constants
/// @{

/// @brief Gets the group ID from a message ID.
/// @hideinitializer
#define GET_MSG_GROUP(id) ((id) >> 8)

/// @brief Converts a message ID to make it relative to its group ID.
/// @hideinitializer
#define GET_MSG_REL_ID(id) ((id) & 0xFF)

/// @brief The identifiers for each text message.
/// @todo Fill this.
enum MESSAGE_e {
    MSG_TEST,

    // The following are used to format escape sequences
    MSG_PICFONT_2_BTN,
    MSG_PICFONT_A_BTN,
    MSG_PICFONT_1_BTN,
    MSG_PICFONT_B_BTN,
    MSG_PICFONT_DPAD,
    MSG_PICFONT_HOME_BTN,
    MSG_PICFONT_PLUS_BTN,
    MSG_PICFONT_MINUS_BTN,
    MSG_PICFONT_NUNCHUCK_PAD,
    MSG_PICFONT_C_BTN,
    MSG_PICFONT_Z_BTN,
    MSG_PICFONT_WIIMOTE,
    MSG_PICFONT_NUNCHUK,
    MSG_PICFONT_CASTLE,
    MSG_PICFONT_TOWER,
    MSG_PICFONT_GHOST_HOUSE,
    MSG_PICFONT_CANNON,
    MSG_PICFONT_AIRSHIP,
    MSG_PICFONT_TOAD_HOUSE_RED,
    MSG_PICFONT_START_NODE_RIGHT,
    MSG_PICFONT_START_NODE_UP,
    MSG_PICFONT_MAP_JUNCTION,
    MSG_PICFONT_PEACH_CASTLE,
    MSG_PICFONT_TOAD_HOUSE_YELLOW,
    MSG_PICFONT_TOAD_HOUSE_GREEN,
    MSG_PICFONT_LEVEL_NODE_RED,
    MSG_PICFONT_LEVEL_NODE_BLUE,
    MSG_PICFONT_LEVEL_NODE_YELLOW,
    MSG_PICFONT_FINAL_CASTLE,
    MSG_PICFONT_AMBUSH,
    MSG_PICFONT_SHIP_ANCHOR,
    MSG_PICFONT_QUESTION_BLOCK,
    MSG_PICFONT_RED_BLOCK,
    MSG_PICFONT_BLOCK_OUTLINE,

    MSG_HOLD_WIIMOTE_SIDEWAYS = 0x6808,
};

/// @}

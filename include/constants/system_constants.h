#pragma once
/// @file
// Please place the defines/enums in alphabetical order!

/// @addtogroup constants
/// @{

#define ARENA_LO 0x806e0000 ///< The start of the arena.
#define ARENA_HI 0x817f0000 ///< The end of the arena.

#define HEAP_SIZE_ARCHIVE 0x1200000 ///< The size of the heap used for loading archives.
#define HEAP_SIZE_COMMAND 0x1000 ///< The size of the heap used for commands.
#define HEAP_SIZE_DYLINK 0x500000 ///< The size of the heap used for dynamic libraries.
#define HEAP_SIZE_EFFECT_MANAGER1 0x100000 ///< The size of the first heap used for the effect manager.
#define HEAP_SIZE_EFFECT_MANAGER2 0x280000 ///< The size of the second heap used for the effect manager.
#define HEAP_SIZE_FONT_MANAGER 0x400000 ///< The size of the heap used for the font manager.
#define HEAP_SIZE_GAME1 0x980000 ///< The size of the first game heap.
#define HEAP_SIZE_GAME2 0x700000 ///< The size of the second game heap.
#define HEAP_SIZE_MESSAGE_MANAGER 0xc800 ///< The size of the heap used for the message manager.
#define HEAP_SIZE_REPLAY_DATA 0x1c2000 ///< The size of the heap used for replay data.
#define HEAP_SIZE_TEXTURE_BUFFER 0x2a0000 ///< The size of the heap used for texture buffers.

#define SAVE_MAGIC "SMNP" ///< The savegame's magic.
#define SAVE_REVISION_MAJOR 14 ///< The savegame's major revision number.
#define SAVE_REVISION_MINOR 0 ///< The savegame's minor revision number.

/// @}

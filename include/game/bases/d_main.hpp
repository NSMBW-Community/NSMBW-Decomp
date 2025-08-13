#pragma once

#include <lib/rvl/os/OSThread.h>
#include <lib/rvl/os/OSTime.h>

/// @brief A wrapper class for the top-level game loop.
/// @ingroup bases
class dMain {
public:
    static void Create(); ///< The function that initializes the main game loop.
    static void Execute(); ///< The function that runs the main game loop.
    static void *main01(void *); ///< The main function to be run on the main thread.

    static OSThread mainThread; ///< The main thread for the game.
    static s64 g_InitialTime; ///< The startup time of the game.
};

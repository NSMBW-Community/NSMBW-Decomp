#pragma once

#include <types.h>

/// @brief Global frame counter utility class.
/// @ingroup clib
class cCounter_c {
public:
    static void clear(); ///< Clears the counters.

    /// @brief The total frames elapsed while the main engine loop (fManager_c::mainLoop) is running.
    /// @details Does not advance when main loop execution is halted or suspended.
    static u32 m_exeFrame;

    /// @brief The total frames elapsed since game startup.
    /// @details Unlike ::m_exeFrame, this counter doesn't pause when the Home Menu is open.
    static u32 m_gameFrame;
};

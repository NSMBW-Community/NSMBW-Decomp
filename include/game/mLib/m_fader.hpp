#pragma once
#include <game/mLib/m_fader_base.hpp>

/// @brief Fader management class.
/// @details This ensures that only one fader is employed at once.
/// @ingroup mlib
class mFader_c {
public:
    static void draw(); ///< Draws the current fader.

    /// @brief Checks if the current fader's status matches @p status .
    static bool isStatus(mFaderBase_c::EStatus status) { return mFader->getStatus() == status; }

    /// @brief Sets the fader to be used.
    /// @return If the operation was successful.
    static bool setFader(mFaderBase_c *fader);

    static mFaderBase_c *mFader; ///< The fader currently in use.
};

#pragma once

#include <types.h>
#include <game/mLib/m_fader_base.hpp>

class dFader_c {
public:
    static void setFader(bool);

    /// @brief Starts a fade in with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade in.
    static bool startFadeIn(u16 duration);

    /// @brief Starts a fade out with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade out.
    static bool startFadeOut(u16 duration);

    static mFaderBase_c *mFader;
};

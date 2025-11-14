#pragma once

#include <types.h>
#include <game/mLib/m_fader.hpp>

class dFader_c : public mFader_c {
public:
    enum fader_type_e {
        FADER_FADE,
        FADER_CIRCLE_MIDDLE,
        FADER_BOWSER,
        FADER_DRIP_DOWN,
        FADER_MARIO,
        FADER_CIRCLE_TARGET
    };

    static void setFader(fader_type_e type);

    /// @brief Starts a fade in with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade in.
    /// @return Whether the fade in was started.
    static bool startFadeIn(u16 duration);

    /// @brief Starts a fade out with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade out.
    /// @return Whether the fade out was started.
    static bool startFadeOut(u16 duration);

    static mFaderBase_c *mFader;
};

#pragma once
#include <types.h>

class dFader_c {
public:
    enum fader_type_e {
        FADER_FADE,
        FADER_CIRCLE_MIDDLE,
        FADER_BOWSER,
        FADER_DRIP_DOWN,
        FADER_MARIO,
        FADER_CIRCLE_TARGET
    };

    /// @brief Starts a fade in with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade in.
    static void startFadeIn(u16 duration);

    /// @brief Starts a fade out with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade out.
    static void startFadeOut(u16 duration);
};

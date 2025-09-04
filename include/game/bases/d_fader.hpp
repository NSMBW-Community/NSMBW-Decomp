#pragma once

#include <types.h>
#include <game/mLib/m_fader.hpp>

class dFader_c : public mFader_c {
public:
    enum fader_type_e {
        FADE
    };

    static void setFader(fader_type_e type);

    /// @brief Starts a fade in with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade in.
    static bool startFadeIn(u16 duration);

    /// @brief Starts a fade out with the @ref mFader_c::mFader "current fader".
    /// @param duration The duration of the fade out.
    static bool startFadeOut(u16 duration);

    static mFaderBase_c *mFader;
};

#pragma once
#include <game/mLib/m_fader_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <nw4r/ut.h>

/// @brief A fader that animates a wavy texture across the screen.
/// @details Seen when entering a cloud area, for example.
/// [This transition was used for the pipe transition earlier in development,
/// which is why this class is called "dWipeDokan_c" (Dokan means "pipe")].
/// @ingroup bases
class dWipeDokan_c : public mFaderBase_c {
public:

    /// @brief The possible actions performed in ::calc.
    enum ACTION_e {
        OPEN_SETUP, ///< Prepare the fade-out transition.
        ANIME_END_CHECK, ///< Fade has begun, waiting for it to end.
        CLOSE_SETUP, ///< Prepare the fade-in transition.
        IDLE ///< The fade has not been started.
    };

    /// @brief The animations for this fader.
    enum ANIM_e {
        IN_DOWN, ///< Downward fade-in.
        OUT_UP, ///< Upward fade-out.
        OUT_DOWN, ///< Downward fade-out.
        IN_UP ///< Upward fade-in.
    };

    dWipeDokan_c(nw4r::ut::Color, mFaderBase_c::EStatus stat); ///< See mFaderBase_c::mFaderBase_c.
    ~dWipeDokan_c(); ///< Destroys the fader.

    /// @brief Loads the resources and creates the layout for the fader.
    /// @return Whether the creation was successful.
    bool createLayout();

    /// @brief Prepares the fade-out transition.
    void OpenSetup();

    /// @brief Checks if the fade animation has finished yet.
    /// @details If the animation has finished, sets the status accordingly
    /// and switches back to the ::IDLE action.
    void AnimeEndCheck();

    /// @brief Returns whether the wipe should go upwards or downwards.
    /// @return True = fade-in upwards, fade-out downwards.
    /// False = fade-in downwards, fade-out upwards.
    bool MuKiDecision();

    /// @brief Prepares the fade-in transition.
    void CloseSetup();

    virtual int calc();
    virtual void draw();

    /// @brief Sets the fader's status.
    /// @details The only allowed values are ::OPAQUE and ::HIDDEN.
    virtual void setStatus(mFaderBase_c::EStatus stat);

    virtual bool fadeIn();
    virtual bool fadeOut();

    static dWipeDokan_c *m_instance; ///< The instance of the fader.

private:
    LytBase_c mLyt; ///< The layout for the fader.
    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the fader layout.
    nw4r::lyt::Picture *mpPic[1]; ///< The picture pane containing the fader texture.
    ACTION_e mAction; ///< The action to be performed in ::calc.
    bool mIsCreated; ///< Whether the layout for the fader has been created.
};

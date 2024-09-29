#pragma once
#include <game/mLib/m_color.hpp>

/// @brief Base fader implementation.
/// @ingroup mlib
class mFaderBase_c {
public:

    /// @brief The fader's status.
    enum EStatus {
        OPAQUE, ///< The screen is completely blacked out.
        HIDDEN, ///< The screen is completely unblocked.
        FADE_IN, ///< Transition from OPAQUE to HIDDEN.
        FADE_OUT ///< Transition from HIDDEN to OPAQUE.
    };

    /// @brief Some flags related to the fader.
    /// @todo Figure out what these do.
    /// @unofficial
    enum FLAG_e {
        FADE_IN_COMPLETE  = BIT_FLAG(0),
        FADE_OUT_COMPLETE = BIT_FLAG(1)
    };

    /// @brief Constructs a new fader.
    /// @param color The fader's color.
    /// @param status The fader's initial status (::OPAQUE or ::HIDDEN).
    mFaderBase_c(const mColor &color, EStatus status);

    virtual ~mFaderBase_c(); ///< Destroys the fader.

    virtual void setStatus(EStatus status) = 0; ///< Sets the fader's status.
    virtual EStatus getStatus() const; ///< Gets the fader's status.

    /// @brief Initiates a fade in from pure blacked-out.
    /// @details The screen must be ::OPAQUE for the operation to be executed.
    /// @return If the action was carried out.
    virtual bool fadeIn();

    /// @brief Initiates a fade out from no-obstruction.
    /// @details The screen must be ::HIDDEN for the operation to be executed.
    /// @return If the action was carried out.
    virtual bool fadeOut();

    /// @brief Calculates the fader at the current frame.
    /// @return If the operation was successful.
    virtual int calc();

    virtual void draw() = 0; ///< Draws the fader.

    void setFrame(u16 duration); ///< Sets the duration of the fade. Duration must not be zero.
    void setColor(const mColor &color); ///< Sets the fader's color. Alpha is not modified.

protected:
    EStatus mStatus; ///< The fader's status.
    u8 mFlag; ///< The fader's flags.
    u16 mFrameCount; ///< The fader's duration.
    u16 mCurrFrame; ///< The fader's current frame.
    mColor mFaderColor; ///< The fader's color.
};

#pragma once
#include <dol/mLib/m_color.hpp>

/// @brief Base fader implementation.
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
    /// @note Unofficial name.
    enum FLAG_e {
        FLAG_1 = 1,
        FLAG_2 = 2
    };

    /// @brief Constructs a new fader.
    /// @param color The fader's color.
    /// @param status The fader's initial status (::OPAQUE or ::HIDDEN).
    mFaderBase_c(const mColor &color, EStatus status);

    /// @brief Destroys the fader.
    virtual ~mFaderBase_c();

    /// @brief Sets the fader's status.
    /// @details The implementation is left to the derived classes.
    virtual void setStatus(EStatus status) = 0;

    /// @brief Gets the fader's status.
    virtual EStatus getStatus() const;

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

    /// @brief Draws the fader.
    /// @details The implementation is left to the derived classes.
    virtual void draw() = 0;

    /// @brief Sets the duration of the fade. Duration must not be zero.
    void setFrame(u16 duration);

    /// @brief Sets the fader's color. Alpha is not modified.
    void setColor(const mColor &color);

protected:
    EStatus mStatus; ///< The fader's status.
    u8 mFlag; ///< The fader's flags.
    u16 mFrameCount; ///< The fader's duration.
    u16 mCurrFrame; ///< The fader's current frame.
    mColor mFaderColor; ///< The fader's color.
};

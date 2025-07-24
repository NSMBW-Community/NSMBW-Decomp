#pragma once

namespace EGG {

/// @brief Interface for a fader.
/// @details This seems to be pretty much a copy-paste of mFaderBase_c (or vice-versa)
/// @ingroup eggcore
class Fader {
public:
    /// @copydoc mFaderBase_c::EStatus
    enum EStatus { OPAQUE, HIDDEN, FADE_IN, FADE_OUT };

    virtual void setStatus(EStatus status) = 0; /// @copydoc mFaderBase_c::setStatus
    virtual EStatus getStatus() const = 0;      /// @copydoc mFaderBase_c::getStatus
    virtual bool fadeIn() = 0;                  /// @copydoc mFaderBase_c::fadeIn
    virtual bool fadeOut() = 0;                 /// @copydoc mFaderBase_c::fadeOut
    virtual bool calc() = 0;                    /// @copydoc mFaderBase_c::calc
    virtual void draw() = 0;                    /// @copydoc mFaderBase_c::draw
};

} // namespace EGG

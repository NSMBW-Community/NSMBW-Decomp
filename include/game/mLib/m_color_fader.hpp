#pragma once
#include <game/mLib/m_fader_base.hpp>

/// @brief A fader that fades a color in and out.
/// @ingroup mlib
class mColorFader_c : public mFaderBase_c {
public:
    mColorFader_c(mColor col, mFaderBase_c::EStatus status);

    virtual ~mColorFader_c();
    virtual void setStatus(EStatus status);
    virtual int calc();
    virtual void draw();

private:
    u8 mAspectRatio; ///< The screen's aspect ratio. 0 = 4:3, 1 = 16:9.
};

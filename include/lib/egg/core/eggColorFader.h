#pragma once

#include <lib/egg/core/eggBitFlag.h>
#include <lib/egg/core/eggFader.h>
#include <nw4r/ut.h>

namespace EGG {

/// @ingroup eggcore
class ColorFader : public Fader {
public:
    ColorFader(float x, float y, float w, float h, nw4r::ut::Color color, EStatus initialStatus);

    virtual void setStatus(EStatus status);
    virtual EStatus getStatus() const { return mStatus; }
    virtual bool fadeIn();
    virtual bool fadeOut();
    virtual bool calc();
    virtual void draw();
    virtual ~ColorFader() {}

    void setFrame(u16 frame);
    void setColor(nw4r::ut::Color color);

protected:
    enum eFlag { SIGNAL_ON_FADE_IN, SIGNAL_ON_FADE_OUT };

    EStatus mStatus;
    TBitFlag<u8> mFlag;
    u16 mFrameCount;
    u16 mFrame;
    nw4r::ut::Color mCurrColor;
    nw4r::ut::Rect mDims;
};

} // namespace EGG

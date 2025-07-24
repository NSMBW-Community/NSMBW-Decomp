#pragma once
#include <lib/egg/core/eggFader.h>
#include <lib/nw4r/ut/color.hpp>

namespace EGG {

    /// @ingroup eggcore
    class ColorFader : public Fader {
    public:
        enum ColorFaderFlag {
            FLAG_1 = BIT_FLAG(0),
            FLAG_2 = BIT_FLAG(1)
        };

        ColorFader(float x, float y, float w, float h, nw4r::ut::Color color, EStatus initialStatus);

        virtual void setStatus(EStatus status);
        virtual EStatus getStatus() const { return mStatus; }
        virtual bool fadeIn();
        virtual bool fadeOut();
        virtual bool calc();
        virtual void draw();
        virtual ~ColorFader();

        void setFrame(u16 frame);
        void setColor(nw4r::ut::Color color);

        // [TODO: Belongs to nw4r::ut::Rect]
        float getWidth() const { return mRight - mLeft; }
        // [TODO: Belongs to nw4r::ut::Rect]
        float getHeight() const { return mBottom - mTop; }

    protected:
        EStatus mStatus;
        u8 mFlag; // [TODO: TBitFlag<u8>]
        u16 mFrameCount;
        u16 mFrame;
        nw4r::ut::Color mCurrColor;
        float mLeft; // [TODO: nw4r::ut::Rect]
        float mTop;
        float mRight;
        float mBottom;
    };
}

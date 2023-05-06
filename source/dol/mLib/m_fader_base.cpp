#include <dol/mLib/m_fader_base.hpp>
#include <dol/mLib/m_color.hpp>

mFaderBase_c::mFaderBase_c(const mColor &color, mFaderBase_c::EStatus status) :
mFlag(0),
mFrameCount(20),
mCurrFrame(0),
mFaderColor()
{
    setColor(color);
    mFlag |= FADE_OUT_COMPLETE;
    switch(status) {
        case OPAQUE: mStatus = OPAQUE; break;
        case HIDDEN: mStatus = HIDDEN; break;
    }
}

mFaderBase_c::~mFaderBase_c() {
}

void mFaderBase_c::setFrame(u16 duration) {
    mFrameCount = duration;
}

void mFaderBase_c::setColor(const mColor &color) {
    mFaderColor.r = color.r;
    mFaderColor.g = color.g;
    mFaderColor.b = color.b;
}

mFaderBase_c::EStatus mFaderBase_c::getStatus() const {
    return mStatus;
}

bool mFaderBase_c::fadeIn() {
    bool doFadeIn = mStatus == OPAQUE;
    if (doFadeIn) {
        mStatus = FADE_IN;
        mCurrFrame = 0;
    }

    return doFadeIn;
}

bool mFaderBase_c::fadeOut() {
    bool doFadeIn = mStatus == HIDDEN;
    if (doFadeIn) {
        mStatus = FADE_OUT;
        mCurrFrame = 0;
    }

    return doFadeIn;
}

int mFaderBase_c::calc() {
    int result = 0;

    if (mStatus == FADE_IN) {
        if (mCurrFrame++ > mFrameCount) {
            mStatus = HIDDEN;
            result = (mFlag & FADE_IN_COMPLETE) != 0;
        }

    } else if (mStatus == FADE_OUT) {
        mCurrFrame++;
        if (mCurrFrame > mFrameCount + 1) {
            mStatus = OPAQUE;
            result = (mFlag & FADE_OUT_COMPLETE) != 0;
        }
    }

    return result;
}

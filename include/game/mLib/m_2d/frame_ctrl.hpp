#pragma once
#include <types.h>
#include <nw4r/ut.h>

namespace m2d {

class FrameCtrl_c {
public:
    enum ANM_FLAG_e {
        NO_LOOP = BIT_FLAG(0),
        REVERSE = BIT_FLAG(1),
    };

    virtual ~FrameCtrl_c() {}

    void play();
    void set(float endFrame, u8 flags, float rate, float currFrame);
    void setFrame(float frame);
    void setRate(float rate);
    bool isStop() const;

    float getFrame() const { return mCurrFrame; }
    float getLastActiveFrame() const { return mEndFrame - 1.0f; }

    float mEndFrame;
    float mCurrFrame;
    float mPrevFrame;
    float mRate;
    u8 mFlags;
};

} // namespace m2d

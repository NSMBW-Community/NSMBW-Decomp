#pragma once
#include <lib/nw4r/ut/color.hpp>
#include <_dummy_classes.hpp>

class mFaderBase_c {
public:
    enum EStatus {
        OPAQUE = 0, ///< The screen is completely blacked out.
        HIDDEN = 1, ///< The screen is completely unblocked.
        FADE_IN = 2, ///< Transition from OPAQUE -> HIDDEN
        FADE_OUT = 3 ///< Transition from HIDDEN -> OPAQUE
    };

    mFaderBase_c(const mColor &color, EStatus status);
    virtual ~mFaderBase_c();

    virtual void setStatus(EStatus status) = 0;
    virtual EStatus getStatus() const;
    virtual void fadeIn();
    virtual void fadeOut();
    virtual void calc();
    virtual void draw();
    
    void setFrame(unsigned short duration);
    void setColor(const mColor &color);
};

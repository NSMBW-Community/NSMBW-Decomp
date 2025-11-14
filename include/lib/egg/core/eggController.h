#pragma once

#include <types.h>
#include <revolution/PAD.h>

namespace EGG {

class CoreController {
public:
    virtual void setPosParam(float, float);
    virtual void setHoriParam(float, float);
    virtual void setDistParam(float, float);
    virtual void setAccParam(float, float);
    virtual bool down(ulong) const;
    virtual bool up(ulong) const;
    virtual bool downTrigger(ulong) const;
    virtual bool upTrigger(ulong) const;
    virtual bool downAll(ulong) const;
    virtual bool upAll(ulong) const;
    virtual void beginFrame(PADStatus *);
    virtual void endFrame();

    void startPatternRumble(const char *, int, bool);
};

} // namespace EGG

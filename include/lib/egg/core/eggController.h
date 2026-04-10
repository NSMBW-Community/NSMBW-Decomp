#pragma once

#include <types.h>
#include <revolution/PAD.h>
#include <revolution/KPAD.h>

namespace EGG {

class CoreController {
public:
    virtual void setPosParam(float a, float b) { KPADSetPosParam(mNum, a, b); }
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

    int mNum;
};

class CoreControllerMgr {
public:
    static void createInstance();

    static u32 sWPADWorkSize;
};

} // namespace EGG

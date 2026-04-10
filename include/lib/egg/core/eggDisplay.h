#pragma once

#include <types.h>
#include <lib/nw4r/ut.h>

namespace EGG {

class Display {
public:
    u8 mPad1[0x4];

    Display(u8 param);

    virtual void beginFrame();
    virtual void beginRender();
    virtual void endRender();
    virtual void endFrame();
    virtual u32 getTickPerFrame();

    void setClearColor(nw4r::ut::Color color) { mClearColor = color; }
    nw4r::ut::Color getClearColor() { return mClearColor; }

    u8 mFrameRate;
    volatile u8 mScreenStateFlag;
    u8 mPad2[0x8];
    nw4r::ut::Color mClearColor;
};

} // namespace EGG

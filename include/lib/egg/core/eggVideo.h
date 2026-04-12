#pragma once

#include <revolution/GX.h>

namespace EGG {

class Video {
public:
    class RenderModeObjSet {};

    Video(GXRenderModeObj *renderMode, const RenderModeObjSet *rmSet) : pRenderMode(nullptr), mFlag(0), m_08(0) {
        initialize(renderMode, rmSet);
    }

    void initialize(GXRenderModeObj *renderMode, const RenderModeObjSet *rmSet);

    bool isBlack() { return mFlag & 1; }

    static u32 getTickPerVRetrace();
    static u32 getTickPerVRetrace(ulong);

    const GXRenderModeObj *pRenderMode;
    u8 mFlag;
    u32 m_08;
};

} // namespace EGG

#pragma once
#include <revolution/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;

    static float getSmth(float offs) { return (m_video->mRenderModeObj.efbHeight - offs) * 0.5f; }

    static mVideo *m_video;
};

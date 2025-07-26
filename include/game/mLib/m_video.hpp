#pragma once
#include <lib/rvl/gx/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;

    static float getSmth(float offs) { return (m_video->mRenderModeObj.mEFBHeight - offs) * 0.5f; }

    static mVideo *m_video;
};

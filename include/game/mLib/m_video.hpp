#pragma once
#include <lib/rvl/gx/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;
    static float l_rayoutWidthF;
    static float l_rayoutHeightF;

    static float getLayoutWidth() { return l_rayoutWidthF; }
    static float getLayoutHeight() { return l_rayoutHeightF; }

    static float getSmth(float offs) { return (m_video->mRenderModeObj.mEFBHeight - offs) * 0.5f; }

    static mVideo *m_video;
};

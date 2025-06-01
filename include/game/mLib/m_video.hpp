#pragma once
#include <lib/rvl/gx/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;
    static float l_rayoutWidthF;
    static float l_rayoutHeightF;

    static float getLayoutWidth() { return l_rayoutWidthF; }
    static float getLayoutHeight() { return l_rayoutHeightF; }

    static mVideo *m_video;
};

#pragma once
#include <revolution/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;

    float getWidth() const { return mRenderModeObj.fbWidth; }
    float getHeight() const { return mRenderModeObj.efbHeight; }

    static void create();

    static float getLayoutWidth() { return l_rayoutWidthF; }
    static float getLayoutHeight() { return l_rayoutHeightF; }

    static float getSmth(float offs) { return (m_video->mRenderModeObj.efbHeight - offs) * 0.5f; }

    static float l_rayoutWidthF;
    static float l_rayoutHeightF;

    static mVideo *m_video;
};

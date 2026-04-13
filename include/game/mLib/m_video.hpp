#pragma once
#include <revolution/GX.h>

class mVideo {
public:
    static void create();

    float getWidth() const { return mRenderModeObj.fbWidth; }
    float getHeight() const { return mRenderModeObj.efbHeight; }

    static float getSmth(float offs) { return (m_video->mRenderModeObj.efbHeight - offs) * 0.5f; }

    GXRenderModeObj &mRenderModeObj;

    static mVideo *m_video;
};

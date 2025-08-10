#pragma once
#include <revolution/GX.h>

class mVideo {
public:
    GXRenderModeObj &mRenderModeObj;

    float getScaledWidth(float scale) const { return mRenderModeObj.fbWidth / scale; }
    float getScaledHeight(float scale) const { return mRenderModeObj.efbHeight / scale; }

    static float getSmth(float offs) { return (m_video->mRenderModeObj.efbHeight - offs) * 0.5f; }

    static mVideo *m_video;
};

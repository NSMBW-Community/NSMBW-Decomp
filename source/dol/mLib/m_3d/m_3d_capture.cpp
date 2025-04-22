#include <game/mLib/m_3d.hpp>

#include <game/mLib/m_video.hpp>
#include <lib/egg/gx/eggGlobalDrawState.hpp>
#include <lib/egg/gx/eggScreenEffectBase.hpp>

// [This might be inside a different TU, not sure]
void m3d::screenEffectReset(int cameraID, EGG::Screen &scr) {
    nw4r::math::MTX34 mtx;
    m3d::getCamera(cameraID).GetCameraMtx(&mtx);
    EGG::GlobalDrawState::sDrawFlag = 5;
    EGG::GlobalDrawState::beginDrawView(cameraID, mtx, scr);
    EGG::ScreenEffectBase::sScreen.CopyFromAnother(scr);
}

bool m3d::capture_c::create(
    EGG::Heap *heap,
    u16 width, u16 height,
    GXTexFmt texFmt, GXTexFilter texFilter,
    bool texIsHalfSize
) {
    mTexWidth = width;
    mTexHeight = height;
    if (texIsHalfSize) {
        width /= 2;
        height /= 2;
    }

    size_t bufSize = GXGetTexBufferSize(width, height, texFmt, 0, 0);
    mTexBuffer = heap->alloc(bufSize, 0x20);
    if (mTexBuffer == nullptr) {
        return false;
    }

    mBufferSize = bufSize;
    mTexIsHalfSize = texIsHalfSize;
    mTexFmt = texFmt;
    mTexFilter = texFilter;

    GXInitTexObj(&mTexObj, mTexBuffer, width, height, texFmt, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&mTexObj, mTexFilter, mTexFilter, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    return true;
}

void m3d::capture_c::remove() {
    if (mTexBuffer != nullptr) {
        EGG::Heap::free(mTexBuffer, nullptr);
        mTexBuffer = nullptr;
        mBufferSize = 0;
    }
}

void m3d::capture_c::capture(u16 width, u16 height, bool texIsHalfSize) {
    captureEx(mTexWidth, mTexHeight, mTexFmt, mTexFmt, mTexFilter, mTexIsHalfSize, width, height, texIsHalfSize);
}

void m3d::capture_c::captureEx(
    u16 width, u16 height,
    GXTexFmt texFmt, GXTexFmt tex2Fmt,
    GXTexFilter texFilter,
    bool texIsHalfSize,
    u16 srcTexWidth, u16 srcTexHeight,
    bool srcTexHalfSize
) {
    u16 actualW = width;
    u16 actualH = height;
    if (texIsHalfSize) {
        actualW /= 2;
        actualH /= 2;
    }

    GXInitTexObj(&mTexObj, mTexBuffer, actualW, actualH, tex2Fmt, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&mTexObj, texFilter, texFilter, 0, 0, 0, 0.0f, 0.0f, 0.0f);
    GXSetCopyFilter(0, nullptr, 0, nullptr);

    GXSetTexCopySrc(srcTexWidth, srcTexHeight, width, height);
    GXSetTexCopyDst(actualW, actualH, texFmt, texIsHalfSize);
    GXCopyTex(mTexBuffer, srcTexHalfSize);

    GXRenderModeObj &s = mVideo::m_video->mRenderModeObj;
    GXSetCopyFilter(s.mAntialias, &s.samplePattern, 1, s.filterWeights);
}

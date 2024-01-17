#include <dol/mLib/m_3d.hpp>

#include <dol/mLib/m_video.hpp>
#include <lib/egg/gx/eggScreen.hpp>
#include <lib/egg/gx/eggGlobalDrawState.hpp>
#include <lib/egg/gx/eggScreenEffectBase.hpp>
#include <lib/rvl/gx/GX.h>

// Not sure what TU this function belongs in
void idk(int cameraID, EGG::Screen &scr) {
    nw4r::math::MTX34 mtx;
    m3d::getCamera(cameraID).GetCameraMtx(&mtx);
    EGG::GlobalDrawState::sDrawFlag = 5;
    EGG::GlobalDrawState::beginDrawView(cameraID, mtx, scr);
    EGG::ScreenEffectBase::sScreen.CopyFromAnother(scr);
}

// m3d/capture.cpp

inline u16 actualDim(u16 size, bool half) {
    return half ? size / 2 : size;
}

bool m3d::capture_c::create(EGG::Heap *heap, u16 width, u16 height, GXTexFmt texFmt, GXTexFilter texFilter, bool halfSize) {
    mTexWidth = width;
    mTexHeight = height;
    if (halfSize) {
        width /= 2;
        height /= 2;
    }
    int bufSize = GXGetTexBufferSize(width, height, texFmt, 0, 0);
    void *texBuffer = heap->alloc(bufSize, 0x20);
    this->texBuf = texBuffer;
    if (texBuffer == nullptr) {
        return false;
    }
    this->bufferSize = bufSize;
    this->texHalfSize = halfSize;
    this->texFmt = texFmt;
    this->texFilter = texFilter;
    GXInitTexObj(&this->texObj, texBuffer, width, height, texFmt, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&this->texObj, this->texFilter, this->texFilter, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    return true;
}

void m3d::capture_c::remove() {
    if (texBuf != nullptr) {
        EGG::Heap::free(texBuf, nullptr);
        texBuf = nullptr;
        bufferSize = 0;
    }
}

void m3d::capture_c::capture(u16 width, u16 height, bool halfSize) {
    captureEx(mTexWidth, mTexHeight, texFmt, texFmt, texFilter, texHalfSize, width, height, halfSize);
}

void m3d::capture_c::captureEx(u16 width, u16 height, GXTexFmt texFmt, GXTexFmt texFmt2, GXTexFilter texFilter, bool halfSize, u16 width2, u16 height2, bool halfSize2) {
    u16 actualW = width;
    u16 actualH = height;
    if (halfSize) {
        actualW /= 2;
        actualH /= 2;
    }
    GXInitTexObj(&texObj, texBuf, actualW, actualH, texFmt2, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&texObj, texFilter, texFilter, 0, 0, 0, 0.0f, 0.0f, 0.0f);
    GXSetCopyFilter(0, nullptr, 0, nullptr);
    GXSetTexCopySrc(width2, height2, width, height);
    GXSetTexCopyDst(actualW, actualH, texFmt, halfSize);
    GXCopyTex(texBuf, halfSize2);
    GXRenderModeObj &s = mVideo::m_video->obj;
    GXSetCopyFilter(s.s1, &s.stuff, 1, s.stuff2);
}

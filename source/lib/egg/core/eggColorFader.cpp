#include <lib/egg/core/eggColorFader.h>

#include <lib/rvl/gx/GX.h>

namespace EGG {

ColorFader::ColorFader(float x, float y, float w, float h, nw4r::ut::Color color,
        Fader::EStatus initialStatus)
    : mFlag(),
      mFrameCount(20),
      mFrame(0),
      mCurrColor(),
      mDims(x, y, x + w, y + h) {
    setColor(color);
    setStatus(initialStatus);
    mFlag.setBit(ColorFader::SIGNAL_ON_FADE_OUT);
}

void ColorFader::setFrame(u16 frame) {
    EGG_ASSERT(frame != 0);
    mFrameCount = frame;
}

void ColorFader::setColor(nw4r::ut::Color color) {
    mCurrColor.r = color.r;
    mCurrColor.g = color.g;
    mCurrColor.b = color.b;
    // Don't set alpha
}

void ColorFader::setStatus(Fader::EStatus status) {
    if (status == Fader::OPAQUE) {
        mStatus = Fader::OPAQUE;
        mCurrColor.a = 255;
    } else if (status == Fader::HIDDEN) {
        mStatus = Fader::HIDDEN;
        mCurrColor.a = 0;
    }
}

bool ColorFader::fadeIn() {
    bool start = mStatus == Fader::OPAQUE;
    if (start) {
        mStatus = Fader::FADE_IN;
        mFrame = 0;
    }

    return start;
}

bool ColorFader::fadeOut() {
    bool start = mStatus == Fader::HIDDEN;
    if (start) {
        mStatus = Fader::FADE_OUT;
        mFrame = 0;
    }

    return start;
}

bool ColorFader::calc() {
    bool result = false;

    if (mStatus == Fader::HIDDEN) {
        mCurrColor.a = 0;
    } else if (mStatus == Fader::OPAQUE) {
        mCurrColor.a = 255;
    } else if (mStatus == Fader::FADE_IN) {
        u16 endFrame = mFrameCount;
        u16 currFrame = mFrame++;

        if (currFrame > endFrame) {
            mStatus = Fader::HIDDEN;
            result = mFlag.onBit(ColorFader::SIGNAL_ON_FADE_IN);
            currFrame = endFrame;
        }

        mCurrColor.a = 255 - (currFrame * 255 / mFrameCount);
    } else if (mStatus == Fader::FADE_OUT) {
        u16 endFrame = mFrameCount;
        u16 currFrame = mFrame++;

        if (currFrame > endFrame) {
            if (currFrame > endFrame + 1) {
                mStatus = Fader::OPAQUE;
                result = mFlag.onBit(ColorFader::SIGNAL_ON_FADE_OUT);
            }

            endFrame = mFrameCount;
            currFrame = endFrame;
        }

        mCurrColor.a = currFrame * 255 / endFrame;
    }

    return result;
}

void ColorFader::draw() {
    if (mCurrColor.a == 0) {
        return;
    }

    Mtx44 projMtx;
    C_MTXOrtho(&projMtx, mDims.top, mDims.bottom, mDims.left, mDims.right, 0.0f, 1.0f);
    GXSetProjection(&projMtx, GX_ORTHOGRAPHIC);

    GXSetViewport(mDims.left, mDims.top, mDims.GetWidth(), mDims.GetHeight(), 0.0f, 1.0f);
    GXSetScissor(mDims.left, mDims.top, mDims.GetWidth(), mDims.GetHeight());

    Mtx posMtx;
    PSMTXIdentity(&posMtx);
    GXLoadPosMtxImm(&posMtx, 0);
    GXSetCurrentMtx(0);

    GXClearVtxDesc();
    GXInvalidateVtxCache();

    GXSetVtxDesc(GX_VA_POS, GX_VA_TEX0MTXIDX);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

    GXSetNumChans(1);
    GXSetChanMatColor(GX_COLOR0A0, mCurrColor);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    GXSetNumTexGens(0);
    GXSetNumIndStages(0);
    __GXSetIndirectMask(0);

    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, 4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);

    if (mCurrColor.a == 255) {
        GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);
    } else {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }

    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetZMode(0, GX_NEVER, 0);
    GXSetCullMode(2);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition3f32(mDims.left, mDims.top, 0.0f);
    GXPosition3f32(mDims.right, mDims.top, 0.0f);
    GXPosition3f32(mDims.right, mDims.bottom, 0.0f);
    GXPosition3f32(mDims.left, mDims.bottom, 0.0f);

    GXEnd();
}

} // namespace EGG

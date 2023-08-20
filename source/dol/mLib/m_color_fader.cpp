#include <dol/mLib/m_color_fader.hpp>
#include <lib/rvl/sc/SC.h>
#include <lib/rvl/vi/VI.h>

mColorFader_c::mColorFader_c(mColor col, mFaderBase_c::EStatus status) : mFaderBase_c(col, status) {
    mAspectRatio = SCGetAspectRatio();
}

mColorFader_c::~mColorFader_c() {
}

void mColorFader_c::setStatus(EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;
        mFaderColor.a = 255;
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;
        mFaderColor.a = 0;
    }
}

int mColorFader_c::calc() {
    int res = mFaderBase_c::calc();

    u16 currFrame = mCurrFrame;
    u16 endFrame = mFrameCount;

    // Clamp to the duration of the fader
    if (currFrame > mFrameCount) {
        currFrame = mFrameCount;
    }

    switch (mStatus) {
        case HIDDEN: mFaderColor.a = 0; break;
        case OPAQUE: mFaderColor.a = 255; break;

        // Linearly interpolate the alpha value
        case FADE_IN: mFaderColor.a = 255 - (currFrame * 255 / endFrame); break;
        case FADE_OUT: mFaderColor.a = (currFrame * 255 / endFrame); break;
    }

    return res;
}

void mColorFader_c::draw() {
    float h = (mAspectRatio == 1) ? VI_VIRTUAL_HALF_WIDTH_WIDE : VI_VIRTUAL_HALF_WIDTH_STD;

    if (mFaderColor.a == 0) {
        return;
    }

    Mtx44 projMtx;
    C_MTXOrtho(&projMtx, -VI_VIRTUAL_HALF_HEIGHT, VI_VIRTUAL_HALF_HEIGHT, -h, h, 0, 1);
    GXSetProjection(&projMtx, 1);

    Mtx posMtx;
    PSMTXIdentity(&posMtx);
    GXLoadPosMtxImm(&posMtx, 0);
    GXSetCurrentMtx(0);

    GXClearVtxDesc();
    GXInvalidateVtxCache();

    GXSetVtxDesc(GX_VA_POS, GX_VA_TEX0MTXIDX);
    GXSetVtxAttrFmt(0, GX_VA_POS, 1, 4, 0);

    GXSetNumChans(1);
    GXSetChanMatColor(GX_COLOR0A0, mFaderColor);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    GXSetNumTexGens(0);
    GXSetNumIndStages(0);
    __GXSetIndirectMask(0);

    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, 4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);

    if (mFaderColor.a == 255) {
        GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);
    } else {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }

    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetZMode(0, GX_NEVER, 0);
    GXSetCullMode(2);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition3f32(-h, -VI_VIRTUAL_HALF_HEIGHT, 0);
    GXPosition3f32(h, -VI_VIRTUAL_HALF_HEIGHT, 0);
    GXPosition3f32(h, VI_VIRTUAL_HALF_HEIGHT, 0);
    GXPosition3f32(-h, VI_VIRTUAL_HALF_HEIGHT, 0);

    GXEnd();
}

#include <lib/egg/fader/eggColorFader.hpp>
#include <lib/rvl/gx/GX.h>

EGG::ColorFader::ColorFader(float x, float y, float w, float h, nw4r::ut::Color color, Fader::EStatus initialStatus) :
    mFlag(0),
    mFrameCount(20),
    mFrame(0),
    mCurrColor(),
    mLeft(x),
    mTop(y),
    mRight(x + w),
    mBottom(y + h) {
    setColor(color);
    setStatus(initialStatus);
    mFlag |= 2;
}

void EGG::ColorFader::setFrame(u16 frames) {
    mFrameCount = frames;
}

void EGG::ColorFader::setColor(nw4r::ut::Color color) {
    mCurrColor.r = color.r;
    mCurrColor.g = color.g;
    mCurrColor.b = color.b;
    // Don't set alpha
}

void EGG::ColorFader::setStatus(Fader::EStatus status) {
    if (status == Fader::OPAQUE) {
        mStatus = Fader::OPAQUE;
        mCurrColor.a = 255;
    } else if (status == Fader::HIDDEN) {
        mStatus = Fader::HIDDEN;
        mCurrColor.a = 0;
    }
}

bool EGG::ColorFader::fadeIn() {
    bool doFadeIn = mStatus == Fader::OPAQUE;
    if (doFadeIn) {
        mStatus = Fader::FADE_IN;
        mFrame = 0;
    }

    return doFadeIn;
}

bool EGG::ColorFader::fadeOut() {
    bool doFadeIn = mStatus == Fader::HIDDEN;
    if (doFadeIn) {
        mStatus = Fader::FADE_OUT;
        mFrame = 0;
    }

    return doFadeIn;
}

int EGG::ColorFader::calc() {
    int result = 0;

    if (mStatus == Fader::HIDDEN) {
        mCurrColor.a = 0;
    } else if (mStatus == Fader::OPAQUE) {
        mCurrColor.a = 255;
    } else if (mStatus == Fader::FADE_IN) {
        u16 currFrame = mFrame;
        u16 endFrame = mFrameCount;
        mFrame++;
        if (currFrame > endFrame) {
            mStatus = Fader::HIDDEN;
            result = (mFlag & ColorFader::FLAG_1) != 0;
            currFrame = endFrame;
        }
        mCurrColor.a = 255 - (currFrame * 255 / mFrameCount);
    } else if (mStatus == Fader::FADE_OUT) {
        u16 endFrame = mFrameCount;
        u16 currFrame = mFrame;
        mFrame++;
        if (currFrame > endFrame) {
            if (currFrame > endFrame + 1) {
                mStatus = Fader::OPAQUE;
                result = (mFlag & ColorFader::FLAG_2) != 0;
            }
            endFrame = mFrameCount;
            currFrame = endFrame;
        }
        mCurrColor.a = currFrame * 255 / endFrame;
    }

    return result;
}

void EGG::ColorFader::draw() {
    if (mCurrColor.a == 0) {
        return;
    }

    Mtx44 projMtx;
    C_MTXOrtho(&projMtx, mTop, mBottom, mLeft, mRight, 0, 1);
    GXSetProjection(&projMtx, 1);

    GXSetViewport(mLeft, mTop, getWidth(), getHeight(), 0, 1);
    GXSetScissor(mLeft, mTop, getWidth(), getHeight());

    Mtx posMtx;
    PSMTXIdentity(&posMtx);
    GXLoadPosMtxImm(&posMtx, 0);
    GXSetCurrentMtx(0);

    GXClearVtxDesc();
    GXInvalidateVtxCache();

    GXSetVtxDesc(GX_VA_POS, GX_VA_TEX0MTXIDX);
    GXSetVtxAttrFmt(0, GX_VA_POS, 1, 4, 0);

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

    GXPosition3f32(mLeft, mTop, 0.0f);
    GXPosition3f32(mRight, mTop, 0.0f);
    GXPosition3f32(mRight, mBottom, 0.0f);
    GXPosition3f32(mLeft, mBottom, 0.0f);

    GXEnd();
}

EGG::ColorFader::~ColorFader() {
}

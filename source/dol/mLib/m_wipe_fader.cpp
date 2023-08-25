#include <game/mLib/m_wipe_fader.hpp>
#include <lib/rvl/sc/SC.h>
#include <lib/rvl/vi/VI.h>

mWipeFader_c::mWipeFader_c(mColor col, mFaderBase_c::EStatus status) :
    mFaderBase_c(col, status),
    mpTextureData(nullptr) {

    PSMTXIdentity(&mTexMtx);
    mTexWidth = 0;
    mTexHeight = 0;
    if (SCGetAspectRatio() == 1) {
        // [Should just be VI_VIRTUAL_WIDTH_WIDE / VI_VIRTUAL_WIDTH_STD,
        // but that way of calculating the constant doesn't work...]
        mAspectRatioFactor = 1.36842095f;
    } else {
        mAspectRatioFactor = 1;
    }
}

mWipeFader_c::~mWipeFader_c() {}

void mWipeFader_c::setTexture(void *data, int width, int height) {
    mpTextureData = data;
    mTexWidth = width;
    mTexHeight = height;
}

void mWipeFader_c::setStatus(EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;
        mProgress = 255;
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;
        mProgress = 0;
    }
}

int mWipeFader_c::calc() {
    int res = mFaderBase_c::calc();

    u16 currFrame = mCurrFrame;
    u16 endFrame = mFrameCount;

    if (currFrame > mFrameCount) {
        currFrame = mFrameCount;
    }

    switch (mStatus) {
        case OPAQUE: mProgress = 255; break;
        case HIDDEN: mProgress = 0; break;
        case FADE_IN: mProgress = 255 - (currFrame * 255 / endFrame); break;
        case FADE_OUT: mProgress = (currFrame * 255 / endFrame); break;
    }

    calcMtx();
    return res;
}

void mWipeFader_c::calcMtx() {
    // Scales from 0.5x to 128.0x the screen size [This seems like a terrible scaling function...?]
    float scale = 128.0f / (256 - mProgress);

    // Move to center, scale, move back
    PSMTXTrans(&mTexMtx, 0.5f, 0.5f, 0.0f);

    Mtx scaleMtx;
    float scrW = mAspectRatioFactor * VI_VIRTUAL_WIDTH_STD;
    float scrH = VI_VIRTUAL_HEIGHT;
    PSMTXScale(&scaleMtx, scale, scale * scrH / scrW, 0.0f);
    PSMTXConcat(&mTexMtx, &scaleMtx, &mTexMtx);

    Mtx transMtx;
    PSMTXTrans(&transMtx, -0.5f, -0.5f, 0.0f);
    PSMTXConcat(&mTexMtx, &transMtx, &mTexMtx);
}

void mWipeFader_c::draw() {
    float width = mAspectRatioFactor * VI_VIRTUAL_HALF_WIDTH_STD;
    if (mProgress == 0) {
        return;
    }

    Mtx44 projMtx;
    C_MTXOrtho(&projMtx, -VI_VIRTUAL_HALF_HEIGHT, VI_VIRTUAL_HALF_HEIGHT, -width, width, 0.0, 1.0);
    GXSetProjection(&projMtx, 1);

    Mtx posMtx;
    PSMTXIdentity(&posMtx);
    GXLoadPosMtxImm(&posMtx, 0);
    GXSetCurrentMtx(0);
    if (mProgress >= 255) {
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

        GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);

        GXSetColorUpdate(1);
        GXSetAlphaUpdate(1);
        GXSetZMode(0, GX_NEVER, 0);
        GXSetCullMode(2);

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);

        GXPosition3f32(-width, -VI_VIRTUAL_HALF_HEIGHT, 0);
        GXPosition3f32(width, -VI_VIRTUAL_HALF_HEIGHT, 0);
        GXPosition3f32(width, VI_VIRTUAL_HALF_HEIGHT, 0);
        GXPosition3f32(-width, VI_VIRTUAL_HALF_HEIGHT, 0);

        GXEnd();
    } else {
        GXClearVtxDesc();
        GXInvalidateVtxCache();

        GXSetVtxDesc(GX_VA_POS, GX_VA_TEX0MTXIDX);
        GXSetVtxDesc(GX_VA_TEX0, GX_VA_TEX0MTXIDX);
        GXSetVtxAttrFmt(0, GX_VA_POS, 1, 4, 0);
        GXSetVtxAttrFmt(0, GX_VA_TEX0, 1, 4, 0);

        GXSetNumChans(0);
        GXSetChanMatColor(GX_COLOR0A0, mFaderColor);
        GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

        GXTexObj texObj;
        GXInitTexObj(&texObj, mpTextureData, mTexWidth, mTexHeight, GX_TF_I4, GX_CLAMP, GX_CLAMP, 0);
        GXLoadTexObj(&texObj, GX_TEXMAP0);

        Mtx texMtx;
        PSMTXCopy(&mTexMtx, &texMtx);
        GXLoadTexMtxImm(&texMtx, 30, GX_MTX_2x4);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 30, 0, 125);

        GXSetNumTexGens(1);
        GXSetNumIndStages(0);
        __GXSetIndirectMask(0);

        GXSetNumTevStages(1);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_TEV_SCALE_0, 1, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_TEV_SCALE_0, 1, GX_TEVPREV);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);

        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);

        GXSetZCompLoc(1);
        GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);
        GXSetZMode(0, GX_NEVER, 0);
        GXSetCullMode(2);

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);

        GXPosition3f32(-width, -VI_VIRTUAL_HALF_HEIGHT, 0);
        GXTexCoord2f32(0, 0);
        GXPosition3f32(width, -VI_VIRTUAL_HALF_HEIGHT, 0);
        GXTexCoord2f32(1, 0);
        GXPosition3f32(width, VI_VIRTUAL_HALF_HEIGHT, 0);
        GXTexCoord2f32(1, 1);
        GXPosition3f32(-width, VI_VIRTUAL_HALF_HEIGHT, 0);
        GXTexCoord2f32(0, 1);

        GXEnd();
    }
}

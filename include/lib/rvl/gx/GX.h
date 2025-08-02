#pragma once
#include <types.h>
#include <lib/rvl/mtx/mtx.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup gx
/// @{

/// @brief A 32-bit RGBA color.
typedef struct _GXColor {
    union {
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        };
        u32 rgba;
    };
} GXColor;

typedef enum _GXAttr {
    GX_VA_PNMTXIDX,
    GX_VA_TEX0MTXIDX,
    GX_VA_TEX1MTXIDX,
    GX_VA_TEX2MTXIDX,
    GX_VA_TEX3MTXIDX,
    GX_VA_TEX4MTXIDX,
    GX_VA_TEX5MTXIDX,
    GX_VA_TEX6MTXIDX,
    GX_VA_TEX7MTXIDX,
    GX_VA_POS,
    GX_VA_NRM,
    GX_VA_CLR0,
    GX_VA_CLR1,
    GX_VA_TEX0,
    GX_VA_TEX1,
    GX_VA_TEX2,
    GX_VA_TEX3,
    GX_VA_TEX4,
    GX_VA_TEX5,
    GX_VA_TEX6,
    GX_VA_TEX7,
    GX_POS_MTX_ARRAY,
    GX_NRM_MTX_ARRAY,
    GX_TEX_MTX_ARRAY,
    GX_LIGHT_ARRAY,
    GX_VA_NBT,

    GX_VA_MAX_ATTR,
    GX_VA_NULL = 255
} GXAttr;

typedef enum _GXMtxType {
    GX_MTX_3x4,
    GX_MTX_2x4,
} GXMtxType;

typedef enum _GXVtxFmt {
    GX_VTXFMT0,
} GXVtxFmt;

typedef enum _GXPrimitive {
    GX_POINTS = 0xB8,
    GX_LINES = 0xA8,
    GX_LINESTRIP = 0xB0,
    GX_TRIANGLES = 0x90,
    GX_TRIANGLESTRIP = 0x98,
    GX_TRIANGLEFAN = 0xA0,
    GX_QUADS = 0x80,
} GXPrimitive;

typedef enum _GXCompare {
    GX_NEVER,
    GX_LESS,
    GX_EQUAL,
    GX_LEQUAL,
    GX_GREATER,
    GX_NEQUAL,
    GX_GEQUAL,
    GX_ALWAYS
} GXCompare;

typedef enum _GXCompCnt {
    GX_POS_XY = 0,
    GX_POS_XYZ,

    GX_NRM_XYZ = 0,
    GX_NRM_NBT,
    GX_NRM_NBT3,

    GX_CLR_RGB = 0,
    GX_CLR_RGBA,

    GX_TEX_S = 0,
    GX_TEX_ST
} GXCompCnt;

typedef enum _GXCompType {
    GX_U8,
    GX_S8,
    GX_U16,
    GX_S16,
    GX_F32,

    GX_RGB565 = 0,
    GX_RGB8,
    GX_RGBX8,
    GX_RGBA4,
    GX_RGBA6,
    GX_RGBA8
} GXCompType;

typedef enum _GXTevStageID {
    GX_TEVSTAGE0,
    GX_TEVSTAGE1,
    GX_TEVSTAGE2,
    GX_TEVSTAGE3,
    GX_TEVSTAGE4,
    GX_TEVSTAGE5,
    GX_TEVSTAGE6,
    GX_TEVSTAGE7,
    GX_TEVSTAGE8,
    GX_TEVSTAGE9,
    GX_TEVSTAGE10,
    GX_TEVSTAGE11,
    GX_TEVSTAGE12,
    GX_TEVSTAGE13,
    GX_TEVSTAGE14,
    GX_TEVSTAGE15,

    GX_MAX_TEVSTAGE
} GXTevStageID;

typedef enum _GXTevKColorID {
    GX_KCOLOR0,
    GX_KCOLOR1,
    GX_KCOLOR2,
    GX_KCOLOR3,

    GX_MAX_KCOLOR
} GXTevKColorID;

typedef enum _GXTevColorArg {
    GX_CC_CPREV,
    GX_CC_APREV,
    GX_CC_C0,
    GX_CC_A0,
    GX_CC_C1,
    GX_CC_A1,
    GX_CC_C2,
    GX_CC_A2,
    GX_CC_TEXC,
    GX_CC_TEXA,
    GX_CC_RASC,
    GX_CC_RASA,
    GX_CC_ONE,
    GX_CC_HALF,
    GX_CC_KONST,
    GX_CC_ZERO,
    GX_CC_TEXRRR,
    GX_CC_TEXGGG,
    GX_CC_TEXBBB,

    GX_CC_QUARTER = GX_CC_KONST
} GXTevColorArg;

typedef enum _GXTevAlphaArg {
    GX_CA_APREV,
    GX_CA_A0,
    GX_CA_A1,
    GX_CA_A2,
    GX_CA_TEXA,
    GX_CA_RASA,
    GX_CA_KONST,
    GX_CA_ZERO,
    GX_CA_ONE
} GXTevAlphaArg;

typedef enum _GXTevBias {
    GX_TB_ZERO,
    GX_TB_ADDHALF,
    GX_TB_SUBHALF,

    GX_MAX_TEVBIAS
} GXTevBias;

typedef enum _GXTevScale {
    GX_TEV_SCALE_0,
    GX_TEV_SCALE_1,
    GX_TEV_SCALE_2,
    GX_TEV_SCALE_3,
} GXTevScale;

typedef enum _GXTevOp {
    GX_TEV_ADD,
    GX_TEV_SUB,

    GX_TEV_COMP_R8_GT = 8,
    GX_TEV_COMP_R8_EQ,
    GX_TEV_COMP_GR16_GT,
    GX_TEV_COMP_GR16_EQ,
    GX_TEV_COMP_BGR24_GT,
    GX_TEV_COMP_BGR24_EQ,
    GX_TEV_COMP_RGB8_GT,
    GX_TEV_COMP_RGB8_EQ,

    GX_TEV_COMP_A8_GT = GX_TEV_COMP_RGB8_GT,
    GX_TEV_COMP_A8_EQ = GX_TEV_COMP_RGB8_EQ
} GXTevOp;

typedef enum _GXTevRegID {
    GX_TEVPREV,
    GX_TEVREG0,
    GX_TEVREG1,
    GX_TEVREG2,

    GX_MAX_TEVREG
} GXTevRegID;

typedef enum _GXAlphaOp {
    GX_AOP_AND,
    GX_AOP_OR,
    GX_AOP_XOR,
    GX_AOP_XNOR,

    GX_MAX_ALPHAOP
} GXAlphaOp;

typedef struct _GXTexObj {
    char data[32];
} GXTexObj;

typedef enum _GXTexFmt {
    GX_TF_I4,
    GX_TF_I8,
    GX_TF_IA4,
    GX_TF_IA8,
    GX_TF_RGB565,
    GX_TF_RGB5A3,
    GX_TF_RGBA8,
    GX_TF_CMPR = 14,

    GX_CTF_R4 = 32,
    GX_CTF_RA4 = 34,
    GX_CTF_RA8 = 35,
    GX_CTF_YUVA8 = 38,
    GX_CTF_A8 = 39,
    GX_CTF_R8 = 40,
    GX_CTF_G8 = 41,
    GX_CTF_B8 = 42,
    GX_CTF_RG8 = 43,
    GX_CTF_GB8 = 44,

    GX_TF_Z8 = 17,
    GX_TF_Z16 = 19,
    GX_TF_Z24X8 = 22,

    GX_CTF_Z4 = 48,
    GX_CTF_Z8M = 57,
    GX_CTF_Z8L = 58,
    GX_CTF_Z16L = 60,

    GX_TF_A8 = GX_CTF_YUVA8
} GXTexFmt;

typedef enum _GXTexFilter {

} GXTexFilter;

typedef enum _GXPixelFmt {

} GXPixelFmt;

typedef enum _GXTexWrapMode {
    GX_CLAMP,
    GX_REPEAT,
    GX_MIRROR,

    GX_MAX_TEXWRAPMODE
} GXTexWrapMode;

typedef enum _GXTexCoordID {
    GX_TEXCOORD0,
    GX_TEXCOORD1,
    GX_TEXCOORD2,
    GX_TEXCOORD3,
    GX_TEXCOORD4,
    GX_TEXCOORD5,
    GX_TEXCOORD6,
    GX_TEXCOORD7,

    GX_MAX_TEXCOORD,
    GX_TEXCOORD_NULL = 255
} GXTexCoordID;

typedef enum _GXTexGenSrc {
    GX_TG_POS,
    GX_TG_NRM,
    GX_TG_BINRM,
    GX_TG_TANGENT,
    GX_TG_TEX0,
    GX_TG_TEX1,
    GX_TG_TEX2,
    GX_TG_TEX3,
    GX_TG_TEX4,
    GX_TG_TEX5,
    GX_TG_TEX6,
    GX_TG_TEX7,
    GX_TG_TEXCOORD0,
    GX_TG_TEXCOORD1,
    GX_TG_TEXCOORD2,
    GX_TG_TEXCOORD3,
    GX_TG_TEXCOORD4,
    GX_TG_TEXCOORD5,
    GX_TG_TEXCOORD6,
    GX_TG_COLOR0,
    GX_TG_COLOR1,
} GXTexGenSrc;

typedef enum _GXTexGenType {
    GX_TG_MTX3x4,
    GX_TG_MTX2x4,
    GX_TG_BUMP0,
    GX_TG_BUMP1,
    GX_TG_BUMP2,
    GX_TG_BUMP3,
    GX_TG_BUMP4,
    GX_TG_BUMP5,
    GX_TG_BUMP6,
    GX_TG_BUMP7,
    GX_TG_SRTG
} GXTexGenType;

typedef enum _GXTexMapID {
    GX_TEXMAP0,
    GX_TEXMAP1,
    GX_TEXMAP2,
    GX_TEXMAP3,
    GX_TEXMAP4,
    GX_TEXMAP5,
    GX_TEXMAP6,
    GX_TEXMAP7,
    GX_MAX_TEXMAP,

    GX_TEXMAP_NULL = 255,
    GX_TEX_DISABLE
} GXTexMapID;

typedef enum _GXProjectionType {
    GX_PERSPECTIVE,
    GX_ORTHOGRAPHIC
} GXProjectionType;

typedef enum _GXChannelID {
    GX_COLOR0,
    GX_COLOR1,
    GX_ALPHA0,
    GX_ALPHA1,
    GX_COLOR0A0,
    GX_COLOR1A1,
    GX_COLOR_ZERO,
    GX_ALPHA_BUMP,
    GX_ALPHA_BUMPN,

    GX_COLOR_NULL = 255
} GXChannelID;

typedef enum _GXColorSrc { GX_SRC_REG, GX_SRC_VTX } GXColorSrc;

typedef enum _GXLightID {
    GX_LIGHT0 = 1,
    GX_LIGHT1 = 2,
    GX_LIGHT2 = 4,
    GX_LIGHT3 = 8,
    GX_LIGHT4 = 16,
    GX_LIGHT5 = 32,
    GX_LIGHT6 = 64,
    GX_LIGHT7 = 128,

    GX_MAX_LIGHT = 256,
    GX_LIGHT_NULL = 0
} GXLightID;

typedef enum _GXDiffuseFn { GX_DF_NONE, GX_DF_SIGN, GX_DF_CLAMP } GXDiffuseFn;

typedef enum _GXAttnFn {
    GX_AF_SPEC,
    GX_AF_SPOT,
    GX_AF_NONE,
} GXAttnFn;

typedef enum _GXBlendMode {
    GX_BM_NONE,
    GX_BM_BLEND,
    GX_BM_LOGIC,
    GX_BM_SUBTRACT,

    GX_MAX_BLENDMODE
} GXBlendMode;

typedef enum _GXBlendFactor {
    GX_BL_ZERO,
    GX_BL_ONE,
    GX_BL_SRCCLR,
    GX_BL_INVSRCCLR,
    GX_BL_SRCALPHA,
    GX_BL_INVSRCALPHA,
    GX_BL_DSTALPHA,
    GX_BL_INVDSTALPHA,

    GX_BL_DSTCLR = GX_BL_SRCCLR,
    GX_BL_INVDSTCLR = GX_BL_INVSRCCLR
} GXBlendFactor;

typedef enum _GXLogicOp {
    GX_LO_CLEAR,
    GX_LO_AND,
    GX_LO_REVAND,
    GX_LO_COPY,
    GX_LO_INVAND,
    GX_LO_NOOP,
    GX_LO_XOR,
    GX_LO_OR,
    GX_LO_NOR,
    GX_LO_EQUIV,
    GX_LO_INV,
    GX_LO_REVOR,
    GX_LO_INVCOPY,
    GX_LO_INVOR,
    GX_LO_NAND,
    GX_LO_SET
} GXLogicOp;

typedef struct _GXRenderModeObj {
    u32 mVITVMode;
    u16 mFBWidth;
    u16 mEFBHeight;
    u16 mXFBHeight;
    u16 mVIXOrigin;
    u16 mVIYOrigin;
    u16 mVIWidth;
    u16 mVIHeight;
    u32 mXFBMode;
    bool mFieldRendering;
    bool mAntialias;
    u8 mSamplePattern[12][2];
    u8 mFilterWeights[7];
} GXRenderModeObj;

typedef union _GXFifo {
    // 1-byte
    char c;
    unsigned char uc;
    // 2-byte
    short s;
    unsigned short us;
    // 4-byte
    int i;
    unsigned int ui;
    void* p;
    float f;
} GXFifo;

#ifdef __CWCC__
extern volatile GXFifo WGPIPE : 0xcc008000;
#else
// For non-mwcc compiler error checking
extern volatile GXFifo WGPIPE;
#endif

void GXSetProjection(const Mtx44 *, GXProjectionType);
void GXLoadPosMtxImm(const Mtx *, u32);
void GXSetCurrentMtx(u32 id);
void GXLoadTexMtxImm(const Mtx *, u32, GXMtxType);
void GXSetViewport(float, float, float, float, float, float);
void GXSetScissor(u32, u32, u32, u32);

void GXSetVtxDesc(GXAttr, GXAttr);
void GXClearVtxDesc();
void GXSetVtxAttrFmt(GXVtxFmt, GXAttr, GXCompCnt, GXCompType, u8);
void GXSetNumTexGens(u8);
void GXSetTexCoordGen2(GXTexCoordID, GXTexGenType, GXTexGenSrc, u8, u8, u32);

void GXBegin(GXPrimitive, GXVtxFmt, u16);
void GXSetCullMode(int);

inline static void GXEnd() {}

void GXSetNumIndStages(u8);
void __GXSetIndirectMask(u32);

void GXSetColorUpdate(u8);
void GXSetAlphaUpdate(u8);
void GXSetZMode(u8, GXCompare, u8);
void GXSetZCompLoc(u8);

void GXSetTevColorIn(GXTevStageID, GXTevColorArg, GXTevColorArg, GXTevColorArg, GXTevColorArg);
void GXSetTevAlphaIn(GXTevStageID, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg);
void GXSetTevColorOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevAlphaOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevOp(GXTevStageID, u32);
void GXSetTevOrder(GXTevStageID, GXTexCoordID, GXTexMapID, GXChannelID);
void GXSetAlphaCompare(GXCompare, u8, GXAlphaOp, GXCompare, u8);
void GXSetNumTevStages(u8);

void GXInitTexObj(GXTexObj *, void *, u16, u16, GXTexFmt, GXTexWrapMode, GXTexWrapMode, u8);
void GXLoadTexObj(GXTexObj *, GXTexMapID);

void GXSetChanMatColor(GXChannelID, GXColor);
void GXSetNumChans(u8);
void GXSetChanCtrl(GXChannelID, u8, GXColorSrc, GXColorSrc, GXLightID, GXDiffuseFn, GXAttnFn);

void GXSetBlendMode(GXBlendMode, GXBlendFactor, GXBlendFactor, GXLogicOp);

void GXInvalidateVtxCache();

size_t GXGetTexBufferSize(int width, int height, GXTexFmt fmt, bool, bool);
void GXInitTexObjLOD(GXTexObj *, GXTexFilter, GXTexFilter, float, float, float, int, int, int);
void GXSetCopyFilter(int, void *, int, void *);
void GXSetTexCopySrc(int, int, int, int);
void GXSetTexCopyDst(int, int, GXTexFmt, u8);
void GXCopyTex(void *, int);
void GXSetTevDirect(int);

static inline void GXPosition3f32(float x, float y, float z) {
    WGPIPE.f = x;
    WGPIPE.f = y;
    WGPIPE.f = z;
}

static inline void GXTexCoord2f32(float x, float y) {
    WGPIPE.f = x;
    WGPIPE.f = y;
}

/// @}

#ifdef __cplusplus
}
#endif

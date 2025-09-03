#ifndef NW4R_G3D_RES_RES_MDL_H
#define NW4R_G3D_RES_RES_MDL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>
#include <nw4r/g3d/res/g3d_resmat.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <nw4r/g3d/res/g3d_resshp.h>
#include <nw4r/g3d/res/g3d_resvtx.h>

#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

/******************************************************************************
 *
 * ResMdlInfo
 *
 ******************************************************************************/
struct ResMdlInfoDataTypedef {
    enum TexMatrixMode {
        TEXMATRIXMODE_MAYA,
        TEXMATRIXMODE_XSI,
        TEXMATRIXMODE_3DSMAX
    };

    enum EnvelopeMatrixMode {
        EVPMATRIXMODE_NORMAL,
        EVPMATRIXMODE_APPROX,
        EVPMATRIXMODE_EXACT
    };
};

enum ScalingRule {
    SCALINGRULE_STANDARD,
    SCALINGRULE_SOFTIMAGE,
    SCALINGRULE_MAYA
};

struct ResMtxIDToNodeIDData {
    ulong numMtxID; // at 0x0
};

struct ResMdlInfoData : ResMdlInfoDataTypedef {
    ulong size;                   // at 0x0
    s32 toResMdlData;           // at 0x4
    ScalingRule scaling_rule;   // at 0x8
    TexMatrixMode tex_mtx_mode; // at 0xC
    s32 vertex_size;            // at 0x10
    s32 triangle_size;          // at 0x14
    s32 original_path;          // at 0x18
    ulong numViewMtx;             // at 0x1C
    bool need_nrm_mtx_array;    // at 0x20
    bool need_tex_mtx_array;    // at 0x21
    bool is_valid_volume;       // at 0x22
    u8 envelope_mtx_mode;       // at 0x23
    s32 toMtxIDToNodeID;        // at 0x24
    math::_VEC3 volume_min;     // at 0x28
    math::_VEC3 volume_max;     // at 0x34
};

class ResMdlInfo : public ResCommon<ResMdlInfoData>,
                   public ResMdlInfoDataTypedef {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResMdlInfo);

    ScalingRule GetScalingRule() const {
        return ref().scaling_rule;
    }

    ulong GetNumViewMtx() const {
        return ref().numViewMtx;
    }

    EnvelopeMatrixMode GetEnvelopeMatrixMode() const {
        return static_cast<EnvelopeMatrixMode>(ref().envelope_mtx_mode);
    }

    ulong GetNumPosNrmMtx() const {
        const ResMtxIDToNodeIDData* pData =
            reinterpret_cast<const ResMtxIDToNodeIDData*>(
                reinterpret_cast<const u8*>(&ref()) + ref().toMtxIDToNodeID);

        return pData->numMtxID;
    }

    s32 GetNodeIDFromMtxID(ulong id) const {
        const s32* pArray = reinterpret_cast<const s32*>(
            reinterpret_cast<const u8*>(&ref()) + ref().toMtxIDToNodeID +
            sizeof(ResMtxIDToNodeIDData));

        return pArray[id];
    }
};

/******************************************************************************
 *
 * ResMdl
 *
 ******************************************************************************/
struct ResMdlData {
    ResBlockHeaderData header;         // at 0x0
    ulong revision;                      // at 0x8
    s32 toResFileData;                 // at 0xC
    s32 toResByteCodeDic;              // at 0x10
    s32 toResNodeDic;                  // at 0x14
    s32 toResVtxPosDic;                // at 0x18
    s32 toResVtxNrmDic;                // at 0x1C
    s32 toResVtxClrDic;                // at 0x20
    s32 toResVtxTexCoordDic;           // at 0x24
    s32 toResMatDic;                   // at 0x28
    s32 toResTevDic;                   // at 0x2C
    s32 toResShpDic;                   // at 0x30
    s32 toResTexNameToTexPlttInfoDic;  // at 0x34
    s32 toResPlttNameToTexPlttInfoDic; // at 0x38
    s32 name;                          // at 0x3C
    ResMdlInfoData info;               // at 0x40
};

class ResMdl : public ResCommon<ResMdlData> {
public:
    static const ulong SIGNATURE = 'MDL0';
    static const int REVISION = 9;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResMdl);

    void Init();
    void Terminate();

    bool Bind(const ResFile file);
    void Release();

    ulong GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    const u8* GetResByteCode(const char* pName) const;

    ResNode GetResNode(const char* pName) const;
    ResNode GetResNode(const ResName name) const;
    ResNode GetResNode(int idx) const;
    ResNode GetResNode(ulong idx) const;
    ulong GetResNodeNumEntries() const;

    ResVtxPos GetResVtxPos(const ResName name) const;
    ResVtxPos GetResVtxPos(int idx) const;
    ResVtxPos GetResVtxPos(ulong idx) const;
    ulong GetResVtxPosNumEntries() const;

    ResVtxNrm GetResVtxNrm(const ResName name) const;
    ResVtxNrm GetResVtxNrm(int idx) const;
    ResVtxNrm GetResVtxNrm(ulong idx) const;
    ulong GetResVtxNrmNumEntries() const;

    ResVtxClr GetResVtxClr(const ResName name) const;
    ResVtxClr GetResVtxClr(int idx) const;
    ResVtxClr GetResVtxClr(ulong idx) const;
    ulong GetResVtxClrNumEntries() const;

    ResVtxTexCoord GetResVtxTexCoord(int idx) const;
    ulong GetResVtxTexCoordNumEntries() const;

    ResMat GetResMat(const char* pName) const;
    ResMat GetResMat(const ResName name) const;
    ResMat GetResMat(int idx) const;
    ResMat GetResMat(size_t idx) const;
    ulong GetResMatNumEntries() const;

    ResShp GetResShp(const char* pName) const;
    ResShp GetResShp(int idx) const;
    ResShp GetResShp(ulong idx) const;
    ulong GetResShpNumEntries() const;

    ResTexPlttInfo GetResTexPlttInfoOffsetFromTexName(int idx) const;
    ulong GetResTexPlttInfoOffsetFromTexNameNumEntries() const;

    ResMdlInfo GetResMdlInfo() {
        return ResMdlInfo(&ref().info);
    }
    ResMdlInfo GetResMdlInfo() const {
        return ResMdlInfo(const_cast<ResMdlInfoData*>(&ref().info));
    }
};

} // namespace g3d
} // namespace nw4r

#endif

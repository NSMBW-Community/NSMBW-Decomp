#ifndef NW4R_G3D_RES_RES_ANM_CLR_H
#define NW4R_G3D_RES_RES_ANM_CLR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>

namespace nw4r {
namespace g3d {

/******************************************************************************
 *
 * ClrAnmResult
 *
 ******************************************************************************/
struct ClrAnmResult {
    enum ColorAnim {
        CLA_CLR0,
        CLA_CLR1,
        CLA_AMB0,
        CLA_AMB1,

        CLA_TEV0,
        CLA_TEV1,
        CLA_TEV2,

        CLA_TEVK0,
        CLA_TEVK1,
        CLA_TEVK2,
        CLA_TEVK3,

        CLA_MAX
    };

    ulong bRgbaExist;        // at 0x0
    ulong rgba[CLA_MAX];     // at 0x4
    ulong rgbaMask[CLA_MAX]; // at 0x30
};

/******************************************************************************
 *
 * ResAnmClr
 *
 ******************************************************************************/
struct ResAnmClrAnmData {
    ulong mask;              // at 0x0
    ResColorAnmData color; // at 0x4
};

struct ResAnmClrMatData {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_ANM_CONSTANT = (1 << 1),

        // Two bits in 'flags' for each animation
        NUM_OF_FLAGS = 2
    };

    s32 name;                 // at 0x0
    ulong flags;                // at 0x4
    ResAnmClrAnmData anms[1]; // at 0x8
};

struct ResAnmClrInfoData {
    u16 numFrame;     // at 0x0
    u16 numMaterial;  // at 0x2
    AnmPolicy policy; // at 0x4
};

struct ResAnmClrData {
    ResBlockHeaderData header; // at 0x0
    ulong revision;              // at 0x8
    s32 toResFileData;         // at 0xC
    s32 toClrDataDic;          // at 0x10
    s32 toResUserData;         // at 0x14
    s32 name;                  // at 0x18
    s32 original_path;         // at 0x1C
    ResAnmClrInfoData info;    // at 0x20
};

class ResAnmClr : public ResCommon<ResAnmClrData> {
public:
    static const ulong SIGNATURE = 'CLR0';
    static const int REVISION = 3;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmClr);

    ulong GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    void GetAnmResult(ClrAnmResult* pResult, ulong idx, f32 frame) const;

    const ResAnmClrMatData* GetMatAnm(int idx) const {
        return static_cast<ResAnmClrMatData*>(
            ofs_to_obj<ResDic>(ref().toClrDataDic)[idx]);
    }
    const ResAnmClrMatData* GetMatAnm(ulong idx) const {
        return static_cast<ResAnmClrMatData*>(
            ofs_to_obj<ResDic>(ref().toClrDataDic)[idx]);
    }

    int GetNumFrame() const {
        return ref().info.numFrame;
    }

    int GetNumMaterial() const {
        return ref().info.numMaterial;
    }

    AnmPolicy GetAnmPolicy() const {
        return ref().info.policy;
    }
};

} // namespace g3d
} // namespace nw4r

#endif

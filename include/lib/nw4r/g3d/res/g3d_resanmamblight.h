#ifndef NW4R_G3D_RES_RES_ANM_AMB_LIGHT_H
#define NW4R_G3D_RES_RES_ANM_AMB_LIGHT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

namespace nw4r {
namespace g3d {

/******************************************************************************
 *
 * AmbLightResult
 *
 ******************************************************************************/
struct AmbLightAnmResult {
    enum Flag {
        FLAG_COLOR_ENABLE = (1 << 0),
        FLAG_ALPHA_ENABLE = (1 << 1),
    };

    ulong flags; // at 0x0
    ulong color; // at 0x4
};

/******************************************************************************
 *
 * ResAnmAmbLight
 *
 ******************************************************************************/
struct ResAnmAmbLightData {
    enum Flag {
        FLAG_COLOR_ENABLE = (1 << 0),
        FLAG_ALPHA_ENABLE = (1 << 1),

        FLAG_CONST = (1 << 31)
    };

    ulong size;              // at 0x0
    s32 toResAnmScnData;   // at 0x4
    s32 name;              // at 0x8
    ulong id;                // at 0xC
    ulong refNumber;         // at 0x10
    ulong flags;             // at 0x14
    ResColorAnmData color; // at 0x18
};

class ResAnmAmbLight : public ResCommon<ResAnmAmbLightData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmAmbLight);

    void GetAnmResult(AmbLightAnmResult* pResult, f32 frame) const;

    ulong GetID() const {
        return ref().id;
    }

    ulong GetRefNumber() const {
        return ref().refNumber;
    }
};

} // namespace g3d
} // namespace nw4r

#endif

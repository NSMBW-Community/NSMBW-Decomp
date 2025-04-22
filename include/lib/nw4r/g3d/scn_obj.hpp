#pragma once

#include <lib/nw4r/g3d/g3d_obj.hpp>
#include <lib/nw4r/math/aabb.hpp>
#include <lib/nw4r/math/mtx.hpp>

#include <lib/rvl/mem/MEMHeapCommon.h>

namespace nw4r {
namespace g3d {

class ScnObj : public G3dObj {
public:
    virtual ~ScnObj();
    virtual void ForEach(); // can't be bothered to enter the actual signature rn
    virtual void SetScnObjOption(ulong option, ulong value);
    virtual bool GetScnObjOption(ulong option, ulong *value) const;
    virtual float GetValueForSortOpa() const;
    virtual float GetValueForSortXlu() const;
    void SetPriorityDrawOpa(int priority);
    void SetPriorityDrawXlu(int priority);

    enum ScnObjMtxType {
        TYPE_LOCAL,
        TYPE_UNK,
        TYPE_VIEW
    };

    /// @brief Operation options for ScnObj.
    enum OptID {
        OPTID_DISABLE_GATHER_DRAW_SCNOBJ = 1, ///< Execution of ScnRoot::GatherDrawScnObj is skipped.
        OPTID_DISABLE_CALC_WORLD, ///< Execution of ScnRoot::CalcWorld is skipped.
        OPTID_DISABLE_CALC_MAT, ///< Execution of ScnRoot::CalcMaterial is skipped.
        OPTID_DISABLE_CALC_VTX, ///< Execution of ScnRoot::CalcVtx is skipped.
        OPTID_DISABLE_CALC_VIEW, ///< Execution of ScnRoot::CalcView is skipped.
        OPTID_DISABLE_DRAW_OPA, ///< Execution of ScnRoot::DrawOpa is skipped.
        OPTID_DISABLE_DRAW_XLU, ///< Execution of ScnRoot::DrawXlu is skipped.
        OPTID_DISABLE_UPDATEFRAME, ///< Execution of ScnRoot::UpdateFrame is skipped.
        OPTID_ENABLE_CULLING, ///< Enables culling based on the bounding volume (AABB).
    };

    enum Timing {
        CALLBACK_TIMING_A = BIT_FLAG(0), ///< Callback processing is performed before making calculations.
        CALLBACK_TIMING_B = BIT_FLAG(1), ///< Callback processing is performed in the middle of making calculations.
        CALLBACK_TIMING_C = BIT_FLAG(2), ///< Callback processing is performed after making calculations.
        CALLBACK_TIMING_ALL = CALLBACK_TIMING_A | CALLBACK_TIMING_B | CALLBACK_TIMING_C
    };

    virtual void CalcWorldMtx(const nw4r::math::MTX34 *, unsigned long *) const;

    void SetMtx(ScnObjMtxType, const nw4r::math::MTX34 *);
    void GetMtx(ScnObjMtxType, nw4r::math::MTX34 *) const;

    NW4R_G3D_TYPE_OBJ_DECL(ScnObj);

    u32 unk1;
    MEMAllocator *mpAllocator;
    math::MTX34 mMtxs[3];
    math::AABB aabbs[2];
    u32 mFlags;
    u8 opaPriority;
    u8 xluPriority;
    u32 unk2;
    u8 callbackTimingFlags;
    u16 scnCallbackOp;
};

} // namespace g3d
} // namespace nw4r

#pragma once

#include <lib/nw4r/g3d/g3d_obj.hpp>
#include <lib/nw4r/math/aabb.hpp>
#include <lib/nw4r/math/mtx.hpp>

#include <lib/rvl/mem/MEM.h>

namespace nw4r {
namespace g3d {

class ScnObj : public G3dObj {
public:
    virtual ~ScnObj();
    virtual void ForEach(); // can't be bothered to enter the actual signature rn
    virtual void SetScnObjOption(unsigned long, unsigned long);
    virtual bool GetScnObjOption(unsigned long, unsigned long *) const;
    virtual float GetValueForSortOpa() const;
    virtual float GetValueForSortXlu() const;
    void SetPriorityDrawOpa(int);
    void SetPriorityDrawXlu(int);

    enum ScnObjMtxType {
        TYPE_LOCAL,
        TYPE_UNK,
        TYPE_VIEW
    };

    enum Timing {
        TIMING_7 = 7
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

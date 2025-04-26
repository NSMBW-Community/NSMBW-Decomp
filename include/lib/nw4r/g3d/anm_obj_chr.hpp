#pragma once

#include <lib/nw4r/g3d/anm_obj.hpp>
#include <lib/nw4r/g3d/res_anm.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <lib/nw4r/math/mtx.hpp>

#include <lib/rvl/mem/MEMHeapCommon.h>

namespace nw4r {
namespace g3d {

/// @brief Character animation result.
struct ChrAnmResult {

    /// @brief Information flags related to the character animation result.
    enum Flag {
        FLAG_ANM_EXISTS = BIT_FLAG(0), ///< The animation exists.
        FLAG_IDENTITY = BIT_FLAG(1), ///< The node's matrix is always an identity matrix.
        FLAG_RT_ZERO = BIT_FLAG(2), ///< The rotation and translation are always 0.
        FLAG_SCALE_ONE = BIT_FLAG(3), ///< The scale is always 1.
        FLAG_SCALE_UNIFORM = BIT_FLAG(4), ///< The scale is always the same along all axes.
        FLAG_ROTATE_ZERO = BIT_FLAG(5), ///< The rotation is always 0.
        FLAG_TRANS_ZERO = BIT_FLAG(6), ///< The translation is always 0.

        /// @brief The model's value is used for the scale, and a result is not stored in ChrAnmResult::mScale.
        FLAG_SCALE_USE_MODEL = BIT_FLAG(7),

        /// @brief The model's value is used for the rotation, and a result is not stored in ChrAnmResult::mMtx.
        FLAG_ROTATE_USE_MODEL = BIT_FLAG(8),

        /// @brief The model's value is used for the translation, and a result is not stored in ChrAnmResult::mMtx.
        FLAG_TRANS_USE_MODEL = BIT_FLAG(9),

        /// @brief Scale calculations at this node must involve Maya's Segment Scale Compensate.
        FLAG_SEGMENT_SCALE_COMPENSATE_APPLY = BIT_FLAG(10),

        /// @brief One of this node's child nodes will involve Maya's Segment Scale Compensate in scale calculations.
        FLAG_SEGMENT_SCALE_COMPENSATE_PARENT = BIT_FLAG(11),

        /// @brief Softimage's Classic Scale Off must be taken into account for scale calculations at this node.
        FLAG_CLASSIC_SCALE_OFF = BIT_FLAG(12),

        /// @brief An appropriate value is stored in ChrAnmResult::mRotation.
        FLAG_RAW_ROTATE_VALID = BIT_FLAG(31),
    };

    u32 mFlags; ///< The attributes held by the character animation result data. See Flag.
    nw4r::math::VEC3 mScale; ///< The result scale.
    nw4r::math::VEC3 mRotation; ///< The result rotation.
    nw4r::math::MTX34 mMtx; ///< The matrix for rotation and translation.
};

class AnmObjChr : public AnmObj {
public:
    virtual void DetachAll();
    virtual void SetWeight(int, float);
    virtual float GetWeight(int) const;

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChr);
};

class AnmObjChrRes : public AnmObjChr {
public:
    static AnmObjChrRes *Construct(MEMAllocator*, size_t*, nw4r::g3d::ResAnmChr, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChr);
};

class AnmObjChrNode : public AnmObjChr {
public:
    static AnmObjChrNode *Construct(MEMAllocator*, size_t*, nw4r::g3d::ResAnmChr, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChrNode);

    char pad[0x10];
    int a;
    AnmObj **arr;

    int getCount() const { return a; }
};

class AnmObjChrBlend : public AnmObjChr {
public:
    static AnmObjChrBlend *Construct(MEMAllocator*, size_t*, nw4r::g3d::ResMdl, int);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChrBlend);
};

} // namespace g3d
} // namespace nw4r

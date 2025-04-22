#pragma once
#include <types.h>

// See https://github.com/kiwi515/ogws/blob/3729af7dc7d92939be12b14f0af415b22ce5d0ee/include/nw4r/g3d/g3d_obj.h

#define NW4R_G3D_TYPE_OBJ_DECL(VAL) static const nw4r::g3d::G3dObj::ResNameDataT<sizeof(#VAL)> TYPE_NAME; \
static const TypeObj GetTypeObjStatic() { return TypeObj(TYPE_NAME); }
#define NW4R_G3D_TYPE_OBJ_DEF(VAL) const nw4r::g3d::G3dObj::ResNameDataT<sizeof(#VAL)> VAL::TYPE_NAME = {sizeof(#VAL), #VAL}

namespace nw4r {
namespace g3d {

class G3dObj {
public:

    template <u32 N>
    struct ResNameDataT {
        u32 mLength;
        // Bug? +1 seemingly for null terminator,
        // but size N already includes it
        char mName[N + 1 + 3 & ~3];
    };

    struct TypeObj {
        struct TypeObjData {
            u32 mLength;
            char mName[];
        };

        template <u32 N>
        TypeObj(const ResNameDataT<N>& pRes) : mData((const TypeObjData *)&pRes) {}

        const TypeObjData *GetTypeObj() const {
            return mData;
        }

        const TypeObjData *mData;
    };

    /// @brief The possible G3D process operations.
    enum G3dProcOp {
        G3DPROC_CALC_WORLD = 1, ///< Calculates the world transform matrix.
        G3DPROC_CALC_MAT, ///< Calculates the material settings.
        G3DPROC_CALC_VTX, ///< Performs vertex array calculations.

        /// @brief Calculates the view transform matrix.
        /// @details A pointer to the camera matrix (MTX34) must be passed to the processing function.
        G3DPROC_CALC_VIEW,

        G3DPROC_GATHER_SCNOBJ, ///< Gathers the ScnObj to be rendered.
        G3DPROC_DRAW_OPA, ///< Draws the material specified as OPA.
        G3DPROC_DRAW_XLU, ///< Draws the material specified as XLU.
        G3DPROC_UPDATEFRAME, ///< Calls the UpdateFrame member function for all animation objects.

        G3DPROC_CHILD_DETACHED  = 0x00010001, ///< The child is about to be detached.
        G3DPROC_ATTACHED, ///< A parent has been attached.
        G3DPROC_DETACHED, ///< The child has been detached from the parent.
        G3DPROC_SORT, ///< Sorts the gathered ScnObj.
    };

    /// @brief Additional parameters for G3D process operations.
    enum G3dProcParam {

        /// @brief Will not update the world conversion matrix of the ScnObj
        G3DPROCPARAM_CALC_WORLD_SCNOBJMTX_NOUPDATE = BIT_FLAG(0),

        G3DPROCPARAM_SORT_ZSORT = BIT_FLAG(-1), ///< Use ScnRoot::ZSort() for sorting.
        G3DPROCPARAM_SORT_SORT = BIT_FLAG(0), ///< Use ScnRoot::Sort() for sorting.
        G3DPROCPARAM_SORT_SORT_WITHFUNC = BIT_FLAG(1), ///< Use ScnRoot::Sort(FuncLess, FuncLess) for sorting.
    };

    virtual bool IsDerivedFrom(G3dObj::TypeObj) const;
    virtual void G3dProc(ulong proc, ulong param, void *info) = 0;
    virtual ~G3dObj();
    virtual const TypeObj GetTypeObj();
    virtual const char *GetTypeName();

    void Destroy();
    void DetachFromParent();

    template <typename DerivedType, typename Type>
    static inline DerivedType *DynamicCast(Type *obj) {
        if (obj != nullptr && obj->IsDerivedFrom(DerivedType::GetTypeObjStatic())) {
            return (DerivedType*) (obj);
        }
        return nullptr;
    }

    private:
    NW4R_G3D_TYPE_OBJ_DECL(G3dObj);
};

} // namespace g3d
} // namespace nw4r

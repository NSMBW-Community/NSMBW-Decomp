#ifndef NW4R_G3D_OBJ_H
#define NW4R_G3D_OBJ_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_rtti.h>
#include <nw4r/g3d/platform/g3d_allocator.h>

namespace nw4r {
namespace g3d {
namespace {

/******************************************************************************
 *
 * Alignment
 *
 ******************************************************************************/
inline ulong align4(ulong x) {
    return ROUND_UP(x, 4);
}
inline ulong align32(ulong x) {
    return ROUND_UP(x, 32);
}

} // namespace

/******************************************************************************
 *
 * G3dObj
 *
 ******************************************************************************/
class G3dObj {
public:
    template <ulong N> struct ResNameDataT {
        ulong len; // at 0x0
        // @bug 'N' already includes the null terminator
        char str[ROUND_UP(N + 1, 4)]; // at 0x4
    };

    class TypeObj {
    public:
        struct ResNameDataPT {
            ulong len;     // at 0x0
            char str[4]; // at 0x4
        };

    public:
        template <ulong N>
        explicit TypeObj(const ResNameDataT<N>& rName)
            : mName(reinterpret_cast<const ResNameDataPT*>(&rName)) {}

        ulong GetTypeID() const {
            // @note Address is used for comparing TypeObjs
            return reinterpret_cast<ulong>(mName);
        }

        const char* GetTypeName() const {
            return mName->str;
        }

        bool operator==(const TypeObj& rOther) const {
            return GetTypeID() == rOther.GetTypeID();
        }

    private:
        const ResNameDataPT* mName; // at 0x0
    };

    enum G3dProcTask {
        G3DPROC_NONE = 0x00000,

        G3DPROC_CALC_WORLD = 0x00001,
        G3DPROC_CALC_MAT = 0x00002,
        G3DPROC_CALC_VTX = 0x00003,
        G3DPROC_CALC_VIEW = 0x00004,
        G3DPROC_GATHER_SCNOBJ = 0x00005,
        G3DPROC_DRAW_OPA = 0x00006,
        G3DPROC_DRAW_XLU = 0x00007,
        G3DPROC_UPDATEFRAME = 0x00008,

        // Tasks below cannot be disabled
        __G3DPROC_OPTIONAL_END,

        G3DPROC_CHILD_DETACHED = 0x10001,
        G3DPROC_ATTACH_PARENT = 0x10002,
        G3DPROC_DETACH_PARENT = 0x10003,
        G3DPROC_ZSORT = 0x10004
    };

public:
    virtual bool IsDerivedFrom(TypeObj other) const {
        return other == GetTypeObjStatic();
    } // at 0x8

    virtual void G3dProc(ulong task, ulong param, void* pInfo) = 0; // at 0xC
    virtual ~G3dObj();                                          // at 0x10

    virtual const TypeObj GetTypeObj() const {
        return TypeObj(TYPE_NAME);
    } // at 0x14
    static const G3dObj::TypeObj GetTypeObjStatic() {
        return TypeObj(TYPE_NAME);
    }
    virtual const char* GetTypeName() const {
        return GetTypeObj().GetTypeName();
    } // at 0x18

    G3dObj(MEMAllocator* pAllocator, G3dObj* pParent)
        : mpHeap(pAllocator), mpParent(pParent) {}

    G3dObj* GetParent() const {
        return mpParent;
    }
    void SetParent(G3dObj* pParent) {
        mpParent = pParent;
    }
    void DetachFromParent();

    void Destroy();

    static void* Alloc(MEMAllocator* pAllocator, ulong size) {
        return detail::AllocFromAllocator(pAllocator, size);
    }
    static void Dealloc(MEMAllocator* pAllocator, void* pBlock) {
        detail::FreeToAllocator(pAllocator, pBlock);
    }

    static inline void* operator new(size_t /* size */, void* pBlock) {
        return pBlock;
    }
    static inline void operator delete(void* /* pBlock */) {}

    template <typename TTo> static TTo* DynamicCast(G3dObj* pObj) {
        if (pObj != NULL && pObj->IsDerivedFrom(TTo::GetTypeObjStatic())) {
            return static_cast<TTo*>(pObj);
        }
        return NULL;
    }

private:
    G3dObj* mpParent;     // at 0x4
    MEMAllocator* mpHeap; // at 0x8

    __NW4R_G3D_TYPEOBJ_DECL(G3dObj);
};

} // namespace g3d
} // namespace nw4r

#endif

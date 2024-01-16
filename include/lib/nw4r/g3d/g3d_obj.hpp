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

    virtual bool IsDerivedFrom(G3dObj::TypeObj) const;
    virtual void G3dProc(unsigned long, unsigned long, void *) = 0;
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

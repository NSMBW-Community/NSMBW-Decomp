#ifndef NW4R_G3D_DCC_H
#define NW4R_G3D_DCC_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanmtexsrt.h>
#include <nw4r/g3d/res/g3d_resnode.h>

#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

void CalcTexMtx(math::MTX34* pMtx, bool set, const TexSrt& rSrt,
                TexSrt::Flag flag, TexSrtTypedef::TexMatrixMode mode);

namespace detail {

class WorldMtxAttr {
public:
    enum Attr {
        ATTR_BILLBOARD_MASK = (1 << 8) - 1,

        ATTR_T_IGNORE = (1 << 27),
        ATTR_S_UNIFORM = (1 << 28),
        ATTR_ALL_S_UNIFORM = (1 << 29),
        ATTR_S_ONE = (1 << 30),
        ATTR_ALL_S_ONE = (1 << 31),
    };

public:
    static ResNodeData::Billboard GetBillboard(ulong attr) {
        return static_cast<ResNodeData::Billboard>(attr & ATTR_BILLBOARD_MASK);
    }
    static ulong SetBillboard(ulong attr, ResNodeData::Billboard billboard) {
        return (attr & ~ATTR_BILLBOARD_MASK) | billboard;
    }

    static bool IsIgnoreTrans(ulong attr) {
        return (attr & ATTR_T_IGNORE) ? true : false;
    }
    static ulong AnmIgnoreTrans(ulong attr) {
        return attr | ATTR_T_IGNORE;
    }
    static ulong AnmNotIgnoreTrans(ulong attr) {
        return attr & ~ATTR_T_IGNORE;
    }

    static bool IsScaleUniform(ulong attr) {
        return (attr & ATTR_S_UNIFORM) ? true : false;
    }
    static ulong AnmScaleUniform(ulong attr) {
        return attr | ATTR_S_UNIFORM;
    }
    static ulong AnmNotScaleUniform(ulong attr) {
        return attr & ~(ATTR_S_UNIFORM | ATTR_ALL_S_UNIFORM | ATTR_S_ONE |
                        ATTR_ALL_S_ONE);
    }

    static bool IsAllScaleUniform(ulong attr) {
        return (attr & ATTR_ALL_S_UNIFORM) ? true : false;
    }
    static ulong AnmAllScaleUniform(ulong attr) {
        return attr | ATTR_ALL_S_UNIFORM;
    }
    static ulong AnmNotAllScaleUniform(ulong attr) {
        return attr & ~(ATTR_ALL_S_UNIFORM | ATTR_ALL_S_ONE);
    }

    static bool IsScaleOne(ulong attr) {
        return (attr & ATTR_S_ONE) ? true : false;
    }
    static ulong AnmScaleOne(ulong attr) {
        return attr | ATTR_S_ONE;
    }
    static ulong AnmNotScaleOne(ulong attr) {
        return attr & ~(ATTR_S_ONE | ATTR_ALL_S_ONE);
    }

    static bool IsAllScaleOne(ulong attr) {
        return (attr & ATTR_ALL_S_ONE) ? true : false;
    }
    static ulong AnmAllScaleOne(ulong attr) {
        return attr | ATTR_ALL_S_ONE;
    }
    static ulong AnmNotAllScaleOne(ulong attr) {
        return attr & ~ATTR_ALL_S_ONE;
    }

    static ulong GetRootMtxAttr() {
        return ATTR_S_UNIFORM | ATTR_ALL_S_UNIFORM | ATTR_S_ONE |
               ATTR_ALL_S_ONE;
    }
};

} // namespace detail
} // namespace g3d
} // namespace nw4r

#endif

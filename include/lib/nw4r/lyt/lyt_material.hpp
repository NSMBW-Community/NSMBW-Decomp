#pragma once

#include <lib/nw4r/ut/detail/link_list_impl.hpp>
#include <lib/nw4r/ut/color.hpp>

namespace nw4r {
namespace lyt {

class GXColorS10 {
public:
    s16 r;
    s16 g;
    s16 b;
    s16 a;

    GXColorS10(s16 r, s16 g, s16 b, s16 a) : r(r), g(g), b(b), a(a) { }
    GXColorS10(const nw4r::ut::Color &color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }
};

class Material {
public:
    virtual ~Material();

    ut::detail::LinkListImpl mAnimList;
    GXColorS10 mTevCols[3];
    nw4r::ut::Color mTevKCols[4];
    u32 mGXMemCap; // actually a bitfield
    u32 mGXMemNum; // actually a bitfield
    void * mpGXMem;
    char mName[21];
    bool mbUserAllocated;

    void setTev(int index, GXColorS10 color) {
        mTevCols[index] = color;
    }

    char pad[2];
};

} // namespace lyt
} // namespace nw4r
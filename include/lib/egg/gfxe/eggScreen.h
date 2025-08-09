#pragma once

#include <types.h>
#include <lib/egg/gfxe/eggFrustum.h>
#include <nw4r/math.h>

namespace EGG {

class Screen : public Frustum {
public:
    enum TVMode {
        TV_MODE_4_3,
        TV_MODE_16_9,
        TV_MODE_UNK_3,
        TV_MODE_MAX
    };

    struct DataEfb {
        // Required for struct copy
        struct Viewport {
            f32 x1;
            f32 y1;
            f32 x2;
            f32 y2;
            f32 z1;
            f32 z2;
        } vp;

        u32 sc_x;
        u32 sc_y;
        u32 sc_w;
        u32 sc_h;

        s32 sc_ox;
        s32 sc_oy;
    };

    struct TVModeInfo {
        u16 width;
        u16 height;
        nw4r::math::VEC2 ratios;
    };

    Screen();
    void SetProjectionGX() const;
    void CopyFromAnother(const Screen &);

    static void Initialize(const unsigned short *, const unsigned short *, Screen *);
    static void SetTVModeDefault();

    const Screen *mParent;
    nw4r::math::VEC2 mPosition;
    nw4r::math::VEC2 field_0x48;
    nw4r::math::VEC2 field_0x50;
    mutable DataEfb mDataEfb;

    static TVMode sTVMode;
    static TVModeInfo sTVModeInfo[Screen::TV_MODE_MAX];
};

} // namespace EGG

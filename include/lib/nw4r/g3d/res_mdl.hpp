#pragma once

#include <types.h>
#include <lib/nw4r/g3d/res_node.hpp>
#include <lib/rvl/gx/GX.h>

namespace nw4r {
namespace g3d {

class ResMatTevColor {
public:
    void GXSetTevColor(GXTevRegID, GXColor);
    void GXSetTevKColor(GXTevKColorID, GXColor);
    void DCStore(bool);

    u32 val;
};

class ResMatMisc {
public:
    void SetLightSetIdx(int);

    u32 smth;
};

class ResMat {
public:
    ResMatTevColor GetResMatTevColor() {
        ResMatTevColor col;
        col.val = (((num != 0) ? ((int) this + num) : 0)) + 0x20; // :thonk:
        return col;
    }

    char pad[0x3c];
    u32 num;
};

class ResMdl {
public:
    struct data {
        char pad[0x20];
        u16 x20;
        int x24;
    };
    data *p;

    int GetResNodeNumEntries() const;
    int GetResMatNumEntries() const;
    ResNode GetResNode(char const *) const;
    ResMat *GetResMat(unsigned long) const;

    bool IsValid() const { return p != nullptr; }
};

} // namespace g3d
} // namespace nw4r

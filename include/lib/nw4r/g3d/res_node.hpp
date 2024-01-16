#pragma once

#include <types.h>

namespace nw4r {
namespace g3d {

class ResNode {
public:
    struct data {
        char pad[0xc];
        u32 id;
    };

    bool IsValid() const { return p != nullptr; }
    int GetID() const { return IsValid() ? p->id : 0; }

    data *p;
};

} // namespace g3d
} // namespace nw4r

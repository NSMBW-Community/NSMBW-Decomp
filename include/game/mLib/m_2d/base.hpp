#pragma once
#include <types.h>
#include <nw4r/ut.h>

namespace m2d {

class Base_c : nw4r::ut::Link {
public:
    virtual ~Base_c();
    virtual void draw();

    void entry();

    u8 mDrawOrder;
};

} // namespace m2d

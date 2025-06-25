#pragma once
#include <types.h>
#include <lib/nw4r/ut/list.hpp>

namespace m2d {

class Base_c {
public:
    nw4r::ut::Link mNode;

    void entry();

    virtual ~Base_c();
    virtual void draw();

    u8 mDrawOrder;
};

} // namespace m2d

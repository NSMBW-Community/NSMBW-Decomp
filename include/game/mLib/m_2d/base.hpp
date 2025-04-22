#pragma once
#include <types.h>
#include <lib/nw4r/ut/list.hpp>

namespace m2d {

class Base_c {
public:
    nw4r::ut::Link mNode;

    virtual ~Base_c();

    u8 mDrawOrder;
};

} // namespace m2d

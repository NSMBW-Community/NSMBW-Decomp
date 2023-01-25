#pragma once
#include <lib/nw4r/ut/list.hpp>

namespace nw4r {
namespace ut {
namespace detail {

class LinkListImpl {
    class Iterator;

    u32 mCount;
    Link mNode;
};

} // namespace detail
} // namespace ut
} // namespace nw4r

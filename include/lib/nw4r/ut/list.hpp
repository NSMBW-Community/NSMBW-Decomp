#pragma once
#include <types.h>

namespace nw4r {
namespace ut {

struct LinkListNode {
    LinkListNode* mpPrev;
    LinkListNode* mpNext;
};

class List {
    LinkListNode* mpHead;
    LinkListNode* mpTail;
    u16 mpCount;
    u16 mpOffset;
};

} // namespace ut
} // namespace nw4r

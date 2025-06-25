#pragma once
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <lib/nw4r/lyt/lyt_resource.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class AnimationLink {
public:
    ut::LinkListNode mLink; // 0x00
    AnimTransform* mAnimTrans; // 0x08
    u16             mIdx;       // 0x0C
    bool            mbDisable;  // 0x0E
};
typedef ut::LinkList<AnimationLink, 0 /*mLink*/> AnimationLinkList;

} // namespace lyt
} // namespace nw4r

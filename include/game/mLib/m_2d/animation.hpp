#pragma once
#include <types.h>
#include <game/mLib/m_2d/frame_ctrl.hpp>
#include <game/mLib/m_2d/layout.hpp>
#include <game/mLib/m_2d/resource.hpp>
#include <nw4r/lyt.h>
#include <nw4r/ut.h>

namespace m2d {

/// @unofficial
struct GroupAnimTransform_c {
    nw4r::lyt::Group *mpGroup;
    nw4r::lyt::AnimTransform *mpAnimTransform;
};

class AnmResV2_c {
public:
    AnmResV2_c() {}
    virtual ~AnmResV2_c() {}

    bool create(const char *name, m2d::ResAccIf_c *resAcc, m2d::Layout_c *layout, bool useOverride);
    bool remove();
    GroupAnimTransform_c *getGroupAnimTransform(const char *);
    void setAnmEnable(nw4r::lyt::Group*, bool);
    void updateFrame(nw4r::lyt::Group*, float);
    nw4r::lyt::AnimationLink *findAnmLink(nw4r::lyt::Pane*, bool);
    void setAnmFrame(nw4r::lyt::Pane*, float);

    nw4r::lyt::AnimResource mAnimResource;
    GroupAnimTransform_c *mGroupAnim;
    int mGroupNum;
};

class AnmGroupBase_c {
public:
    AnmGroupBase_c() {}

    bool create(AnmResV2_c *anmRes, const char *name);
    void setAnmEnable(bool enable);
    void updateFrame();

    FrameCtrl_c *mpFrameCtrl;
    AnmResV2_c *mpAnmRes;
    GroupAnimTransform_c *mpGroupAnim;
    u8 mFlags;
};

class AnmGroup_c : public AnmGroupBase_c {
public:
};

} // namespace m2d

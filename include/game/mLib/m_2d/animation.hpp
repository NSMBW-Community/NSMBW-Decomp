#pragma once
#include <types.h>
#include <game/mLib/m_2d/frame_ctrl.hpp>
#include <game/mLib/m_2d/layout.hpp>
#include <game/mLib/m_2d/resource.hpp>
#include <nw4r/lyt.h>
#include <nw4r/ut.h>

namespace m2d {

/// @unofficial
struct GroupAnimTransform_s {
    nw4r::lyt::Group *mpGroup;
    nw4r::lyt::AnimTransform *mpAnimTransform;
};

class AnmResV2_c {
public:
    AnmResV2_c() {}
    virtual ~AnmResV2_c() {}

    bool create(const char *name, m2d::ResAccIf_c *resAcc, m2d::Layout_c *layout, bool useOverride);
    bool remove();
    GroupAnimTransform_s *getGroupAnimTransform(const char *name);
    void setAnmEnable(nw4r::lyt::Group *group, bool enable);
    void updateFrame(nw4r::lyt::Group *group, float frame);
    nw4r::lyt::AnimationLink *findAnmLink(nw4r::lyt::Pane *pane, bool recursive);
    void setAnmFrame(nw4r::lyt::Pane *pane, float frame);

    nw4r::lyt::AnimResource mAnimResource;
    GroupAnimTransform_s *mGroupAnim;
    int mGroupNum;
};

class AnmGroupBase_c {
public:
    enum FLAG_e {
        FLAG_ENABLED = BIT_FLAG(0),
    };

    AnmGroupBase_c() {}

    bool create(AnmResV2_c *anmRes, const char *name);
    void setAnmEnable(bool enable);
    void updateFrame();

    FrameCtrl_c *mpFrameCtrl;
    AnmResV2_c *mpAnmRes;
    GroupAnimTransform_s *mpGroupAnim;
    u8 mFlags;
};

class AnmGroup_c : public AnmGroupBase_c {
public:
};

} // namespace m2d

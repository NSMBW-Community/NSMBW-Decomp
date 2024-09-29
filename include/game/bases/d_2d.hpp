#pragma once
#include <game/bases/d_resource.hpp>
#include <game/mLib/m2d/base.hpp>
#include <lib/nw4r/lyt/lyt_layout.hpp>
#include <lib/nw4r/lyt/lyt_resource.hpp>
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/math/vec.hpp>

namespace d2d {

class Multi_c : public m2d::Base_c {
public:
    virtual ~Multi_c();
    virtual void draw();
    virtual void calc();
    virtual bool build(const char *, ResAccMult_c *);

    void entry();
    nw4r::lyt::Pane *getRootPane();

private:
    nw4r::lyt::Layout mLayout;
    nw4r::lyt::DrawInfo mDrawInfo;
    ResAccMult_c *mpResAccessor;

    nw4r::math::VEC2 mPos;

    nw4r::math::VEC2 mClipPos;
    nw4r::math::VEC2 mClipSize;
    bool mClipEnabled;

    u32 mFlags;
    u32 mUnknown_98;
};

} // namespace d2d

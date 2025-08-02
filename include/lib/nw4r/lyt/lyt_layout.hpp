#pragma once
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <lib/nw4r/lyt/lyt_resource.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <lib/nw4r/ut/Rect.h>
#include <lib/nw4r/ut/detail/link_list_impl.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class Layout {
public:
    Layout();
    virtual ~Layout();

    virtual bool Build(const void *, ResourceAccessor *);
    virtual AnimTransformBasic *CreateAnimTransform();
    virtual AnimTransformBasic *CreateAnimTransform(const void *, ResourceAccessor *);
    virtual AnimTransformBasic *CreateAnimTransform(const AnimResource &, ResourceAccessor *);
    virtual void BindAnimation(AnimTransform *);
    virtual void UnbindAnimation(AnimTransform *);
    virtual void UnbindAllAnimation();
    virtual bool BindAnimationAuto(const AnimResource &, ResourceAccessor *);
    virtual void SetAnimationEnable(AnimTransform *, bool);
    virtual void CalculateMtx(const DrawInfo &);
    virtual void Draw(const DrawInfo &);
    virtual void Animate(unsigned long);
    virtual void SetTagProcessor(ut::TagProcessorBase<wchar_t> *);

    ut::Rect GetLayoutRect() const;

    static Pane *BuildPaneObj(long, const void *, const ResBlockSet &);

private:
    ut::detail::LinkListImpl mAnimTransList;
    Pane *mpRootPane;
    GroupContainer *mpGroupContainer;
    math::VEC2 mSize;
};

} // namespace lyt
} // namespace nw4r

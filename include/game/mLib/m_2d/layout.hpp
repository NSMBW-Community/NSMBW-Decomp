#pragma once
#include <types.h>
#include <nw4r/lyt.h>

namespace m2d {

class Layout_c : public nw4r::lyt::Layout {
public:
    virtual ~Layout_c() {}

    virtual nw4r::lyt::AnimTransform *CreateAnimTransform(const void *, nw4r::lyt::ResourceAccessor *);

    bool bindAnimationAuto(const nw4r::lyt::AnimResource &, nw4r::lyt::ResourceAccessor *);
};

} // namespace m2d

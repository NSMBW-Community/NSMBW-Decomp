#pragma once
#include <lib/rvl/arc/ARC.h>
#include <lib/nw4r/ut/detail/link_list_impl.hpp>

namespace nw4r {
namespace lyt {

class ResourceAccessor {
public:
    virtual ~ResourceAccessor();
    virtual void *GetResource(unsigned long, const char *, unsigned long *) = 0;
    virtual void *GetFont(const char *);
};

class ArcResourceAccessor : public ResourceAccessor {
public:
    virtual ~ArcResourceAccessor();
    virtual void *GetResource(unsigned long, const char *, unsigned long *);
    virtual void *GetFont(const char *);

    bool Attach(void *, const char *);
    void Detach();

private:
    ARCHandle mArcHandle;
    void* mpArcBuf;
    ut::detail::LinkListImpl mFontList;
    char mResRootDir[128];
};

} // namespace lyt
} // namespace nw4r

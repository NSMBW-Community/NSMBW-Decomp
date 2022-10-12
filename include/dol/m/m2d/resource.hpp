#pragma once
#include <lib/nw4r/lyt/lyt_resource.hpp>

namespace m2d {

class ResAccIf_c {
public:
    virtual ~ResAccIf_c() = 0;
    virtual void creater() = 0;

    bool attach(void *, const char *);
    void detach();
    void *getResource(unsigned long, const char *);

private:
    nw4r::lyt::ResourceAccessor *mResAccessor;
    void *mResource;
};

} // namespace m2d

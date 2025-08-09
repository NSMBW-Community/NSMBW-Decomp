#pragma once
#include <nw4r/lyt.h>

namespace m2d {

class ResAccIf_c {
public:
    virtual ~ResAccIf_c() = 0;
    virtual void creater() = 0;

    bool attach(void *, const char *);
    void detach();
    void *getResource(unsigned long, const char *);

    nw4r::lyt::ArcResourceAccessor *getResAccessor() const { return mpResAccessor; }

private:
    nw4r::lyt::ArcResourceAccessor *mpResAccessor;
    void *mpResource;
};

} // namespace m2d

#pragma once
#include <nw4r/lyt.h>

namespace m2d {

class ResAccIf_c {
public:
    ResAccIf_c() : mpResAccessor(nullptr), mpResource(nullptr) {}
    virtual ~ResAccIf_c() {}
    virtual void creater() = 0;

    bool attach(void *resource, const char *rootDir);
    void detach();
    void *getResource(ulong type, const char *name);

    void setResAccessor(nw4r::lyt::ArcResourceAccessor *resAccessor) { mpResAccessor = resAccessor; }
    nw4r::lyt::ArcResourceAccessor *getResAccessor() const { return mpResAccessor; }

private:
    nw4r::lyt::ArcResourceAccessor *mpResAccessor;
    void *mpResource;
};

} // namespace m2d

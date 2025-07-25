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

    nw4r::lyt::ResourceAccessor* getResAccessor() { return mpResAccessor; }
    void setResAccessor(nw4r::lyt::ResourceAccessor* ra) { mpResAccessor = ra; }
    void setResource(void* res) { mpResource = res; }
        
private:
    nw4r::lyt::ResourceAccessor *mpResAccessor;
    void *mpResource;
};

} // namespace m2d

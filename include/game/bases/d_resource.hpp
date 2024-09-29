#pragma once
#include <game/bases/d_dvd.hpp>
#include <game/mLib/m2d/resource.hpp>
#include <lib/nw4r/lyt/lyt_resource.hpp>
#include <lib/egg/heap/eggHeap.hpp>

namespace d2d {

class ResAccMult_c : public m2d::ResAccIf_c {
    class InternalAccessor : nw4r::lyt::ArcResourceAccessor {};

    ResAccMult_c();

    virtual void creater();

    InternalAccessor mInternalAccessor;
};

class ResAccMultLoader_c : public ResAccMult_c {
public:
    bool create(EGG::Heap *, unsigned long);
    void remove();

    bool requestEx(const char *, int);
    bool request(const char *);

private:
    void *mpResource;
    d2d::dDvd::loader_c mLoader;
};

} // namespace d2d

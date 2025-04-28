#pragma once
#include <game/bases/d_dvd.hpp>
#include <game/mLib/m_2d/resource.hpp>
#include <lib/egg/core/eggHeap.h>
#include <nw4r/lyt.h>

namespace d2d {

class ResAccMult_c : public m2d::ResAccIf_c {
    class InternalAccessor : nw4r::lyt::ArcResourceAccessor {};

public:
    ResAccMult_c();

    virtual void creater();

    InternalAccessor mInternalAccessor;
};

class ResAccMultLoader_c : public ResAccMult_c {
public:
    ResAccMultLoader_c();
    ~ResAccMultLoader_c();

    bool create(EGG::Heap *, unsigned long);
    bool remove();

    bool requestEx(const char *, int);
    bool request(const char *);

private:
    void *mpResource;
    dDvd::loader_c mLoader;
};

} // namespace d2d

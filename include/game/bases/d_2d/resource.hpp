#pragma once
#include <game/bases/d_dvd.hpp>
#include <game/mLib/m_2d/resource.hpp>
#include <game/mLib/m_allocator.hpp>
#include <lib/egg/core/eggHeap.h>
#include <nw4r/lyt.h>

namespace d2d {

class ResAccMult_c : public m2d::ResAccIf_c {
    class InternalAccessor : public nw4r::lyt::ArcResourceAccessor {
    public:
        virtual void* GetResource(ulong type, const char *name, size_t *size);
        virtual nw4r::ut::Font *GetFont(const char *name);
    };

public:
    ResAccMult_c();
    virtual ~ResAccMult_c();
    virtual void creater();

    void *getResource(unsigned long, const char *);

    InternalAccessor mInternalAccessor;
};

class ResAccMultLoader_c : public ResAccMult_c {
public:
    ResAccMultLoader_c();
    virtual ~ResAccMultLoader_c();

    bool requestEx(const char *, int);
    bool request(const char *);
    void remove();

    static bool create(EGG::Heap *heap, ulong size);

private:
    void *mpResource;
    dDvd::loader_c mLoader;

    static mAllocator_c *ms_res_allocator;
};

} // namespace d2d

#include <game/mLib/m_3d.hpp>

class dWmSVMdl_c : public m3d::proc_c {
public:
    virtual ~dWmSVMdl_c() {}

    virtual void create(mAllocator_c *, m3d::smdl_c &);

    u32 m_00;
};

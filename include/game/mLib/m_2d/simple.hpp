#pragma once
#include <types.h>
#include <game/mLib/m_2d/base.hpp>
#include <game/mLib/m_2d/frame_ctrl.hpp>
#include <game/mLib/m_2d/layout.hpp>
#include <game/mLib/m_2d/resource.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/lyt.h>

namespace m2d {

class Simple_c : public Base_c {
public:
    enum FLAG_e {
        SKIP_INVISIBLE = BIT_FLAG(0),
    };

    typedef int (*patrolPaneFunc1)(nw4r::lyt::Pane *pane, void *arg);
    typedef bool (*patrolPaneFunc2)(nw4r::lyt::Pane *pane);

    Simple_c();
    virtual ~Simple_c() {}
    virtual void draw();
    virtual void calc();
    virtual bool build(const char *lytName, m2d::ResAccIf_c *resAcc);

    void calcBefore();
    void calcAfter();
    int patrolPane_local(nw4r::lyt::Pane *pane, patrolPaneFunc1 func1, patrolPaneFunc2 func2, void *arg);
    bool patrolPane(patrolPaneFunc1 func1, patrolPaneFunc2 func2, void *arg);

    Layout_c mLayout;
    nw4r::lyt::DrawInfo mDrawInfo;
    ResAccIf_c *mpResAcc;
    mVec3_c mPos;
    u32 mFlags;
};

} // namespace m2d

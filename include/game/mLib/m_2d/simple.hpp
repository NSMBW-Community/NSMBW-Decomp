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
    typedef int (*patrolPaneFunc1)(nw4r::lyt::Pane*, void*);
    typedef bool (*patrolPaneFunc2)(nw4r::lyt::Pane*);

    Simple_c();
    virtual ~Simple_c() {}
    virtual void draw();
    virtual void calc();
    virtual bool build(const char *, m2d::ResAccIf_c *);

    void calcBefore();
    void calcAfter();
    int patrolPane_local(nw4r::lyt::Pane *, patrolPaneFunc1, patrolPaneFunc2, void *);
    bool patrolPane(patrolPaneFunc1, patrolPaneFunc2, void *);

    Layout_c mLayout;
    nw4r::lyt::DrawInfo mDrawInfo;
    ResAccIf_c *mpResAcc;
    mVec3_c mPos;
    u32 mFlags;
};

} // namespace m2d

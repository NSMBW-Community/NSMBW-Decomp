#pragma once
#include <game/mLib/m_fader_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <lib/nw4r/ut/Color.h>

class dWipeCircle_c : public mFaderBase_c {
public:
    dWipeCircle_c(nw4r::ut::Color, mFaderBase_c::EStatus stat);
    ~dWipeCircle_c();

    bool createLayout();
    void CenterPosSet();
    void OpenSetup();
    void AnimeEndCheck();
    void CloseSetup();

    virtual int calc();
    virtual void draw();
    virtual void setStatus(mFaderBase_c::EStatus stat);
    virtual bool fadeIn();
    virtual bool fadeOut();

    static dWipeCircle_c *m_instance;

private:
    LytBase_c mLyt;
    nw4r::lyt::Pane *mpRootPane;
    nw4r::lyt::Window *mpWnd[1];
    mVec2_c mCenterPos;
    int mAction;
    bool mIsCreated;
    bool mField_1c1;
    bool mField_1c2;
};

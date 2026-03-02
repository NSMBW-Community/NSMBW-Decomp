#pragma once

#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

class dWiiStrapScreen_c : public dBase_c {
public:
    dWiiStrapScreen_c();
    virtual ~dWiiStrapScreen_c();

    bool createLayout();
    
    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    LytBase_c mLayout;
    bool mHasLoadedLayout;
    bool mVisible;
};

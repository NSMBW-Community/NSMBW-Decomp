#pragma once

#include <game/bases/d_base.hpp>

class dLastActor_c : public dBase_c {

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();

    virtual ~dLastActor_c();

};

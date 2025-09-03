#pragma once

#include <game/bases/d_base.hpp>

/// @brief An actor that is always placed last in the actor list.
/// @ingroup bases
class dLastActor_c : public dBase_c {
public:
    dLastActor_c() {} ///< @copydoc dBase_c::dBase_c
    virtual ~dLastActor_c(); ///< @copydoc dBase_c::~dBase_c

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
};

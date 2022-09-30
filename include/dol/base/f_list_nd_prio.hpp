#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/base/f_list_nd.hpp>

// Unofficial name
class fLiNdPrio_c : public fLiNdBa_c {
public:
    u16 mOrder;

    inline fLiNdPrio_c *getPrev() {
        return (fLiNdPrio_c *) fLiNdBa_c::getPrev();
    }
    inline fLiNdPrio_c *getNext() {
        return (fLiNdPrio_c *) fLiNdBa_c::getNext();
    }
};
#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/base/f_list_nd.hpp>

// Unofficial name
class fLiNdPrio_c : public fLiNdBa_c {
public:
    u16 mOrder;
    u16 mOrder2;
    
    fLiNdPrio_c(fBase_c *owner) : fLiNdBa_c(owner), mOrder(0), mOrder2(0) {}

    inline fLiNdPrio_c *getPrev() {
        return (fLiNdPrio_c *) fLiNdBa_c::getPrev();
    }
    inline fLiNdPrio_c *getNext() {
        return (fLiNdPrio_c *) fLiNdBa_c::getNext();
    }
    inline u16 getOrder() const {
        return mOrder;
    }
};
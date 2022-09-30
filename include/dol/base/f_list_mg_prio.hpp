#pragma once

#include <types.h>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_nd_prio.hpp>

class fLiMgPrio_c : public fLiMgBa_c {
public:

    bool addNode(fLiNdPrio_c *node);
    
    inline fLiNdPrio_c *getFirst() {
        return (fLiNdPrio_c *) fLiMgBa_c::getFirst();
    }
    inline fLiNdPrio_c *getLast() {
        return (fLiNdPrio_c *) fLiMgBa_c::getLast();
    }
};
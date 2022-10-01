#pragma once

#include <types.h>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_nd_prio.hpp>

class fBase_c;

// Unofficial name
class fLiMgPTMF_c : public fLiMgBa_c {
    int (fBase_c::*mpPack)();

public:
    fLiMgPTMF_c(int (fBase_c::*ptmf)()) : mpPack(ptmf) {}

    bool addNode(fLiNdPrio_c *node);

    bool walkPack();
    
    inline fLiNdPrio_c *getFirst() {
        return (fLiNdPrio_c *) fLiMgBa_c::getFirst();
    }
    inline fLiNdPrio_c *getLast() {
        return (fLiNdPrio_c *) fLiMgBa_c::getLast();
    }
};
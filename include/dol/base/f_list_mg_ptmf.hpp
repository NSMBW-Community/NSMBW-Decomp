#pragma once

#include <types.h>
#include <dol/base/f_list_mg.hpp>

class fBase_c;

// Unofficial name
class fLiMgPTMF_c : public fLiMgBa_c {
    void (fBase_c::*mpPack)();

public:
    fLiMgPTMF_c(void (fBase_c::*ptmf)()) : mpPack(ptmf) {}

    bool walkPack();
    
    inline fLiMgPTMF_c *getFirst() {
        return (fLiMgPTMF_c *) fLiMgBa_c::getFirst();
    }
    inline fLiMgPTMF_c *getLast() {
        return (fLiMgPTMF_c *) fLiMgBa_c::getLast();
    }
};
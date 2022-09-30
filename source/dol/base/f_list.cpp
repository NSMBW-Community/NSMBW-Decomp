#include <types.h>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_mg_prio.hpp>
#include <dol/base/f_list_mg_ptmf.hpp>
#include <dol/base/f_base.hpp>

bool fLiMgPTMF_c::walkPack() {
    if (mpPack == 0) return true;
    
    fLiNdBa_c *curr = (fLiNdBa_c *) mpFirst;
    while (curr != nullptr) {
        fLiNdBa_c *next = curr->getNext();
        (curr->mpOwner->*mpPack)();
        curr = next;
    }
    return true;
}
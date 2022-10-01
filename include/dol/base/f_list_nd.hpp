#pragma once

#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

class fLiNdBa_c : public cListNd_c {
public:
    fBase_c *mpOwner;
    
    fLiNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    inline fLiNdBa_c *getPrev() {
        return (fLiNdBa_c *) cListNd_c::getPrev();
    }
    inline fLiNdBa_c *getNext() {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }

    // Unofficial name
    // [Might not actually belong to this class (-> xor trick on hash)]
    void removeSelf();
};
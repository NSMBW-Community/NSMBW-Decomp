#pragma once

#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

class fLiNdBa_c : public cListNd_c {
public:
    fBase_c *mpOwner;

    inline fLiNdBa_c *getNext() {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }
};
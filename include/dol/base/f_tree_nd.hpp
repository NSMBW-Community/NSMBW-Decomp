#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/profile/profile.hpp>

class fBase_c;

class fTrNdBa_c : public cTreeNd_c {
public:    
    fBase_c *mpOwner;

    fTrNdBa_c *getTreeNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNext();
    }
};
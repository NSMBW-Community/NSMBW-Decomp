#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/profile/profile.hpp>

class fBase_c;

class fTrNdBa_c : public cTreeNd_c {
public:    
    fBase_c *mpOwner;

    fTrNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    fTrNdBa_c *getTreeNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNext();
    }
    fTrNdBa_c *getTreeNextNotChild() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNextNotChild();
    }

    fTrNdBa_c *getParent() const {
        return (fTrNdBa_c *) cTreeNd_c::getParent();
    }
    fTrNdBa_c *getChild() const {
        return (fTrNdBa_c *) cTreeNd_c::getChild();
    }
    fTrNdBa_c *getBrPrev() const {
        return (fTrNdBa_c *) cTreeNd_c::getBrPrev();
    }
    fTrNdBa_c *getBrNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getBrNext();
    }
};
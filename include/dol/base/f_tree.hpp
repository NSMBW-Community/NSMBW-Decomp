#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/profile/profile.hpp>
#include <dol/base/f_base.hpp>

class fTrNdBa_c : public cTreeNd_c {
public:    
    fBase_c *mpOwner;

    fTrNdBa_c *getTreeNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNext();
    }
};

class fTrMgBa_c : public cTreeMg_c {
public:
    const fTrNdBa_c *searchNodeByProfName(Profile profName, const fTrNdBa_c *parent);
    const fTrNdBa_c *searchNodeByGroupType(unsigned char groupType, const fTrNdBa_c *parent);
};

// Unofficial name
class fTrMgPTMF_c : public fTrMgBa_c {
public:
    void (fBase_c::*mpPack)();

    bool walkPack();
};
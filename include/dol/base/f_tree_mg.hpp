#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/profile/profile.hpp>
#include <dol/base/f_base.hpp>

class fTrNdBa_c;

class fTrMgBa_c : public cTreeMg_c {
public:
    const fTrNdBa_c *searchNodeByProfName(Profile profName, const fTrNdBa_c *parent);
    const fTrNdBa_c *searchNodeByGroupType(unsigned char groupType, const fTrNdBa_c *parent);
};
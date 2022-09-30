#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/base/f_tree_mg.hpp>
#include <dol/base/f_base.hpp>

class fTrNdBa_c;

// Unofficial name
class fTrMgPTMF_c : public fTrMgBa_c {
    void (fBase_c::*mpPack)();
public:

    bool walkPack();
};

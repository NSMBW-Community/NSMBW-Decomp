#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/base/f_tree_mg.hpp>

class fBase_c;

// Unofficial name
class fTrMgPTMF_c : public fTrMgBa_c {
    int (fBase_c::*mpPack)();
public:
    fTrMgPTMF_c(int (fBase_c::*ptmf)()) : mpPack(ptmf) {}

    bool walkPack();
};

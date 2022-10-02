#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/base/f_tree_mg.hpp>

class fBase_c;

// [Unofficial name]
/// A container for a tree of fTrNdBa_c nodes with a reference to a process function.
class fTrMgPTMF_c : public fTrMgBa_c {
    int (fBase_c::*mpProcFunc)(); ///< The process function for the tree.
public:
    fTrMgPTMF_c(int (fBase_c::*ptmf)()) : mpProcFunc(ptmf) {}

    /**
     * @brief Call the process function ::procFunc on the owner of each node in the tree.
     * 
     * @return If the operation was successful.
     */
    bool walkPack();
};

#pragma once
#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/framework/f_tree_mg.hpp>

class fBase_c;

/// @brief A container for a tree of fTrNdBa_c nodes with a reference to a process function.
/// @note Unofficial name.
class fTrMgPTMF_c : public fTrMgBa_c {
    int (fBase_c::*mpProcFunc)(); ///< The process function for the tree.

public:
    /// @brief Constructs a new fTrMgPTMF_c.
    /// @param procFunc The process function.
    fTrMgPTMF_c(int (fBase_c::*procFunc)()) : mpProcFunc(procFunc) {}

    /// @brief Calls the process function ::procFunc on the owner of each node in the tree.
    /// @return If the operation was successful.
    bool walkPack();
};

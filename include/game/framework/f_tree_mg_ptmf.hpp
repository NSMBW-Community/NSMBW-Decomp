#pragma once
#include <types.h>
#include <game/cLib/c_tree.hpp>
#include <game/framework/f_tree_mg.hpp>

class fBase_c;

/// @brief A base tree made of fTrNdBa_c nodes, with a reference to a process function.
/// @ingroup framework
/// @unofficial
class fTrMgPTMF_c : public fTrMgBa_c {
public:

    /// @brief Constructs a new tree.
    /// @param procFunc The process function, or @p nullptr .
    fTrMgPTMF_c(int (fBase_c::*procFunc)()) : mpProcFunc(procFunc) {}

    /// @brief Calls @ref ::mpProcFunc "the process function" on each base in the tree.
    /// @return Always returns true.
    bool walkPack();

private:
    int (fBase_c::*mpProcFunc)(); ///< The process function for the tree.
};

#pragma once
#include <types.h>
#include <game/cLib/c_tree.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_tree_nd.hpp>

class fBase_c;

/// @brief A base tree, made of fTrNdBa_c nodes.
/// @ingroup framework
/// @decompnote{The class name stands for "Tree Manager (for) Bases".}
class fTrMgBa_c : public cTreeMg_c {
public:

    /**
     * @brief Searches for a base with a given profile name, optionally under a given parent.
     *
     * @param profName The profile name.
     * @param parent The parent node to start searching from, or @p nullptr to use the tree's root node.
     * @return The node of the first base matching this criteria, or @p nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByProfName(ProfileName profName, const fTrNdBa_c *parent) const;

    /**
     * @brief Searches for a base with a given group type, optionally under a given parent.
     *
     * @param groupType The group type. Value is a fBase_c::GROUP_TYPE_e.
     * @param parent The parent node to start searching from, or @p nullptr to use the tree's root node.
     * @return The node of the first base matching this criteria, or @p nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByGroupType(u8 groupType, const fTrNdBa_c *parent) const;
};

/// @brief A base tree made of fTrNdBa_c nodes, with a reference to a process function.
/// @ingroup framework
class fTrMgBaFu_c : public fTrMgBa_c {
public:

    /// @brief Constructs a new tree.
    /// @param procFunc The process function, or @p nullptr .
    fTrMgBaFu_c(int (fBase_c::*procFunc)()) : mpProcFunc(procFunc) {}

    /// @brief Calls @ref mpProcFunc "the process function" on each base in the tree.
    /// @return Always returns true.
    bool treeListProc();

private:
    int (fBase_c::*mpProcFunc)(); ///< The process function for the tree.
};

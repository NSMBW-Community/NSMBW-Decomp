#pragma once
#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/base/f_profile.hpp>

class fTrNdBa_c;

/// @brief A container for a tree of fTrNdBa_c nodes.
class fTrMgBa_c : public cTreeMg_c {
public:
    /**
     * @brief Finds a node with a given profile name, optionally under a given parent root node.
     * 
     * @param profName The profile name.
     * @param parent The root node to start search below, or @p nullptr to use the tree's root node.
     * @return The found node, or @p nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByProfName(ProfileName profName, const fTrNdBa_c *parent);
    /**
     * @brief Find a node with a given group type, optionally under a given parent root node.
     * 
     * @param groupType The group type.
     * @param parent The root node to start search below, or @p nullptr to use the tree's root node.
     * @return The found node, or @p nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByGroupType(u8 groupType, const fTrNdBa_c *parent);
};

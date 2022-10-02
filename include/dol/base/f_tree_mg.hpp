#pragma once

#include <types.h>
#include <dol/cLib/c_tree.hpp>
#include <dol/profile/profile.hpp>

class fTrNdBa_c;

/// A container for a tree of fTrNdBa_c nodes.
class fTrMgBa_c : public cTreeMg_c {
public:
    /**
     * @brief Find a node with a given profile name, optionally under a given parent root node.
     * 
     * @param profName The profile name.
     * @param parent The root node to start search below, or nullptr to use the tree's root node.
     * @return const fTrNdBa_c* The found node, or nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByProfName(Profile profName, const fTrNdBa_c *parent);
    /**
     * @brief Find a node with a given group type, optionally under a given parent root node.
     * 
     * @param groupType The group type.
     * @param parent The root node to start search below, or nullptr to use the tree's root node.
     * @return const fTrNdBa_c* The found node, or nullptr if none was found.
     */
    const fTrNdBa_c *searchNodeByGroupType(unsigned char groupType, const fTrNdBa_c *parent);
};
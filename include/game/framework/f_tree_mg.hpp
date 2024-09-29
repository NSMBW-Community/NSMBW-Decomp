#pragma once
#include <types.h>
#include <game/cLib/c_tree.hpp>
#include <game/framework/f_profile.hpp>

class fTrNdBa_c;

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

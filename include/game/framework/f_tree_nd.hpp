#pragma once
#include <types.h>
#include <game/cLib/c_tree.hpp>
#include <game/framework/f_profile.hpp>

class fBase_c;

/// @brief A base tree node.
/// @details Each node is owned by a @ref fBase_c "base" and represents it in the @ref fTrMgBa_c "tree".
/// @ingroup framework
/// @decompnote{The class name stands for "Tree Node (with) Base".}
class fTrNdBa_c : public cTreeNd_c {
public:

    /// @brief Constructs a new tree node.
    /// @param owner The node's owner.
    fTrNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    /// @copydoc cTreeNd_c::getTreeNext
    fTrNdBa_c *getTreeNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNext();
    }

    /// @copydoc cTreeNd_c::getTreeNextNotChild
    fTrNdBa_c *getTreeNextNotChild() const {
        return (fTrNdBa_c *) cTreeNd_c::getTreeNextNotChild();
    }

    fTrNdBa_c *getParent() const {
        return (fTrNdBa_c *) cTreeNd_c::getParent();
    }

    fTrNdBa_c *getChild() const {
        return (fTrNdBa_c *) cTreeNd_c::getChild();
    }

    fTrNdBa_c *getBrPrev() const {
        return (fTrNdBa_c *) cTreeNd_c::getBrPrev();
    }

    fTrNdBa_c *getBrNext() const {
        return (fTrNdBa_c *) cTreeNd_c::getBrNext();
    }

    fBase_c *mpOwner; ///< The owner of this node.
};

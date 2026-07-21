#pragma once
#include <types.h>
#include <game/cLib/c_line.hpp>

class fBase_c;

/// @brief A base line node.
/// @details Each node is owned by a @ref fBase_c "base" and represents it in the @ref fLiMgBa_c "list".
/// @ingroup framework
class fLiNdBaLink_c : public cLineNd_c {
public:

    /// @brief Constructs a new list node.
    /// @param owner The node's owner.
    fLiNdBaLink_c(fBase_c *owner) : mpOwner(owner) {}

    inline fLiNdBaLink_c *getPrev() const {
        return (fLiNdBaLink_c *) cLineNd_c::getPrev();
    }

    inline fLiNdBaLink_c *getNext() const {
        return (fLiNdBaLink_c *) cLineNd_c::getNext();
    }

    /// @brief Removes this node from the owner's @ref fBase_c::mUnusedList "mUnusedList".
    /// @unofficial
    /// @decompnote{Might not actually belong to this class (XOR trick on hash).}
    void removeSelf();

    fBase_c *mpOwner; ///< The owner of this node.
};

/// @brief A base line node, with priority fields for reordering.
/// @ingroup framework
class fLiNdBaPr_c : public fLiNdBaLink_c {
public:

    /// @brief Constructs a new list node.
    /// @param owner The node's owner.
    fLiNdBaPr_c(fBase_c *owner) : fLiNdBaLink_c(owner), mOrder(0), mNewOrder(0) {}

    fLiNdBaPr_c *getPrev() const {
        return (fLiNdBaPr_c *) fLiNdBaLink_c::getPrev();
    }

    fLiNdBaPr_c *getNext() const {
        return (fLiNdBaPr_c *) fLiNdBaLink_c::getNext();
    }

    u16 getOrder() const {
        return mOrder;
    }

    u16 getNewOrder() const {
        return mNewOrder;
    }

    u16 mOrder; ///< The priority of this node. Lower values mean higher priority.
    u16 mNewOrder; ///< The priority the node should change to if it differs from #mOrder.
};

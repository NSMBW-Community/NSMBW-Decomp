#pragma once
#include <types.h>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_list_nd.hpp>

/// @brief A base list node, with priority fields for reordering.
/// @ingroup framework
/// @unofficial
class fLiNdPrio_c : public fLiNdBa_c {
public:

    /// @brief Constructs a new list node.
    /// @param owner The node's owner.
    fLiNdPrio_c(fBase_c *owner) : fLiNdBa_c(owner), mOrder(0), mNewOrder(0) {}

    fLiNdPrio_c *getPrev() const {
        return (fLiNdPrio_c *) fLiNdBa_c::getPrev();
    }

    fLiNdPrio_c *getNext() const {
        return (fLiNdPrio_c *) fLiNdBa_c::getNext();
    }

    u16 getOrder() const {
        return mOrder;
    }

    u16 getNewOrder() const {
        return mNewOrder;
    }

    u16 mOrder; ///< The priority of this node. Lower values mean higher priority.
    u16 mNewOrder; ///< The priority the node should change to if it differs from ::mOrder.
};

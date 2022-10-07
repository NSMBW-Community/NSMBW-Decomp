#pragma once
#include <types.h>
#include <dol/framework/f_profile.hpp>
#include <dol/framework/f_list_nd.hpp>

/// @brief A list node with priority fields for an order in a list.
/// @note Unofficial name.
class fLiNdPrio_c : public fLiNdBa_c {
public:
    u16 mOrder; ///< The priority of this node.
    u16 mNewOrder; ///< The priority the node should change to if it differs from ::mOrder.
    
    /// @brief Constructs a new fLiNdPrio_c.
    /// @param owner The node's owner.
    fLiNdPrio_c(fBase_c *owner) : fLiNdBa_c(owner), mOrder(0), mNewOrder(0) {}

    /// @copydoc cListNd_c::getPrev
    fLiNdPrio_c *getPrev() const {
        return (fLiNdPrio_c *) fLiNdBa_c::getPrev();
    }

    /// @copydoc cListNd_c::getNext
    fLiNdPrio_c *getNext() const {
        return (fLiNdPrio_c *) fLiNdBa_c::getNext();
    }

    /// @brief Gets the current order.
    u16 getOrder() const {
        return mOrder;
    }

    /// @brief Gets the new order.
    u16 getNewOrder() const {
        return mNewOrder;
    }
};

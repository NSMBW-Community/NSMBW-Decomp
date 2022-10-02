#pragma once

#include <types.h>
#include <dol/base/f_profile.hpp>
#include <dol/base/f_list_nd.hpp>

// [Unofficial name]
/// @brief A list node with priority fields for an order in a list.
class fLiNdPrio_c : public fLiNdBa_c {
public:
    u16 mOrder; ///< The priority of this node.
    u16 mNewOrder; ///< The priority the node should change to if it differs from ::mOrder.
    
    /**
     * @brief Construct a new fLiNdPrio_c.
     * 
     * @param owner The owner of the node.
     */
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
};

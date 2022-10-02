#pragma once

#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

/// @brief A list node with an owner reference.
class fLiNdBa_c : public cListNd_c {
public:
    fBase_c *mpOwner; ///< The owner of this node.
    
    /**
     * @brief Construct a new fLiNdBa_c.
     * 
     * @param owner The owner of the node.
     */
    fLiNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    inline fLiNdBa_c *getPrev() const {
        return (fLiNdBa_c *) cListNd_c::getPrev();
    }
    inline fLiNdBa_c *getNext() const {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }

    // [Unofficial name, might not actually belong to this class (-> xor trick on hash)]
    /// @brief Removes this node from the owner's fBase_c::mUnusedList.
    void removeSelf();
};

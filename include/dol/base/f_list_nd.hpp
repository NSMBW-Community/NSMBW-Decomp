#pragma once

#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

/// A list node with an owner reference.
class fLiNdBa_c : public cListNd_c {
public:
    fBase_c *mpOwner; ///< The owner of this node.
    
    fLiNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    inline fLiNdBa_c *getPrev() const {
        return (fLiNdBa_c *) cListNd_c::getPrev();
    }
    inline fLiNdBa_c *getNext() const {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }

    // [Unofficial name, might not actually belong to this class (-> xor trick on hash)]
    /// Removes this node from the owner's fBase_c::mUnusedList.
    void removeSelf();
};

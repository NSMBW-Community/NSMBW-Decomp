#pragma once
#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

/// @brief A list node with an owner reference.
/// @note Unofficial name.
class fLiNdBa_c : public cListNd_c {
public:

    /// @brief Constructs a new list node.
    /// @param owner The node's owner.
    fLiNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    inline fLiNdBa_c *getPrev() const {
        return (fLiNdBa_c *) cListNd_c::getPrev();
    }

    inline fLiNdBa_c *getNext() const {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }

    /// @brief Removes this node from the owner's fBase_c::mUnusedList.
    /// @note Unofficial name. Might not actually belong to this class (xor trick on hash).
    void removeSelf();

    fBase_c *mpOwner; ///< The owner of this node.
};

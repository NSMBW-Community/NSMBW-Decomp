#pragma once
#include <types.h>
#include <dol/cLib/c_list.hpp>

class fBase_c;

/// @brief A list node with an owner reference.
/// @note Unofficial name.
class fLiNdBa_c : public cListNd_c {
public:
    fBase_c *mpOwner; ///< The owner of this node.

    /// @brief Constructs a new fLiNdBa_c.
    /// @param owner The node's owner.
    fLiNdBa_c(fBase_c *owner) : mpOwner(owner) {}

    /// @copydoc cListNd_c::getPrev
    inline fLiNdBa_c *getPrev() const {
        return (fLiNdBa_c *) cListNd_c::getPrev();
    }

    /// @copydoc cListNd_c::getNext
    inline fLiNdBa_c *getNext() const {
        return (fLiNdBa_c *) cListNd_c::getNext();
    }

    /// @brief Removes this node from the owner's fBase_c::mUnusedList.
    /// @note Might not actually belong to this class (xor trick on hash).
    void removeSelf();
};

#pragma once
#include <types.h>
#include <game/cLib/c_list.hpp>

class fBase_c;

/// @brief A base list node.
/// @details Each node is owned by a @ref fBase_c "base" and represents it in the @ref fLiMgBa_c "list".
/// @ingroup framework
/// @unofficial
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

    /// @brief Removes this node from the owner's @ref fBase_c::mUnusedList "mUnusedList".
    /// @unofficial
    /// @decompnote{Might not actually belong to this class (XOR trick on hash).}
    void removeSelf();

    fBase_c *mpOwner; ///< The owner of this node.
};

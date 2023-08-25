#pragma once
#include <types.h>
/// @file

/// @brief A set node with a pointer to the owning container. See cOwnerSetMg_c.
/// @ingroup clib
/// @unofficial
class cOwnerSetNd_c {
public:
    void *getOwner() const { return mpOwner; }
    cOwnerSetNd_c *getNext() const { return mpNext; }

private:
    void* mpOwner; ///< The set that contains this node.
    cOwnerSetNd_c* mpNext; ///< The next node in the set.

    friend class cOwnerSetMg_c;
};

/// @brief A set container. See cOwnerSetNd_c.
/// @details The set is implemented as a singly-linked list.
/// @ingroup clib
/// @unofficial
class cOwnerSetMg_c {
public:
    /// @brief Constructs a new set container.
    cOwnerSetMg_c() : mpRoot(nullptr) {}

    /// @brief Destroys the set.
    ~cOwnerSetMg_c() { clear(); }

    /**
     * @brief Adds a node to the set.
     *
     * @param nd The node to add.
     * @param owner The owner of the node.
     */
    void add(cOwnerSetNd_c *nd, void *owner);

    /**
     * @brief Removes a node from the set.
     *
     * @param nd The node to remove.
     * @param owner The owner of the node.
     */
    void remove(cOwnerSetNd_c *nd, void *owner);

private:
    /// @brief Clears out the set and unlinks all child nodes.
    void clear();

    cOwnerSetNd_c* mpRoot; ///< The first element of the set.
};

/// @brief [Looks like a badly stripped assert].
/// @ingroup clib
/// @unofficial
void UNK_80161880();

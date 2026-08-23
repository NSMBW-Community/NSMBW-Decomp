#pragma once
#include <types.h>

/// @brief A doubly-linked list node. See cLineMg_c.
/// @ingroup clib
class cLineNd_c {
public:
    /// @brief Constructs a new list node.
    cLineNd_c() : mpPrev(nullptr), mpNext(nullptr) {}

    cLineNd_c *getPrev() const { return mpPrev; }
    cLineNd_c *getNext() const { return mpNext; }

protected:
    cLineNd_c *mpPrev; ///< The previous node.
    cLineNd_c *mpNext; ///< The next node.

    friend class cLineMg_c;
};

/// @brief A doubly-linked list container. See cLineNd_c.
/// @ingroup clib
class cLineMg_c {
public:
    /// @brief Constructs a new list container.
    cLineMg_c() : mpFirst(nullptr), mpLast(nullptr) {}

    /**
     * @brief Inserts a node after the given previous node.
     *
     * @param node The node to insert.
     * @param prevNode The node to insert it after, or @p nullptr to insert it at the beginning.
     * @return If the operation was successful.
     */
    bool insertLineNode(cLineNd_c *node, cLineNd_c *prevNode);

    /**
     * @brief Removes a node from the list.
     *
     * @param node The node to remove.
     * @return If the operation was successful.
     */
    bool removeLineNode(cLineNd_c *node);

    /**
     * @brief Adds a node to the end of the list.
     *
     * @param node The node to append.
     * @return If the operation was successful.
     */
    bool addLastLineNode(cLineNd_c *node);

    /**
     * @brief Adds a node to the beginning of the list.
     *
     * @param node The node to prepend.
     * @return If the operation was successful.
     */
    bool addTopLineNode(cLineNd_c *node);

    cLineNd_c *getFirst() const { return mpFirst; }
    cLineNd_c *getLast() const { return mpLast; }

protected:
    cLineNd_c *mpFirst; ///< The first node in the list.
    cLineNd_c *mpLast; ///< The last node in the list.
};

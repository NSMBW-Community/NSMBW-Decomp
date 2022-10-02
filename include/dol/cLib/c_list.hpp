#pragma once

#include <types.h>

// [Unofficial name]
/**
 * @brief A doubly-linked list node.
 * 
 * @see cListMg_c
 */
class cListNd_c {
public:
    cListNd_c *mpPrev; ///< The previous node.
    cListNd_c *mpNext; ///< The next node.
    
    /// @brief Construct a new cListNd_c.
    cListNd_c() : mpPrev(nullptr), mpNext(nullptr) {}

    cListNd_c *getPrev() const { return mpPrev; }
    cListNd_c *getNext() const { return mpNext; }
};

// [Unofficial name]
/**
 * @brief A doubly-linked list container.
 * 
 * @see cListNd_c
 */
class cListMg_c {
public:
    cListNd_c *mpFirst; ///< The first node in the list.
    cListNd_c *mpLast; ///< The last node in the list.

    /// @brief Construct a new cListMg_c.
    cListMg_c() : mpFirst(nullptr), mpLast(nullptr) {}

    /**
     * @brief Insert a node after a given previous node.
     * 
     * @param node The node to insert.
     * @param prevNode The node to insert it after, or nullptr to insert it at the beginning.
     * @return If the operation was successful.
     */
    bool insertAfter(cListNd_c *node, cListNd_c *prevNode);
    bool remove(cListNd_c *node); ///< Remove a node from the list.
    bool append(cListNd_c *node); ///< Add a node to the end of the list.
    bool prepend(cListNd_c *node); ///< Add a node to the beginning of the list.

    cListNd_c *getFirst() const { return mpFirst; }
    cListNd_c *getLast() const { return mpLast; }
};

#pragma once
#include <types.h>

/// @brief A tree node.
/// @details The tree is represented as a doubly-linked LCRS tree.
/// @see cTreeMg_c
class cTreeNd_c {
public:
    cTreeNd_c *mpParent; ///< The parent node.
    cTreeNd_c *mpChild; ///< The child node.
    cTreeNd_c *mpPrev; ///< The previous sibling node.
    cTreeNd_c *mpNext; ///< The next sibling node.

    /// @brief Constructs a new cTreeNd_c.
    cTreeNd_c();

    /// @brief Nulls out all fields.
    void forcedClear();

    /// @brief Gets the next node in preorder traversal order.
    cTreeNd_c *getTreeNext() const;

    /// @brief Gets the next node in preorder traversal order, excluding the node's children.
    cTreeNd_c *getTreeNextNotChild() const;

    cTreeNd_c *getParent() const { return mpParent; } ///< Gets the node's parent.
    cTreeNd_c *getChild() const { return mpChild; } ///< Gets the node's first child.
    cTreeNd_c *getBrPrev() const { return mpPrev; } ///< Gets the node's previous sibling.
    cTreeNd_c *getBrNext() const { return mpNext; } ///< Gets the node's next sibling.
};

/// @brief A tree container.
/// @see cTreeNd_c
class cTreeMg_c {
public:
    cTreeNd_c *mpRootNode; ///< The root node of the tree.

    /// @brief Constructs a new cTreeMg_c.
    cTreeMg_c() : mpRootNode(nullptr) {}

    /**
     * @brief Adds a node to the tree, either to the root node or to a specified parent node.
     * 
     * @param node The node to add.
     * @param parent The parent node to attach it to, or @p nullptr to attach it to the tree root.
     * @return If the operation was successful.
     */
    bool addTreeNode(cTreeNd_c *node, cTreeNd_c *parent);

    /**
     * @brief Removes a node from the tree.
     * 
     * @param node The node to remove.
     * @return If the operation was successful.
     */
    bool removeTreeNode(cTreeNd_c *node);
};

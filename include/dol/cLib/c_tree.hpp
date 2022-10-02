#pragma once

#include <types.h>

/**
 * @brief A tree node.
 * 
 * The tree is represented as a doubly-linked LCRS tree.
 * @see cTreeMg_c
 */
class cTreeNd_c {
public:
    cTreeNd_c *mpParent; ///< The parent node.
    cTreeNd_c *mpChild; ///< The child node.
    cTreeNd_c *mpPrev; ///< The previous sibling node.
    cTreeNd_c *mpNext; ///< The next sibling node.

    /// @brief Construct a new cTreeNd_c.
    cTreeNd_c();

    /// @brief Null out all fields.
    void forcedClear();

    /// @brief Get the next node in preorder traversal order.
    cTreeNd_c *getTreeNext() const;

    /// @brief Get the next node in preorder traversal order, excluding the node's children.
    cTreeNd_c *getTreeNextNotChild() const;

    cTreeNd_c *getParent() const { return mpParent; }
    cTreeNd_c *getChild() const { return mpChild; }
    cTreeNd_c *getBrPrev() const { return mpPrev; }
    cTreeNd_c *getBrNext() const { return mpNext; }
};

/**
 * @brief A tree container.
 * 
 * @see cTreeNd_c
 */
class cTreeMg_c {
public:
    cTreeNd_c *mpRootNode; ///< The root node of the tree.

    /// @brief Construct a new cTreeMg_c.
    cTreeMg_c() : mpRootNode(nullptr) {}

    /**
     * @brief Add a node to the tree, either to the root node or to a specified parent node.
     * 
     * @param node The node to add.
     * @param parent The parent node to attach it to, or nullptr to attach it to the tree root.
     * @return If the operation was successful.
     */
    bool addTreeNode(cTreeNd_c *node, cTreeNd_c *parent);
    /**
     * @brief Remove a node from the tree
     * 
     * @param node The node to add.
     * @return If the operation was successful.
     */
    bool removeTreeNode(cTreeNd_c *node);
};

#include <dol/cLib/c_tree.hpp>
#include <types.h>

cTreeNd_c::cTreeNd_c() {
    forcedClear();
}

void cTreeNd_c::forcedClear() {
    mpParent = nullptr;
    mpChild = nullptr;
    mpPrev = nullptr;
    mpNext = nullptr;
}

bool cTreeMg_c::addTreeNode(cTreeNd_c *node, cTreeNd_c *parent) {
    if (node != nullptr) {

        // If the parent is given, set it
        if (parent != nullptr) {
            node->mpParent = parent;
            cTreeNd_c *currChild = parent->mpChild;

            // If the parent has no children, set the node as the first child
            // Else set the node as the latest sibling
            if (currChild == nullptr) {
                parent->mpChild = node;
            } else {
                while (currChild->mpNext != nullptr) {
                    currChild = currChild->mpNext;
                }
                currChild->mpNext = node;
                node->mpPrev = currChild;
            }
        } else {
            cTreeNd_c *currNode = mpRootNode;

            // If the root node exists, set the new node as its latest sibling
            // Else set the new node as root
            if (currNode != nullptr) {
                while (currNode->mpNext != nullptr) {
                    currNode = currNode->mpNext;
                }
                currNode->mpNext = node;
                node->mpPrev = currNode;
            } else {
                mpRootNode = node;
            }
        }

    } else {
        return false;
    }

    return true;
}

bool cTreeMg_c::removeTreeNode(cTreeNd_c *node) {
    if (node != nullptr) {

        // If the node has children, do not allow its removal
        if (node->mpChild != nullptr) {
            return false;
        }

        // If the node has a previous sibling, update it
        // Else if the node has a parent, set the next sibling as the first child
        // Else set the next sibling as the root node
        if (node->mpPrev != nullptr) {
            node->mpPrev->mpNext = node->mpNext;
        } else if (node->mpParent != nullptr) {
            node->mpParent->mpChild = node->mpNext;
        } else if (node == mpRootNode) {
            mpRootNode = node->mpNext;
        }

        // If the node has a next sibling, update it
        if (node->mpNext != nullptr) {
            node->mpNext->mpPrev = node->mpPrev;
        }

        node->mpPrev = nullptr;
        node->mpNext = nullptr;
        node->mpParent = nullptr;

    } else {
        return false;
    }

    return true;
}

cTreeNd_c *cTreeNd_c::getTreeNext() const {
    // If the node has a child, return it
    cTreeNd_c *child = mpChild;
    if (child != nullptr) {
        return child;
    }

    // Else search in the rest of the tree
    return getTreeNextNotChild();
}

cTreeNd_c *cTreeNd_c::getTreeNextNotChild() const {
    // If the node has a next sibling, return it
    if (mpNext != nullptr) {
        return mpNext;
    }

    // Else, traverse the tree backwards until a parent sibling is found
    cTreeNd_c *currParent = mpParent;
    while (currParent != nullptr) {
        if (currParent->mpNext != nullptr) {
            return currParent->mpNext;
        }
        currParent = currParent->mpParent;
    }

    return nullptr;
}

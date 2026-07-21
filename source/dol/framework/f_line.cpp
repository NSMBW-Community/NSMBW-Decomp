#include <types.h>
#include <game/framework/f_line_mg.hpp>
#include <game/framework/f_base.hpp>

bool fLiMgBaFuPr_c::insertLineNodePriority(fLiNdBaPr_c *node) {
    fLiNdBaPr_c *curr = getFirst();

    // Null pointer check
    if (node == nullptr) {
        return false;
    }

    // If the first node isn't set, make the new node the first and last
    if (curr == nullptr) {
        return addLastLineNode(node);
    }

    // If the new node's order is higher than the first node, set the new node as first
    if (curr->getOrder() > node->getOrder()) {
        return insertLineNode(node, nullptr);
    }

    // Else traverse through the list until a node with lower order is found or the end is reached
    while (curr->getNext() && curr->getNext()->getOrder() <= node->getOrder()) {
        curr = curr->getNext();
    }
    return insertLineNode(node, curr);
}

bool fLiMgBaFu_c::lineListProc() {

    // Gracefully fail if the processing function isn't set
    if (mpProcFunc == 0) {
        return true;
    }

    // Call the processing function for each node in the list
    fLiNdBaLink_c *curr = getFirst();
    while (curr != nullptr) {
        fLiNdBaLink_c *next = curr->getNext();
        (curr->mpOwner->*mpProcFunc)();
        curr = next;
    }
    return true;
}

const fLiNdBaLink_c *fLiMgBa_c::searchNodeByID(fBaseID_e id) const {
    // [Possible optimizations:
    // - Cancel search if the null base ID is passed (it will never yield a result)
    // - Since IDs are assigned through an incrementing counter and lists are ordered by
    //   decreasing ID, the search can be ended early if the current ID is less than the
    //   searched one]
    for (fLiNdBaLink_c *curr = getFirst(); curr != nullptr; curr = curr->getNext()) {
        if (curr->mpOwner->mUniqueID == id) {
            return curr;
        }
    }
    return nullptr;
}

int fLiMgBa_c::countNodeByProfName(ProfileName prof) const {
    int count = 0;

    for (fLiNdBaLink_c *curr = getFirst(); curr != nullptr; curr = curr->getNext()) {
        if (curr->mpOwner->mProfName == prof) {
            count++;
        }
    }
    return count;
}

void fLiNdBaLink_c::clearData() {
    if (mpOwner != nullptr) {
        mpOwner->mUnusedList.removeLineNode(this);
        mpOwner = nullptr;
    }
}

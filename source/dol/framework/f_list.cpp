#include <types.h>
#include <game/framework/f_list_mg.hpp>
#include <game/framework/f_list_mg_ptmf.hpp>
#include <game/framework/f_base.hpp>

bool fLiMgPTMF_c::addNode(fLiNdPrio_c *node) {
    fLiNdPrio_c *curr = getFirst();

    // Null pointer check
    if (node == nullptr) {
        return false;
    }

    // If the first node isn't set, make the new node the first and last
    if (curr == nullptr) {
        return append(node);
    }

    // If the new node's order is higher than the first node, set the new node as first
    if (curr->getOrder() > node->getOrder()) {
        return insertAfter(node, nullptr);
    }

    // Else traverse through the list until a node with lower order is found or the end is reached
    while (curr->getNext() && curr->getNext()->getOrder() <= node->getOrder()) {
        curr = curr->getNext();
    }
    return insertAfter(node, curr);
}

bool fLiMgPTMF_c::walkPack() {

    // Gracefully fail if the processing function isn't set
    if (mpProcFunc == 0) {
        return true;
    }

    // Call the processing function for each node in the list
    fLiNdBa_c *curr = (fLiNdBa_c *) mpFirst;
    while (curr != nullptr) {
        fLiNdBa_c *next = curr->getNext();
        (curr->mpOwner->*mpProcFunc)();
        curr = next;
    }
    return true;
}

const fLiNdBa_c *fLiMgBa_c::searchNodeByID(fBaseID_e id) const {
    // [Possible optimizations:
    // - Cancel search if the null base ID is passed (it will never yield a result)
    // - Since IDs are assigned through an incrementing counter and lists are ordered by
    //   decreasing ID, the search can be ended early if the current ID is less than the
    //   searched one]
    for (fLiNdBa_c *curr = (fLiNdBa_c *) mpFirst; curr != nullptr; curr = curr->getNext()) {
        if (curr->mpOwner->mUniqueID == id) {
            return curr;
        }
    }
    return nullptr;
}

int fLiMgBa_c::countNodeByProfName(ProfileName prof) const {
    int count = 0;

    for (fLiNdBa_c *curr = (fLiNdBa_c *) mpFirst; curr != nullptr; curr = curr->getNext()) {
        if (curr->mpOwner->mProfName == prof) {
            count++;
        }
    }
    return count;
}

void fLiNdBa_c::removeSelf() {
    if (mpOwner != nullptr) {
        mpOwner->mUnusedList.remove(this);
        mpOwner = nullptr;
    }
}

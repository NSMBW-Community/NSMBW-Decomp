#include <types.h>
#include <dol/framework/f_list_mg.hpp>
#include <dol/framework/f_list_mg_ptmf.hpp>
#include <dol/framework/f_base.hpp>

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

    // If the first node's order is lower than the new node, place the latter first
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

#include <types.h>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_mg_ptmf.hpp>
#include <dol/base/f_base.hpp>

bool fLiMgPTMF_c::addNode(fLiNdPrio_c *node) {
    fLiNdPrio_c *curr = getFirst();
    if (node == nullptr) {
        return false;
    }

    if (curr == nullptr) {
        return append(node);
    }
    
    if (curr->getOrder() > node->getOrder()) {
        return insertAfter(node, nullptr);
    }
    
    while (curr->getNext() && curr->getNext()->getOrder() <= node->getOrder()) {
        curr = curr->getNext();
    }
    return insertAfter(node, curr);
}

bool fLiMgPTMF_c::walkPack() {
    if (mpProcFunc == 0) {
        return true;
    }
    
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
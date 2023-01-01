#include <dol/framework/f_base.hpp>
#include <dol/mLib/m_heap.hpp>
#include <lib/MSL_C/string.h>
#include <constants/sjis_constants.h>

fBaseID_e fBase_c::m_rootUniqueID = FIRST_ID;
ProfileName fBase_c::m_tmpCtProfName;
u32 fBase_c::m_tmpCtParam;
u8 fBase_c::m_tmpCtGroupType;
fTrNdBa_c *fBase_c::m_tmpCtConnectParent;

int (*fBase_c::sLoadAsyncCallback)();
void (*fBase_c::sUnloadCallback)();

fBase_c::fBase_c() :
    mUniqueID(m_rootUniqueID),
    mParam(m_tmpCtParam),
    mProfName(m_tmpCtProfName),
    mGroupType(m_tmpCtGroupType),
    mMng(this) {

    // Update the unique ID. Should it max out the counter, stall the game.
    m_rootUniqueID = (fBaseID_e) (m_rootUniqueID + 1);
    if (m_rootUniqueID == INVALID) {
        while (true);
    }

    // Add the base to the connect and search trees
    fManager_c::m_connectManage.addTreeNode(&mMng.mConnectNode, m_tmpCtConnectParent);
    int idx = mMng.getSearchTableNum();
    fManager_c::m_searchManage[idx].prepend(&mMng.mSearchNode);

    // Try to get the profile and set the order fields
    fProfile::fBaseProfile_c *prof = (*fProfile::sProfileList)[mProfName];
    if (prof != nullptr) {
        u16 executeOrder = prof->mExecuteOrder;
        mMng.mExecuteNode.mOrder = executeOrder;
        mMng.mExecuteNode.mNewOrder = executeOrder;

        u16 drawOrder = prof->mDrawOrder;
        mMng.mDrawNode.mOrder = drawOrder;
        mMng.mDrawNode.mNewOrder = drawOrder;
    }

    // Use the process flags from the parent, if set
    fBase_c *parent = getConnectParent();
    if (parent != nullptr) {
        if (parent->isProcessFlag(fManager_c::PROC_FLAG_CONNECT) || parent->isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
            mProcessFlags |= fManager_c::PROC_FLAG_CREATE;
        }
        if (parent->isProcessFlag(fManager_c::PROC_FLAG_EXECUTE) || parent->isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
            mProcessFlags |= fManager_c::PROC_FLAG_DELETE;
        }
    }
}

fBase_c::~fBase_c() {
    // [Clear the unused list]
    fLiNdBa_c *curr = mUnusedList.getFirst();
    while (curr != nullptr) {
        curr->removeSelf();
        curr = mUnusedList.getFirst();
    }
}

int fBase_c::commonPack(int (fBase_c::*doFunc)(), int (fBase_c::*preFunc)(), void (fBase_c::*postFunc)(MAIN_STATE_e)) {
    MAIN_STATE_e state;

    int result = (this->*preFunc)();
    if (result) {
        result = (this->*doFunc)();
        if (result == NOT_READY) {
            state = WAITING;
        } else if (result == SUCCEEDED) {
            state = SUCCESS;
        } else {
            state = ERROR;
        }

    } else {
        state = CANCELED;
    }

    (this->*postFunc)(state);
    return result;
}

int fBase_c::create() {
    return SUCCEEDED;
}

int fBase_c::preCreate() {
    return SUCCEEDED;
}

void fBase_c::postCreate(MAIN_STATE_e state) {

    // Creation successful, remove the base from the creation list
    if (state == SUCCESS) {
        fManager_c::m_createManage.remove(&mMng.mExecuteNode);

        // The base cannot be added to the execute/draw list during the execute process [(why?)],
        // so delay the operation
        if (fManager_c::m_nowLoopProc == fManager_c::EXECUTE) {
            mDelayManageAdd = true;
        } else {
            fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
            fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
            mLifecycleState = ACTIVE;
        }

    // Something went wrong, the base must be discarded
    } else if (state == ERROR) {
        deleteRequest();
    }
}

int fBase_c::doDelete() {
    return SUCCEEDED;
}

int fBase_c::createPack() {
    return commonPack(&fBase_c::create, &fBase_c::preCreate, &fBase_c::postCreate);
}

int fBase_c::preDelete() {
    // [Unused code]
    if (mpUnusedHelper != nullptr) {
        if (!mpUnusedHelper->LoadOnlyOne()) {
            return NOT_READY;
        }
    }

    // If the base has children, do not delete it
    fBase_c *child = getConnectChild();
    if (child != nullptr) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void fBase_c::postDelete(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        // Remove from all manager lists
        fManager_c::m_connectManage.removeTreeNode(&mMng.mConnectNode);
        fManager_c::m_searchManage[mMng.getSearchTableNum()].remove(&mMng.mSearchNode);
        fManager_c::m_deleteManage.remove(&mMng.mExecuteNode);

        // Delete the heap
        if (mpHeap != nullptr) {
            mpHeap->destroy();
        }

        // Delete the unused helper
        if (mpUnusedHelper != nullptr) {
            mpUnusedHelper->Delete();
        }

        // Delete the base itself
        delete this;
    }
}

int fBase_c::deletePack() {
    return commonPack(&fBase_c::doDelete, &fBase_c::preDelete, &fBase_c::postDelete);
}

int fBase_c::execute() {
    return SUCCEEDED;
}

int fBase_c::preExecute() {
    // Can only execute if not creating or deleting
    if (mDeleteRequested || isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void fBase_c::postExecute(MAIN_STATE_e state) {
    // Do nothing
}

int fBase_c::executePack() {
    return commonPack(&fBase_c::execute, &fBase_c::preExecute, &fBase_c::postExecute);
}

int fBase_c::draw() {
    return SUCCEEDED;
}

int fBase_c::preDraw() {
    // Can only draw if not deleting
    if (mDeleteRequested || isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void fBase_c::postDraw(MAIN_STATE_e state) {
    // Do nothing
}

int fBase_c::drawPack() {
    return commonPack(&fBase_c::draw, &fBase_c::preDraw, &fBase_c::postDraw);
}

void fBase_c::deleteReady() {
    // Do nothing
}

int fBase_c::connectProc() {
    if (mDeleteRequested) {
        mDeleteRequested = false;

        // Remove the base from the relevant manager lists and add it to the delete list
        if (mLifecycleState == ACTIVE) {
            fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
            fManager_c::m_drawManage.remove(&mMng.mDrawNode);
        } else {
            fManager_c::m_createManage.remove(&mMng.mExecuteNode);
        }

        fManager_c::m_deleteManage.prepend(&mMng.mExecuteNode);
        mLifecycleState = TO_BE_DELETED;

        // Also delete all its children
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }

    } else {
        // Copy over the process flags from the parent, if any
        fBase_c *parent = getConnectParent();
        if (parent != nullptr) {
            if (parent->isProcessFlag(fManager_c::PROC_FLAG_CONNECT) || parent->isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
                mProcessFlags |= fManager_c::PROC_FLAG_CREATE;
            } else if (isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
                mProcessFlags &= ~fManager_c::PROC_FLAG_CREATE;
            }

            if (parent->isProcessFlag(fManager_c::PROC_FLAG_EXECUTE) || parent->isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
                mProcessFlags |= fManager_c::PROC_FLAG_DELETE;
            } else if (isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
                mProcessFlags &= ~fManager_c::PROC_FLAG_DELETE;
            }
        }

        // Assert the correct order in the execute and draw lists
        // [There's probably some inlining going on here, which is the reason
        // for all those duplicated temporaries. Couldn't figure out a better solution, though]
        if (mLifecycleState == ACTIVE) {

            fLiNdPrio_c *executeNode = &mMng.mExecuteNode;
            if (executeNode->mNewOrder != executeNode->mOrder) {
                fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
                fLiNdPrio_c *executeNode2 = &mMng.mExecuteNode;
                executeNode2->mOrder = executeNode2->mNewOrder;
                fManager_c::m_executeManage.addNode(executeNode2);
            }

            fLiNdPrio_c *drawNode = &mMng.mDrawNode;
            if (drawNode->mNewOrder != drawNode->mOrder) {
                fManager_c::m_drawManage.remove(&mMng.mDrawNode);
                fLiNdPrio_c *drawNode2 = &mMng.mDrawNode;
                drawNode2->mOrder = drawNode2->mNewOrder;
                fManager_c::m_drawManage.addNode(drawNode2);
            }

        // The base has delayed adding itself to the manager lists, try again
        } else if (mLifecycleState != TO_BE_DELETED) {
            if (mRetryCreate) {
                mRetryCreate = false;
                fManager_c::m_createManage.append(&mMng.mExecuteNode);

            } else if (mDelayManageAdd) {
                mDelayManageAdd = false;
                fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
                fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
                mLifecycleState = ACTIVE;
            }
        }
    }

    return 1;
}

void fBase_c::deleteRequest() {
    // Check that deletion hasn't already been requested
    if (!mDeleteRequested && mLifecycleState != TO_BE_DELETED) {
        mDeleteRequested = true;
        deleteReady();

        // Also delete all children
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }
    }
}

fBase_c *fBase_c::getConnectParent() const {
    if (mMng.mConnectNode.getParent() != nullptr) {
        return mMng.mConnectNode.getParent()->mpOwner;
    }
    return nullptr;
}

fBase_c *fBase_c::getConnectChild() const {
    if (mMng.mConnectNode.getChild() != nullptr) {
        return mMng.mConnectNode.getChild()->mpOwner;
    }
    return nullptr;
}

fBase_c *fBase_c::getConnectBrNext() const {
    if (mMng.mConnectNode.getBrNext() != nullptr) {
        return mMng.mConnectNode.getBrNext()->mpOwner;
    }
    return nullptr;
}

bool fBase_c::entryFrmHeap(unsigned long size, EGG::Heap *parentHeap) {

    // Check if heap was already set
    if (mpHeap != nullptr) {
        return true;
    }

    unsigned long heapSize = 0;
    EGG::FrmHeap *newHeap = nullptr;

    // First, try to make a heap with the given size
    if (size != 0) {
        newHeap = mHeap::makeFrmHeapAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);

        if (newHeap != nullptr) {
            bool createSuccess = createHeap();
            mHeap::restoreCurrentHeap();

            if (!createSuccess) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
            } else {
                heapSize = mHeap::adjustFrmHeap(newHeap);
                if (size == heapSize) {
                    mpHeap = newHeap;
                    return true;
                }
            }
        }
    }

    // If that failed, try to make a heap with maximum size
    if (newHeap == nullptr) {
        newHeap = mHeap::makeFrmHeapAndUpdate(-1, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);

        if (newHeap != nullptr) {
            bool createSuccess = createHeap();
            mHeap::restoreCurrentHeap();

            if (!createSuccess) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
            } else {
                heapSize = mHeap::adjustFrmHeap(newHeap);
            }
        }
    }

    // [I think this is trying to verify if adjusting the new heap freed up some more space]
    if (newHeap != nullptr) {
        EGG::FrmHeap *largerHeap = mHeap::makeFrmHeapAndUpdate(heapSize, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);

        if (largerHeap != nullptr) {
            if (largerHeap < newHeap) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
                bool createSuccess = createHeap();
                mHeap::restoreCurrentHeap();

                if (!createSuccess) {
                    mHeap::destroyFrmHeap(largerHeap);
                } else {
                    mHeap::adjustFrmHeap(largerHeap);
                    mpHeap = largerHeap;
                    return true;
                }

            } else {
                mHeap::restoreCurrentHeap();
                mHeap::destroyFrmHeap(largerHeap);
            }
        }
    }

    if (newHeap != nullptr) {
        mpHeap = newHeap;
        return true;
    }

    // Everything failed, delete self
    deleteRequest();
    return false;
}

bool fBase_c::entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap) {
    // Check if heap was already set
    if (mpHeap != nullptr) {
        return true;
    }

    // Try to make a heap with the given size
    EGG::FrmHeap *newHeap = mHeap::makeFrmHeapAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
    if (newHeap != nullptr) {
        bool createSuccess = createHeap();
        mHeap::restoreCurrentHeap();

        if (!createSuccess) {
            mHeap::destroyFrmHeap(newHeap);
        } else {
            mpHeap = newHeap;
            return true;
        }
    }

    // Heap creation failed, delete self
    deleteRequest();
    return false;
}

bool fBase_c::createHeap() {
    return true;
}

void *fBase_c::operator new(size_t size) {
    void *mem = EGG::Heap::alloc(size, -4, mHeap::g_gameHeaps[0]);
    if (mem != nullptr) {
        memset(mem, 0, size);
    }
    return mem;
}

void fBase_c::operator delete(void *mem) {
    EGG::Heap::free(mem, mHeap::g_gameHeaps[0]);
}

void fBase_c::runCreate() {
    createPack();

    // If the base is ready for creation check for the current process,
    // as entries cannot be added to a list while the corresponding process
    // is running. In that case, retry later
    if (!mDeleteRequested && !mDelayManageAdd && mLifecycleState == WAITING_FOR_CREATE) {
        if (fManager_c::m_nowLoopProc == fManager_c::CREATE) {
            mRetryCreate = true;
        } else {
            fManager_c::m_createManage.append(&mMng.mExecuteNode);
        }
    }
}

fBase_c *fBase_c::getNonReadyChild() const {
    const fTrNdBa_c *connectNode = &mMng.mConnectNode;
    fTrNdBa_c *end = connectNode->getTreeNextNotChild();
    fTrNdBa_c *curr = connectNode->getChild();

    while (curr != nullptr && curr != end) {
        if (curr->mpOwner->mLifecycleState == WAITING_FOR_CREATE) {
            return curr->mpOwner;
        }
        curr = curr->getTreeNext();
    }
    return nullptr;
}

bool fBase_c::hasNonReadyChild() const {
    return getNonReadyChild() != nullptr;
}

void fBase_c::setTmpCtData(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {
    m_tmpCtParam = param;
    m_tmpCtProfName = profName;
    m_tmpCtGroupType = groupType;
    m_tmpCtConnectParent = connectParent;
}

fBase_c *fBase_c::fBase_make(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {

    // Check if the profile exists
    if ((*fProfile::sProfileList)[profName] == nullptr) {
        return nullptr;
    }

    // Construct the base
    setTmpCtData(profName, connectParent, param, groupType);
    fBase_c *res = (fBase_c *) (*fProfile::sProfileList)[profName]->mpClassInit();

    // Reset the temporary data
    setTmpCtData(0, nullptr, 0, 0);

    // Run create if the construction was successful
    if (res != nullptr) {
        res->runCreate();
    }
    return res;
}

fBase_c *fBase_c::createChild(ProfileName profName, fBase_c *connectParent, unsigned long param, u8 groupType) {
    if (connectParent == nullptr) {
        return nullptr;
    }
    return fBase_make(profName, &connectParent->mMng.mConnectNode, param, groupType);
}

fBase_c *fBase_c::createRoot(ProfileName profName, unsigned long param, u8 groupType) {
    return fBase_make(profName, nullptr, param, groupType);
}

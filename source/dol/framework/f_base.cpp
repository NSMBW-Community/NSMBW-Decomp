#include <game/framework/f_base.hpp>
#include <game/mLib/m_heap.hpp>
#include <lib/MSL_C/string.h>
#include <constants/sjis_constants.h>

fBaseID_e fBase_c::m_rootUniqueID = BASE_ID_FIRST;
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

    // Update the unique ID. If it maxes out the counter, stall the game
    m_rootUniqueID = (fBaseID_e) (m_rootUniqueID + 1);
    if (m_rootUniqueID == BASE_ID_MAX) {
        while (true);
    }

    // Add the base to the connect and search trees
    fManager_c::m_connectManage.addTreeNode(&mMng.mConnectNode, m_tmpCtConnectParent);
    int idx = mMng.getSearchTableNum();
    fManager_c::m_searchManage[idx].prepend(&mMng.mSearchNode);

    // Try to get the profile and set the order fields
    const fProfile::fBaseProfile_c *prof = (*fProfile::sProfileList)[mProfName].mBaseProfile;
    if (prof != nullptr) {
        u16 executeOrder = prof->mExecuteOrder;
        mMng.mMainNode.mOrder = executeOrder;
        mMng.mMainNode.mNewOrder = executeOrder;

        u16 drawOrder = prof->mDrawOrder;
        mMng.mDrawNode.mOrder = drawOrder;
        mMng.mDrawNode.mNewOrder = drawOrder;
    }

    // Update process control flags to match the parent base
    fBase_c *parent = getConnectParent();
    if (parent != nullptr) {
        if (parent->isProcControlFlag(ROOT_DISABLE_EXECUTE) || parent->isProcControlFlag(DISABLE_EXECUTE)) {
            setProcControlFlag(DISABLE_EXECUTE);
        }
        if (parent->isProcControlFlag(ROOT_DISABLE_DRAW) || parent->isProcControlFlag(DISABLE_DRAW)) {
            setProcControlFlag(DISABLE_DRAW);
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
        fManager_c::m_createManage.remove(&mMng.mMainNode);

        // Since operations cannot be scheduled while the corresponding process
        // is running, defer execution scheduling to the connect operation
        if (fManager_c::m_nowLoopProc == fManager_c::EXECUTE) {
            mDeferExecute = true;
        } else {
            fManager_c::m_executeManage.addNode(&mMng.mMainNode);
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

    // If the base has children, do not delete it yet
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
        fManager_c::m_deleteManage.remove(&mMng.mMainNode);

        // Delete the heap
        if (mHeap != nullptr) {
            mHeap->destroy();
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
    // Can only execute if not deleting and execution is enabled
    if (mDeleteRequested || isProcControlFlag(DISABLE_EXECUTE)) {
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
    // Can only draw if not deleting and drawing is enabled
    if (mDeleteRequested || isProcControlFlag(DISABLE_DRAW)) {
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

        // Move the base from its current manager lists to the delete list
        if (mLifecycleState == ACTIVE) {
            fManager_c::m_executeManage.remove(&mMng.mMainNode);
            fManager_c::m_drawManage.remove(&mMng.mDrawNode);
        } else {
            fManager_c::m_createManage.remove(&mMng.mMainNode);
        }

        fManager_c::m_deleteManage.prepend(&mMng.mMainNode);
        mLifecycleState = DELETING;

        // Delete all its children recursively
        // [Pointless code, since this is already done in deleteRequest]
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }

    } else {
        // Update process control flags to match the parent base
        fBase_c *parent = getConnectParent();
        if (parent != nullptr) {
            if (parent->isProcControlFlag(ROOT_DISABLE_EXECUTE) || parent->isProcControlFlag(DISABLE_EXECUTE)) {
                setProcControlFlag(DISABLE_EXECUTE);
            } else if (isProcControlFlag(DISABLE_EXECUTE)) {
                clearProcControlFlag(DISABLE_EXECUTE);
            }

            if (parent->isProcControlFlag(ROOT_DISABLE_DRAW) || parent->isProcControlFlag(DISABLE_DRAW)) {
                setProcControlFlag(DISABLE_DRAW);
            } else if (isProcControlFlag(DISABLE_DRAW)) {
                clearProcControlFlag(DISABLE_DRAW);
            }
        }

        // Assert the correct order in the execute and draw lists
        // [There's probably some inlining going on here, which is the reason
        // for all those duplicated temporaries. Couldn't figure out a better solution, though]
        if (mLifecycleState == ACTIVE) {

            fLiNdPrio_c *executeNode = &mMng.mMainNode;
            if (executeNode->mNewOrder != executeNode->mOrder) {
                fManager_c::m_executeManage.remove(&mMng.mMainNode);
                fLiNdPrio_c *executeNode2 = &mMng.mMainNode;
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

        // Process deferred operation scheduling requests
        } else if (mLifecycleState != DELETING) {
            if (mDeferRetryCreate) {
                mDeferRetryCreate = false;
                fManager_c::m_createManage.append(&mMng.mMainNode);

            } else if (mDeferExecute) {
                mDeferExecute = false;
                fManager_c::m_executeManage.addNode(&mMng.mMainNode);
                fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
                mLifecycleState = ACTIVE;
            }
        }
    }

    return SUCCEEDED;
}

void fBase_c::deleteRequest() {

    // Check that deletion hasn't already been requested
    if (!mDeleteRequested && mLifecycleState != DELETING) {
        mDeleteRequested = true;
        deleteReady();

        // Delete all children recursively
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
    if (mHeap != nullptr) {
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
                    mHeap = newHeap;
                    return true;
                }
            }
        }
    }

    // If that failed, allocate all available space
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
                    mHeap = largerHeap;
                    return true;
                }

            } else {
                mHeap::restoreCurrentHeap();
                mHeap::destroyFrmHeap(largerHeap);
            }
        }
    }

    if (newHeap != nullptr) {
        mHeap = newHeap;
        return true;
    }

    // Everything failed, delete self
    deleteRequest();
    return false;
}

bool fBase_c::entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap) {

    // Check if heap was already set
    if (mHeap != nullptr) {
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
            mHeap = newHeap;
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

    // If the create operation has not yet been completed, reschedule it for the next tick
    // Since operations cannot be scheduled while the corresponding process is running, defer
    // it to the connect operation
    if (!mDeleteRequested && !mDeferExecute && mLifecycleState == CREATING) {
        if (fManager_c::m_nowLoopProc == fManager_c::CREATE) {
            mDeferRetryCreate = true;
        } else {
            fManager_c::m_createManage.append(&mMng.mMainNode);
        }
    }
}

fBase_c *fBase_c::getChildProcessCreateState() const {
    const fTrNdBa_c *connectNode = &mMng.mConnectNode;
    fTrNdBa_c *end = connectNode->getTreeNextNotChild();
    fTrNdBa_c *curr = connectNode->getChild();

    while (curr != nullptr && curr != end) {
        if (curr->mpOwner->mLifecycleState == CREATING) {
            return curr->mpOwner;
        }
        curr = curr->getTreeNext();
    }
    return nullptr;
}

bool fBase_c::checkChildProcessCreateState() const {
    return getChildProcessCreateState() != nullptr;
}

void fBase_c::setTmpCtData(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {
    m_tmpCtParam = param;
    m_tmpCtProfName = profName;
    m_tmpCtGroupType = groupType;
    m_tmpCtConnectParent = connectParent;
}

fBase_c *fBase_c::fBase_make(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {

    // Check if the profile exists
    if ((*fProfile::sProfileList)[profName].mBaseProfile == nullptr) {
        return nullptr;
    }

    // Construct the base
    setTmpCtData(profName, connectParent, param, groupType);
    fBase_c *res = (fBase_c *) (*fProfile::sProfileList)[profName].mBaseProfile->mpClassInit();

    // Reset the temporary data
    setTmpCtData(0, nullptr, 0, 0);

    // Run create operation if the construction was successful
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

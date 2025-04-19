#include <game/bases/d_res.hpp>
#include <game/mLib/m_heap.hpp>
#include <lib/rvl/vi/VI.h>
#include <lib/rvl/os/OSCache.h>
#include <lib/MSL_C/string.h>

dRes_c::info_c::info_c() :
    mRefCount(0),
    mpDvdCmd(nullptr),
    mpArchive(nullptr),
    mpMountHeap(nullptr),
    mpArcBinary(nullptr),
    mDataHeap(nullptr),
    mpFiles(nullptr) {
}

dRes_c::info_c::~info_c() {
    if (!mpDvdCmd->mDone) {
        // Wait until the command is done
        do {
            VIWaitForRetrace();
        } while (!mpDvdCmd->mDone);
    }
    cleanup();
}

bool dRes_c::info_c::cleanup() {
    // Clean up the DVD command
    if (mpDvdCmd != nullptr) {
        if (!mpDvdCmd->mDone) {
            return false;
        }
        mpArchive = mpDvdCmd->mpArchive;
        mpArcBinary = mpDvdCmd->getArcBinary();
        mpMountHeap = mpDvdCmd->mpHeap;
        mArchiveSize = mpDvdCmd->mArchiveSize;
        mpDvdCmd->destroy();

        mpDvdCmd = nullptr;
    }

    // Clean up the archive and its associated heap
    if (mpArchive != nullptr) {
        if (mDataHeap != nullptr) {
            mHeap::destroyFrmHeap(mDataHeap);
            mDataHeap = nullptr;
        }
        mpArchive->unmount();

        mpArchive = nullptr;
        mpFiles = nullptr;
    }

    // Free the archive binary
    if (mpArcBinary != nullptr) {
        EGG::Heap::free(mpArcBinary, nullptr);

        mpArcBinary = nullptr;
    }
    return true;
}

bool dRes_c::info_c::set(const char *arcName, const char *containingFolder, u8 allocDir, EGG::Heap *heap) {
    char pathBuf[80];
    snprintf(pathBuf, sizeof(pathBuf), "/%s/%s.arc", containingFolder, arcName);
    mpDvdCmd = mDvd_mountMemArchive_c::create(pathBuf, allocDir, heap);

    if (mpDvdCmd == nullptr) {
        return false;
    }

    strncpy(mName, arcName, sizeof(mName));
    if (mSetCallback != nullptr) {
        mSetCallback(pathBuf, heap);
    }
    return true;
}

dRes_c::info_c::LOAD_STATUS_e dRes_c::info_c::setRes(callback_c *callback) {
    if (mpArchive == nullptr) {
        if (mpDvdCmd == nullptr) {
            return LOAD_ERROR;
        }
        if (!mpDvdCmd->mDone) {
            return LOAD_IN_PROGRESS;
        }

        // Copy over the properties from the DVD command, then destroy it
        mpArchive = mpDvdCmd->mpArchive;
        mpArcBinary = mpDvdCmd->getArcBinary();
        mpMountHeap = mpDvdCmd->mpHeap;
        mArchiveSize = mpDvdCmd->mArchiveSize;
        mpDvdCmd->destroy();
        mpDvdCmd = nullptr;

        if (mpArchive == nullptr) {
            return LOAD_ERROR;
        }

        // Prepare the heap for the archive data
        EGG::Heap *heap = (mpMountHeap != nullptr) ? mpMountHeap : mHeap::g_gameHeaps[0];
        mDataHeap = mHeap::makeFrmHeapAndUpdate(-1, heap, "dRes_c::info_c::mDataHeap", 0x20, 0);
        if (mDataHeap == nullptr) {
            return LOAD_ERROR;
        }

        LOAD_STATUS_e resStatus = loadRes(callback);

        // Restore the previous heap
        mHeap::restoreCurrentHeap();
        mHeap::adjustFrmHeap(mDataHeap);
        if (resStatus == LOAD_ERROR) {
            return resStatus;
        }

        // Ensure data is written to main memory
        DCStoreRangeNoSync(mDataHeap, (size_t) mDataHeap->mpHeapHandle->mpHeapEnd - (size_t) mDataHeap);
    }

    return LOAD_SUCCESS;
}

void dRes_c::searchCallback_c::callback(void *file, const ARCDirEntry *dirEntry, const char *path) {
    if (dirEntry->mIsDirectory) {
        // Copy the first 4 characters of the folder name and pad with spaces
        const char *name = dirEntry->mpName;
        mFolderSig = '    ';
        int nameLen = strlen(name);
        memcpy(&mFolderSig, name, (nameLen <= 4) ? nameLen : 4);
    } else {
        // Callback only called for files
        u8 *cbRes = (u8 *) mpCallback->execute(file, mFolderSig, path);
        if (mFileIdx < mNumFiles) {
            mpFiles[mFileIdx++] = cbRes;
        }
    }
}

void dRes_c::searchCallback_c::callback(void *cbInfo, void *file, const ARCDirEntry *dirEntry, const char *path) {
    ((searchCallback_c *) cbInfo)->callback(file, dirEntry, path);
}

dRes_c::info_c::LOAD_STATUS_e dRes_c::info_c::loadRes(dRes_c::callback_c *callback) {
    int numFiles = mpArchive->countFile();
    mpFiles = new u8 *[numFiles];
    if (mpFiles == nullptr) {
        return LOAD_ERROR;
    }

    // Prepare the file array
    for (unsigned int i = 0; i < numFiles; i++) {
        mpFiles[i] = nullptr;
    }

    if (callback != nullptr) {
        // Use the callback to populate ::mpFiles
        callback->init(mName);
        searchCallback_c searchCallback(callback, mpFiles, numFiles, 0, '    ');
        mpArchive->searchInside(&dRes_c::searchCallback_c::callback, &searchCallback);
    } else {
        // Simply populate ::mpFiles with the files
        mpArchive->getFileArray(mpFiles, numFiles);
    }
    return LOAD_SUCCESS;
}

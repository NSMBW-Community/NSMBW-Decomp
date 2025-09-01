#include <game/bases/d_res.hpp>
#include <revolution/CX.h>
#include <string.h>

void (*dRes_c::mSetCallback)(const char *arcName, EGG::Heap *heap);

dRes_c::dRes_c() :
    mpArcInfos(nullptr),
    mNumArcs(0),
    mpCallback(nullptr) {
}

dRes_c::~dRes_c() {
    if (mpArcInfos != nullptr) {
        delete[] mpArcInfos;
        mpArcInfos = nullptr;
        mNumArcs = 0;
    }
}

bool dRes_c::init(u16 maxCount, callback_c *callback) {
    mpArcInfos = new info_c[maxCount];
    if (!mpArcInfos) {
        return false;
    }
    mNumArcs = maxCount;
    mpCallback = callback;

    return true;
}

bool dRes_c::setRes(const char *arcName, const char *containingFolder, u8 allocDir, EGG::Heap *heap) {
    // First try to find an existing instance of the archive
    info_c *info = getResInfo(arcName);
    if (info == nullptr) {
        // We need a new instance
        info = newResInfo();
        if (info == nullptr) {
            return false;
        }
        if (!info->set(arcName, containingFolder, allocDir, heap)) {
            info->cleanup();
            return false;
        }
    }
    info->incRefCount();
    return true;
}

bool dRes_c::deleteRes(const char *arcName) {
    info_c *info = getResInfo(arcName);
    if (info == nullptr) {
        return false;
    }

    info->decRefCount();
    if (info->getRefCount() == 0) {
        // Archive not needed anymore
        info->cleanup();
    }
    return true;
}

nw4r::g3d::ResFile dRes_c::getRes(const char *arcName, const char *resPath) const {
    void *data = nullptr;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        data = info->getArchive()->getFile(resPath, nullptr);
    }
    return nw4r::g3d::ResFile(data);
}

nw4r::g3d::ResFile dRes_c::getRes(const char *arcName, const char *resPath, unsigned long *size) const {
    void *data = nullptr;
    int newSize = 0;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        EGG::Archive::FileInfo fi;
        data = archive->getFile(resPath, &fi);
        newSize = fi.mFileSize;
    }
    *size = newSize;
    return nw4r::g3d::ResFile(data);
}

void *dRes_c::getRes(const char *arcName, const char *resPath, unsigned long *size, int *compressionType) const {
    void *data = nullptr;
    int newSize = 0;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(resPath);
        if (entryID >= 0) {
            // Resource exists
            EGG::Archive::FileInfo fi;
            data = archive->getFileFast(entryID, &fi);

            if (data != nullptr) {
                if (compressionType != nullptr) {
                    *compressionType = CX_COMPRESSION_NONE;
                }
                if (size != nullptr) {
                    *size = fi.mFileSize;
                }
            }
        } else {
            // Resource not found, try with .LZ extension
            char path[256];
            snprintf(path, sizeof(path), "%s.LZ", resPath);
            entryID = archive->convertPathToEntryID(path);

            if (entryID >= 0) {
                data = archive->getFileFast(entryID, nullptr);

                if (data != nullptr) {
                    if (compressionType != nullptr) {
                        *compressionType = ((u8 *) data)[0] & 0xF0;
                    }
                    if (size != nullptr) {
                        *size = CXGetUncompressedSize(data);
                    }
                }
            }
        }
    }
    return data;
}

 void dRes_c::copyRes(const void *from, void *to, int size) {
    memcpy(to, from, size);
}

 void dRes_c::copyRes(const void *from, void *to, int size, int compressionType) {
    if (compressionType == CX_COMPRESSION_NONE) {
        copyRes(from, to, size);
    } else {
        CXUncompressLZ(from, to);
    }
}

void *dRes_c::getResSilently(const char *arcName, const char *resPath) const {
    void *data = nullptr;

    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(resPath);

        if (entryID >= 0) {
            // Resource exists, load it
            data = archive->getFileFast(entryID, nullptr);
        }
    }

    return data;
}

void *dRes_c::getResSilently(const char *arcName, const char *resPath, unsigned long *size) const {
    void *data = nullptr;
    int newSize = 0;

    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(resPath);

        if (entryID >= 0) {
            // Resource exists, load it
            EGG::Archive::FileInfo fi;
            data = archive->getFileFast(entryID, &fi);
            newSize = fi.mFileSize;
        }
    }

    *size = newSize;

    return data;
}

bool dRes_c::syncAllRes() {
    info_c *info = &mpArcInfos[0];
    for (int i = 0; i < mNumArcs; i++) {
        if (info->getDvdCmd() != nullptr) {
            if (info->setRes(mpCallback) > info_c::LOAD_SUCCESS) {
                return true;
            }
        }
        info++;
    }
    return false;
}

dRes_c::info_c *dRes_c::getResInfo(const char *arcName) const {
    int count = mNumArcs; // [Needed for matching]
    info_c *info = &mpArcInfos[0];
    for (int i = 0; i < count; i++) {
        if (info->getRefCount() > 0 && strcmp(arcName, info->getName()) == 0) {
            return info;
        }
        info++;
    }
    return nullptr;
}

dRes_c::info_c *dRes_c::newResInfo() {
    info_c *info = &mpArcInfos[0];
    for (int i = 0; i < mNumArcs; i++) {
        // An info_c is free if it has no references
        if (info->getRefCount() == 0) {
            return info;
        }
        info++;
    }
    return nullptr;
}

dRes_c::info_c *dRes_c::getResInfoLoaded(const char *arcName) const {
    info_c *info = getResInfo(arcName);
    if (info == nullptr) {
        return nullptr;
    } else if (info->getArchive() == nullptr) {
        return nullptr;
    }
    return info;
}

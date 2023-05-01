#include <dol/bases/d_res.hpp>
#include <lib/MSL_C/string.h>
#include <lib/rvl/cx/cx.h>

void (*dRes_c::mSetCallback)(const char *arcName, EGG::Heap *heap);

dRes_c::dRes_c() {
    mpInfos = nullptr;
    mNumInfos = 0;
    mpCallback = nullptr;
}

dRes_c::~dRes_c() {
    if (mpInfos != nullptr) {
        delete[] mpInfos;
        mpInfos = nullptr;
        mNumInfos = 0;
    }
}

bool dRes_c::init(u16 maxCount, callback_c *callback) {
    mpInfos = new info_c[maxCount];
    if (!mpInfos) {
        return false;
    }
    mNumInfos = maxCount;
    mpCallback = callback;

    return true;
}

bool dRes_c::setRes(const char *arcName, const char *filePath, u8 allocDir, EGG::Heap *heap) {
    // First try to find an existing instance of the resource
    info_c *info = getResInfo(arcName);
    if (info == nullptr) {
        // We need a new instance
        info = newResInfo();
        if (info == nullptr) {
            return false;
        }
        if (!info->set(arcName, filePath, allocDir, heap)) {
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
        // Resource not needed anymore
        info->cleanup();
    }
    return true;
}

void *dRes_c::getRes(const char *arcName, const char *filePath) const {
    void *data = nullptr;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        data = info->getArchive()->getFile(filePath, nullptr);
    }
    return data;
}

void *dRes_c::getRes(const char *arcName, const char *filePath, unsigned long *size) const {
    void *data = nullptr;
    int newSize = 0;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        EGG::Archive::FileInfo fi = { 0, 0 };
        data = archive->getFile(filePath, &fi);
        newSize = fi.mFileSize;
    }
    *size = newSize;
    return data;
}

void *dRes_c::getRes(const char *arcName, const char *filePath, unsigned long *size, int *compressionType) const {
    void *data = nullptr;
    int newSize = 0;
    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(filePath);
        if (entryID >= 0) {
            // File exists
            EGG::Archive::FileInfo fi = { 0, 0 };
            data = archive->getFileFast(entryID, &fi);

            if (data != nullptr) {
                if (compressionType != nullptr) {
                    *compressionType = 0;
                }
                if (size != nullptr) {
                    *size = fi.mFileSize;
                }
            }
        } else {
            // File not found, try with .LZ extension
            char path[256];
            snprintf(path, sizeof(path), "%s.LZ", filePath);
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

 void dRes_c::copyRes(const void *from, void *to, int size, int compressionLevel) {
    if (compressionLevel == 0) {
        copyRes(from, to, size);
    } else {
        CXUncompressLZ(from, to);
    }
}

void *dRes_c::getResSilently(const char *arcName, const char *filePath) const {
    void *data = nullptr;

    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(filePath);

        if (entryID >= 0) {
            // File exists, load it
            data = archive->getFileFast(entryID, nullptr);
        }
    }

    return data;
}

void *dRes_c::getResSilently(const char *arcName, const char *filePath, unsigned long *size) const {
    void *data = nullptr;
    int newSize = 0;

    info_c *info = getResInfoLoaded(arcName);
    if (info != nullptr && info->getArchive() != nullptr) {
        EGG::Archive *archive = info->getArchive();
        long entryID = archive->convertPathToEntryID(filePath);

        if (entryID >= 0) {
            // File exists, load it
            EGG::Archive::FileInfo fi = { 0, 0 };
            data = archive->getFileFast(entryID, &fi);
            newSize = fi.mFileSize;
        }
    }

    *size = newSize;

    return data;
}

bool dRes_c::syncAllRes() {
    info_c *info = &mpInfos[0];
    for (int i = 0; i < mNumInfos; i++) {
        if (info->getDvdCmd() != nullptr) {
            if (info->setRes(mpCallback) > 0) {
                return true;
            }
        }
        info++;
    }
    return false;
}

dRes_c::info_c *dRes_c::getResInfo(const char *arcName) const {
    int count = mNumInfos; // [Needed for matching]
    info_c *info = &mpInfos[0];
    for (int i = 0; i < count; i++) {
        if (info->getRefCount() > 0 && strcmp(arcName, info->getName()) == 0) {
            return info;
        }
        info++;
    }
    return nullptr;
}

dRes_c::info_c *dRes_c::newResInfo() {
    info_c *info = &mpInfos[0];
    for (int i = 0; i < mNumInfos; i++) {
        // An info_c can be used if it has no references
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

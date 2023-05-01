#pragma once
#include <lib/rvl/mem/MEM.h>
#include <lib/rvl/arc/ARC.h>

namespace EGG {
    class Archive {
    public:
        struct FileInfo {
            int mFileOffset;
            int mFileSize;
        };

        typedef void (*searchCallback)(void *, void *, const ARCDirEntry *, const char *);

        void unmount();
        int countFile();

        void getFileArray(u8 **files, unsigned long numFiles);
        void searchInside(searchCallback callback, void *callbackParam);

        long convertPathToEntryID(const char *path);
        void *getFile(const char *path, FileInfo *fileInfo);
        void *getFileFast(long entryID, FileInfo *fileInfo);
    };
}

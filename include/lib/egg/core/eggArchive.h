#pragma once
#include <revolution/ARC.h>

namespace EGG {
    class Archive {
    public:
        struct FileInfo {
            FileInfo() : mFileOffset(0), mFileSize(0) {}
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

#pragma once

#include <types.h>

namespace EGG {

class DvdFile {
public:
    DvdFile();
    virtual ~DvdFile();

    virtual bool openFilename(const char *filename);
    virtual void close();
    virtual void readData(void *buffer, long size, long offset);
    virtual void writeData(const void *, long, long);
    virtual size_t getFileSize() const { return mFileSize; }
    virtual void open(const char *);
    virtual void open(const char *, void *);

    u8 mPad1[0x6c];
    size_t mFileSize;
    u8 mPad2[0x5c];

    static void initialize();
};

} // namespace EGG

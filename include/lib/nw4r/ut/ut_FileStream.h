#ifndef NW4R_UT_FILE_STREAM_H
#define NW4R_UT_FILE_STREAM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_IOStream.h>

namespace nw4r {
namespace ut {

class FileStream : public IOStream {
public:
    NW4R_UT_RTTI_DECL(FileStream);

    enum SeekOrigin { SEEK_BEG, SEEK_CUR, SEEK_END };

public:
    FileStream() {}
    virtual ~FileStream() {} // at 0xC

    virtual u32 GetSize() const = 0; // at 0x40

    virtual void Seek(s32 offset, u32 origin); // at 0x44

    virtual void Cancel(); // at 0x48
    virtual bool CancelAsync(StreamCallback pCallback,
                             void* pCallbackArg); // at 0x4C

    virtual bool CanSeek() const = 0;   // at 0x50
    virtual bool CanCancel() const = 0; // at 0x54

    virtual u32 Tell() const = 0; // at 0x58

protected:
    class FilePosition {
    public:
        FilePosition() : mFileSize(0), mPosition(0) {}

        u32 GetFileSize() const {
            return mFileSize;
        }
        void SetFileSize(u32 size) {
            mFileSize = size;
        }

        u32 Tell() const {
            return mPosition;
        }

        u32 Skip(s32 offset);
        u32 Append(s32 offset);
        void Seek(s32 offset, u32 origin);

    private:
        u32 mFileSize; // at 0x0
        u32 mPosition; // at 0x4
    };
};

} // namespace ut
} // namespace nw4r

#endif

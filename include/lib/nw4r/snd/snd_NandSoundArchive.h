#ifndef NW4R_SND_NAND_SOUND_ARCHIVE_H
#define NW4R_SND_NAND_SOUND_ARCHIVE_H
#include "nw4r/snd/snd_SoundArchive.h"
#include "nw4r/snd/snd_SoundArchiveFile.h"
#include "nw4r/types_nw4r.h"
#include "nw4r/ut.h"  // IWYU pragma: export
#include <revolution/NAND.h> // IWYU pragma: export

namespace nw4r {
namespace snd {

class NandSoundArchive : public SoundArchive {
private:
    struct NandFileStream;

public:
    NandSoundArchive();
    virtual ~NandSoundArchive(); // at 0x8

    virtual const void *detail_GetFileAddress(ulong /* id */) const {
        return NULL;
    } // at 0xC

    virtual const void *detail_GetWaveDataFileAddress(ulong /* id */) const {
        return NULL;
    } // at 0x10

    virtual int detail_GetRequiredStreamBufferSize() const; // at 0x14

    virtual ut::FileStream *OpenStream(void *pBuffer, int size, ulong offset,
                                       ulong length) const; // at 0x18

    virtual ut::FileStream *OpenExtStream(
        void *pBuffer, int size, const char *pExtPath, ulong offset,
        ulong length
    ) const; // at 0x1C

    bool Open(const char *pPath);
    void Close();

    bool LoadHeader(void *pBuffer, ulong size);
    bool LoadLabelStringData(void *pBuffer, ulong size);

    ulong GetHeaderSize() const {
        return mFileReader.GetInfoChunkSize();
    }
    ulong GetLabelStringDataSize() const {
        return mFileReader.GetLabelStringChunkSize();
    }

private:
    bool LoadFileHeader() DONT_INLINE;

private:
    detail::SoundArchiveFileReader mFileReader; // at 0x108
    NANDFileInfo mFileInfo;                     // at 0x14C
    bool mOpen;                                 // at 0x1D8
};

} // namespace snd
} // namespace nw4r

#endif

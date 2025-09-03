#ifndef NW4R_SND_DVD_SOUND_ARCHIVE_H
#define NW4R_SND_DVD_SOUND_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

// WARNING: DO NOT REORDER these #include directives, data pooling depends on it

// clang-format off
#include "nw4r/ut/ut_FileStream.h" // This needs to be
#include "nw4r/snd/snd_SoundArchive.h" // before this
#include "nw4r/snd/snd_SoundArchiveFile.h" // before this
#include <revolution/DVD/dvd.h>
// clang-format on

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class DvdSoundArchive : public SoundArchive
	{
	public:
		class DvdFileStream;
		DvdSoundArchive();
		virtual ~DvdSoundArchive(); // at 0x8

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

		bool Open(s32 entrynum);
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
		bool LoadFileHeader();

	private:
		detail::SoundArchiveFileReader mFileReader; // at 0x108
		DVDFileInfo mFileInfo;                      // at 0x14C
		bool mOpen;                                 // at 0x188
	};
}} // namespace nw4r::snd

#endif // NW4R_SND_DVD_SOUND_ARCHIVE_H

#ifndef NW4R_SND_SOUND_ARCHIVE_H
#define NW4R_SND_SOUND_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "nw4r/snd/snd_global.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SoundArchiveFileReader; }}}

namespace nw4r { namespace ut { class FileStream; }}

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x265b9
	class SoundArchive
	{
	// enums
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x26182
		enum SoundType
		{
			SOUND_TYPE_INVALID,

			SOUND_TYPE_SEQ,
			SOUND_TYPE_STRM,
			SOUND_TYPE_WAVE,
		};

	// nested types
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25c18
		struct BankInfo
		{
			ulong	fileId;	// size 0x04, offset 0x00
		}; // size 0x04

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25ac7
		struct FileInfo
		{
			ulong		fileSize;			// size 0x04, offset 0x00
			ulong		waveDataFileSize;	// size 0x04, offset 0x04
			char	const *extFilePath;	// size 0x04, offset 0x08
			ulong		filePosCount;		// size 0x04, offset 0x0c
		}; // size 0x10

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25a5b
		struct FilePos
		{
			ulong	groupId;	// size 0x04, offset 0x00
			ulong	index;		// size 0x04, offset 0x04
		}; // size 0x08

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2593d
		struct GroupInfo
		{
			ulong		itemCount;			// size 0x04, offset 0x00
			char	const *extFilePath;	// size 0x04, offset 0x04
			ulong		offset;				// size 0x04, offset 0x08
			ulong		size;				// size 0x04, offset 0x0c
			ulong		waveDataOffset;		// size 0x04, offset 0x10
			ulong		waveDataSize;		// size 0x04, offset 0x14
		}; // size 0x18

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2584d
		struct GroupItemInfo
		{
			ulong	fileId;			// size 0x04, offset 0x00
			ulong	offset;			// size 0x04, offset 0x04
			ulong	size;			// size 0x04, offset 0x08
			ulong	waveDataOffset;	// size 0x04, offset 0x0c
			ulong	waveDataSize;	// size 0x04, offset 0x10
		}; // size 0x14

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25b9b
		struct PlayerInfo
		{
			int	playableSoundCount;	// size 0x04, offset 0x00
			ulong	heapSize;			// size 0x04, offset 0x04
		}; // size 0x08

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25dc1
		struct SeqSoundInfo
		{
			ulong		dataOffset;				// size 0x04, offset 0x00
			ulong		bankId;					// size 0x04, offset 0x04
			ulong		allocTrack;				// size 0x04, offset 0x08
			int		channelPriority;		// size 0x04, offset 0x0c
			bool	releasePriorityFixFlag;	// size 0x01, offset 0x10
			/* 3 bytes padding */
		}; // size 0x14

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25d0f
		struct StrmSoundInfo
		{
			ulong		startPosition;		// size 0x04, offset 0x00
			u16		allocChannelCount;	// size 0x02, offset 0x04
			u16	allocTrackFlag;		// size 0x02, offset 0x06
		}; // size 0x08

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25c5f
		struct WaveSoundInfo
		{
			s32		subNo;					// size 0x04, offset 0x00
			int		channelPriority;		// size 0x04, offset 0x04
			bool	releasePriorityFixFlag;	// size 0x01, offset 0x08
			/* 3 bytes padding */
		}; // size 0x0c

		struct Sound3DParam {
			ulong flags;     // at 0x0
			u8 decayCurve; // at 0x4
			u8 decayRatio; // at 0x5
			u8 field_0x06; // at 0x6
		};

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x256dc
		struct SoundArchivePlayerInfo
		{
			int	seqSoundCount;		// size 0x04, offset 0x00
			int	seqTrackCount;		// size 0x04, offset 0x04
			int	strmSoundCount;		// size 0x04, offset 0x08
			int	strmTrackCount;		// size 0x04, offset 0x0c
			int	strmChannelCount;	// size 0x04, offset 0x10
			int	waveSoundCount;		// size 0x04, offset 0x14
			int	waveTrackCount;		// size 0x04, offset 0x18
		}; // size 0x1c

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x26012
		struct SoundInfo
		{
			ulong			fileId;			// size 0x04, offset 0x00
			ulong			playerId;		// size 0x04, offset 0x04
			int			actorPlayerId;	// size 0x04, offset 0x08
			int			playerPriority;	// size 0x04, offset 0x0c
			int			volume;			// size 0x04, offset 0x10
			int			remoteFilter;	// size 0x04, offset 0x14
			PanMode		panMode;		// size 0x04, offset 0x18
			PanCurve	panCurve;		// size 0x04, offset 0x1c
		}; // size 0x20

	// methods
	public:
		// cdtors
		SoundArchive();
		virtual ~SoundArchive();

		// virtual function ordering
		// vtable SoundArchive
		virtual void const *detail_GetFileAddress(ulong fileId) const = 0;
		virtual void const *detail_GetWaveDataFileAddress(ulong fileId) const = 0;
		virtual int detail_GetRequiredStreamBufferSize() const = 0;
		virtual ut::FileStream *OpenStream(void *buffer, int size, ulong begin,
		                                   ulong length) const = 0;
		virtual ut::FileStream *OpenExtStream(void *buffer, int size,
		                                      char const *extFilePath,
		                                      ulong begin, ulong length) const = 0;

		// methods
		void Setup(detail::SoundArchiveFileReader *fileReader);
		void Shutdown();

		ulong GetPlayerCount() const;
		ulong GetGroupCount() const;
		bool IsAvailable() const;
		SoundType GetSoundType(ulong soundId) const;

		const char* GetSoundLabelString(ulong id) const;
		ulong ConvertLabelStringToSoundId(char const *label) const;
		ulong ConvertLabelStringToPlayerId(const char* pLabel) const;
		ulong ConvertLabelStringToGroupId(const char* pLabel) const;
		ulong ConvertLabelStringToBankId(const char* pLabel) const;
		ulong GetSoundUserParam(ulong id) const;

		bool ReadSoundInfo(ulong soundId, SoundInfo *info) const;
		bool ReadSeqSoundInfo(ulong soundId, SeqSoundInfo *info) const;
		bool detail_ReadStrmSoundInfo(ulong soundId, StrmSoundInfo *info) const;
		bool detail_ReadWaveSoundInfo(ulong soundId, WaveSoundInfo *info) const;

		bool ReadPlayerInfo(ulong playerId, PlayerInfo *info) const;
		bool ReadSoundArchivePlayerInfo(SoundArchivePlayerInfo *info) const;
		bool detail_ReadSound3DParam(ulong soundId, nw4r::snd::SoundArchive::Sound3DParam*) const;

		bool ReadBankInfo(ulong bankId, BankInfo *info) const;

		bool detail_ReadGroupInfo(ulong groupId, GroupInfo *info) const;
		bool detail_ReadGroupItemInfo(ulong groupId, ulong index,
		                              GroupItemInfo *info) const;

		ulong detail_GetFileCount() const;
		bool detail_ReadFileInfo(ulong fileId, FileInfo *info) const;
		bool detail_ReadFilePos(ulong fileId, ulong index, FilePos *info) const;

		ut::FileStream *detail_OpenFileStream(ulong fileId, void *buffer,
		                                      int size) const;
		ut::FileStream* detail_OpenGroupStream(ulong id, void* pBuffer,
												int bufferSize) const;
		ut::FileStream* detail_OpenGroupWaveDataStream(ulong id, void* pBuffer,
														int bufferSize) const;

		void SetExternalFileRoot(const char* pExtFileRoot);

	private:
		ut::FileStream *OpenExtStreamImpl(void *buffer, int size,
		                                  char const *extFilePath, ulong begin,
		                                  ulong length) const;

	// static members
	public:
		static int const FILE_PATH_MAX = 255;
		static ulong const INVALID_ID = -1;

	// members
	private:
		/* vtable */														// size 0x004, offset 0x000
		detail::SoundArchiveFileReader	*mFileReader;						// size 0x004, offset 0x004
		char							mExtFileRoot[FILE_PATH_MAX + 1];	// size 0x100, offset 0x008
	}; // size 0x108
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_ARCHIVE_H

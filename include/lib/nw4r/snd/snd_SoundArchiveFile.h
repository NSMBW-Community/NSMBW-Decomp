#ifndef NW4R_SND_SOUND_ARCHIVE_FILE_H
#define NW4R_SND_SOUND_ARCHIVE_FILE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "nw4r/snd/snd_SoundArchive.h"
#include "nw4r/snd/snd_Util.h"

#include "nw4r/ut/ut_binaryFileFormat.h"
#include "nw4r/ut/ut_algorithm.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail
{
	struct SoundArchiveFile
	{
		/* Header */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x240a6
		struct Header
		{
			ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
			ulong						symbolDataOffset;	// size 0x04, offset 0x10
			ulong						symbolDataSize;		// size 0x04, offset 0x14
			ulong						infoOffset;			// size 0x04, offset 0x18
			ulong						infoSize;			// size 0x04, offset 0x1c
			ulong						fileImageOffset;	// size 0x04, offset 0x20
			ulong						fileImageSize;		// size 0x04, offset 0x24
		}; // size 0x28

		static const int HEADER_AREA_SIZE = ROUND_UP(sizeof(Header), 32) + 40;

		/* SymbolBlock */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24881
		struct StringTreeNode
		{
			u16	flags;		// size 0x02, offset 0x00
			u16	bit;		// size 0x02, offset 0x02
			ulong		leftIdx;	// size 0x04, offset 0x04
			ulong		rightIdx;	// size 0x04, offset 0x08
			ulong		strIdx;		// size 0x04, offset 0x0c
			ulong		id;			// size 0x04, offset 0x10
		}; // size 0x14

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24a2e
		struct StringTree
		{
			ulong							rootIdx;	// size 0x04, offset 0x00
			Util::Table<StringTreeNode>	nodeTable;	// size 0x18, offset 0x04
		}; // size 0x1c

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24830
		struct StringTable
		{
			Util::Table<ulong>	offsetTable;	// size 0x08, offset 0x00
		}; // size 0x08

		struct StringChunk
		{
			ulong	tableOffset;		// size 0x04, offset 0x00
			ulong	soundTreeOffset;	// size 0x04, offset 0x04
			ulong	playerTreeOffset;	// size 0x04, offset 0x08
			ulong	groupTreeOffset;	// size 0x04, offset 0x0c
			ulong	bankTreeOffset;		// size 0x04, offset 0x10
		}; // size 0x14

		union StringBlock
		{
			StringTable	stringTable;	// size 0x08
			StringChunk	stringChunk;	// size 0x14
		}; // size 0x14

		static ulong const SIGNATURE_SYMB_BLOCK =
			NW4R_FOUR_BYTE('S', 'Y', 'M', 'B');

		struct SymbolBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			StringBlock				stringBlock;	// size 0x14, offset 0x08
		}; // size 0x20

		/* InfoBlock */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2da8f9
		enum SoundType
		{
			SOUND_TYPE_INVALID,

			SOUND_TYPE_SEQ,
			SOUND_TYPE_STRM,
			SOUND_TYPE_WAVE,
		};

		struct Sound3DParam {
			ulong flags;
			u8 decayCurve;
			u8 decayRatio;
			u8 field_0x06;
		};

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25183
		struct SeqSoundInfo
		{
			ulong		dataOffset;			// size 0x04, offset 0x00
			ulong		bankId;				// size 0x04, offset 0x04
			ulong		allocTrack;			// size 0x04, offset 0x08
			u8		channelPriority;	// size 0x01, offset 0x0c
			u8		releasePriorityFix;	// size 0x01, offset 0x0d
			u8	padding[2];
			ulong	reserved;
		}; // size 0x14

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x25089
		struct StrmSoundInfo
		{
			ulong		startPosition;		// size 0x04, offset 0x00
			u16		allocChannelCount;	// size 0x02, offset 0x04
			u16	allocTrackFlag;		// size 0x02, offset 0x06
			ulong	reserved;
		}; // size 0x0c

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24f62
		struct WaveSoundInfo
		{
			s32		subNo;				// size 0x04, offset 0x00
			ulong		allocTrack;			// size 0x04, offset 0x04
			u8		channelPriority;	// size 0x01, offset 0x08
			u8		releasePriorityFix;	// size 0x01, offset 0x09
			u8	padding[2];
			ulong	reserved;
		}; // size 0x10

		typedef Util::DataRef<void, SeqSoundInfo, StrmSoundInfo, WaveSoundInfo>
			SoundInfoRef;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2546c
		struct SoundCommonInfo
		{
			ulong							stringId;		// size 0x04, offset 0x00
			ulong							fileId;			// size 0x04, offset 0x04
			ulong							playerId;		// size 0x04, offset 0x08
			Util::DataRef<Sound3DParam>	param3dRef;		// size 0x08, offset 0x0c
			u8							volume;			// size 0x01, offset 0x14
			u8							playerPriority;	// size 0x01, offset 0x15
			u8							soundType;		// size 0x01, offset 0x16
			u8							remoteFilter;	// size 0x01, offset 0x17
			SoundInfoRef				soundInfoRef;	// size 0x08, offset 0x18
			ulong					userParam[2];	// size 0x08, offset 0x20
			u8							panMode;		// size 0x01, offset 0x28
			u8							panCurve;		// size 0x01, offset 0x29
			u8							actorPlayerId;	// size 0x01, offset 0x2a
			u8						reserved;
		}; // size 0x2c

		typedef Util::Table<Util::DataRef<SoundCommonInfo> >
			SoundCommonInfoTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2db9b5
		struct SoundArchivePlayerInfo
		{
			u16		seqSoundCount;		// size 0x02, offset 0x00
			u16		seqTrackCount;		// size 0x02, offset 0x02
			u16		strmSoundCount;		// size 0x02, offset 0x04
			u16		strmTrackCount;		// size 0x02, offset 0x06
			u16		strmChannelCount;	// size 0x02, offset 0x08
			u16		waveSoundCount;		// size 0x02, offset 0x0a
			u16		waveTrackCount;		// size 0x02, offset 0x0c
			u16	padding;
			ulong	reserved;
		}; // size 0x14

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24eab
		struct BankInfo
		{
			ulong		stringId;	// size 0x04, offset 0x00
			ulong		fileId;		// size 0x04, offset 0x04
			ulong	reserved;
		}; // size 0x0c

		typedef Util::Table<Util::DataRef<BankInfo> > BankInfoTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24d8f
		struct PlayerInfo
		{
			ulong		stringId;			// size 0x04, offset 0x00
			u8		playableSoundCount;	// size 0x01, offset 0x04
			u8	padding;
			u16	padding2;
			ulong		heapSize;			// size 0x04, offset 0x08
			ulong	reserved;
		}; // size 0x10

		typedef Util::Table<Util::DataRef<PlayerInfo> > PlayerInfoTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2db6e3
		struct GroupItemInfo
		{
			ulong		fileId;			// size 0x04, offset 0x00
			ulong		offset;			// size 0x04, offset 0x04
			ulong		size;			// size 0x04, offset 0x08
			ulong		waveDataOffset;	// size 0x04, offset 0x0c
			ulong		waveDataSize;	// size 0x04, offset 0x10
			ulong	reserved;
		}; // size 0x18

		typedef Util::Table<Util::DataRef<GroupItemInfo> > GroupItemInfoTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24c1b
		struct GroupInfo
		{
			ulong									stringId;		// size 0x04, offset 0x00
			s32									entryNum;		// size 0x04, offset 0x04
			Util::DataRef<char>					extFilePathRef;	// size 0x08, offset 0x08
			ulong									offset;			// size 0x04, offset 0x10
			ulong									size;			// size 0x04, offset 0x14
			ulong									waveDataOffset;	// size 0x04, offset 0x18
			ulong									waveDataSize;	// size 0x04, offset 0x1c
			Util::DataRef<GroupItemInfoTable>	itemTableRef;	// size 0x08, offset 0x20
		}; // size 0x28

		typedef Util::Table<Util::DataRef<GroupInfo> > GroupInfoTable;

		typedef Util::Table<Util::DataRef<SoundArchive::FilePos> > FilePosTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2dc40b
		struct FileInfo
		{
			ulong							fileSize;			// size 0x04, offset 0x00
			ulong							waveDataFileSize;	// size 0x04, offset 0x04
			s32							entryNum;			// size 0x04, offset 0x08
			Util::DataRef<char>			extFilePathRef;		// size 0x08, offset 0x0c
			Util::DataRef<FilePosTable>	filePosTableRef;	// size 0x08, offset 0x14
		}; // size 0x1c

		typedef Util::Table<Util::DataRef<FileInfo> > FileInfoTable;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x24665
		struct Info
		{
			Util::DataRef<SoundCommonInfoTable>		soundTableRef;				// size 0x08, offset 0x00
			Util::DataRef<BankInfoTable>			bankTableRef;				// size 0x08, offset 0x08
			Util::DataRef<PlayerInfoTable>			playerTableRef;				// size 0x08, offset 0x10
			Util::DataRef<FileInfoTable>			fileTableRef;				// size 0x08, offset 0x18
			Util::DataRef<GroupInfoTable>			groupTableRef;				// size 0x08, offset 0x20
			Util::DataRef<SoundArchivePlayerInfo>	soundArchivePlayerInfoRef;	// size 0x08, offset 0x28
		}; // size 0x30

		static ulong const SIGNATURE_INFO_BLOCK =
			NW4R_FOUR_BYTE('I', 'N', 'F', 'O');

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2da76d
		struct InfoBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			Info					info;			// size 0x30, offset 0x08
		}; // size 0x38

		/* SoundArchiveFile */

		static ulong const SIGNATURE_FILE =
			NW4R_FOUR_BYTE('R', 'S', 'A', 'R');
		static int const FILE_VERSION = NW4R_FILE_VERSION(1, 4);
	}; // "namespace" SoundArchiveFile
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x261f8
	class SoundArchiveFileReader
	{
	// methods
	public:
		// cdtors
		SoundArchiveFileReader();
		~SoundArchiveFileReader() {} // not default?

		// methods
		void Init(void const *soundArchiveData);

		u16 GetVersion() const { return mHeader.fileHeader.version; }
		ulong GetLabelStringChunkOffset() const
		{
			return mHeader.symbolDataOffset;
		}
		ulong GetLabelStringChunkSize() const { return mHeader.symbolDataSize; }
		ulong GetInfoChunkOffset() const { return mHeader.infoOffset; }
		ulong GetInfoChunkSize() const { return mHeader.infoSize; }

		char const *GetString(ulong id) const;
		SoundArchive::SoundType GetSoundType(ulong soundId) const;
		ulong GetSoundStringId(ulong id) const;
		ulong GetBankCount() const;
		ulong GetPlayerCount() const;
		ulong GetGroupCount() const;
		ulong GetFileCount() const;
		void const *GetPtrConst(void const *base, ulong offset) const
		{
			if (offset == 0)
				return nullptr;

			return ut::AddOffsetToPtr(base, offset);
		}

		void SetStringChunk(void const *stringChunk, ulong stringChunkSize);
		void SetInfoChunk(void const *infoChunk, ulong infoChunkSize);

		bool ReadSeqSoundInfo(ulong soundId,
		                      SoundArchive::SeqSoundInfo *info) const;
		bool ReadStrmSoundInfo(ulong soundId,
		                       SoundArchive::StrmSoundInfo *info) const;
		bool ReadWaveSoundInfo(ulong soundId,
		                       SoundArchive::WaveSoundInfo *info) const;
		bool ReadSoundArchivePlayerInfo(
			SoundArchive::SoundArchivePlayerInfo *info) const;
		bool ReadSoundInfo(ulong soundId, SoundArchive::SoundInfo *info) const;

		bool ReadBankInfo(ulong bankId, SoundArchive::BankInfo *info) const;

		bool ReadPlayerInfo(ulong playerId, SoundArchive::PlayerInfo *info) const;
		bool ReadSound3DParam(ulong soundId, nw4r::snd::SoundArchive::Sound3DParam* info) const;

		bool ReadGroupItemInfo(ulong groupId, ulong index,
		                       SoundArchive::GroupItemInfo *info) const;
		bool ReadGroupInfo(ulong groupId, SoundArchive::GroupInfo *info) const;

		bool ReadFileInfo(ulong fileId, SoundArchive::FileInfo *info) const;
		bool ReadFilePos(ulong fileId, ulong index,
		                 SoundArchive::FilePos *filePos) const;

		ulong ConvertLabelStringToId(SoundArchiveFile::StringTree const *tree,
		                           char const *str) const;
		const char* GetSoundLabelString(ulong id) const;
		ulong GetSoundUserParam(ulong id) const;

		ulong ConvertLabelStringToSoundId(const char* pLabel) const {
			return ConvertLabelStringToId(mStringTreeSound, pLabel);
		}
		ulong ConvertLabelStringToPlayerId(const char* pLabel) const {
			return ConvertLabelStringToId(mStringTreePlayer, pLabel);
		}
		ulong ConvertLabelStringToGroupId(const char* pLabel) const {
			return ConvertLabelStringToId(mStringTreeGroup, pLabel);
		}
		ulong ConvertLabelStringToBankId(const char* pLabel) const {
			return ConvertLabelStringToId(mStringTreeBank, pLabel);
		}

	private:
		static bool IsValidFileHeader(void const *soundArchiveData);

		SoundArchiveFile::SeqSoundInfo const *impl_GetSeqSoundInfo(
			ulong soundId) const;
		SoundArchiveFile::StrmSoundInfo const *impl_GetStrmSoundInfo(
			ulong soundId) const;
		SoundArchiveFile::WaveSoundInfo const *impl_GetWaveSoundInfo(
			ulong soundId) const;
		SoundArchiveFile::SoundCommonInfo const *impl_GetSoundInfo(
			ulong soundId) const;

		bool impl_GetSoundInfoOffset(
			ulong soundId, SoundArchiveFile::SoundInfoRef *soundInfoRef) const;

		SoundArchiveFile::BankInfo const *impl_GetBankInfo(ulong bankId) const;

		SoundArchiveFile::PlayerInfo const *impl_GetPlayerInfo(
			ulong playerId) const;

		SoundArchiveFile::GroupInfo const *impl_GetGroupInfo(ulong groupId) const;

	// static members
	public:
		static u16 const SUPPORTED_FILE_VERSION = NW4R_FILE_VERSION(1, 4);

	// members
	private:
		SoundArchiveFile::Header		mHeader;					// size 0x28, offset 0x00
		SoundArchiveFile::Info			const *mInfo;				// size 0x04, offset 0x28
		void							const *mStringBase;			// size 0x04, offset 0x2c
		SoundArchiveFile::StringTable	const *mStringTable;		// size 0x04, offset 0x30
		SoundArchiveFile::StringTree	const *mStringTreeSound;	// size 0x04, offset 0x34
		SoundArchiveFile::StringTree	const *mStringTreePlayer;	// size 0x04, offset 0x38
		SoundArchiveFile::StringTree	const *mStringTreeGroup;	// size 0x04, offset 0x3c
		SoundArchiveFile::StringTree	const *mStringTreeBank;		// size 0x04, offset 0x40
	}; // size 0x44
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SOUND_ARCHIVE_FILE_H

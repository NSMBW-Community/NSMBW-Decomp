#ifndef NW4R_SND_WAVE_FILE_H
#define NW4R_SND_WAVE_FILE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "nw4r/snd/snd_Channel.h"
#include "nw4r/snd/snd_global.h" // SampleFormat
#include "nw4r/snd/snd_adpcm.h"
#include "nw4r/types_nw4r.h"

#include "nw4r/ut/ut_binaryFileFormat.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail
{
	struct WaveFile
	{
		/* Header */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b2ab1
		struct FileHeader
		{
			ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
			ulong						infoChunkOffset;	// size 0x04, offset 0x10
			ulong						infoChunkSize;		// size 0x04, offset 0x14
			ulong						dataChunkOffset;	// size 0x04, offset 0x18
			ulong						dataChunkSize;		// size 0x04, offset 0x1c
		}; // size 0x20

		/* InfoBlock */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b2185
		struct WaveInfo
		{
			u8		format;					// size 0x01, offset 0x00
			u8		loopFlag;				// size 0x01, offset 0x01
			u8		numChannels;			// size 0x01, offset 0x02
			u8		sampleRate24;			// size 0x01, offset 0x03
			u16		sampleRate;				// size 0x02, offset 0x04
			u8		dataLocationType;		// size 0x01, offset 0x06
			u8	padding;
			ulong		loopStart;				// size 0x04, offset 0x08
			ulong		loopEnd;				// size 0x04, offset 0x0c
			ulong		channelInfoTableOffset;	// size 0x04, offset 0x10
			ulong		dataLocation;			// size 0x04, offset 0x14
			ulong	reserved;
		}; // size 0x1c

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b2c9d
		struct WaveChannelInfo
		{
			ulong	channelDataOffset;	// size 0x04, offset 0x00
			ulong	adpcmOffset;		// size 0x04, offset 0x04
			ulong	volumeFrontLeft;	// size 0x04, offset 0x08
			ulong	volumeFrontRight;	// size 0x04, offset 0x0c
			ulong	volumeRearLeft;		// size 0x04, offset 0x10
			ulong	volumeRearRight;	// size 0x04, offset 0x14
			ulong	reserved;
		}; // size 0x1c

		static ulong const SIGNATURE_INFO_BLOCK =
			NW4R_FOUR_BYTE('I', 'N', 'F', 'O');

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x3087c5
		struct InfoBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			WaveInfo				waveInfo;		// size 0x1c, offset 0x08
		}; // size 0x24

		/* Other */

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x308a10
		struct AdpcmParamSet
		{
			AdpcmParam		adpcmParam;		// size 0x28, offset 0x00
			AdpcmLoopParam	adpcmLoopParam;	// size 0x06, offset 0x28
		}; // size 0x2e

		/* WaveFile */

		static ulong const SIGNATURE_FILE =
			NW4R_FOUR_BYTE('R', 'W', 'A', 'V');
		static int const FILE_VERSION; // does it even have one?
	}; // "namespace" WaveFile

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x29336
	struct ChannelParam
	{
		void			*dataAddr;			// size 0x04, offset 0x00
		AdpcmParam		adpcmParam;			// size 0x28, offset 0x04
		AdpcmLoopParam	adpcmLoopParam;		// size 0x06, offset 0x2c
		/* 2 bytes padding */
	}; // size 0x34

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x29401
	struct WaveInfo
	{
		SampleFormat	sampleFormat;						// size 0x04, offset 0x00
		bool			loopFlag;							// size 0x01, offset 0x04
		/* 3 bytes padding */
		int				numChannels;						// size 0x04, offset 0x08
		int				sampleRate;							// size 0x04, offset 0x0c
		ulong				loopStart;							// size 0x04, offset 0x10
		ulong				loopEnd;							// size 0x04, offset 0x14
		ChannelParam	channelParam[Channel::CHANNEL_MAX];	// size 0x68, offset 0x18
	}; // size 0x80

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2e58c
	struct WaveSoundInfo
	{
		f32	pitch;			// size 0x04, offset 0x00
		u8	pan;			// size 0x01, offset 0x04
		u8	surroundPan;	// size 0x01, offset 0x05
		u8	fxSendA;		// size 0x01, offset 0x06
		u8	fxSendB;		// size 0x01, offset 0x07
		u8	fxSendC;		// size 0x01, offset 0x08
		u8	mainSend;		// size 0x01, offset 0x09
		/* 2 bytes padding */
	}; // size 0x0c

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2e6c2
	struct WaveSoundNoteInfo
	{
		s32	waveIndex;		// size 0x04, offset 0x00
		u8	attack;			// size 0x01, offset 0x04
		u8	hold;			// size 0x01, offset 0x05
		u8	decay;			// size 0x01, offset 0x06
		u8	sustain;		// size 0x01, offset 0x07
		u8	release;		// size 0x01, offset 0x08
		u8	originalKey;	// size 0x01, offset 0x09
		u8	pan;			// size 0x01, offset 0x0a
		u8	surroundPan;	// size 0x01, offset 0x0b
		u8	volume;			// size 0x01, offset 0x0c
		/* 3 bytes padding */
		f32	pitch;			// size 0x04, offset 0x10
	}; // size 0x14
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b2e05
	class WaveFileReader
	{
	// methods
	public:
		// cdtors
		WaveFileReader(WaveFile::FileHeader const *waveFileHeader);
		WaveFileReader(WaveFile::WaveInfo const *waveInfo);

		// methods
		bool ReadWaveInfo(WaveInfo *waveData) const
		{
			return ReadWaveInfo(waveData, nullptr);
		}

		bool ReadWaveInfo(WaveInfo *waveData,
		                  void const *waveDataOffsetOrigin) const;

		void const *GetWaveDataAddress(
			WaveFile::WaveChannelInfo const *waveChannelInfo,
			void const *waveDataOffsetOrigin) const;

		static SampleFormat GetSampleFormatFromWaveFileFormat(u8 format);

	// members
	private:
		WaveFile::WaveInfo	const *mWaveInfo;	// size 0x04, offset 0x00
	}; // size 0x04

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2954b
	class WaveDataLocationCallback
	{
	// methods
	public:
		// virtual function ordering
		// vtable WaveDataLocationCallback
		virtual WaveInfo *at_0x08();
		virtual void at_0x0c(WaveInfo const *);

		// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_WAVE_FILE_H

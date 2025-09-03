#ifndef NW4R_SND_MCS_SOUND_ARCHIVE_H
#define NW4R_SND_MCS_SOUND_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "SoundArchive.h"

#include "nw4r/ut/ut_FileStream.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class McsSoundArchive : public SoundArchive
	{
		class McsFileStream;
		McsSoundArchive();
	};

	class McsSoundArchive::McsFileStream : public ut::FileStream
	{
		McsFileStream();
		virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const { return 0; }
		virtual void Close() {}
		virtual s32 Read(void *, ulong) { return 0; }
		virtual bool IsBusy() const { return 0; }
		virtual bool CanAsync() const { return 0; }
		virtual bool CanRead() const { return 0; }
		virtual bool CanWrite() const { return 0; }
		virtual ulong GetSize() const { return 0; }
		virtual void Seek(s32, ulong) {}
		virtual void Cancel() {}
		virtual bool CancelAsync(StreamCallback *, void *) { return 0; }
		virtual bool CanSeek() const { return 0; }
		virtual bool CanCancel() const { return 0; }
		virtual ulong Tell() const { return 0; }
		friend class McsSoundArchive;
	};
}} // namespace nw4r::snd

#endif // NW4R_SND_MCS_SOUND_ARCHIVE_H

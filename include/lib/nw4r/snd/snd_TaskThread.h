#ifndef NW4R_SND_TASK_THREAD_H
#define NW4R_SND_TASK_THREAD_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include <revolution/OS/OSThread.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x4b348
	class TaskThread
	{
	// methods
	public:
		// cdtors
		TaskThread();
		~TaskThread();

		// methods
		bool Create(s32 priority, void *stack, ulong stackSize);
		void Destroy();

	private:
		// fibers, callbacks, and procedures
		static void *ThreadFunc(void *arg);
		void ThreadProc();

	// members
	private:
		OSThread	mThread;		// size 0x318, offset 0x000
		ulong		*mStackEnd;		// size 0x004, offset 0x318
		bool		mFinishFlag;	// size 0x001, offset 0x31c // TODO: volatile? (see ThreadProc)
		bool		mCreateFlag;	// size 0x001, offset 0x31d
		/* 2 bytes padding */
	}; // size 0x320
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_TASK_THREAD_H

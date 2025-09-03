#ifndef NW4R_SND_DISPOSE_CALLBACK_MANAGER_H
#define NW4R_SND_DISPOSE_CALLBACK_MANAGER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "nw4r/snd/snd_DisposeCallback.h"

#include "nw4r/ut/ut_LinkList.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2bb987
	class DisposeCallbackManager
	{
	// methods
	public:
		// instance accessors
		static DisposeCallbackManager &GetInstance();

		// methods
		void RegisterDisposeCallback(DisposeCallback *callback);
		void UnregisterDisposeCallback(DisposeCallback *callback);

		void Dispose(void *mem, ulong size, void *arg);
		void DisposeWave(void *mem, ulong size, void *arg);

	private:
		// cdtors
		DisposeCallbackManager();

	// members
	private:
		DisposeCallback::LinkList	mCallbackList;	// size 0x0c, offset 0x00
	}; // size 0x0c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_DISPOSE_CALLBACK_MANAGER_H

#ifndef NW4R_SND_DISPOSE_CALLBACK_H
#define NW4R_SND_DISPOSE_CALLBACK_H
#include "nw4r/types_nw4r.h"
#include "nw4r/ut.h"


namespace nw4r {
namespace snd {
namespace detail {

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x26773
	class DisposeCallback
	{
	// typedefs
	public:
		typedef ut::LinkList<DisposeCallback, 0x00> LinkList;

	// methods
	public:
		// cdtors
		/* virtual ~DisposeCallback() {} */ // virtual function ordering

	// members
	public:
		ut::LinkListNode	mDisposeLink;	// size 0x08, offset 0x00
		/* vtable */						// size 0x04, offset 0x08

	// late virtual methods
	public:
		// virtual function ordering
		// vtable DisposeCallback
		virtual ~DisposeCallback() {}
		virtual void InvalidateData(void const *pStart, void const *pEnd) = 0;
		virtual void InvalidateWaveData(void const *pStart,
		                                void const *pEnd) = 0;
	}; // size 0x0c

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif

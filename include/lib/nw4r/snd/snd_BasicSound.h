#ifndef NW4R_SND_BASIC_SOUND_H
#define NW4R_SND_BASIC_SOUND_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "nw4r/snd/snd_global.h"
#include "nw4r/snd/snd_MoveValue.h"

#include "nw4r/ut/ut_algorithm.h" // ut::Clamp
#include "nw4r/ut/ut_LinkList.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class BasicPlayer; }}}
namespace nw4r { namespace snd { namespace detail { class ExternalSoundPlayer; }}}
namespace nw4r { namespace snd { namespace detail { class PlayerHeap; }}}
namespace nw4r { namespace snd { class SoundActor; }}
namespace nw4r { namespace snd { class SoundHandle; }}
namespace nw4r { namespace snd { class SoundPlayer; }}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}

namespace nw4r { namespace snd
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x27a05
	struct SoundParam
	{
	// methods
	public:
		// cdtors
		SoundParam() :
			volume				(1.0f),
			pitch				(1.0f),
			pan					(0.0f),
			surroundPan			(0.0f),
			fxSend				(0.0f),
			lpf					(0.0f),
			biquadFilterValue	(0.0f),
			biquadFilterType	(0),
			priority			(0),
			field_0x24			(0)
		{
		}

	// members
	public:
		f32	volume;				// size 0x04, offset 0x00
		f32	pitch;				// size 0x04, offset 0x04
		f32	pan;				// size 0x04, offset 0x08
		f32	surroundPan;		// size 0x04, offset 0x0c
		f32	fxSend;				// size 0x04, offset 0x10
		f32	lpf;				// size 0x04, offset 0x14
		f32	biquadFilterValue;	// size 0x04, offset 0x18
		int	biquadFilterType;	// size 0x04, offset 0x1c
		int	priority;			// size 0x04, offset 0x20
		UNKWORD field_0x24;
	}; // size 0x24

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x27514
	struct SoundAmbientParam
	{
	// methods
	public:
		// cdtors
		SoundAmbientParam() : // where is my nsdmi !!!
			volume				(1.0f),
			pitch				(1.0f),
			pan					(0.0f),
			surroundPan			(0.0f),
			fxSend				(0.0f),
			lpf					(0.0f),
			biquadFilterValue	(0.0f),
			biquadFilterType	(0),
			priority			(0),
			field_0x24			(0)
		{
		}

	// members
	public:
		f32				volume;				// size 0x04, offset 0x00
		f32				pitch;				// size 0x04, offset 0x04
		f32				pan;				// size 0x04, offset 0x08
		f32				surroundPan;		// size 0x04, offset 0x0c
		f32				fxSend;				// size 0x04, offset 0x10
		f32				lpf;				// size 0x04, offset 0x14
		f32				biquadFilterValue;	// size 0x04, offset 0x18
		int				biquadFilterType;	// size 0x04, offset 0x1c
		int				priority;			// size 0x04, offset 0x20
		UNKWORD         field_0x24;
		VoiceOutParam	voiceOutParam[4];	// size 0x60, offset 0x24
	}; // size 0x84

	namespace detail
	{
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x270f1
		struct SoundActorParam
		{
		// methods
		public:
			// cdtors
			SoundActorParam() :
				volume	(1.0f),
				pitch	(1.0f),
				pan		(0.0f)
			{
			}

		// members
		public:
			f32	volume;	// size 0x04, offset 0x00
			f32	pitch;	// size 0x04, offset 0x04
			f32	pan;	// size 0x04, offset 0x08
		}; // size 0x0c
	} // namespace detail
}} // namespace nw4r::snd

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x28177
	// NOTE: different from ketteiban: no remote fields. something else instead
	class BasicSound
	{
	// enums
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x27c51
		enum PauseState
		{
			PAUSE_STATE_NORMAL,
			PAUSE_STATE_PAUSING,
			PAUSE_STATE_PAUSED,
			PAUSE_STATE_UNPAUSING,
		};

	// typedefs
	public:
		typedef ut::LinkList<BasicSound, 0xf0> PriorityLinkList;
		typedef ut::LinkList<BasicSound, 0xf8> SoundPlayerPlayLinkList;
		typedef ut::LinkList<BasicSound, 0x100> SoundPlayerPriorityLinkList;
		typedef ut::LinkList<BasicSound, 0x108> ExtSoundPlayerPlayLinkList;

	// nested types
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x277e5
		struct AmbientInfo
		{
		// nested types
		public:
			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x276d8
			class AmbientParamUpdateCallback
			{
			public:
				enum ParamUpdateFlags {
					PARAM_UPDATE_VOLUME = (1 << 0),
					PARAM_UPDATE_PAN = (1 << 1),
					PARAM_UPDATE_SURROUND_PAN = (1 << 2),
					PARAM_UPDATE_PRIORITY = (1 << 3),
				};
			// methods
			public:
				virtual ~AmbientParamUpdateCallback() {}
				virtual void detail_UpdateAmbientParam(const void*, ulong, int, SoundAmbientParam*);
				virtual int detail_GetAmbientPriority(const void*, ulong);
				virtual int detail_GetRequiredVoiceOutCount(const void*, ulong);

			// members
			private:
				/* vtable */	// size 0x04, offset 0x00
			}; // size 0x04

			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x27732
			class AmbientArgUpdateCallback
			{
			// methods
			public:
				// virtual function ordering
				// vtable AmbientArgUpdateCallback
				virtual ~AmbientArgUpdateCallback() {}
				virtual void at_0x0c(void *, BasicSound *);

			// members
			private:
				/* vtable */	// size 0x04, offset 0x00
			}; // size 0x04

			// NOTE: "Allocator" is misspelled as "Allocater"
			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2778a
			class AmbientArgAllocaterCallback
			{
			// methods
			public:
				// virtual function ordering
				// vtable AmbientArgAllocaterCallback
				virtual ~AmbientArgAllocaterCallback() {}
				virtual void *detail_AllocAmbientArg(ulong size); // at 0x8

    			virtual void detail_FreeAmbientArg(void *pArg,
                                       const detail::BasicSound *pSound); // at 0xC

			// members
			private:
				/* vtable */	// size 0x04, offset 0x00
			}; // size 0x04

		// members
		public:
			AmbientParamUpdateCallback	*paramUpdateCallback;	// size 0x04, offset 0x00
			AmbientArgUpdateCallback	*argUpdateCallback;		// size 0x04, offset 0x04
			AmbientArgAllocaterCallback	*argAllocaterCallback;	// size 0x04, offset 0x08
			void						*arg;					// size 0x04, offset 0x0c
			ulong							argSize;				// size 0x04, offset 0x10
		}; // size 0x14

	// methods
	public:
		// cdtors
		BasicSound(int priority, int ambientPriority);
		/* virtual ~BasicSound() {} */ // virtual function ordering

		// virtual function ordering
		// vtable BasicSound
		virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}
		virtual ~BasicSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const = 0;
		virtual bool IsAttachedTempSpecialHandle() = 0;
		virtual void DetachTempSpecialHandle() = 0;
		virtual void InitParam();
		virtual BasicPlayer &GetBasicPlayer() = 0;
		virtual BasicPlayer const &GetBasicPlayer() const = 0;
		virtual void OnUpdatePlayerPriority() {}
		virtual void UpdateMoveValue();
		virtual void UpdateParam();

		// methods
		void StartPrepared();
		void Update();
		void Pause(bool flag, int fadeFrames);
		bool IsPause() const;
		void Stop(int fadeFrames);
		void SetAutoStopCounter(int count);
		void FadeIn(int fadeFrames);

		bool GetStartedFlag() const { return mStartedFlag; }
		ulong GetId() const { return mId; }
		PlayerHeap *GetPlayerHeap() { return mPlayerHeap; }
		SoundPlayer *GetSoundPlayer() { return mSoundPlayer; }
		int GetVoiceOutCount() const;
		int GetPriority() const { return mPriority; }

		void SetPlayerPriority(int priority);
		void SetInitialVolume(f32 volume);
		void SetVolume(f32 volume, int frames);
		void SetPitch(f32 pitch);
		void SetPan(f32 pan);
		void SetSurroundPan(f32 pan);
		void SetLpfFreq(f32 freq);
		void SetFxSend(AuxBus bus, f32 send);
		void SetRemoteFilter(int filter);
		void SetPanMode(PanMode mode);
		void SetPanCurve(PanCurve curve);
		void SetAmbientInfo(AmbientInfo const &ambientArgInfo);
		void SetId(ulong id);
		void SetOutputLineFlag(int flag);

		void AttachPlayerHeap(PlayerHeap *heap);
		void AttachSoundPlayer(SoundPlayer *player);
		void AttachSoundActor(SoundActor *actor);
		void AttachExternalSoundPlayer(ExternalSoundPlayer *extPlayer);

		bool IsAttachedGeneralHandle();
		bool IsAttachedTempGeneralHandle();

		void DetachPlayerHeap(PlayerHeap *heap);
		void DetachSoundPlayer(SoundPlayer *player);
		void DetachSoundActor(SoundActor *actor);
		void DetachExternalSoundPlayer(ExternalSoundPlayer *extPlayer);
		void DetachGeneralHandle();
		void DetachTempGeneralHandle();

		int GetRemainingFadeFrames() const;

		int CalcCurrentPlayerPriority() const
		{
			return ut::Clamp(mPriority + mAmbientParam.priority, PRIORITY_MIN,
		                     PRIORITY_MAX);
		}

		void ClearAmbientArgUpdateCallback() {
			mAmbientInfo.argUpdateCallback = NULL;
		}

		static int GetAmbientPriority(AmbientInfo const &ambientInfo,
		                              ulong soundId);

	// static members
	public:
		// NOTE: PRIORITY_MAX is a dependent name (see SoundInstanceManager)
		static ulong const INVALID_ID = -1;
		static int const PRIORITY_MAX = 127;
		static int const PRIORITY_MIN = 0;

		static ut::detail::RuntimeTypeInfo const typeInfo;

	// members
	private:
		/* vtable */									// size 0x04, offset 0x00
		PlayerHeap			*mPlayerHeap;				// size 0x04, offset 0x04
		SoundHandle			*mGeneralHandle;			// size 0x04, offset 0x08
		SoundHandle			*mTempGeneralHandle;		// size 0x04, offset 0x0c
		SoundPlayer			*mSoundPlayer;				// size 0x04, offset 0x10
		SoundActor			*mSoundActor;				// size 0x04, offset 0x14
		ExternalSoundPlayer	*mExtSoundPlayer;			// size 0x04, offset 0x18
		AmbientInfo			mAmbientInfo;				// size 0x14, offset 0x1c
		SoundParam			mAmbientParam;				// size 0x24, offset 0x30
		SoundActorParam		mActorParam;				// size 0x0c, offset 0x58
		MoveValue<f32, int>	mFadeVolume;				// size 0x10, offset 0x64
		MoveValue<f32, int>	mPauseFadeVolume;			// size 0x10, offset 0x74
		bool				mStartFlag;					// size 0x01, offset 0x84
		bool				mStartedFlag;				// size 0x01, offset 0x85
		bool				mAutoStopFlag;				// size 0x01, offset 0x86
		bool				mFadeOutFlag;				// size 0x01, offset 0x87
		PauseState			mPauseState;				// size 0x04, offset 0x88
		bool				mUnPauseFlag;				// size 0x01, offset 0x8c
		/* 3 bytes padding */
		s32					mAutoStopCounter;			// size 0x04, offset 0x90
		ulong					mUpdateCounter;				// size 0x04, offset 0x94
		u8					mPriority;					// size 0x01, offset 0x98
		u8					mVoiceOutCount;				// size 0x01, offset 0x99
		u8					mBiquadFilterType;			// size 0x01, offset 0x9a
		/* 1 byte padding */
		ulong					mId;						// size 0x04, offset 0x9c
		MoveValue<f32, int>	mExtMoveVolume;				// size 0x10, offset 0xa0
		f32					mInitVolume;				// size 0x04, offset 0xb0
		f32					mExtPan;					// size 0x04, offset 0xb4
		f32					mExtSurroundPan;			// size 0x04, offset 0xb8
		f32					mExtPitch;					// size 0x04, offset 0xbc
		f32					mLpfFreq;					// size 0x04, offset 0xc0
		f32					mBiquadFilterValue;			// size 0x04, offset 0xc4
		int					mOutputLineFlag;			// size 0x04, offset 0xc8
		f32					mMainOutVolume;				// size 0x04, offset 0xcc
		f32					mMainSend;					// size 0x04, offset 0xd0
		f32					mFxSend[AUX_BUS_NUM];		// size 0x0c, offset 0xd4
		f32					mRemoteOutVolume[4];		// size 0x10, offset 0xe0
	public:
		ut::LinkListNode	mPriorityLink;				// size 0x08, offset 0xf0
		ut::LinkListNode	mSoundPlayerPlayLink;		// size 0x08, offset 0xf8
		ut::LinkListNode	mSoundPlayerPriorityLink;	// size 0x08, offset 0x100
		ut::LinkListNode	mExtSoundPlayerPlayLink;	// size 0x08, offset 0x108

	// friends
	private:
		friend class snd::SoundHandle;
	}; // size 0x100
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_BASIC_SOUND_H

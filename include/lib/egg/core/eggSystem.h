#pragma once
#include <game/mLib/m_heap.hpp>
#include <lib/egg/core/eggAsyncDisplay.h>
#include <lib/egg/core/eggController.h>
#include <lib/egg/core/eggDvdFile.h>
#include <lib/egg/core/eggException.h>
#include <lib/egg/core/eggGraphicsFifo.h>
#include <lib/egg/core/eggHeap.h>
#include <lib/egg/core/eggThread.h>
#include <lib/egg/core/eggVideo.h>
#include <lib/egg/core/eggXfb.h>
#include <lib/egg/core/eggXfbManager.h>
#include <lib/egg/gfxe/eggGfxEngine.h>
#include <lib/egg/gfxe/eggScreen.h>
#include <lib/egg/gfxe/eggStateGX.h>
#include <lib/nw4r/db.h>
#include <revolution/GX.h>
#include <revolution/SC.h>
#include <revolution/WPAD.h>

namespace EGG {

class PerformanceView {};
class ProcessMeter : public Thread, public PerformanceView {};
class SceneManager;
class SimpleAudioMgr;

class ConfigurationData {
public:
    ConfigurationData() : mSysHeapSize(0x87400), mGraphicsFifoSize(0x80000) {}

    virtual Video *getVideo() = 0;
    virtual Heap *getSystemHeap() = 0;
    virtual Display *getDisplay() = 0;
    virtual XfbManager *getXfbMgr() = 0;
    virtual PerformanceView *getPerfView() = 0;
    virtual SceneManager *getSceneMgr() = 0;
    virtual SimpleAudioMgr *getAudioMgr() = 0;
    virtual void onBeginFrame() {}
    virtual void onEndFrame() {}
    virtual void initRenderMode() {}
    virtual void initMemory();
    virtual void run();

    u8 mPad[0x14];
    Heap *mRootHeapMem1;
    Heap *mRootHeapMem2;
    Heap *mRootHeapDebug;
    Heap *mSystemHeap;
    Thread *mThread;
    u8 mPad2[0x8];
    u32 mSysHeapSize;
    u32 mGraphicsFifoSize;
    GXRenderModeObj *mRenderMode;
};

class BaseSystem {
public:
    static ConfigurationData *mConfigData;
};

template <
    class TVideo = Video,
    class TDisplay = AsyncDisplay,
    class TXfbManager = XfbManager,
    class TAudioMgr = SimpleAudioMgr,
    class TSceneManager = SceneManager,
    class TPerformanceView = ProcessMeter
>
class TSystem : public BaseSystem {
public:
    class Configuration : public ConfigurationData {
    public:
        Configuration() {
            mGraphicsFifoSize = 0x80000;
            mRenderMode = nullptr;
        }

        virtual Video *getVideo() { return mpVideo; }
        virtual Heap *getSystemHeap() { return mSystemHeap; }
        virtual Display *getDisplay() { return mpDisplay; }
        virtual XfbManager *getXfbMgr() { return mpXfbMgr; }
        virtual PerformanceView *getPerfView() { return mpPerfView; }
        virtual SceneManager *getSceneMgr() { return mpSceneMgr; }
        virtual SimpleAudioMgr *getAudioMgr() { return mpAudioMgr; }

        virtual void initialize() {
            DVDInit();
            SCInit();

            initMemory();

            Heap *currHeap = Heap::sCurrentHeap;

            GraphicsFifo::create(mGraphicsFifoSize, currHeap);
            mHeap::createAssertHeap(mRootHeapMem1);

            mpVideo = new(currHeap, 4) TVideo(mRenderMode, nullptr);
            mpXfbMgr = new(currHeap, 4) TXfbManager();
            for (int i = 0; i < 2; i++) {
                mpXfbMgr->attach(new(currHeap, 4) Xfb(mRootHeapMem2));
            }

            mpDisplay = new(currHeap, 4) TDisplay(1);

            if (Video::getTickPerVRetrace(0) != Video::getTickPerVRetrace()) {
                mpDisplay->FUN_802bb750();
            }

            mpDisplay->setClearColor(nw4r::ut::Color::BLACK);
            mpDisplay->clearEFB();

            GfxEngine::Configuration cfg;
            u16 a[] = { cfg.m_14, cfg.m_16 };
            u16 b[] = { cfg.m_02, cfg.m_02 };
            StateGX::initialize(cfg.m_00, cfg.m_02, cfg.mColor, cfg.mPixelFmt);
            Screen::Initialize(a, b, nullptr);
            Screen::SetTVModeDefault();

            Thread::initialize();
            mThread = new(currHeap, 4) Thread(OSGetCurrentThread(), 4);

            DvdFile::initialize();

            CoreControllerMgr::sWPADWorkSize = WPADGetWorkMemorySize() + 0xe8;
            CoreControllerMgr::createInstance();

            // Button combo to trigger the exception handler
            static const u16 s_PadInfoBp[] = {
                WPAD_BUTTON_HOME,
                WPAD_BUTTON_MINUS,
                WPAD_BUTTON_PLUS,
                WPAD_BUTTON_MINUS,
                WPAD_BUTTON_PLUS,
                WPAD_BUTTON_1,
                WPAD_BUTTON_2,
                WPAD_BUTTON_1,
                WPAD_BUTTON_2,
                WPAD_BUTTON_A,
                0
            };
            Exception::create(0x40, 100, 1, currHeap, 1);
            Exception::setPadInfo(s_PadInfoBp);

            nw4r::db::Exception_SetDisplayInfo(63);
            mRootHeapMem1->becomeCurrentHeap();
        }

        TAudioMgr *mpAudioMgr;
        TVideo *mpVideo;
        TXfbManager *mpXfbMgr;
        TDisplay *mpDisplay;
        TPerformanceView *mpPerfView;
        TSceneManager *mpSceneMgr;
    };

    static Configuration *sConfiguration;
};

} // namespace EGG
#include <cstddef>
#include <game/bases/d_system.hpp>
#include <game/bases/d_2d.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_dvd.hpp>
#include <game/bases/d_dvd_err.hpp>
#include <game/bases/d_dylink.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_fader.hpp>
#include <game/bases/d_font_manager.hpp>
#include <game/bases/d_game_key.hpp>
#include <game/bases/d_graph.hpp>
#include <game/bases/d_hbm.hpp>
#include <game/bases/d_nand_thread.hpp>
#include <game/bases/d_message.hpp>
#include <game/bases/d_pad.hpp>
#include <game/bases/d_remocon_manager.hpp>
#include <game/bases/d_reset.hpp>
#include <game/bases/d_rom_font_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_save_mng.hpp>
#include <game/cLib/c_counter.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_allocator.hpp>
#include <game/mLib/m_heap.hpp>
#include <game/mLib/m_pad.hpp>
#include <game/mLib/m_video.hpp>
#include <game/sLib/s_Phase.hpp>
#include <lib/egg/core/eggSystem.h>
#include <lib/egg/core/eggTextureBuffer.h>
#include <lib/revolution/KPAD.h>
#include <lib/revolution/VI.h>
#include <constants/sjis_constants.h>

const char dSystem::sc_EffectManagerHeap1Name[] = EFFECT_HEAP_1_NAME;
const char dSystem::sc_EffectManagerHeap2Name[] = EFFECT_HEAP_2_NAME;

EGG::Heap *dSystem::s_EffectManagerHeap1;
EGG::Heap *dSystem::s_EffectManagerHeap2;

OSThread *s_MainThread;

template<> EGG::TSystem<>::Configuration *EGG::TSystem<>::sConfiguration = nullptr;

static EGG::TSystem<>::Configuration s_configuration;

EGG::Heap *dSys_c::ms_RootHeapMem1;
EGG::Heap *dSys_c::ms_RootHeapMem2;

const char dSystem::sc_FontManagerHeapName[] = FONT_MANAGER_HEAP_NAME;
EGG::Heap *dSystem::s_FontManagerHeap;

void dSystem::createEffectManagerHeap(EGG::Heap *heap1, EGG::Heap *heap2) {
    s_EffectManagerHeap1 = mHeap::createFrmHeap(0x100000, heap1, sc_EffectManagerHeap1Name, 0x20, mHeap::OPT_NONE);
    s_EffectManagerHeap2 = mHeap::createFrmHeap(0x280000, heap2, sc_EffectManagerHeap2Name, 0x20, mHeap::OPT_THREAD_SAFE);
}

void dSys_c::beginRender() {
    EGG::TSystem<>::sConfiguration->mpDisplay->beginRender();
}

class dummy_unused_static_local {
public:
    dummy_unused_static_local() {}

    int x[4];
};

void dSys_c::endRender() {
    // [@LOCAL@ gets deadstripped, but the guard stays]
    static const dummy_unused_static_local a;
    static const dummy_unused_static_local b;
    static const dummy_unused_static_local c;
    EGG::TSystem<>::sConfiguration->mpDisplay->endRender();
}

void dSys_c::beginFrame() {
    EGG::TSystem<>::sConfiguration->mpDisplay->beginFrame();
    EGG::TSystem<>::sConfiguration->onBeginFrame();
}

void dSys_c::endFrame() {
    EGG::TSystem<>::sConfiguration->mpDisplay->endFrame();
    EGG::TSystem<>::sConfiguration->onEndFrame();
}

bool dSys_c::setBlack(bool makeBlack) {
    EGG::Display *display = EGG::TSystem<>::sConfiguration->mpDisplay;
    EGG::Video *video = EGG::BaseSystem::mConfigData->getVideo();
    if (makeBlack != video->isBlack() && (display->mScreenStateFlag & 1) == 0) {
        display->mScreenStateFlag |= 1;
        return true;
    }
    return false;
}

void dSys_c::calcAudio() {
    dAudio::execute();
}

void dSys_c::setFrameRate(u8 rate) {
    if (EGG::TSystem<>::sConfiguration->mpDisplay != nullptr) {
        EGG::TSystem<>::sConfiguration->mpDisplay->mFrameRate = rate;
    }
}

void dSys_c::setClearColor(nw4r::ut::Color color) {
    if (EGG::TSystem<>::sConfiguration->mpDisplay != nullptr) {
        EGG::TSystem<>::sConfiguration->mpDisplay->setClearColor(color);
    }
}

nw4r::ut::Color dSys_c::getClearColor() {
    if (EGG::TSystem<>::sConfiguration->mpDisplay != nullptr) {
        return EGG::TSystem<>::sConfiguration->mpDisplay->getClearColor();
    }
    return nw4r::ut::Color::BLACK;
}

void dSys_c::create() {
    s_MainThread = OSGetCurrentThread();

    EGG::TSystem<>::Configuration *config = &s_configuration;
    EGG::TSystem<>::sConfiguration = config;
    EGG::BaseSystem::mConfigData = config;
    config->initialize();

    ms_RootHeapMem1 = mHeap::createExpHeap(-1, EGG::BaseSystem::mConfigData->mRootHeapMem1, "dSys_c::RootHeapMEM1", 0x20, mHeap::OPT_THREAD_SAFE);
    ms_RootHeapMem2 = mHeap::createExpHeap(-1, EGG::BaseSystem::mConfigData->mRootHeapMem2, "dSys_c::RootHeapMEM2", 0x20, mHeap::OPT_THREAD_SAFE);
    EGG::Heap *rootHeap1 = ms_RootHeapMem1;
    EGG::Heap *rootHeap2 = ms_RootHeapMem2;

    mVideo::create();

    mHeap::createDylinkHeap(0x500000, ms_RootHeapMem1);
    mHeap::createGameHeap1(0x980000, ms_RootHeapMem1);
    mHeap::createGameHeap2(0x700000, ms_RootHeapMem2);
    mHeap::createArchiveHeap(0x1200000, ms_RootHeapMem2);
    mHeap::createCommandHeap(0x1000, ms_RootHeapMem2);
    mHeap::setCurrentHeap(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    mAllocator_c::init(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);

    EGG::Heap *prevHeap = mHeap::setCurrentHeap(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    EGG::TextureBuffer::initialize(0x2a0000, ms_RootHeapMem2);
    mHeap::setCurrentHeap(prevHeap);

    mPad::create();
    dGameKey_c::createInstance(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    dRemoconMng_c::create(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    for (int i = 0; i < 4; i++) {
        KPADDisableDPD(i);
    }
    mPad::beginPad();
    mPad::g_currentCore->setPosParam(0.1f, 1.0f);
    mPad::endPad();

    dGraph_c::create(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], ms_RootHeapMem1, ms_RootHeapMem2);

    dFader_c::createFader(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    dFader_c::setFader(dFader_c::FADE);

    if (dSystem::s_FontManagerHeap == nullptr) {
        dSystem::s_FontManagerHeap = mHeap::createFrmHeap(0x400000, ms_RootHeapMem2, dSystem::sc_FontManagerHeapName, 0x20, mHeap::OPT_THREAD_SAFE);
    }

    dRomFontMgr_c::createInstance(dSystem::s_FontManagerHeap);

    dDvdErr_c::createInstance(mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM1]);

    cCounter_c::clear();

    OSGetCurrentThread();
    dDvd::create(OSGetThreadPriority() - 1, mHeap::g_commandHeap, mHeap::g_archiveHeap);

    dSaveMng_c::create(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    dNandThread_c::create(mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT]);
    dReset::Manage_c::CreateInstance(mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM1]);
    dHbm::Manage_c::CreateInstance(mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM1]);
    dAudio::init(ms_RootHeapMem2);
    dSystem::createEffectManagerHeap(ms_RootHeapMem1, ms_RootHeapMem2);
    dScStage_c::createReplayDataHeap(ms_RootHeapMem2, 0x1c2000, mHeap::OPT_THREAD_SAFE);

    rootHeap1->mFlag |= 1;
    rootHeap2->mFlag |= 1;

    dScene_c::setStartScene();
    setFrameRate(1);
    setClearColor(255);

    d2d::init();
}

namespace {

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_0(void *) {
    dDyl::InitAsync();
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_1(void *) {
    if (dDyl::InitAsyncIsDone()) {
        return sPhase_c::OK;
    }
    return sPhase_c::WAIT;
}

sPhase_c::METHOD_RESULT_e myDylinkInitPhase_4(void *) {
    dBase_c::initLoader();
    return sPhase_c::OK;
}

sPhase_c::phaseMethod myDylinkInitPhaseMethod[] = {
    myDylinkInitPhase_0,
    myDylinkInitPhase_1,
    myDylinkInitPhase_4
};

sPhase_c myDylinkInitPhase(myDylinkInitPhaseMethod, ARRAY_SIZE(myDylinkInitPhaseMethod));

}

void dSys_c::execute() {
    bool dvdErr = dDvdErr_c::m_pInstance->isErrorOccured();
    bool inHbm = dHbm::Manage_c::GetInstance()->m_1d0 == 2;
    beginFrame();
    m3d::calcMaterial();
    dPad::beginPad_BR();
    m3d::reset();
    beginRender();

    if (!inHbm && !dvdErr) {
        dGraph_c::ms_Instance->painter();
        dFader_c::draw();
    }

    dHbm::Manage_c::GetInstance()->DrawMenu();
    dReset::Manage_c::GetInstance()->Draw();
    dDvdErr_c::m_pInstance->draw();
    dHbm::Manage_c::GetInstance()->DrawIcon();
    endRender();

    if (!inHbm) {
        m2d::reset();
        m3d::clear();
    }

    dDvdErr_c::m_pInstance->execute();
    bool dvdErrNew = dDvdErr_c::m_pInstance->isErrorOccured();
    if (dvdErrNew && !dvdErr) {
        VIEnableDimming(1);
    }

    dHbm::Manage_c::GetInstance()->Calculate();
    dReset::Manage_c::GetInstance()->Calculate();
    inHbm = dHbm::Manage_c::GetInstance()->m_1d0 == 2;

    dRemoconMng_c::execute();
    dScStage_c::play();
    dGameKey_c::m_instance->read();
    dAudio::updateBgmInfo();

    if (!inHbm && !dvdErrNew) {
        if (myDylinkInitPhase.callMethod(nullptr) == sPhase_c::DONE) {
            dScene_c::createNextScene();
            fManager_c::mainLoop();
            cCounter_c::m_exeFrame++;
        }
        dFader_c::calc();
    }

    calcAudio();
    dPad::endPad_BR();
    endFrame();
    cCounter_c::m_gameFrame++;
}

mDvd_toMainRam_c *dSystem::l_breffCommand;
mDvd_toMainRam_c *dSystem::l_breftCommand;

sPhase_c::METHOD_RESULT_e dSystem::createEffectManagerPhase1(void *) {
    if (l_breffCommand == nullptr) {
        l_breffCommand = mDvd_toMainRam_c::create("/Effect/effect_wnmario.breff", 0, s_EffectManagerHeap2);
        if (l_breffCommand == nullptr) {
            return sPhase_c::WAIT;
        }
    }
    if (l_breftCommand == nullptr) {
        l_breftCommand = mDvd_toMainRam_c::create("/Effect/effect_wnmario.breft", 0, s_EffectManagerHeap2);
        if (l_breftCommand == nullptr) {
            return sPhase_c::WAIT;
        }
    }
    if (!l_breffCommand->isDone() || !l_breftCommand->isDone()) {
        return sPhase_c::WAIT;
    }
    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e dSystem::createEffectManagerPhase2(void *) {
    EffectManager_c::create(s_EffectManagerHeap1, s_EffectManagerHeap2);
    s_EffectManagerHeap2->dump();
    EffectManager_c::setResource(l_breffCommand->mpData, l_breftCommand->mpData);

    l_breffCommand->destroy();
    l_breftCommand->destroy();

    l_breffCommand = nullptr;
    l_breftCommand = nullptr;

    return sPhase_c::OK;
}

sPhase_c::METHOD_RESULT_e dSystem::createFontManagerPhase(void *) {
    if (!dFontMng_c::create(s_FontManagerHeap)) {
        return sPhase_c::WAIT;
    }

    s_FontManagerHeap->dump();
    return sPhase_c::OK;
}

const char dSystem::sc_MessageManagerHeapName[] = MESSAGE_MANAGER_HEAP_NAME;
EGG::Heap *dSystem::s_MessageManagerHeap;

sPhase_c::METHOD_RESULT_e dSystem::createMessageManagerPhase(void *) {
    if (s_MessageManagerHeap == nullptr) {
        s_MessageManagerHeap = mHeap::createFrmHeap(0xc800, mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM2], dSystem::sc_MessageManagerHeapName, 0x20, mHeap::OPT_THREAD_SAFE);
    }

    if (!dMessage_c::create(s_MessageManagerHeap)) {
        return sPhase_c::WAIT;
    }

    s_MessageManagerHeap->dump();
    s_MessageManagerHeap->adjust();
    return sPhase_c::OK;
}

void *dSystem::s_OrgMEM1ArenaLo;
void *dSystem::s_NewMEM1ArenaLo;
void *dSystem::s_OrgMEM1ArenaHi;
void *dSystem::s_NewMEM1ArenaHi;

void dSystem::fixArena() {
    s_OrgMEM1ArenaLo = OSGetMEM1ArenaLo();
    s_NewMEM1ArenaLo = (void *) 0x806e0000;
    if (s_OrgMEM1ArenaLo < s_NewMEM1ArenaLo) {
        OSSetMEM1ArenaLo(s_NewMEM1ArenaLo);
    }

    s_OrgMEM1ArenaHi = OSGetMEM1ArenaHi();
    s_NewMEM1ArenaHi = (void *) 0x817f0000;
    if (s_OrgMEM1ArenaHi > s_NewMEM1ArenaHi) {
        OSSetMEM1ArenaHi(s_NewMEM1ArenaHi);
    }
}

void dSystem::fixHeapsSub(EGG::ExpHeap *heap, int size) {
    heap->setGroupID(255);

    void *block = heap->alloc(size, -0x20);
    if (block == nullptr) {
        while (true);
    }

    while (true) {
        size_t adjust = heap->getAllocatableSize(4);
        if (adjust == 0) {
            break;
        }
        heap->alloc(adjust, 4);
    }

    heap->free(block);
    heap->setGroupID(1);
}

void dSystem::fixHeaps() {
    EGG::ExpHeap *gameHeap1 = mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM1];
    EGG::ExpHeap *gameHeap2 = mHeap::g_gameHeaps[mHeap::GAME_HEAP_MEM2];
    EGG::ExpHeap *archiveHeap = mHeap::g_archiveHeap;
    fixHeapsSub(gameHeap1, 0x680000);
    fixHeapsSub(gameHeap2, 0x500000);
    fixHeapsSub(archiveHeap, 0xa00000);
}

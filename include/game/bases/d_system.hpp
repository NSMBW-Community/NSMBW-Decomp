#pragma once

#include <game/sLib/s_Phase.hpp>
#include <game/mLib/m_dvd.hpp>
#include <lib/egg/core/eggHeap.h>
#include <lib/egg/core/eggExpHeap.h>
#include <nw4r/ut.h>

namespace dSys_c {
    void beginRender();
    void endRender();
    void beginFrame();
    void endFrame();
    bool setBlack(bool makeBlack);
    void calcAudio();
    void setFrameRate(u8 rate);
    void setClearColor(nw4r::ut::Color color);
    nw4r::ut::Color getClearColor();
    void create();
    void execute();

    extern EGG::Heap *ms_RootHeapMem1;
    extern EGG::Heap *ms_RootHeapMem2;
}

namespace dSystem {
    sPhase_c::METHOD_RESULT_e createFontManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createMessageManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase1(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase2(void *);
    void createEffectManagerHeap(EGG::Heap *heap1, EGG::Heap *heap2);
    void fixArena();
    void fixHeapsSub(EGG::ExpHeap *heap, int a);
    void fixHeaps();

    extern const char sc_EffectManagerHeap1Name[];
    extern const char sc_EffectManagerHeap2Name[];
    extern const char sc_FontManagerHeapName[];
    extern const char sc_MessageManagerHeapName[];

    extern mDvd_toMainRam_c *l_breffCommand;
    extern mDvd_toMainRam_c *l_breftCommand;

    extern EGG::Heap *s_FontManagerHeap;
    extern EGG::Heap *s_EffectManagerHeap1;
    extern EGG::Heap *s_EffectManagerHeap2;
    extern EGG::Heap *s_MessageManagerHeap;

    extern void *s_OrgMEM1ArenaLo;
    extern void *s_NewMEM1ArenaLo;
    extern void *s_OrgMEM1ArenaHi;
    extern void *s_NewMEM1ArenaHi;
}

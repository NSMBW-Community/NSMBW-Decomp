#pragma once

#include <game/sLib/s_Phase.hpp>
#include <game/mLib/m_dvd.hpp>
#include <lib/egg/core/eggHeap.h>
#include <lib/egg/core/eggExpHeap.h>
#include <nw4r/ut.h>

/// @brief Main game handler class. Handles initialization and the main game loop.
/// @ingroup bases
class dSys_c {
public:
    static void beginFrame(); ///< Marks the beginning of a frame.
    static void endFrame(); ///< Marks the end of a frame.
    static void beginRender();
    static void endRender();
    static bool setBlack(bool makeBlack);
    static void calcAudio();
    static void setFrameRate(u8 rate);
    static void setClearColor(nw4r::ut::Color color);
    static nw4r::ut::Color getClearColor();
    static void create(); ///< Initializes the game.
    static void execute(); ///< Executes one frame of the game loop.

    static EGG::Heap *ms_RootHeapMem1;
    static EGG::Heap *ms_RootHeapMem2;
};

/// @brief System initialization and management functions.
/// @ingroup bases
namespace dSystem {
    sPhase_c::METHOD_RESULT_e createFontManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createMessageManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase1(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase2(void *);
    void createEffectManagerHeap(EGG::Heap *heap1, EGG::Heap *heap2);

    void fixArena();
    void fixHeapsSub(EGG::ExpHeap *heap, int size);
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

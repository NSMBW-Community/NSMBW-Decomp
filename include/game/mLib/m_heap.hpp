#pragma once
#include <lib/egg/core/eggAssertHeap.h>
#include <lib/egg/core/eggFrmHeap.h>
#include <lib/egg/core/eggExpHeap.h>
#include <lib/egg/core/eggUnitHeap.h>

/**
 * @brief Provides high-level heap management utilities built on top of the @ref EGG heap system.
 * @details The mHeap namespace wraps and extends the functionality of the EGG heap framework, providing
 * helper functions for creating and managing different heap types used by the game.
 *
 * It supports:
 * - @ref EGG::ExpHeap "Expandable heaps".
 * - @ref EGG::FrmHeap "Frame heaps".
 * - @ref EGG::UnitHeap "Unit heaps".
 *
 * It also manages various game-specific heaps:
 * - The @ref g_gameHeaps "game heaps", used for most game-related content. One game heap is created in MEM1 and one in MEM2.
 * - The @ref g_archiveHeap "archive heap", used for loading resource files.
 * - The @ref g_commandHeap "command heap", used for @ref mDvd_command_c "DVD commands".
 * - The @ref g_dylinkHeap "dylink heap", used for loading and linking the game's REL files.
 * - The @ref g_assertHeap "assert heap".
 */
namespace mHeap {
    
    /// @brief Bit flags controlling heap allocation behavior.
    /// These flags are translated into internal MEM heap flags via GetOptFlag().
    enum AllocOptBit_t {
        OPT_NONE = 0,                  ///< No special allocation options.
        OPT_CLEAR_ALLOC = BIT_FLAG(0), ///< Memory blocks are cleared upon allocation.
        OPT_DEBUG_FILL = BIT_FLAG(1),  ///< Memory blocks are filled with different values depending on the heap status.
        OPT_THREAD_SAFE = BIT_FLAG(2)  ///< Enables thread-safe memory block de/allocation.
    };

    /// @brief The identifiers for the predefined game heaps.
    /// @unofficial
    enum GAME_HEAP_e {
        GAME_HEAP_DEFAULT, ///< The default game heap (alias of MEM1 or MEM2).
        GAME_HEAP_MEM1,    ///< The game heap allocated in MEM1.
        GAME_HEAP_MEM2,    ///< The game heap allocated in MEM2.
        GAME_HEAP_COUNT,   ///< The total number of game heaps.
    };

    /// @brief Converts the allocation option bits to internal MEM heap flags.
    /// @param opt The allocation option bits.
    /// @return The corresponding MEM heap flag mask.
    u16 GetOptFlag(AllocOptBit_t opt);

    /// @brief Sets the specified heap as the current heap.
    /// @param heap The heap to become current.
    /// @return The previously current heap.
    EGG::Heap *setCurrentHeap(EGG::Heap *heap);

    /// @brief Saves the currently active heap.
    /// The saved heap can later be restored using restoreCurrentHeap().
    void saveCurrentHeap();
    void restoreCurrentHeap(); ///< Restores the previously saved heap as current.

    /// @brief Calculates the total required size for an @ref EGG::ExpHeap "expandable heap", including internal overhead.
    /// @param size The requested usable size.
    /// @param align The lignment requirement.
    /// @return The total allocation size required.
    size_t expHeapCost(size_t size, ulong align);

    /// @brief Calculates the total required size for a @ref EGG::FrmHeap "frame heap", including internal overhead.
    /// @param size The requested usable size.
    /// @param align The lignment requirement.
    /// @return The total allocation size required.
    size_t frmHeapCost(size_t size, ulong align);

    /**
    * @brief Calculates the total required size for an @ref EGG::UnitHeap "unit heap", including internal overhead.
    * @param size The requested usable size.
    * @param count The number of units.
    * @param align The lignment requirement.
    * @return The total allocation size required.
    */
    size_t untHeapCost(size_t size, ulong count, ulong align);

    /// @brief Destroys a frame heap.
    /// @param heap The frame heap to destroy, or @p nullptr .
    void destroyFrmHeap(EGG::FrmHeap *heap);

    /// @brief Adjusts a frame heap to release unused memory.
    /// @param heap The frame heap to adjust, or @p nullptr .
    /// @return The total available space in the heap, or @p 0 if the adjust operation failed.
    size_t adjustFrmHeap(EGG::FrmHeap *heap); 

    /**
     * @brief Creates an expandable heap.
     * @param size The size of the heap, or -1 to use all space available.
     * @param parent The parent heap.
     * @param name The heap name, or @p nullptr .
     * @param align The heap alignment (minimum 0x20).
     * @param opt The allocation options.
     * @return A pointer to the created heap, or @p nullptr on failure.
     */
    EGG::ExpHeap *createExpHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt);

    /**
     * @brief Creates a frame heap and sets it as current.
     * @param size The size of the heap, or -1 to use all space available.
     * @param parent The parent heap.
     * @param name The heap name, or @p nullptr .
     * @param align The heap alignment (minimum 0x20).
     * @param opt The allocation options.
     * @return A pointer to the created heap, or @p nullptr on failure.
     */
    EGG::FrmHeap *createFrmHeapToCurrent(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt);

    /**
     * @brief Creates a frame heap.
     * @param size The size of the heap, or -1 to use all space available.
     * @param parent The parent heap.
     * @param name The heap name, or @p nullptr .
     * @param align The heap alignment (minimum 0x20).
     * @param opt The allocation options.
     * @return A pointer to the created heap, or @p nullptr on failure.
     */
    EGG::FrmHeap *createFrmHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt);

    /**
     * @brief Creates a unit heap.
     * @param size The size of the heap, or -1 to use all space available.
     * @param count The number of units.
     * @param parent The parent heap.
     * @param name The heap name, or @p nullptr .
     * @param align The heap alignment (minimum 0x20).
     * @param opt The allocation options.
     * @return A pointer to the created heap, or @p nullptr on failure.
     */
    EGG::UnitHeap *createUntHeap(size_t size, ulong count, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt);

    inline bool isValidGameHeapId(u32 idx) {
        return GAME_HEAP_MEM1 <= idx && idx <= GAME_HEAP_MEM2;
    }

    /**
     * @brief Creates a generic expandable heap, with @ref MEM_EXP_HEAP_ALLOC_FAST "fast allocation mode" and @ref OPT_THREAD_SAFE "thread-safe de/allocation".
     * @param size The heap size.
     * @param parent The parent heap.
     * @param name The heap name, or @p nullptr .
     * @return A pointer to the created heap, or @p nullptr on failure.
     */
    EGG::Heap *createHeap(size_t size, EGG::Heap *parent, const char *name);

    /**
    * @brief Creates a game heap.
    * @param idx The game heap index.
    * @param size The heap size.
    * @param parent The parent heap.
    * @return A pointer to the created heap, or @p nullptr if invalid index.
    */
    EGG::Heap *createGameHeap(int idx, size_t size, EGG::Heap* parent);
    EGG::Heap *createGameHeap1(size_t size, EGG::Heap *parent); ///< Creates the MEM1 game heap. See createGameHeap().
    EGG::Heap *createGameHeap2(size_t size, EGG::Heap *parent); ///< Creates the MEM2 game heap. See createGameHeap().
    EGG::Heap *createArchiveHeap(size_t size, EGG::Heap *parent); ///< Creates the archive heap. See createHeap().
    EGG::Heap *createCommandHeap(size_t size, EGG::Heap *parent); ///< Creates the DVD command heap. See createHeap().
    EGG::Heap *createDylinkHeap(size_t size, EGG::Heap *parent); ///< Creates the REL linking heap. See createHeap().

    /// @brief Creates the assert heap.
    /// @details The size is determined automatically using EGG::AssertHeap::getMinSizeForCreate().
    EGG::Heap *createAssertHeap(EGG::Heap *parent); 

    extern u8 g_DefaultGameHeapId; ///< The default game heap to be used if one isn't specified.
    extern const char * const s_GameHeapNames[GAME_HEAP_COUNT]; ///< The game heap names.

    extern EGG::Heap *s_SavedCurrentHeap; ///< The saved current heap.
    extern EGG::Heap *g_gameHeaps[GAME_HEAP_COUNT]; ///< The game heaps.
    extern EGG::Heap *g_archiveHeap; ///< The archive resource heap.
    extern EGG::Heap *g_commandHeap; ///< The DVD command heap.
    extern EGG::Heap *g_dylinkHeap; ///< The REL linking heap.
    extern EGG::Heap *g_assertHeap; ///< The assert heap.

}; // namespace mHeap

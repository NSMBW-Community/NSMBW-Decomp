#pragma once

#include <types.h>
#include <lib/egg/core/eggHeap.h>
#include <game/mLib/m_dvd.hpp>

/// @brief The game's DVD loading layer, built on top of @ref mDvd .
/// @ingroup bases
namespace dDvd {

/// @brief Loads a single file from the disc and keeps ownership of the resulting buffer.
/// @ingroup bases
class loader_c {
public:
    loader_c(); ///< Creates a new loader with no pending request.
    virtual ~loader_c(); ///< Destroys the loader.
    virtual void freeHeap(); ///< Frees the loaded buffer from the heap it was allocated on.

    /// @brief Starts (or polls) a load request for the given file.
    /// @details The first call issues the command; subsequent calls poll it and return the buffer
    /// once the transfer is done. Once the file is loaded, the buffer is returned immediately.
    /// @param name The path of the file to load.
    /// @param allocDirection The allocation direction to use for the buffer.
    /// @param heap The heap to allocate the buffer on, or @p nullptr to use the archive heap.
    /// @return The loaded buffer, or @p nullptr if the load is not done yet.
    void *request(const char *name, u8 allocDirection, EGG::Heap *heap);

    bool remove(); ///< Cancels any pending command and forgets the loaded buffer.

private:
    bool cancelCommand(); ///< Waits for the pending command to finish and frees its heap.
    bool freeResouce(); ///< Cancels any pending command and frees the loaded buffer. [sic]

    u32 mSize; ///< The size of the loaded file, or @p -1 if nothing is loaded.
    mDvd_toMainRam_c *mpCommand; ///< The pending load command.
    EGG::Heap *mpHeap; ///< The heap the buffer is allocated on.
    void *mpBuffer; ///< The loaded buffer.

    /// @brief The callback fired whenever a request completes.
    static void (*m_DoneCallback)(const char *, EGG::Heap *);
};

/// @brief Initializes the DVD subsystem.
void create(long, EGG::Heap *, EGG::Heap *);

} // namespace dDvd

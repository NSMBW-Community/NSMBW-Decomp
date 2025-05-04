#pragma once
#include <types.h>
#include <game/bases/d_res.hpp>
/// @file

#define MAX_NUM_RES 300 ///< The maximum number of resources that can be loaded at once.

/// @brief A higher-level archive resource management class.
/// @ingroup bases
class dResMng_c {
private:
    /// @brief A callback class for processing resources that handles loading g3d data.
    class resCallback_c : public dRes_c::callback_c {
    public:
        virtual void init(const char *name);
        virtual void *execute(void *data, u32 folderSig, const char* path);
    };

public:
    dResMng_c(); ///< Constructs a new dResMng_c.
    virtual ~dResMng_c(); ///< Destroys this dResMng_c.

    /**
     * @brief Schedules multiple archives for loading.
     * @param path The path to the folder containing the archives. See the path notes in dRes_c.
     * @param names An array of archive names. See the path notes in dRes_c.
     * @param count The number of archives in the array.
     * @param heap The heap to load the archives into, or @p nullptr to use the default archive heap.
     */
    void setRes(const char *path, const char **names, int count, EGG::Heap *heap);

    /**
     * @brief Schedules a single archive for loading.
     * @param path The path to the folder containing the archive. See the path notes in dRes_c.
     * @param name The name of the archive. See the path notes in dRes_c.
     * @param heap The heap to load the archive into, or @p nullptr to use the default archive heap.
     * @return Whether the archive was loaded successfully.
     */
    bool setRes(const char *path, const char *name, EGG::Heap *heap);

    dRes_c mRes; ///< The resource manager.

private:
    resCallback_c mCallback; ///< The callback for after a file has been loaded.

public:
    static dResMng_c *m_instance; ///< The instance of this class.
};

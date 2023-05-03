#pragma once
#include <types.h>
#include <dol/bases/d_res.hpp>

#define MAX_NUM_RES 300 ///< The maximum number of resources that can be loaded at once.

/// @brief A singleton class that manages loading resource files (files contained within a .arc file).
class dResMng_c {
private:
    /// @brief A callback class for processing resources that handles loading g3d data.
    class resCallback_c : public dRes_c::callback_c {
    public:
        virtual void init(const char *name);
        virtual void *execute(void *data, u32 folderSig);
    };

public:
    dResMng_c(); ///< Constructs a new dResMng_c.
    virtual ~dResMng_c(); ///< Destroys this dResMng_c.

    /**
     * @brief Registers multiple archives to be loaded next.
     * 
     * @param path The path to the folder containing the archives.
     * @param names An array of archive names.
     * @param count The number of archives in ::names.
     * @param heap The heap to load the archive into.
     */
    void setRes(const char *path, char **names, int count, EGG::Heap *heap);

    /**
     * @brief Registers a single archive to be loaded next.
     * 
     * @param path The path to the folder containing the archive.
     * @param name The name of the archive.
     * @param heap The heap to load the archive into.
     * @return Whether the archive was loaded successfully.
     */
    bool setRes(const char *path, const char *name, EGG::Heap *heap);

    dRes_c mRes; ///< The resource manager.

private:
    resCallback_c mCallback; ///< The callback for after a file has been loaded.

public:
    static dResMng_c *m_instance; ///< The instance of this class.
};

#pragma once
#include <types.h>
#include <dol/bases/d_res.hpp>

#define MAX_NUM_RES 300 ///< The maximum number of resources that can be loaded at once.

/// @brief A singleton class that manages loading resource files. A resource file is a .arc file.
class dResMng_c {
private:
    /// @brief A callback class for EGG::Archive::searchInside that returns the file data and handles loading g3d data.
    class resCallback_c : public dRes_c::callback_c {
        virtual void init(const char *name);
        virtual void *execute(void *data, u32 folderSig);
    };

public:
    dResMng_c(); ///< Constructs a new dResMng_c.
    virtual ~dResMng_c(); ///< Destroys this dResMng_c.

    /**
     * @brief Load multiple resources from a folder.
     * 
     * @param path The path to the folder.
     * @param names An array of resource names.
     * @param count The number of resources in ::names.
     * @param heap The heap to load the resources into.
     */
    void setRes(const char *path, char **names, int count, EGG::Heap *heap);

    /**
     * @brief Load a single resource from a folder.
     * 
     * @param path The path to the folder.
     * @param name The name of the resource.
     * @param heap The heap to load the resource into.
     * @return Whether the resource was loaded successfully.
     */
    bool setRes(const char *path, const char *name, EGG::Heap *heap);

    static dResMng_c *m_instance; ///< The singleton instance of this class.

private:
    dRes_c mRes; ///< The resource manager.
    resCallback_c mCallback; ///< The callback for after a file has been loaded.
};

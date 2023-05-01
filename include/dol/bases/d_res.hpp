#pragma once
#include <types.h>
#include <lib/rvl/arc/ARC.h>
#include <lib/egg/heap/heap.hpp>
#include <lib/egg/heap/frm_heap.hpp>
#include <lib/egg/archive.hpp>
#include <dol/mLib/m_dvd.h>

/**
 * @brief A utility class for loading resource files. A resource file is a .arc file.
 * 
 * @details 
 */
class dRes_c {
public:

    /// @brief A callback class for EGG::Archive::searchInside.
    class callback_c {
    public:
        /// @brief Initializes the callback with the file name.
        virtual void init(const char *name) = 0;

        /**
         * @brief Executes the callback.
         * 
         * @param data The file data.
         * @param folderSig The first 4 characters of the folder name.
         * @return The file data.
         */
        virtual void *execute(void *data, u32 folderSig) = 0;
    };

    /// @brief A callback class for loading resources from an archive.
    /// @note Unofficial name.
    class searchCallback_c {
    public:
        /// @brief Constructs a new searchCallback_c.
        searchCallback_c(callback_c *callback, u8 **files, int numFiles, unsigned int fileIdx, u32 folderSig) {
            mpCallback = callback;
            mpFiles = files;
            mNumFiles = numFiles;
            mFileIdx = fileIdx;
            mFolderSig = folderSig;
        }

        /// @brief The callback function for files.
        /// @note Unofficial name.
        static void callback(void *cbInfo, void *file, const ARCDirEntry *dirEntry, const char *path);

    private:
        /// @brief The internal callback function for files.
        /// @note Unofficial name.
        void callback(void *file, const ARCDirEntry *dirEntry, const char *path);

        callback_c *mpCallback; ///< The callback to be called if ::dirEntry is a file.
        u8 **mpFiles; ///< A pointer to an array of file data pointers.
        int mNumFiles; ///< The number of files in ::mpFiles.
        unsigned int mFileIdx; ///< The index of the current file in ::mpFiles.
        u32 mFolderSig; ///< The first 4 characters of the current folder.
    };

    /// @brief A class that contains information about a resource.
    class info_c {
    public:
        info_c(); ///< Constructs a new info_c.
        ~info_c(); ///< Destroys this info_c.

        /// @brief Frees up the resources used by this info_c.
        /// @return Whether the resources were cleaned up successfully.
        bool cleanup();

        /**
         * @brief Sets information about the resource to be loaded.
         * 
         * @param arcName The name of the archive.
         * @param containingFolder The path to the archive.
         * @param allocDirection The allocation direction. 1: top-down, anything else: bottom-up.
         * @param heap The heap to load the resource into.
         * @return Whether the resource was prepared successfully and will be loaded.
         */
        bool set(const char *arcName, const char *containingFolder, u8 allocDirection, EGG::Heap *heap);

        /**
         * @brief Attempts to load the resource into memory and execute the callback function.
         * 
         * @param callback The callback for this resource.
         * @return A status code for the result of the operation. -1: error, 0: success, 1: not finished loading
         */
        int setRes(callback_c *callback);

        const char *getName() const { return mName; }
        int getRefCount() const { return mRefCount; }
        void *getDvdCmd() const { return mpDvdCmd; }
        EGG::Archive *getArchive() const { return mpArchive; }

        void incRefCount() { mRefCount++; } ///< Increments the reference count.
        void decRefCount() { mRefCount--; } ///< Decrements the reference count.

    private:

        /**
         * @brief Opens the resource and executes the callback on each file and folder.
         * 
         * @details If the callback is nullptr, the files will still be loaded and pointers to each file will be stored in ::mpFiles.
         * @param callback The callback for this resource.
         * @return Whether the resource was opened successfully.
         */
        int loadRes(callback_c *callback);

        char mName[0x20]; ///< The name of the resource.
        u16 mRefCount; ///< The number of references to this resource.
        mDvd_mountMemArchive_c *mpDvdCmd; ///< The DVD command for mounting the resource.
        EGG::Archive *mpArchive; ///< The archive of the resource.
        EGG::Heap *mpMountHeap; ///< The heap used for mounting the resource.
        void *mpArcBinary; ///< The binary data of the resource.
        unsigned long mUnk; ///< Unknown.
        EGG::FrmHeap *mDataHeap; ///< The heap used for loading the resource. [No p because of the string "dRes_c::info_c::mDataHeap"]
        u8 **mpFiles; ///< A pointer to an array of file data pointers.

    };

    dRes_c(); ///< Constructs a new dRes_c.
    ~dRes_c(); ///< Destroys this dRes_c.

    /**
     * @brief Initializes the dRes_c.
     * 
     * @param maxCount Number of resource holders to allocate.
     * @param callback Callback for after a resouce has been loaded.
     * @return Whether the initialization was successful.
     */
    bool init(u16 maxCount, callback_c *callback);

    /**
     * @brief Sets an resource to be loaded next.
     * 
     * @param arcName The name of the archive to load from.
     * @param filePath The path to the archive.
     * @param allocDir The allocation direction. 1: top-down, anything else: bottom-up.
     * @param heap The heap to load the resource into.
     * @return Whether the operation was successful.
     */
    bool setRes(const char *arcName, const char *filePath, u8 allocDir, EGG::Heap *heap);

    /**
     * @brief Marks the resource as not needed anymore.
     * @param arcName The name of the archive to unload.
     * @return Whether the operation was successful.
     */
    bool deleteRes(const char *arcName);

    /**
     * @brief Loads a resource.
     * 
     * @param arcName The name of the resource to load.
     * @param filePath The path to the resource.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *filePath) const;

    /**
     * @brief Loads a resource.
     * 
     * @param arcName The name of the resource to load.
     * @param filePath The path to the resource.
     * @param size A pointer where the size of the resource will be written to.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *filePath, unsigned long *size) const;

    /**
     * @brief Loads a resource.
     * 
     * @param arcName The name of the resource to load.
     * @param filePath The path to the resource.
     * @param size A pointer where the size of the resource will be written to.
     * @param compressionType A pointer where the compression type of the resource will be written to.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *filePath, unsigned long *size, int *compressionType) const;

    /// @brief Loads a resource without logging if the resource is not found.
    /// @see ::getRes(const char*, const char*) const
    void *getResSilently(const char *arcName, const char *filePath) const;
    
    /// @brief Loads a resource without logging if the resource is not found.
    /// @see ::getRes(const char*, const char*, unsigned long*) const
    void *getResSilently(const char *arcName, const char *filePath, unsigned long *size) const;

    /// @brief Attempts to trigger the callback on a resource that has finished loading since the last call to this function.
    /// @return Whether the callback was able to be triggered.
    bool syncAllRes();
    
    /**
     * @brief Gets the info_c for a resource.
     * 
     * @param arcName The name of the resource.
     * @return The info_c for the resource.
     */
    info_c *getResInfo(const char *arcName) const;

    /**
     * @brief Gets the info_c for a resource if it has a valid archive.
     * @param arcName The name of the resource.
     * @return The info_c for the resource if it has a valid archive, otherwise nullptr.
     */
    info_c *getResInfoLoaded(const char *arcName) const;

    /// @brief Gets an info_c that can be used to store a new resource.
    /// @return The info_c for the new resource.
    info_c *newResInfo();

    /// @brief Copies an uncompressed resource.
    static void copyRes(const void *from, void *to, int size);

    /// @brief Copies a compressed resource.
    static void copyRes(const void *from, void *to, int size, int compressionLevel);


private:
    info_c *mpInfos; ///< An array of resource holders.
    u16 mNumInfos; ///< The number of info_c instances.
    callback_c *mpCallback; ///< The callback for when a resource is loaded.

    static void (*mSetCallback)(const char *arcName, EGG::Heap *heap); ///< The callback for when dRes_c::info_c::set is called.
};

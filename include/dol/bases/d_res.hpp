#pragma once
#include <types.h>
#include <lib/rvl/arc/ARC.h>
#include <lib/egg/heap/eggHeap.hpp>
#include <lib/egg/heap/eggFrmHeap.hpp>
#include <lib/egg/archive.hpp>
#include <dol/mLib/m_dvd.h>

/// @brief A utility class for loading resource files (files contained within a .arc file).
/// @details dRes_c is a holder for multiple archive (.arc) files.
class dRes_c {
public:

    /// @brief A callback class for processing resources.
    /// @details The calls to ::execute occur during the initial loading of an archive.
    class callback_c {
    public:
        /// @brief Initializes the callback with the resource name.
        /// @pure
        virtual void init(const char *name) = 0;

        /**
         * @brief Executes the callback.
         * @pure
         * @param data The resource data.
         * @param folderSig The first 4 characters of the folder name.
         * @return The resource data.
         */
        virtual void *execute(void *data, u32 folderSig) = 0;
    };

    /// @brief A callback class for EGG::Archive::searchInside.
    /// @unofficial
    class searchCallback_c {
    public:
        /// @brief Constructs a new searchCallback_c.
        searchCallback_c(callback_c *callback, u8 **files, int numFiles, int fileIdx, u32 folderSig) :
            mpCallback(callback),
            mpFiles(files),
            mNumFiles(numFiles),
            mFileIdx(fileIdx),
            mFolderSig(folderSig) {
        }

        /// @brief The callback function.
        static void callback(void *cbInfo, void *file, const ARCDirEntry *dirEntry, const char *path);

    private:
        /// @brief The internal callback function.
        void callback(void *file, const ARCDirEntry *dirEntry, const char *path);

        callback_c *mpCallback; ///< The file processing callback.
        u8 **mpFiles; ///< An array of pointers to the data of each loaded resource.
        int mNumFiles; ///< The number of files in ::mpFiles.
        unsigned int mFileIdx; ///< The index of the current file in ::mpFiles.
        u32 mFolderSig; ///< The first 4 characters of the current folder.
    };

    /// @brief A class that holds information about an archive.
    class info_c {
    public:
        info_c(); ///< Constructs a new info_c.
        ~info_c(); ///< Destroys this info_c.

        /// @brief Unmounts the archive and frees the holder for use.
        /// @return Whether the operation was successful.
        bool cleanup();

        /**
         * @brief Sets information about the archive to be loaded.
         *
         * @param arcName The name of the archive.
         * @param containingFolder The path to the folder which the archive is in.
         * @param allocDirection The allocation direction. 1: top-down, anything else: bottom-up.
         * @param heap The heap to load the resources of the archive into.
         * @return Whether the archive was prepared successfully and will be loaded.
         */
        bool set(const char *arcName, const char *containingFolder, u8 allocDirection, EGG::Heap *heap);

        /**
         * @brief Attempts to load the archive into memory and load the resources with a callback.
         *
         * @param callback The callback for the resources.
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
         * @brief Executes a callback on each file and folder of the archive.
         *
         * @details If the callback is @p nullptr , the files will still be loaded and pointers to each file will be stored in ::mpFiles.
         * @param callback The callback for this resource, or @p nullptr .
         * @return Whether the resource was opened successfully.
         */
        int loadRes(callback_c *callback);

        char mName[0x20]; ///< The name of the archive.
        u16 mRefCount; ///< The number of references to this archive.
        mDvd_mountMemArchive_c *mpDvdCmd; ///< The DVD command for mounting the archive.
        EGG::Archive *mpArchive; ///< An EGG::Archive instance for this archive.
        EGG::Heap *mpMountHeap; ///< The heap used for mounting the archive.
        void *mpArcBinary; ///< The raw archive data.
        int mUnk; ///< Unknown.
        EGG::FrmHeap *mDataHeap; ///< The heap used for loading the resources of the archive. [No p because of the string "dRes_c::info_c::mDataHeap"]
        u8 **mpFiles; ///< An array of pointers to the data of each loaded resource.

    };

    dRes_c(); ///< Constructs a new dRes_c.
    ~dRes_c(); ///< Destroys this dRes_c.

    /**
     * @brief Initializes the dRes_c.
     *
     * @param maxCount The number of archive holders to allocate.
     * @param callback The callback for after a archive has been loaded.
     * @return Whether the initialization was successful.
     */
    bool init(u16 maxCount, callback_c *callback);

    /**
     * @brief Registers an archive to be loaded next.
     *
     * @param resName The name of the archive to load.
     * @param filePath The path to the folder which the archive is in.
     * @param allocDir The allocation direction. 1: top-down, anything else: bottom-up.
     * @param heap The heap to load the archive into.
     * @return Whether the operation was successful.
     */
    bool setRes(const char *arcName, const char *containingFolder, u8 allocDir, EGG::Heap *heap);

    /**
     * @brief Marks the archive as no longer needed.
     * @param arcName The name of the archive to unload.
     * @return Whether the operation was successful.
     */
    bool deleteRes(const char *arcName);

    /**
     * @brief Loads a resource.
     *
     * @param arcName The name of the archive which contains the resource.
     * @param resPath The path to the resource within the archive.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath) const;

    /**
     * @brief Loads a resource.
     *
     * @param arcName The name of the archive which contains the resource.
     * @param resPath The path to the resource within the archive.
     * @param size A pointer where the size of the resource will be written to.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath, unsigned long *size) const;

    /**
     * @brief Loads a resource which may optionally be compressed.
     * @details If the resource is not found, it tries to load "<resource name>.LZ".
     * Only .LZ compression is supported.
     *
     * @param arcName The name of the archive which contains the resource.
     * @param resPath The path to the resource within the archive.
     * @param size A pointer where the size of the resource will be written to.
     * @param compressionType A pointer where the compression type of the resource will be written to.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath, unsigned long *size, int *compressionType) const;

    /// @brief Loads a resource without logging a message if the resource is not found.
    /// @see ::getRes(const char*, const char*) const
    void *getResSilently(const char *arcName, const char *resPath) const;

    /// @brief Loads a resource without logging a message if the resource is not found.
    /// @see ::getRes(const char*, const char*, unsigned long*) const
    void *getResSilently(const char *arcName, const char *resPath, unsigned long *size) const;

    /// @brief Attempts to load the resources of an archive that has finished loading since the last call to this function. The callback is executed on all files and folders.
    /// @return Whether such an archive was found.
    bool syncAllRes();

    /**
     * @brief Gets the info_c for an archive, if it exists.
     *
     * @param arcName The name of the archive.
     * @return The info_c for the archive, if it exists, otherwise @p nullptr .
     */
    info_c *getResInfo(const char *arcName) const;

    /**
     * @brief Gets the info_c for an archive, if it exists and has been loaded into memory.
     * @param arcName The name of the archive.
     * @return The info_c for the archive, or @p nullptr if the info_c doesn't exist or the archive hasn't been loaded into memory.
     */
    info_c *getResInfoLoaded(const char *arcName) const;

    /// @brief Gets a free info_c that can be used to store an archive reference.
    /// @return The info_c for the archive, or @p nullptr if none was available.
    info_c *newResInfo();

    /// @brief Copies an uncompressed resource.
    static void copyRes(const void *from, void *to, int size);

    /// @brief Copies an optionally compressed resource.
    static void copyRes(const void *from, void *to, int size, int compressionType);


private:
    info_c *mpArcInfos; ///< An array of archive holders.
    u16 mNumArcs; ///< The number of archive holders.
    callback_c *mpCallback; ///< The callback for when a resource is loaded.

    /// @unused The callback for when dRes_c::info_c::set is called.
    static void (*mSetCallback)(const char *arcName, EGG::Heap *heap);
};

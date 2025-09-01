#pragma once
#include <types.h>
#include <game/mLib/m_dvd.hpp>
#include <lib/egg/core/eggFrmHeap.h>
#include <lib/egg/core/eggArchive.h>
#include <revolution/ARC.h>
#include <nw4r/g3d.h>

/// @brief An archive resource management class.
/// @ingroup bases
class dRes_c {
public:

    /// @brief A callback class for processing resources.
    /// @details The calls to ::execute occur during the initial loading of an archive.
    class callback_c {
    public:
        /// @brief Initializes the callback with the resource name.
        virtual void init(const char *name) = 0;

        /**
         * @brief Executes the callback.
         * @param data The resource data.
         * @param folderSig The first 4 characters of the folder name.
         * @param path The path to the current resource.
         * @return The resource data.
         */
        virtual void *execute(void *data, u32 folderSig, const char* path) = 0;
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

    /// @brief An archive holder.
    class info_c {
    public:

        /// @brief The loading status of the archive.
        enum LOAD_STATUS_e {
            LOAD_ERROR = -1, ///< An error occurred while loading the archive.
            LOAD_SUCCESS = 0, ///< The archive was loaded successfully.
            LOAD_IN_PROGRESS = 1, ///< The archive is currently being loaded.
        };

        info_c(); ///< Constructs a new archive holder.
        ~info_c(); ///< Destroys the archive holder.

        /// @brief Unmounts the archive and frees the holder for use.
        /// @return Whether the operation was successful.
        bool cleanup();

        /**
         * @brief Sets information about the archive to be loaded.
         *
         * @param arcName The name of the archive.
         * @param containingFolder The path to the folder which the archive is in.
         * @param allocDirection The allocation direction. See ::MEMExpHeapAllocDir.
         * @param heap The heap to load the resources of the archive into, or @p nullptr to use the default heap.
         * @return Whether the archive was prepared successfully and will be loaded.
         */
        bool set(const char *arcName, const char *containingFolder, u8 allocDirection, EGG::Heap *heap);

        /**
         * @brief Attempts to load the archive into memory and load the resources with a callback.
         *
         * @param callback The resource loaded callback, or @p nullptr .
         * @return The result of the operation.
         */
        LOAD_STATUS_e setRes(callback_c *callback);

        const char *getName() const { return mName; }
        int getRefCount() const { return mRefCount; }

        /// @brief Gets the file loading command.
        /// @return The file loading command, or @p nullptr if the archive has already been loaded
        /// or the holder is empty.
        mDvd_mountMemArchive_c *getDvdCmd() const { return mpDvdCmd; }

        /// @brief Gets the archive accessor.
        /// @return The archive accessor, or @p nullptr if the archive has not yet been loaded
        /// or the holder is empty.
        EGG::Archive *getArchive() const { return mpArchive; }

        void incRefCount() { mRefCount++; } ///< Increments the reference count.
        void decRefCount() { mRefCount--; } ///< Decrements the reference count.

    private:

        /**
         * @brief Builds the resource list and executes a callback on each file and directory.
         * @param callback The resource loaded callback, or @p nullptr .
         * @return The result of the operation.
         */
        LOAD_STATUS_e loadRes(callback_c *callback);

        char mName[0x20]; ///< The name of the archive.
        u16 mRefCount; ///< The number of references to this archive.
        mDvd_mountMemArchive_c *mpDvdCmd; ///< The DVD command for mounting the archive.
        EGG::Archive *mpArchive; ///< The accessor for this archive.
        EGG::Heap *mpMountHeap; ///< The heap used for mounting the archive.
        void *mpArcBinary; ///< The raw archive data.
        int mArchiveSize; ///< The archive size.

        /// @brief The heap used for loading the resources of the archive.
        /// @decompnote{No p because of the string "dRes_c::info_c::mDataHeap"}
        EGG::FrmHeap *mDataHeap;

        u8 **mpFiles; ///< An array of pointers to the data of each loaded resource.
    };

    dRes_c(); ///< Constructs a new manager.
    ~dRes_c(); ///< Destroys the manager.

    /**
     * @brief Initializes the manager by allocating the archive holders and setting the callback.
     *
     * @param maxCount The number of archive holders to allocate.
     * @param callback The resource loaded callback.
     * @return Whether the initialization was successful.
     */
    bool init(u16 maxCount, callback_c *callback);

    /**
     * @brief Schedules an archive for loading.
     * @param arcName The name of the archive to load. See the [path notes](#path-notes).
     * @param containingFolder The path to the folder the archive is in. See the [path notes](#path-notes).
     * @param allocDir The allocation direction. See ::MEMExpHeapAllocDir.
     * @param heap The heap to load the archive into, or @p nullptr to use the default archive heap.
     * @return Whether the operation was successful.
     */
    bool setRes(const char *arcName, const char *containingFolder, u8 allocDir, EGG::Heap *heap);

    /**
     * @brief Marks an archive as no longer needed.
     *
     * @param arcName The name of the archive to unload. See the [path notes](#path-notes).
     * @return Whether the operation was successful.
     */
    bool deleteRes(const char *arcName);

    /**
     * @brief Gets a resource.
     *
     * @param arcName The name of the archive which contains the resource. See the [path notes](#path-notes).
     * @param resPath The path to the resource within the archive. See the [path notes](#path-notes).
     * @return The resource file.
     */
    nw4r::g3d::ResFile getRes(const char *arcName, const char *resPath) const;

    /**
     * @brief Gets a resource.
     *
     * @param arcName The name of the archive which contains the resource. See the [path notes](#path-notes).
     * @param resPath The path to the resource within the archive. See the [path notes](#path-notes).
     * @param size A pointer where the size of the resource will be written to.
     * @return The resource file.
     */
    nw4r::g3d::ResFile getRes(const char *arcName, const char *resPath, unsigned long *size) const;

    /**
     * @brief Gets a resource which may optionally be compressed.
     * @details If the resource is available in both compressed and uncompressed form, the latter is
     * prioritized. Only LZ77 compression is supported (the resource must be named @p \<resPath\>\.LZ
     * for it to be found).
     *
     * @param arcName The name of the archive which contains the resource. See the [path notes](#path-notes).
     * @param resPath The path to the resource within the archive. See the [path notes](#path-notes).
     * @param size A pointer where the uncompressed size of the resource will be written to, or @p nullptr .
     * @param compressionType A pointer where the compression type of the resource will be written to, or
     * @p nullptr . See ::CXCompressionType.
     * @return A pointer to the compressed contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath, unsigned long *size, int *compressionType) const;

    /// @brief Gets a resource without logging a message if the resource is not found.
    /// @see ::getRes(const char*, const char*) const
    void *getResSilently(const char *arcName, const char *resPath) const;

    /// @brief Gets a resource without logging a message if the resource is not found.
    /// @see ::getRes(const char*, const char*, unsigned long*) const
    void *getResSilently(const char *arcName, const char *resPath, unsigned long *size) const;

    /// @brief Attempts to load the resources of an archive that has finished loading since the last call to this function. The callback is executed on all files and folders.
    /// @return Whether such an archive was found.
    bool syncAllRes();

private:
    /**
     * @brief Gets the holder for an archive.
     * @param arcName The name of the archive.
     * @return The holder for the archive, or @p nullptr if the holder doesn't exist.
     */
    info_c *getResInfo(const char *arcName) const;

    /**
     * @brief Gets the holder for a loaded archive.
     * @param arcName The name of the archive.
     * @return The holder for the archive, or @p nullptr if the holder doesn't exist or the archive
     * hasn't been loaded into memory yet.
     */
    info_c *getResInfoLoaded(const char *arcName) const;

    /// @brief Gets a free archive holder that can be used to store an archive reference.
    /// @return The free archive holder, or @p nullptr if none is available.
    info_c *newResInfo();

public:
    /// @brief Copies an uncompressed resource.
    static void copyRes(const void *from, void *to, int size);

    /// @brief Copies an optionally compressed resource.
    /// @param size The size of the data. Only used for uncompressed resources.
    /// @param compressionType The compression type. See ::CXCompressionType for the possible values.
    static void copyRes(const void *from, void *to, int size, int compressionType);

private:
    info_c *mpArcInfos; ///< An array of archive holders.
    u16 mNumArcs; ///< The number of archive holders.
    callback_c *mpCallback; ///< The callback for when a resource is loaded.

    /// @brief The callback for when an archive is scheduled for loading.
    /// @unused
    static void (*mSetCallback)(const char *arcName, EGG::Heap *heap);
};

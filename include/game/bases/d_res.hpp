#pragma once
#include <types.h>
#include <lib/rvl/arc/ARC.h>
#include <lib/egg/heap/eggHeap.hpp>
#include <lib/egg/heap/eggFrmHeap.hpp>
#include <lib/egg/archive.hpp>
#include <game/mLib/m_dvd.h>

/**
 * @brief An archive resource management class.
 * @ingroup bases
 * @details
 * The dRes_c class manages resource archives ( @p \.arc files). It does so by storing multiple
 * @ref info_c "archive holders". The class is instantiated automatically by dResMng_c, which provides
 * higher level access.
 *
 * ## Path Notes
 * Resource paths are subject to the following restrictions:
 * - The @p \.arc extension is appended automatically to archive names and therefore must not be passed.
 * - Full paths to archive files (including the appended extension) cannot exceed 80 characters and must
 * not begin with a @p / character.
 * - Full paths to resources inside an archive should not exceed 250 characters.
 *
 * ## Loading Archives
 * Use the ::setRes function to schedule an archive for loading. For loading multiple archives
 * at once, see @ref dResMng_c::setRes "the dResMng_c equivalent".
 *
 * @note Many kinds of assets are loaded automatically, therefore doing so manually is not necessary.
 * These include:
 * - Stages
 * - Tilesets
 * - Backgrounds
 * - Actor resources
 * - Worldmaps and their resources
 *
 * @hint{If it doesn't fall in any of the aforementioned cases, it is recommended to use this class to load
 * archives containing nw4r-based assets (such as @p \.brres files), as the game will automatically handle
 * the necessary initialization.
 *
 * For layout archives, it's recommended to use the
 * @ref LytBase_c::ReadResource3 "dedicated LytBase_c functions" instead.}
 *
 * ## Getting Archive Contents
 * Use the ::getRes function to obtain a resource inside a specific archive. Overloads are provided to
 * obtain the size of the resource and optionally detect compression.
 *
 * If the file could not be found under the archive, a warning is printed to the console.
 * Use ::getResSilently if this behaviour is undesired (such as with an optional file).
 *
 * @note No overload for ::getResSilently with compression support is available.
 *
 * ### Compressed Resources
 * While the @ref getRes(const char*, const char*, unsigned long*, int*) const "getRes" function can detect
 * compression and return the decompressed data size, it does not actually perform the decompression itself.
 * This can be achieved by calling the @ref copyRes(const void*, void*, int, int) "copyRes" function with
 * the compressed data and the compression type returned from the previous function, plus a buffer to store
 * the decompressed data into.
 *
 * @note Only LZ compression is supported.
 *
 * ## Deleting Archives
 * Use the ::deleteRes function to mark an archive as no longer needed by the calling entity.
 * This ensures that archives no longer in use are properly disposed of.
 *
 * ## Reading Archive Files
 * Use the ::syncAllRes function to perform the actual archive file reading. The function will return
 * @p true until all files are loaded.
 *
 * ## Unused Content
 * - ::mSetCallback is an unused callback for when an archive is added to the class. It was probably
 * used to debug resource loading.
 */
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

        /// @brief The loading status of the archive.
        enum LOAD_STATUS_e {
            LOAD_ERROR = -1, ///< An error occurred while loading.
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
         * @param heap The heap to load the resources of the archive into.
         * @return Whether the archive was prepared successfully and will be loaded.
         */
        bool set(const char *arcName, const char *containingFolder, u8 allocDirection, EGG::Heap *heap);

        /**
         * @brief Attempts to load the archive into memory and load the resources with a callback.
         *
         * @param callback The callback for the resources.
         * @return The result of the operation.
         */
        LOAD_STATUS_e setRes(callback_c *callback);

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
         * @return The result of the operation.
         */
        LOAD_STATUS_e loadRes(callback_c *callback);

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

    dRes_c(); ///< Constructs a new manager.
    ~dRes_c(); ///< Destroys the manager.

    /**
     * @brief Initializes the manager.
     *
     * @param maxCount The number of archive holders to allocate.
     * @param callback The resource loaded callback.
     * @return Whether the initialization was successful.
     */
    bool init(u16 maxCount, callback_c *callback);

    /**
     * @brief Schedules an archive for loading.
     *
     * @param arcName The name of the archive to load. See the [path notes](#path-notes).
     * @param containingFolder The path to the folder the archive is in. See the [path notes](#path-notes).
     * @param allocDir The allocation direction. See ::MEMExpHeapAllocDir.
     * @param heap The heap to load the archive into, or @p nullptr to use the default heap.
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
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath) const;

    /**
     * @brief Gets a resource.
     *
     * @param arcName The name of the archive which contains the resource. See the [path notes](#path-notes).
     * @param resPath The path to the resource within the archive. See the [path notes](#path-notes).
     * @param size A pointer where the size of the resource will be written to.
     * @return A pointer to the contents of the resource.
     */
    void *getRes(const char *arcName, const char *resPath, unsigned long *size) const;

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

    /// @unused The callback for when an archive is scheduled for loading.
    static void (*mSetCallback)(const char *arcName, EGG::Heap *heap);
};

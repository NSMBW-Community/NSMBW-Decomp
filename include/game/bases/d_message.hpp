#pragma once

#include <game/bases/d_msg_res.hpp>
#include <game/bases/d_dvd.hpp>

/// @brief Helper class for managing message resources.
/// @ingroup bases
class dMessage_c {
public:
    dMessage_c(); ///< Constructs a new @p dMessage_c instance.
    ~dMessage_c(); ///< Destroys the @p dMessage_c instance.

private:
    void buildMsgRes(EGG::Heap *heap); ///< Creates a new @p MsgRes_c instance from the loaded message resource file.

public:
    static bool create(EGG::Heap *heap); ///< Loads the message resource file from disk and initializes it.
    static wchar_t *getMsg(ulong messageGroup, ulong messageID); ///< Gets a message from the group number and ID.
    static MsgRes_c *getMesRes(); ///< Gets the @p MsgRes_c instance used.

    /**
     * @brief Writes @p num into @p dst with @p digitCount decimal digits, replacing
     * suppressed leading zeros with spaces. At most @p writeLimit characters are
     * written, followed by a null terminator.
     * @unofficial
     *
     * @param num The number to convert.
     * @param dst The destination buffer.
     * @param writeLimit The maximum number of characters to write.
     * @param digitCount The number of digits to write.
     * @param suppressLeadingZero Whether to suppress leading zeros (1) or not (0).
     */
    static void numToString(int num, wchar_t *dst, size_t writeLimit, int digitCount, int suppressLeadingZero);

    /// @brief Same as ::numToString, but uses full-width characters.
    /// @unofficial
    static void numToFullWidth(int num, wchar_t *dst, size_t writeLimit, int digitCount, int suppressLeadingZero);

    static void changetoFullSize(wchar_t *str); ///< Converts regular ASCII characters in @p str to full-width characters.

    dDvd::loader_c mLoader; ///< The loader for loading the message resource file from disk.
    void *mpFileData; ///< The raw message resource file data.
    MsgRes_c *mpMsgRes; ///< The message resource instance used to retrieve messages.
};

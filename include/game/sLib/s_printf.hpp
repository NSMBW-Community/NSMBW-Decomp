#pragma once
#include <types.h>
#include <game/sLib/s_lib.hpp>

/// @brief Wrapper around the runtime's formatted print function.
/// @ingroup slib
namespace sPrintf {
    /// @brief Gets the function used to print formatted output.
    vprintfFunc GetVPrintfFunc();

    /**
     * @brief Prints formatted output.
     *
     * @param format The format string.
     * @param args The arguments for the format string.
     * @return The number of characters printed.
     */
    int vprintf(const char *format, va_list *args);
};

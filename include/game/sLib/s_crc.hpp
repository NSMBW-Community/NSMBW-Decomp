#pragma once
#include <types.h>

/// @brief Wrapper around the runtime's CRC32 calculation function.
/// @ingroup slib
class sCrc {
public:
    /**
     * @brief Calculates the CRC32 checksum of a buffer.
     *
     * @param data The buffer to checksum.
     * @param size The size of the buffer, in bytes.
     * @return The CRC32 checksum.
     */
    static ulong calcCRC32(const void *data, ulong size);
};

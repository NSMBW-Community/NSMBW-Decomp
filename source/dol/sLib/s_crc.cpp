#include <game/sLib/s_crc.hpp>
#include <revolution/OS.h>

ulong sCrc::calcCRC32(const void *data, ulong size) {
    return OSCalcCRC32(data, size);
}

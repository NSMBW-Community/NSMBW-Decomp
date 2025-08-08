#ifndef NW4R_UT_BINARY_FILE_FORMAT_H
#define NW4R_UT_BINARY_FILE_FORMAT_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ut {

struct BinaryBlockHeader {
    u32 kind; // at 0x0
    u32 size; // at 0x4
};

struct BinaryFileHeader {
    u32 signature;  // at 0x0
    u16 byteOrder;  // at 0x4
    u16 version;    // at 0x6
    u32 fileSize;   // at 0x8
    u16 headerSize; // at 0xC
    u16 dataBlocks; // at 0xE
};

bool IsValidBinaryFile(const BinaryFileHeader* pHeader, u32 signature,
                       u16 version, u16 minBlocks);

} // namespace ut
} // namespace nw4r

#endif
